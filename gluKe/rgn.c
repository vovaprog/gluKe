#include <glinc.h>
#include <rgn.h>
#include <utils.h>

#define ALS_TABLE_BYTES  (MAX_NOF_ALS*sizeof(struct rgn_allocation))

extern unsigned int KERNEL_SIZE;
extern unsigned int RAM_SIZE;

struct rgn_allocation{
	void *base;
	int nof_pages;
	int task_id;
} *free_als,*reserved_als;

static void* st_rgn_alloc_basic(int nof_bytes,int task_id)
{
	int i,j,nof_pages;
	void *base;

	if(nof_bytes<=0) return 0;

	nof_pages=NOF_PAGES(nof_bytes);

	for(i=0;i<MAX_NOF_ALS;i++)
	{
		if(free_als[i].nof_pages>=nof_pages)
		{			
			base=free_als[i].base;

			/* first find empty allocation in reserved list */
			for(j=0;j<MAX_NOF_ALS && reserved_als[j].nof_pages!=0;j++);
			if(j==MAX_NOF_ALS) return 0;

			/* only now change values */
			//(unsigned int)free_als[i].base+=nof_pages*PAGE_SIZE;
            free_als[i].base = (unsigned int)free_als[i].base + nof_pages*PAGE_SIZE;
			free_als[i].nof_pages-=nof_pages;

			reserved_als[j].base=base;
			reserved_als[j].nof_pages=nof_pages;

			reserved_als[j].task_id=task_id;

			return base;
		}
	}
	return 0;
}

void* rgn_alloc(int nof_bytes)
{
	void *p;

	one();

	p=st_rgn_alloc_basic(nof_bytes,task_cur_id());

	all();

	return p;
}

void* rgn_alloc_t(int nof_bytes,int task_id)
{
	char *p;

	one();

	p=st_rgn_alloc_basic(nof_bytes,task_id);

	all();

	return p;
}

static void* st_rgn_alloc_base_basic(void *base,int nof_bytes)
{
	int i,dif_pages,tail_pages,nof_pages,reserved_index=-1,free_for_tail_index=-1;

	if((unsigned int)base % PAGE_SIZE || nof_bytes<=0) return 0;

	nof_pages=NOF_PAGES(nof_bytes);

	for(i=0;i<MAX_NOF_ALS;i++)
	{
		if(free_als[i].nof_pages!=0 &&
			base>=free_als[i].base &&
			base<(void*)((unsigned int)free_als[i].base+free_als[i].nof_pages*PAGE_SIZE))
		{
			dif_pages=(base-free_als[i].base)/PAGE_SIZE;
			tail_pages=free_als[i].nof_pages-dif_pages-nof_pages;

			if(tail_pages<0) return 0;

			/* find free al for tail */		
			if(tail_pages>0)
			{
				for(free_for_tail_index=0;free_for_tail_index<MAX_NOF_ALS && free_als[free_for_tail_index].nof_pages!=0;free_for_tail_index++);
				if(free_for_tail_index>=MAX_NOF_ALS) return 0;
			}

			/* find reserved al */
			for(reserved_index=0;reserved_index<MAX_NOF_ALS && reserved_als[reserved_index].nof_pages!=0;reserved_index++);
			if(reserved_index>=MAX_NOF_ALS) return 0;

			/* and now make changes */
			free_als[i].nof_pages=dif_pages;

			reserved_als[reserved_index].base=base;
			reserved_als[reserved_index].nof_pages=nof_pages;

			if(tail_pages>0)
			{
				free_als[free_for_tail_index].base=base+nof_pages*PAGE_SIZE;
				free_als[free_for_tail_index].nof_pages=tail_pages;
			}

			return base;
		}
	}
	return 0;
}

void* rgn_alloc_base(void *base,int nof_bytes)
{
	char *p;

	one();

	p=st_rgn_alloc_base_basic(base,nof_bytes);

	all();

	return p;
}


static int st_reserved_with_base(void *base)
{
	int i;

	for(i=0;i<MAX_NOF_ALS;i++)
		if(reserved_als[i].base==base) return i;

	return -1;
}

static int st_free_before(void *base)
{
	int i;

	for(i=0;i<MAX_NOF_ALS;i++)
		if(free_als[i].nof_pages &&
			((char*)free_als[i].base+free_als[i].nof_pages*PAGE_SIZE)==base)
			return i;

	return -1;
}


static int st_free_after(void *base,int nof_pages)
{
	void *p=(char*)base+nof_pages*PAGE_SIZE;
	int i;

	for(i=0;i<MAX_NOF_ALS;i++)
		if(free_als[i].nof_pages!=0 && free_als[i].base==p)
			return i;

	return -1;
}


static int st_free_index(int r_index)
{
	int i,free_before=-1,free_after=-1;

	//free_before - index of free als with region ending at base
	//              or, if such als does not exist - empty als
	if((free_before=st_free_before(reserved_als[r_index].base))<0)
		for(i=0;i<MAX_NOF_ALS;i++)
			if(free_als[i].nof_pages==0)
			{
				free_before=i;
				free_als[free_before].base=reserved_als[r_index].base;
				break;
			}

	if(free_before<0) return -1;

	free_after=st_free_after(reserved_als[r_index].base,
		reserved_als[r_index].nof_pages);

	if(free_after>=0)
	{
		reserved_als[r_index].nof_pages+=free_als[free_after].nof_pages;
		free_als[free_after].nof_pages=0;
	}

	free_als[free_before].nof_pages+=reserved_als[r_index].nof_pages;
	reserved_als[r_index].nof_pages=0;

	return 0;
}

int rgn_free(void *base)
{
	int r_index,ret;

	one();

	if((r_index=st_reserved_with_base(base))<0)
	{
		all();
		return -1;
	}

	ret=st_free_index(r_index);

	all();

	return ret;
}

int rgn_free_task_memory(int task_id)
{
	int i,ret=0;

	one();

	for(i=0;i<MAX_NOF_ALS;i++)
		if(reserved_als[i].nof_pages!=0 && reserved_als[i].task_id==task_id)
		{
			if(st_free_index(i)) ret=-1;
		}

	all();

	return ret;
}

int rgn_init()
{
	free_als=(struct rgn_allocation*)ALS_FREE_BASE;
	reserved_als=(struct rgn_allocation*)ALS_RESERVED_BASE;

	kmemset(free_als,0,ALS_TABLE_BYTES);
	kmemset(reserved_als,0,ALS_TABLE_BYTES);

	free_als[0].base=0;
	free_als[0].nof_pages=RAM_SIZE/PAGE_SIZE;

	rgn_alloc_base(free_als,ALS_TABLE_BYTES);
	rgn_alloc_base(reserved_als,ALS_TABLE_BYTES);

	return 0;
}

void rgn_print_info()
{
	int i;
	char itos_buf[ITOS_BSIZE];

	kwr("\n--- rgn info free ---\n");
	for(i=0;i<MAX_NOF_ALS;i++)
	{
		if(free_als[i].nof_pages)
		{
			kwr(itos_ux((unsigned int)free_als[i].base,itos_buf));
			kwr(":");
			kwr(itos_ux(free_als[i].nof_pages,itos_buf));
			kwr(" ");
		}
	}
	kwr("\n--- rgn info free ---\n");

	kwr("\n--- rgn info reserved ---\n");
	for(i=0;i<MAX_NOF_ALS;i++)
	{
		if(reserved_als[i].nof_pages)
		{
			kwr(itos_ux((unsigned int)reserved_als[i].base,itos_buf));
			kwr(":");
			kwr(itos_ux(reserved_als[i].nof_pages,itos_buf));
			kwr(" ");
		}
	}
	kwr("\n--- rgn info reserved ---\n");
}

