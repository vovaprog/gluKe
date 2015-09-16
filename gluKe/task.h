#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include <linked_list.h>

struct TSS{
	short int prev_task;
	short int empty_space0;

	unsigned int ESP_CPL0;

	short int SS_CPL0;
	short int empty_space1;

	unsigned int ESP_CPL1;

	short int SS_CPL1;
	short int empty_space2;

	unsigned int ESP_CPL2;

	short int SS_CPL2;
	short int empty_space3;

	unsigned int CR3;
	unsigned int EIP;
	unsigned int EFLAGS;

	unsigned int EAX;
	unsigned int ECX;
	unsigned int EDX;
	unsigned int EBX;
	unsigned int ESP;
	unsigned int EBP;
	unsigned int ESI;
	unsigned int EDI;

	short int ES;
	short int empty_space4;

	short int CS;
	short int empty_space5;

	short int SS;
	short int empty_space6;

	short int DS;
	short int empty_space7;

	short int FS;
	short int empty_space8;

	short int GS;
	short int empty_space9;

	short int LDTR;
	short int empty_space10;

	short int task_flags;

	short int io_map;

/*	int my_used;*/

/* all above 108 bytes */

/*	char empty_space11[20]; */ // to 128 bytes

} __attribute__((packed));


struct task{
	struct TSS tss;

	void *gdt_tss_descr;
	short int selector;
	short int plan;
/*	unsigned int suspend_start;
	unsigned int suspend_msec;*/
	int id;
	unsigned int sleep_mksec;
};


typedef void (*task_entry)();

//returns id of first task
//or -1 if error occured
int task_init();

int task_newk(task_entry t_entry,int plan);

int task_switch(int task_id);

struct task* task_get_by_id(int task_id);

struct task* task_get_by_sel(unsigned short int selector);

int task_fill_idt_task_gate(int task_id,void *idt_descr);

int task_set_previous(struct task *target_task);

int task_cur_id();

/*pointer to first task struct */
extern struct task *first_task;
extern int first_task_id;
extern struct ll_info tasks;

int sleep_mksec(unsigned int mksec);

int task_set_planned(int task_id,int planned);

int task_kill(int task_id);

void task_kill_me();

int task_info_print_tasks();

#endif
