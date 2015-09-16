#include <glinc.h>

#include <handle.h>

static struct ll_info handles;

// just take one greater than max existing file id.
static int new_handle_id()
{
	struct handle *h;
	int max_id=0;

	h=ll_first(&handles);

	while(h)
	{
		if(h->id>max_id) max_id=h->id;
		h=ll_next(&handles,h);
	}

	return max_id+1;
}

static int handle_by_id_predicate(void *p_item,void *arg)
{
	struct handle *h=(struct handle*)p_item;
	int id=*((int*)arg);

	if(h->id==id) return 1;

	return 0;
}

struct handle* handle_by_id(int hid)
{
	return ll_map_1arg(&handles,handle_by_id_predicate,&hid);
}

struct handle* handle_open()
{
	struct handle *h;

	one();

	if((h=ll_push_back(&handles))==0) return 0;

	h->id=new_handle_id();

	h->task_id=task_cur_id();

	all();
	
	return h;
}

int handle_remove(struct handle *h)
{
	ll_delete(&handles,h);
	return 0;
}

int handle_remove_id(int hid)
{
	return handle_remove(handle_by_id(hid));
}

static int free_task_handles_predicate(void *p_item,void *arg)
{
	struct handle *h=(struct handle*)p_item;
	int task_id=((int*)arg)[0];

	if(h->task_id==task_id)
	{
		if(h->close) h->close(h);
		return 1;
	}
	
	return 0;
}

int handle_close_task_handles(int task_id)
{
	one();
	ll_delete_p_1arg(&handles,free_task_handles_predicate,&task_id);
	all();
	return 0;
}

int handle_init()
{
	if(ll_init(&handles,sizeof(struct handle))==0) return -1;
	return 0;
}

void* handle_map(predicate f)
{
	return ll_map(&handles,f);
}

void* handle_map_1arg(predicate_1arg f,void *arg)
{
	return ll_map_1arg(&handles,f,arg);
}

static int handle_print_handles_callback(void *p_item)
{
	char itos_buf[ITOS_BSIZE];

	struct handle *h=(struct handle*)p_item;

	kwr("id:");
	kwr(itos_ud(h->id,itos_buf));
	kwr(" task_id:");
	kwr(itos_ud(h->task_id,itos_buf));
	kwr("\n");
	return 0;
}

int handle_print_handles()
{
	ll_map(&handles,handle_print_handles_callback);
	return 0;
}
