#ifndef PREDICATE_H_INCLUDED
#define PREDICATE_H_INCLUDED

/*#include <defs.h>*/

typedef int (*predicate_1arg)(void *p_item,void *arg);

typedef int (*predicate_2args)(void *p_item,void *arg0,void *arg1);

typedef int (*predicate)(void *p_item);

int P_true(void *p_item);

int P_true_1arg(void *p_item,void *arg);

#endif

