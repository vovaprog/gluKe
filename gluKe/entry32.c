#define ENTRY32_C

#include <glinc.h>

#include <crc.h>

struct driver_descr drivers[]=
{
	{term_init_driver},
	{0}
};

struct{
	unsigned int base;
	unsigned int bytes;
} reserved_regions[]={
	{REG0_BASE,REG0_SIZE},
	{GDT_BASE,GDT_SIZE_BYTES},
	{0,0}
};

struct{
	short int size_bytes;
	struct gdt_descr *GDT;
} __attribute__((packed)) gdt_info;

unsigned int RAM_SIZE;

int kfile;

void *GDT;

/* without next line kernel is linked incorrect. Why? */
char *__needed_for_linker__="\0";

static int drv_init()
{
	int i;

	for(i=0;drivers[i].init_function;i++)
		if(drivers[i].init_function(drivers+i)) return -1;

	return 0;
}

static int init_GDT()
{
	GDT=(void*)GDT_BASE;

	gdt_info.GDT=GDT;
	gdt_info.size_bytes=GDT_SIZE_BYTES;

	asm("lgdt gdt_info");

	return 0;
}

int alloc_memory_not_in_use()
{
	int i;

	for(i=0;reserved_regions[i].bytes;i++)
		if(rgn_alloc_base((void*)reserved_regions[i].base,reserved_regions[i].bytes)==0)
			return -1;

	/* alloc kernel code */
	if(rgn_alloc_base((void*)ENTRY16_BASE,ENTRY16_SIZE+KERNEL_SIZE)==0)
		return -1;

	/* alloc kernel stack */
	if(rgn_alloc_base((void*)(ENTRY16_BASE-KTASK_DEFAULT_STACK_SIZE),KTASK_DEFAULT_STACK_SIZE)==0)
		return -1;

	/* alloc 0 address for checks */
	rgn_alloc_base(0,1);

	return 0;
}

void entry32()
{
	int i,first_task_id;
	char itos_buf[ITOS_BSIZE];

	/* kernel crc is counted starting with byte
		after KERNEL_SIZE and KERNEL_CRC.
		don't forget to change next line and crc utility, if you
		want to add some variables there.
		first instruction in entry32.s calls this function,
		so kernel image in memory will not be changed and
		nothing will affect computation of crc. */
	if((i=crc_sum(KERNEL_CRC_BASE,KERNEL_CRC_BYTES))!=KERNEL_CRC)
		die("kernel code loaded incorrectly");

	RAM_SIZE=get_RAM_size();

	if(rgn_init()) die("'rgn_init' failed");

	if(alloc_memory_not_in_use()) die("can't allocate memory");

	if(init_GDT()) die("'init_GDT' failed");

	if(init_paging()) die("'init_paging' failed");

	if(set_timer_freq(TIMER_FREQUENCY)) die("'set_timer_freq' failed");

	if(int_init()) die("'int_init' failed");

	if((first_task_id=task_init())<0) die("'task_init' failed");

	if(handle_init()) die("'file_init' failed");

	rgn_print_info();

	if(drv_init()) die("'drv_init' failed");

	if((kfile=open("terminal"))<0) die("can't create kernel terminal");

	kwr("protected mode on\npaging on\ninterrupts on\nRAM size: ");
	kwr(itos_ud(RAM_SIZE,itos_buf));kwr("\n");

	if(plan_init()) die("'plan_init' failed");

	kwr("kernel init complete.\n");

	if(task_newk(work,1)<0) die("'task_newk' failed");

	//set first task not planned
	task_set_planned(first_task_id,0);

stop:
	goto stop;
}

