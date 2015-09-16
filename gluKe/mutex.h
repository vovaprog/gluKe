#ifndef MUTEX_H_INCLUDED
#define MUTEX_H_INCLUDED

int mutex_open(char *name);

int mutex_lock(int hid);

int mutex_lock_nb(int hid);

int mutex_release(int hid);

int mutex_close(int hid);

#endif

