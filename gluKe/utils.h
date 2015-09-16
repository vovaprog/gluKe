#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <defs.h>

#define LWORD(x) ((unsigned int)(x) & 0x0000ffff)
#define HWORD(x) ((unsigned int)(x) >> 16)

void kmemset(void *p,char fill,unsigned int nof_bytes);

void kmemset4(void *p,int fill,unsigned int nof_bytes);

int kstrlen(char *addr);

int kstrcmp(char *s0,char *s1);

void kmemcpy(void *dest,void *src,unsigned int nof_bytes);

void kstrcpy(char *dest,char *src);

char* kstrcat(char *dest,char *src);

void phex(unsigned char *s,int nof_bytes);

//buf must be at last ITOS_BSIZE bytes long
#define ITOS_BSIZE 30

char* itos_ud(unsigned int x,char *buf);
char* itos_ux(unsigned int u,char *buf);
char* itos_ux_nobase_width(unsigned int x,int width,char *buf);

int stois(char *snum,int *result);

int stoi(char *snum,unsigned int *result);

int ksprintf(char *s,char *format,...);

int kfprintf(int fl,char *format,...);

#endif

