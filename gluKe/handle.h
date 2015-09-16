#ifndef HANDLE_H_INCLUDED
#define HANDLE_H_INCLUDED

#define HANDLE_TYPE_FILE  1
#define HANDLE_TYPE_MUTEX 2

struct handle* handle_open();

int handle_remove(struct handle *h);

int handle_remove_id(int hid);

int handle_close_task_handles(int task_id);

int handle_init();

struct handle* handle_by_id(int hid);

void* handle_map(predicate f);

void* handle_map_1arg(predicate_1arg f,void *arg);

int handle_print_handles();

#endif

