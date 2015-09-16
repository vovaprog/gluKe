#ifndef INT_INCLUDED
#define INT_INCLUDED


int int_init();


typedef void (*int_timer_task_callback)();

int int_init_timer_task(int_timer_task_callback f);


typedef void (*int_handler_keyboard_fun_t)(int scan_code);

int int_set_handler_keyboard(int_handler_keyboard_fun_t f);

void int_remove_handler_keyboard();

//int_off_noinc/one_noinc disable interrupts, but do not increase lock count.

#define one() int_off()
#define one_noinc() int_off_noinc()
#define all() int_on()

int int_off();
int int_off_noinc();

int int_on();
void int_on_iret();

unsigned int mseconds();

#endif

