#ifndef LINKED_LIST_INCLUDED
#define LINKED_LIST_INCLUDED

#include <defs.h>

#include <predicate.h>

struct ll_node{
	struct ll_node *prev;
	struct ll_node *next;
};

struct ll_info{
	struct ll_node *head;
	struct ll_node *tail;
	int isize;
#ifdef GLUKE
	struct mem_block *mb;
#endif
};

struct ll_info* ll_init(struct ll_info *inf,int isize);

void ll_delete(struct ll_info *inf,void *p);

void* ll_push_back(struct ll_info *inf);

void *ll_first(struct ll_info *inf);

void *ll_next(struct ll_info *inf,void *p);

void* ll_next_loop(struct ll_info *inf,void *p);

void ll_pop_back(struct ll_info *inf);

void ll_delete_p(struct ll_info *inf,predicate f);

void ll_delete_p_1arg(struct ll_info *inf,predicate_1arg f,void *arg);

void* ll_map_1arg(struct ll_info *inf,predicate_1arg f,void *arg);

void* ll_map(struct ll_info *inf,predicate f);

#endif

