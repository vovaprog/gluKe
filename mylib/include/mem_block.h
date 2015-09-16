#ifndef MEM_BLOCK_H_INCLUDED
#define MEM_BLOCK_H_INCLUDED

#include <defs.h>

struct mem_block{
	int isize;
	int used_items;
	int max_items;
	int block_size_bytes;
	struct mem_block *next_block;
	char *items;
};

struct mem_block* mb_init(int isize,int items_in_block);

void mb_destroy(struct mem_block *mb);

void* mb_alloc(struct mem_block *mb);

int mb_free(struct mem_block *mb,void *p);

void* mb_first(struct mem_block *mb);

void *mb_next(struct mem_block *mb,void *p);

typedef int (*mb_callback_1arg)(void *p_item,void *arg);

void mb_map_1arg(struct mem_block *mb,mb_callback_1arg f,void *arg);

typedef int (*mb_predicate_1arg)(void *p_item,void *arg);

void mb_free_p_1arg(struct mem_block *mb,mb_predicate_1arg pred,void *arg);

#endif

