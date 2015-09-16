#include <defs.h>
#include <linked_list.h>

#ifdef GLUKE
#include <mem_block.h>
#include <int.h>
#else
#include <stdlib.h>
#endif

#define LL_ITEMS_IN_MEM_BLOCK 50

#define LL_SYNC

#ifndef LL_SYNC
#   define LL_SET_LOCK
#   define LL_RELEASE_LOCK
#else
#   ifdef GLUKE
#      define LL_SET_LOCK one()
#      define LL_RELEASE_LOCK all();
#   else
       //define them now empty
#      define LL_SET_LOCK
#      define LL_RELEASE_LOCK
#   endif
#endif

#ifdef GLUKE
#   define LL_SET_LOCK one()
#   define LL_RELEASE_LOCK all();
#endif

struct ll_info* ll_init(struct ll_info *inf,int isize)
{
#ifdef GLUKE
	if((inf->mb=mb_init(isize+sizeof(struct ll_node),LL_ITEMS_IN_MEM_BLOCK))==0)
		return 0;
#endif

	inf->head=inf->tail=0;

	inf->isize=isize;

	return inf;
}

void ll_destroy(struct ll_info *inf)
{
	ll_delete_p(inf,P_true);
}

/* this function should be called with lock already taken before */
static void ll_delete_node(struct ll_info *inf,struct ll_node *node)
{
	if(node->prev)
		node->prev->next=node->next;
		
	if(node->next)
		node->next->prev=node->prev;

	if(node==inf->head)
		inf->head=node->next;
	
	if(node==inf->tail)
		inf->tail=node->prev;
	
#ifdef GLUKE
	mb_free(inf->mb,node);
#else
	free(node);
#endif
	return;
}

static void ll_find_and_delete_node(struct ll_info *inf,void *node)
{
	struct ll_node *cur_node;

	LL_SET_LOCK;

	for(cur_node=inf->head;cur_node;cur_node=cur_node->next)
	{
		if(cur_node==node)
		{
			ll_delete_node(inf,node);

			LL_RELEASE_LOCK

			return;
		}	
	}

	LL_RELEASE_LOCK;
}

void ll_delete(struct ll_info *inf,void *p)
{
	struct ll_node *node;

	node=(struct ll_node*)((char*)p-sizeof(struct ll_node));

	ll_find_and_delete_node(inf,node);
}

void* ll_push_back(struct ll_info *inf)
{
	struct ll_node *node;

	LL_SET_LOCK;

	if((node=
#ifdef GLUKE
	mb_alloc(inf->mb)
#else
	malloc(inf->isize)
#endif
		)==0)
	{
		LL_RELEASE_LOCK;

		return 0;
	}

	if(inf->tail)
	{
		inf->tail->next=node;
		
		node->next=0;
		node->prev=inf->tail;

		inf->tail=node;
   }
   else
   {
   	inf->tail=inf->head=node;

   	node->prev=node->next=0;
   }

	LL_RELEASE_LOCK;

   return (char*)node+sizeof(struct ll_node);
}

void *ll_first(struct ll_info *inf)
{
	if(inf==0) return 0;
	if(inf->head==0) return 0;

	return ((char*)inf->head)+sizeof(struct ll_node);
}

void *ll_next(struct ll_info *inf,void *p)
{
	struct ll_node *node;

	node=(struct ll_node*)((char*)p-sizeof(struct ll_node));

	if(node->next) return ((char*)node->next)+sizeof(struct ll_node);
	
	return 0;
}

void* ll_next_loop(struct ll_info *inf,void *p)
{
	struct ll_node *node;

	if((node=ll_next(inf,p))==0) node=ll_first(inf);

	return node;
}

void ll_pop_back(struct ll_info *inf)
{
	LL_SET_LOCK;

	ll_delete_node(inf,inf->tail);	

	LL_RELEASE_LOCK;
}

static void st_delete_p(struct ll_info *inf,void *f,int nof_args,void *arg0,void *arg1)
{
	void *p,*temp_p;
	int result;

	LL_SET_LOCK;

	p=ll_first(inf);

	while(p)
	{
		switch(nof_args){
		case 0:
			result=((predicate)f)(p);break;
		case 1:
			result=((predicate_1arg)f)(p,arg0);break;
		case 2:
			result=((predicate_2args)f)(p,arg0,arg1);break;
		default:
			LL_RELEASE_LOCK;
			return;
		}

		if(result)
		{
			temp_p=ll_next(inf,p);
			ll_delete(inf,p);
			p=temp_p;
		}
		else p=ll_next(inf,p);
	}

	LL_RELEASE_LOCK;
}

void ll_delete_p(struct ll_info *inf,predicate f)
{
	st_delete_p(inf,f,0,0,0);
}

void ll_delete_p_1arg(struct ll_info *inf,predicate_1arg f,void *p)
{
	st_delete_p(inf,f,1,p,0);
}

static void* st_map(struct ll_info *inf,void *f,int nof_args,void *arg0,void *arg1)
{
	void *p;
	int result;

	p=ll_first(inf);

	while(p)
	{
		switch(nof_args){
		case 0:
			result=((predicate)f)(p);break;
		case 1:
			result=((predicate_1arg)f)(p,arg0);break;
		case 2:
			result=((predicate_2args)f)(p,arg0,arg1);break;
		default:
			return 0;
		}

		if(result>0) return p;
		if(result<0) return 0;
		
		p=ll_next(inf,p);
	}
	return 0;
}

void* ll_map_1arg(struct ll_info *inf,predicate_1arg f,void *arg)
{
	return st_map(inf,f,1,arg,0);
}

void* ll_map(struct ll_info *inf,predicate f)
{
	return st_map(inf,f,0,0,0);
}

