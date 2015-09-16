#ifndef _NEW_SYS_UTILS_H
#define _NEW_SYS_UTILS_H

#include <defs.h>

void die(char *msg);
void die_ud(unsigned int code);
void die_ux(unsigned int code);

int init_paging();

int kwr(char *msg);

void show_string(char *s);

unsigned int get_RAM_size();

unsigned int get_ESP();


short int gdt_selector(int index);

unsigned int gdt_seg_limit(void *descr);

void gdt_fill_tss_descr(void *tss_descriptor,
                        void *tss,short int tss_limit);

int write_string(int fl,char *s);
int read_line(int fl,char* prompt,char *buf,int bsize);

void crt_cursor_on();
void crt_cursor_off();

int set_timer_freq(unsigned int times_per_second);
int set_timer_counter(unsigned short int c);

int speaker_on(unsigned int times_per_second);
int speaker_on_counter(unsigned short int c);
int speaker_off();

int ints_are_on();

#endif
