#include <defs.h>
#include <int.h>
#include <sys_utils.h>
#include <utils.h>
#include <rgn.h>
#include <task.h>
#include <plan.h>

#define INT_IDT_ENTRY_SIZE 8

/* (0x20+0x08+0x08) */
#define INT_IDT_NOF_ENTRIES 0x30

#define INT_HANDLER_SIZE 0x10

extern int IDT_info_main;
extern int int_handler_no_err;
extern int int_handler_with_err;

struct IDT_info{
	short int size;
	void *IDT;
} __attribute__((packed));

static int_handler_keyboard_fun_t handler_keyboard=0;

static int nof_mseconds=0;

static int off_count=0;

int int_inited=0;

int int_init()
{
	int i;
	unsigned short int *idt_entry;
	void *handler,*base;
	struct IDT_info *idt_info=(struct IDT_info*)&IDT_info_main;

	if((base=rgn_alloc(INT_IDT_ENTRY_SIZE*INT_IDT_NOF_ENTRIES+INT_HANDLER_SIZE*INT_IDT_NOF_ENTRIES))==0)
		return -1;

	idt_info->IDT=base;
	handler=(char*)base+INT_IDT_ENTRY_SIZE*INT_IDT_NOF_ENTRIES;

	idt_info->size=INT_IDT_NOF_ENTRIES*INT_IDT_ENTRY_SIZE-1;

	for(i=0;i<INT_IDT_NOF_ENTRIES;
			i++,handler+=INT_HANDLER_SIZE)
	{
		idt_entry=(unsigned short int*)(idt_info->IDT+(INT_IDT_ENTRY_SIZE*i));

		idt_entry[0]=LWORD(handler);
		idt_entry[3]=HWORD(handler);
		idt_entry[1]=K_CS_SEL;
		idt_entry[2]=0x8e00; /* 8 - segment is present. 0xe - interrupt gate */

		if(i<=7 || i==15 || i==16 || i>=18)
		{
			kmemcpy(handler,&int_handler_no_err,INT_HANDLER_SIZE);
			((char*)handler)[4]=(char)i;
		}
		else
		{
			kmemcpy(handler,&int_handler_with_err,INT_HANDLER_SIZE);
			((char*)handler)[2]=(char)i;
		}
	}

	asm("call init_PIC   \n\
	     lidt %0         \n\
	     sti             "::"m" (IDT_info_main):"eax");

	int_inited=1;

	return 0;
}

#define EOI_MASTER asm("push %eax       \n\
                        mov $0x20,%al   \n\
                        out %al,$0x20   \n\
                        pop %eax");

#define EOI_SLAVE  asm("push %eax       \n\
                        mov $0x20,%al   \n\
                        out %al,$0xA1   \n\
                        pop %eax");

#define PROCESSOR_EXCEPTION(x) ((x)<0x20)
#define HARDWARE_INTERRUPT(x) ((x)>=32 && (x)<48)
#define HARDWARE_INTERRUPT_MASTER(x) ((x)>=32 && (x)<40)
#define HARDWARE_INTERRUPT_SLAVE(x)  ((x)>=40 && (x)<48)

#define INT_EXPT_INVALID_OPCODE     0x06
#define INT_EXPT_INVALID_TSS        0x0A
#define INT_EXPT_PAGE_FAULT         0x0E
#define INT_EXPT_GENERAL_PROTECTION 0x0D

#define INT_TIMER           0x20
#define INT_KEYBOARD        0x21
#define INT_TIMER2          0x28


static void int_complete(int number,int argument)
{
	if(HARDWARE_INTERRUPT_MASTER(number)){EOI_MASTER;}
	else if(HARDWARE_INTERRUPT_SLAVE(number)){EOI_SLAVE;}
	else if(!PROCESSOR_EXCEPTION(number)) die("unknown interrupt");
}

int get_scan_code();

void int_handle(unsigned int number,int argument)
{
	if(PROCESSOR_EXCEPTION(number))
	{
		switch(number){
		case INT_EXPT_PAGE_FAULT:
			die("page fault");			
		case INT_EXPT_INVALID_OPCODE:
			die("invalid opcode");
		case INT_EXPT_INVALID_TSS:
			die("invalid TSS");
		case INT_EXPT_GENERAL_PROTECTION:
			die("general protection exception");
		default:
			die("unhandled processor exception");
		}
	}
	else if(HARDWARE_INTERRUPT(number))
	{
		switch(number){
		case INT_TIMER:
			nof_mseconds+=55;
			break;
		case INT_KEYBOARD:
			if(handler_keyboard)
			{
				int scan_code=get_scan_code();
				handler_keyboard(scan_code);
			}
			break;
		case INT_TIMER2:
			break;
		default:
			die("unhandled hardware interrupt");
		}
		int_complete(number,argument);
	}
	else die("unknown interrupt");
}

static int_timer_task_callback timer_task_callback=0;

unsigned int mseconds()
{
	return nof_mseconds;
}

void int_handle_timer()
{
	//it is not good: cli must be the first instruction of handler
	asm("cli");
	off_count++;

	while(1)
	{
		nof_mseconds+=55;

		if(timer_task_callback) timer_task_callback();

		EOI_MASTER;

		off_count--;
		asm("sti\n\
		     iret\n\
		     cli");
		off_count++;
	}
}

int int_init_timer_task(int_timer_task_callback f)
{
	struct IDT_info *idt_info=(struct IDT_info*)&IDT_info_main;
	void *idt_descr=&(((char*)idt_info->IDT)[8*0x20]);
	int task_id;

	one();

	if((task_id=task_newk(int_handle_timer,0))<0) return -1;

	task_fill_idt_task_gate(task_id,idt_descr);

	timer_task_callback=f;

	all();

	return task_id;
}

int int_set_handler_keyboard(int_handler_keyboard_fun_t handler)
{
	handler_keyboard=handler;
	return 0;
}

void int_remove_handler_keyboard()
{
	handler_keyboard=0;
}

//---------------------------------------

int int_off()
{
	if(!int_inited) return -1;

	asm("cli");
	off_count++;

	return 0;
}

int int_off_noinc()
{
	if(!int_inited) return -1;

	asm("cli");

	return 0;
}

int int_on()
{
	if(!int_inited) return -1;

	off_count--;

	if(off_count<0) off_count=0;

	if(off_count==0) asm("sti");

	return 0;
}

void int_on_iret()
{
	if(!int_inited) asm("iret");

	off_count--;

	if(off_count<0) off_count=0;

	if(off_count==0)
		asm("sti\niret");
	else
		asm("iret");
}
