#ifndef _FILE_H
#define _FILE_H

#include <defs.h>

int fut_init();

int open(char *arg);
int read(int fid,char *buf,int bytes);
int write(int fid,char *buf,int bytes);
int ioctl(int fid,int cmd,void *arg);
int close(int fid);

#endif

