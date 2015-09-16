#ifndef MTOK_INCLUDED
#define MTOK_INCLUDED

#include <defs.h>

#include <my_err.h>

/*
Returned values:

Success:
0            - success.
MTOK_END     - no more tokens in this line.

Errors:
E_BAD_ARGS
E_BUF_2SMALL - buf is too small to store token. 
               buf_size is set to needed buf size, cur_pos is untouched, 
               so you can call mtok again with larger buf.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define MTOK_END 1

int mtok(char *input,unsigned int *cur_pos,char *buf,unsigned int *buf_size,char *usep,char *jsep);

#ifdef __cplusplus
}
#endif

#endif

