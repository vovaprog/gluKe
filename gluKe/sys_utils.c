#define SYS_UTILS_C

#include <glinc.h>

#include <term.h>

extern unsigned int RAM_SIZE;

extern int kfile;

void show_string(char *s)
{
	int i=0;

	kmemset4(VIDMEM,0x0F000F00,VIDMEM_SIZE/4);

	while((VIDMEM[i*2]=s[i])) i++;
}

void die(char *msg)
{
	one();

	(msg && (show_string(msg),1)) || (show_string("kernel died"),1);

	while(1);
}

void die_ud(unsigned int code)
{
	char itos_buf[ITOS_BSIZE];			
	die(itos_ud(code,itos_buf));
}

void die_ux(unsigned int code)
{
	char itos_buf[ITOS_BSIZE];			
	die(itos_ux(code,itos_buf));
}


unsigned int *k_page_cat; //page catalog pointer

int init_paging()
{
#define PAGES_PER_TABLE 1024

/* 2:access-from-0 1:read-write 0:present */
#define PG_POINTER_GEN 0x03

	unsigned int *pg_table,                  //page table pointer
	                pg=0,                       //page pointer
	                i,j,total_pages,nof_tables;

	total_pages=RAM_SIZE / PAGE_SIZE;
	nof_tables=total_pages / PAGES_PER_TABLE;

	if((k_page_cat=rgn_alloc(0x1000))==0) return -1;



	for(i=0;i<PAGES_PER_TABLE;i++)
	{
		if(i<nof_tables)
		{
			if((pg_table=rgn_alloc(0x1000))==0) return -1;



			k_page_cat[i]=(PG_POINTER_GEN | (unsigned int)pg_table);

			for(j=0;j<PAGES_PER_TABLE;j++)
			{
				if(j<total_pages)
				{
					pg_table[j]=(PG_POINTER_GEN | (unsigned int)pg);
					pg+=PAGE_SIZE;
				}
				else pg_table[j]=0;
			}
			total_pages-=PAGES_PER_TABLE;
		}
		else k_page_cat[i]=0;
	}

	//die(itos_ux((int)pg_table,itos_buf));

	/* switch to page memory */
	asm("mov %0,%%eax            \n\
        mov %%eax,%%cr3         \n\
        mov %%cr0,%%eax         \n\
        or  $0x80000000,%%eax   \n\
        mov %%eax,%%cr0           "::"m" (k_page_cat));

	return 0;
}

int kwr(char *msg)
{
	return write(kfile,msg,kstrlen(msg));
}

int write_string(int fl,char *s)
{
	return write(fl,s,kstrlen(s));
}

int read_line(int fl,char* prompt,char *buf,int bsize)
{
	if(prompt) write_string(fl,prompt);

	ioctl(fl,TERM_IOCTL_CLEAR_INPUT,(void*)0);

	ioctl(fl,TERM_IOCTL_READ_BLOCK,(void*)1);

	ioctl(fl,TERM_IOCTL_READ_LINE,(void*)1);

	return read(fl,buf,bsize);
}

/* do not use when paging is on!!! */
unsigned int get_RAM_size()
{
#define get_RAM_size_TEST_VAL 0x11223344
	volatile unsigned int *p=(unsigned int*)0x200000;//starting mem check from this address.
	unsigned int temp;

	while(1)
	{
		temp=p[0];
		p[0]=get_RAM_size_TEST_VAL;
		if(p[0]!=get_RAM_size_TEST_VAL) return (unsigned int)p;
		p[0]=temp;
		p=(unsigned int*)((unsigned int)p+PAGE_SIZE);
	}
}

void gdt_fill_tss_descr(void *tss_descriptor,
                        void *tss,short int tss_limit)
{
	short int *p_sh_int=(short int*)tss_descriptor;
	char *p_char=(char*)tss_descriptor;

	p_sh_int[0]=tss_limit;

	p_sh_int[1]=LWORD(tss);

	p_char[4]=((char*)&tss)[2];
	p_char[7]=((char*)&tss)[3];

	p_char[5]=0x89;
	p_char[6]=0x40; /* 32 dig segment */
}

unsigned int gdt_seg_limit(void *descr)
{
	unsigned int s;

	s=((unsigned short int*)descr)[0];
	s+=((unsigned int)((unsigned char*)descr)[6] & 0x0f) * 0x10000;

	if(((char*)descr)[6] & 0x80) s*=0x1000;

	return s;
}

short int gdt_selector(int index)
{
	return (short int)(index<<=3);
}

void crt_cursor_off()
{
	asm("\
		mov $0x03d4,%dx   \n\
		mov $0x0a,%al     \n\
		out %al,%dx       \n\
		                  \n\
		mov $0x3d5,%dx    \n\
		in  %dx,%al       \n\
		or  $0x20,%al     \n\
		out %al,%dx");
}

void crt_cursor_on()
{
	asm("\
		mov $0x03d4,%dx   \n\
		mov $0x0a,%al     \n\
		out %al,%dx       \n\
		                  \n\
		mov $0x3d5,%dx    \n\
		in  %dx,%al       \n\
		and $0xdf,%al     \n\
		out %al,%dx");
}

int set_timer_counter(unsigned short int c)
{
	char low_byte,high_byte;
	
	low_byte=(c & 0x00ff);
	high_byte=(c >> 8);

	asm("mov $0x30,%%al \n\
	     out %%al,$43         \n\
	     mov %0,%%al          \n\
	     out %%al,$0x40       \n\
	     mov %0,%%al          \n\
	     out %%al,$0x40"::"m" (low_byte),"m" (high_byte):"eax");
	
	return 0;
}

int set_timer_freq(unsigned int times_per_second)
{
	unsigned int c;
	
	c=1193180/times_per_second;
	
	if(c==0 || c>0xffff) return -1;
	
	timer_cur_frequency=times_per_second;
	timer_cur_mksec_per_tick=1000000/timer_cur_frequency;
	
	return set_timer_counter((unsigned short int)c);
}

int speaker_on_counter(unsigned short int c)
{
	char low_byte,high_byte;
	
	low_byte=(c & 0x00ff);
	high_byte=(c >> 8);

	asm("mov $0xb6,%%al \n\
	out %%al,$0x43 \n\
	mov %0,%%al \n\
	out %%al,$0x42 \n\
	mov %1,%%al \n\
	out %%al,$0x42 \n\
	in $0x61,%%al \n\
	or $0x03,%%al \n\
	out %%al,$0x61"::"m" (low_byte),"m" (high_byte):"eax");
	
	return 0;
}

int speaker_on(unsigned int times_per_second)
{
	unsigned int c;
	
	c=1193180/times_per_second;
	
	if(c==0 || c>0xffff) return -1;

	return speaker_on_counter((unsigned short int)c);
}

int speaker_off()
{
	asm("in $0x61,%al \n\
	and $0xfc,%al \n\
	out %al,$0x61");
	
	return 0;
}

int ints_are_on()
{
	unsigned int flags;

	asm("pushf	\n\
		mov (%%esp),%%eax \n\
		mov %%eax,%0 \n\
		pop %%eax":"=m" (flags)::"eax");

	if(flags & 0x00000200) return 1;
	return 0;
}

