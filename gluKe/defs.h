#ifndef DEFS_INCLUDED
#define DEFS_INCLUDED

//code that's used not only in kernel should
//check this define to know, if it is compiled as part of kernel.
#ifndef GLUKE
#define GLUKE
#endif

//---------------------------------------------------------------
// set values below to needed values

/* GDT is inited at this address in entry16 and is NOT relocated later */
#define GDT_BASE_SEGMENT  0x9800
#define GDT_BASE         0x98000

#define ENTRY16_BASE 0x30000

#define PAGE_SIZE    0x1000

/*
VGAROM_BIOS 0xC0000-0xC7FFF
ROM_BIOS 0xF0000-0xFFFFF

0xA0000-0xB8FFF should be also reserved

But i reserve this region:
upper bound is 0x1fffff, because
on some computers (my old pentium) memory check
starting lower fails.
*/
#define REG0_BASE 0xA0000
#define REG0_END  0x1FFFFF
#define REG0_SIZE ((REG0_END)-(REG0_BASE))

#define ALS_FREE_BASE 0x230000
#define ALS_RESERVED_BASE 0x250000

#define KTASK_DEFAULT_STACK_SIZE 0x10000

#define TIMER_FREQUENCY 200

#define MAX_NAME_LENGTH 0x20
#define MUTEX_SLEEP_MKSEC 10000

/* maximum number of allocations for rgn functions */
#define MAX_NOF_ALS       300

//---------------------------------------------------------------

#define GDT_MAX_DESCRS           40
#define GDT_DESCR_SIZE            8
#define ENTRY16_GDT_NOF_DESCRS    3 
#define GDT_SIZE_BYTES ((GDT_MAX_DESCRS)*(GDT_DESCR_SIZE))

//---------------------------------------------------------------

#define ENTRY16_SIZE 0x100

//KERNEL_BASE define's and KERNEL_SIZE var's values
//are base and size of entry32 only!
#define KERNEL_BASE ((ENTRY16_BASE)+(ENTRY16_SIZE))

//KERNEL_SIZE goes first in entry32.s's variables,
//so we count its size and size of next variable.
#define KERNEL_CRC_BASE ((char*)&KERNEL_SIZE+sizeof(int)+sizeof(int))

//there 5 bytes of code before KERNEL_SIZE var.
//if code there is chaged - change next line
#define KERNEL_CRC_BYTES (KERNEL_SIZE-5-sizeof(int)-sizeof(int))

//---------------------------------------------------------------

//GDT selectors
#define K_CS_SEL 0x08
#define K_DS_SEL 0x10
#define K_SS_SEL 0x10

//---------------------------------------------------------------
//vidio memory defines

#define VIDMEM_LINES   25
#define VIDMEM_COLUMNS 80

#define VIDMEM_BASE 0xB8000
#define VIDMEM_BYTES_PER_SYMBOL 2
#define VIDMEM_END  ((VIDMEM_BASE)+VIDMEM_COLUMNS*VIDMEM_LINES*VIDMEM_BYTES_PER_SYMBOL)
#define VIDMEM_SIZE ((VIDMEM_END)-(VIDMEM_BASE))

#define VIDMEM_BYTES_PER_LINE (80*VIDMEM_BYTES_PER_SYMBOL)

#define VIDMEM ((char*)(VIDMEM_BASE))

//---------------------------------------------------------------

#ifndef ASSEMBLER

#define TO_PAGE_SIZE(x) ((unsigned int)(x) % PAGE_SIZE==0? \
                        (x):(unsigned int)(x) + PAGE_SIZE - (unsigned int)(x) % PAGE_SIZE)

#define NOF_PAGES(x) (TO_PAGE_SIZE(x)/PAGE_SIZE)


extern struct driver_descr drivers[];

/*#ifndef ENTRY32_C
extern void *GDT;
#endif*/

extern unsigned int *k_page_cat;

extern int kfile;

extern unsigned int KERNEL_SIZE;
extern unsigned int KERNEL_CRC;

extern unsigned int RAM_SIZE;

struct file{
	int driver_id;
	void *data;
	int idata;
};

struct mutex{
	int lock_count;
	int usage_count;
	int owner_task_id;
	char name[MAX_NAME_LENGTH+1];
};

struct handle{
	int id;
	int type;
	int task_id;
	struct file fl;
	struct mutex mtx;
	int (*close)(struct handle *h);
};

typedef int (*driver_init_function)(struct driver_descr *d);

struct driver_descr{
	driver_init_function init_function;
	int (*open)(struct file *fl,char *s);
	int (*read)(struct file *fl,char *buf,int bytes);
	int (*write)(struct file *fl,char *buf,int bytes);
	int (*ioctl)(struct file *fl,int cmd,void *arg);
	int (*close)(struct file *fl);
	char *name;
};

extern unsigned int timer_cur_frequency;
extern unsigned int timer_cur_mksec_per_tick;

#endif   /* #ifndef ASSEMBLER */

#endif

