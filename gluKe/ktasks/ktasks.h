#ifndef KTASKS_H_INCLUDED
#define KTASKS_H_INCLUDED

#include <defs.h>


void work();


#define KTASK_COUNTER_DESCR "\
task opens terminal, increases counter and prints it\n"

void counter();


#define KTASK_REVERSE_DESCR "\
task opens terminal, reads input\n\
and prints it from tail to head\n"

void reverse();


#define KTASK_CPUID_DESCR "\
task opens terminal and outputs information returned by\n\
cpuid processor command\n"

void ktentry_cpuid();

#define KTASK_LIFE_DESCR "\
implementation of conway's life alorithm\n"

void ktentry_life();

#endif

