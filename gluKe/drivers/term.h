#ifndef _TERM_H
#define _TERM_H

#include <defs.h>

#define TERM_IOCTL_CLEAR_INPUT      1
#define TERM_IOCTL_READ_BLOCK       2
#define TERM_IOCTL_READ_LINE        3

#define TERM_IOCTL_OMODE_TEXT       4
#define TERM_IOCTL_OMODE_PIXEL      5

#define TERM_IOCTL_SHOW_ON          6
#define TERM_IOCTL_SHOW_OFF         7
#define TERM_IOCTL_SHOW             8

#define TERM_IOCTL_MODE_TERMINAL    9
#define TERM_IOCTL_MODE_PICTURE    10

int term_init_driver(struct driver_descr *d);

#endif

