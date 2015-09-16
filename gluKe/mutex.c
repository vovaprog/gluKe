#include <glinc.h>

static int st_mutex_by_hid_callback(void *p_item,void *p_hid)
{
	struct handle *h=(struct handle*)p_item;
	if(h->id==*(int*)p_hid)
	{
		if(h->type==HANDLE_TYPE_MUTEX) return 1;
		else return -1;
	}
	return 0;
}

static struct mutex* st_mutex_by_hid(int hid)
{
	struct handle* h=handle_map_1arg(st_mutex_by_hid_callback,&hid);	
	if(h) return &(h->mtx);
	return 0;
}

static int st_handle_by_mutex_name_callback(void *p_item,void *name)
{
	struct handle *h=(struct handle*)p_item;

	if(h->type==HANDLE_TYPE_MUTEX)
		if(kstrcmp(name,h->mtx.name)==0) return 1;
	return 0;
}

static struct handle* st_handle_by_mutex_name(char *name)
{
	return handle_map_1arg(st_handle_by_mutex_name_callback,name);
}

int mutex_open(char *name)
{
	struct handle *h=0;
	
	if(name) 
	{
		if(kstrlen(name)>MAX_NAME_LENGTH) return -1;		
		h=st_handle_by_mutex_name(name);
	}

	if(h==0)
	{
		if((h=handle_open())==0) return -1;
		h->mtx.usage_count=0;

		if(name) kstrcpy(h->mtx.name,name);
		else h->mtx.name[0]=0;
	}
	
	h->type=HANDLE_TYPE_MUTEX;
	
	h->mtx.lock_count=0;
	
	h->mtx.usage_count++;
	
	return h->id;
}

static int st_mutex_lock(int hid,int block)
{
	struct mutex *mtx;
	int cur_tid;	
	
	if((mtx=st_mutex_by_hid(hid))==0) return -1;
	
	cur_tid=task_cur_id();
	
	if(mtx->owner_task_id==cur_tid)
	{
		mtx->lock_count++;
		return 0;
	}
	
	while(1)
	{
		one();
		if(mtx->lock_count==0)
		{
			mtx->lock_count=1;
			mtx->owner_task_id=cur_tid;
			all();
			return 0;
		}		
		all();
		
		if(!block) return -1;
		
		sleep_mksec(MUTEX_SLEEP_MKSEC);
	}
}

int mutex_lock(int hid)
{
	return st_mutex_lock(hid,1);
}
	  
int mutex_lock_nb(int hid)
{
	return st_mutex_lock(hid,0);
}
	  
int mutex_release(int hid)
{
	struct mutex* mtx;
	
	if((mtx=st_mutex_by_hid(hid))==0) return -1;
	
	one();
		
	if(mtx->owner_task_id!=task_cur_id())
	{
		all();return -1;
	}
	
	if(mtx->lock_count>0) mtx->lock_count--;
	
	all();
	
	return 0;
}

int mutex_close(int hid)
{	
	struct mutex *mtx;
	
	if((mtx=st_mutex_by_hid(hid))==0) return -1;
	
	one();
	
	if(--mtx->usage_count==0) handle_remove_id(hid);

	all();
	
	return 0;
}

