#include <mem_block.h>

/*
When using this in kernel, synchronization should be used.
If you use iteration, lock it yourself, don't forget !
*/

#define MEM_BLOCK_SYNC

#ifdef GLUKE

#   include <utils.h>
#   include <rgn.h>
#   include <int.h>


#   define MEMSET kmemset

#   define ALLOC rgn_alloc
#   define FREE rgn_free

#   ifdef MEM_BLOCK_SYNC
#      define MEM_BLOCK_SYNC_LOCK one();
#      define MEM_BLOCK_SYNC_UNLOCK all();
#   else
#      define MEM_BLOCK_SYNC_LOCK
#      define MEM_BLOCK_SYNC_UNLOCK
#   endif

#else

#   include <stdlib.h>
#   include <string.h>

#   define ALLOC malloc
#   define FREE free

#   define MEMSET memset

//define them now empty
#   define MEM_BLOCK_SYNC_LOCK
#   define MEM_BLOCK_SYNC_UNLOCK

#endif


struct mem_block* mb_init(int isize,int items_in_block)
{
	struct mem_block *m;
	int block_size_bytes;

	isize+=sizeof(int);

	block_size_bytes=isize*items_in_block+sizeof(struct mem_block);

//if compiling in gluKe - add number of bytes to page size
//else, if using malloc this is not needed.
#ifdef GLUKE
	block_size_bytes=TO_PAGE_SIZE(block_size_bytes);
	block_size_bytes-=((block_size_bytes-sizeof(struct mem_block)) % isize);
#endif

	if((m=ALLOC(block_size_bytes))==0) return 0;

	MEMSET(m,0,block_size_bytes);

	m->block_size_bytes=block_size_bytes;
	m->isize=isize;

	m->max_items=(m->block_size_bytes-sizeof(struct mem_block))/isize;
	m->used_items=0;

	m->items=(char*)m+sizeof(struct mem_block);

	m->next_block=0;

	return m;
}

void mb_destroy(struct mem_block *mb)
{
	struct mem_block *p;

	while(mb)
	{
		p=mb->next_block;

		FREE(mb);

		mb=p;
	}
}

void* mb_alloc(struct mem_block *mb)
{
	struct mem_block *mb2;
	char *p,*lim;

	if(!mb) return 0;

	mb2=mb;

	MEM_BLOCK_SYNC_LOCK

	while(mb2)
	{
		if(mb2->used_items < mb2->max_items)
		{
			lim=(char*)mb2->items + mb2->isize * mb2->max_items;
			for(p=mb2->items;p<lim;p+=mb2->isize)
			{
				if(((int*)p)[0]==0)
				{
					((int*)p)[0]=1;
					mb2->used_items++;

					MEM_BLOCK_SYNC_UNLOCK

					return p+sizeof(int);
				}
			}
			
			MEM_BLOCK_SYNC_UNLOCK
			
			//invalid struct
			return 0;
		}

		mb=mb2;//mb2 is not 0, if it will become 0 this time last list item
		       //will be saved in mb

		mb2=mb2->next_block;
	}

	if((mb2=ALLOC(mb->block_size_bytes))==0)
	{
		MEM_BLOCK_SYNC_UNLOCK
		return 0;
	}

	MEMSET(mb2,0,mb->block_size_bytes);

	mb2->block_size_bytes=mb->block_size_bytes;
	mb2->max_items=mb->max_items;
	mb2->used_items=0;
	mb2->items=(char*)mb2+sizeof(struct mem_block);
	mb2->isize=mb->isize;
	mb2->next_block=0;

	mb->next_block=mb2;

	mb2->used_items++;
	((int*)mb2->items)[0]=1;

	MEM_BLOCK_SYNC_UNLOCK

	return (char*)mb2->items+sizeof(int);
}

//if there are no allocated items in block - block is freed
//first block is not freed
int mb_free(struct mem_block *mb,void *p)
{
	struct mem_block *prev_mb=0;

	if(!mb) return -1;

	MEM_BLOCK_SYNC_LOCK

	while(mb)
	{
		if((char*)p>(char*)(mb->items) &&
			(char*)p<(char*)mb+mb->block_size_bytes)
		{
			if(((char*)p-sizeof(int)-mb->items) % mb->isize==0)
			{
				//clear used flag
				((int*)p-1)[0]=0;

				--mb->used_items;

				if(mb->used_items==0 && prev_mb!=0)
				{
					prev_mb->next_block=mb->next_block;
					FREE(mb);
				}
				
				MEM_BLOCK_SYNC_UNLOCK
				
				return 0;
			}
			else 
			{
				MEM_BLOCK_SYNC_UNLOCK
				return -1; //p is invalid pointer
			}
		}

		prev_mb=mb;
		mb=mb->next_block;
	}
	MEM_BLOCK_SYNC_UNLOCK
	return -1;
}
