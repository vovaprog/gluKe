#ifndef RGN_H_INCLUDED
#define RGN_H_INCLUDED

int rgn_init();

void* rgn_alloc(int nof_bytes);
void* rgn_alloc_forever(int nof_bytes);

void* rgn_alloc_t(int nof_bytes,int task_id);

void* rgn_alloc_base(void *base,int nof_bytes);
void* rgn_alloc_base_forever(void *base,int nof_bytes);

int rgn_free(void *base);

int rgn_free_task_memory(int task_id);

void rgn_print_info();

#endif
