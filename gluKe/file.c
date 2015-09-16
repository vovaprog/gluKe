#include <glinc.h>

#include <file.h>

int close_handle_struct_callback(struct handle *h)
{
	return drivers[h->fl.driver_id].close(&(h->fl));
}

int open(char *arg)
{
	int i;
	char *s=0;
	struct handle *h;

	for(i=0;arg[i]!='\0' && arg[i]!='/';i++);

	if(arg[i]!='\0')
	{
		arg[i]=0;
		s=arg+i+1;
	}

	for(i=0;drivers[i].init_function;i++)
	{
		if(kstrcmp(drivers[i].name,arg)==0)
		{
			one();
			
			h=handle_open();
			
			h->fl.driver_id=i;
			
			if(drivers[i].open(&(h->fl),s))
			{
				handle_remove(h);
				all();
				return -1;
			}
	
			h->close=close_handle_struct_callback;

			h->type=HANDLE_TYPE_FILE;
	
			all();

			return h->id;
		}
	}
	return -1;
}

static struct file* file_by_handle_id(int hid)
{
   struct handle *h;
	
	if((h=handle_by_id(hid))==0) return 0;
	
	return &(h->fl);
}

int read(int hid,char *buf,int bytes)
{
	struct file* fl;
	
	if((fl=file_by_handle_id(hid))==0) return -1;

	if(drivers[fl->driver_id].read)
		return drivers[fl->driver_id].read(fl,buf,bytes);
	else
		return -1;
}

int write(int fid,char *buf,int bytes)
{
	struct file *fl;
	
	if((fl=file_by_handle_id(fid))==0) return -1;

	if(drivers[fl->driver_id].write)
		return drivers[fl->driver_id].write(fl,buf,bytes);
	else
		return -1;
}

int ioctl(int hid,int cmd,void *arg)
{
	struct file *fl;
	
	if((fl=file_by_handle_id(hid))==0) return -1;

	if(drivers[fl->driver_id].ioctl)
		return drivers[fl->driver_id].ioctl(fl,cmd,arg);
	else
		return -1;
}

int close(int hid)
{
	one();

	struct file *fl;
	
	if((fl=file_by_handle_id(hid))==0) return -1;

	if(drivers[fl->driver_id].close) drivers[fl->driver_id].close(fl);

	handle_remove_id(hid);

	all();

	return 0;
}
