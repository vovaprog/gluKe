#include <glinc.h>

#include <term.h>

#define TERM_OUTPUT_BSIZE VIDMEM_SIZE
#define TERM_INPUT_BSIZE 100

struct term_data{
	int osize;
	int iend;
	int output_mode;
	int show;
	char input[TERM_INPUT_BSIZE];
	char output[TERM_OUTPUT_BSIZE];
	char used;
	char read_block;
	char read_line;
};

#define MAX_NOF_TERMS 10

static struct term_data *terms;

static int active_term_index=-1;

char *scan_code_map="\
\0\0001234567890-=\010\t\
qwertyuiop[]\n\0\
asdfghjkl;'`\0\\\
zxcvbnm,./\0*\0 \0\
\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0-\0005\0+";

char *scan_code_map_shift="\
\0\0!@#$%^&*()_+\010\t\
QWERTYUIOP{}\n\0\
ASDFGHJKL:\"~\0\\\
ZXCVBNM<>?\0*\0 \0\
\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0-\05\0+";

static int to_screen(char *buf,int bsize)
{
	kmemcpy(VIDMEM,buf,VIDMEM_SIZE);

	//move cursor to end of output
	asm("mov $0x0e,%%al         \n\
        mov $0x03d4,%%dx       \n\
        out %%al,%%dx          \n\
                               \n\
                               \n\
        mov %0,%%eax           \n\
        shr $0x01,%%eax        \n\
        and $0x0000ff00,%%eax  \n\
        shr $0x08,%%eax        \n\
                               \n\
        mov $0x03d5,%%dx       \n\
        out %%al,%%dx          \n\
                               \n\
                               \n\
        mov $0x0f,%%al         \n\
        mov $0x03d4,%%dx       \n\
        out %%al,%%dx          \n\
                               \n\
                               \n\
        mov %0,%%eax           \n\
        shr $0x01,%%eax        \n\
        and $0x000000ff,%%eax  \n\
                               \n\
        mov $0x03d5,%%dx       \n\
        out %%al,%%dx"::"m" (bsize):"eax","ebx","edx","ecx");

	return 0;
}

static int st_clear_term(struct term_data* term)
{	
	kmemset4(term->output,0x0F000F00,TERM_OUTPUT_BSIZE/4);
	term->osize=0;
	term->iend=0;
	return 0;
}

static int term_open(struct file *fl,char *unused)
{
	unsigned int i;

	one();

	for(i=0;i<MAX_NOF_TERMS;i++)
	{
		if(!terms[i].used)
		{
			terms[i].used=1;
			break;
		}
	}

	all();

	if(i==MAX_NOF_TERMS) return -1;

	terms[i].read_block=1;

	terms[i].iend=0;
	terms[i].osize=0;

	st_clear_term(&(terms[i]));

	terms[i].output_mode=TERM_IOCTL_OMODE_TEXT;
	terms[i].show=1;

	fl->idata=i;

	if(active_term_index<0) active_term_index=i;

	return 0;
}

static void append_to_buffer(char *buf,int *osize,char *tail,unsigned int bytes)
{
	unsigned int i;

	for(i=0;i<bytes;i++)
	{
		if(tail[i]=='\n')
		{
			*osize+=VIDMEM_BYTES_PER_LINE;
			*osize-=*osize % VIDMEM_BYTES_PER_LINE;
		}
		else if(tail[i]=='\r')
			*osize-=*osize % VIDMEM_BYTES_PER_LINE;
		else if(tail[i]=='\b')
		{
			if(*osize>0){*osize-=2;buf[*osize]=' ';}
		}
		else
		{
			buf[*osize]=tail[i];
			*osize+=2;
		}

		if(*osize>=VIDMEM_SIZE)
		{
			kmemcpy(buf,
				buf+VIDMEM_BYTES_PER_LINE,
				VIDMEM_SIZE-VIDMEM_BYTES_PER_LINE);

			kmemset4(buf+VIDMEM_SIZE-VIDMEM_BYTES_PER_LINE,
				0x0F000F00,VIDMEM_BYTES_PER_LINE/4);

			*osize-=VIDMEM_BYTES_PER_LINE;
		}
	}
}

//----------------------------------------------------------------------------------------------------------

static void st_show_text(struct term_data *td)
{
	int temp_bsize;
	char output[TERM_OUTPUT_BSIZE];

	kmemcpy(output,td->output,TERM_OUTPUT_BSIZE);

	temp_bsize=td->osize;

	append_to_buffer(output,&temp_bsize,td->input,td->iend);	

	to_screen(output,temp_bsize);
}

static void st_show_pixel(struct term_data *td)
{
	one();
	to_screen(td->output,0);
	all();
}

static void st_show(struct term_data *td)
{
	if(td->show)
	{
		if(td->output_mode==TERM_IOCTL_OMODE_PIXEL) st_show_pixel(td);
		else st_show_text(td);
	}
}

static void st_show_no_check(struct term_data *td)
{
	if(td->output_mode==TERM_IOCTL_OMODE_PIXEL) st_show_pixel(td);
	else st_show_text(td);	
}

//----------------------------------------------------------------------------------------------------------

static int st_text_add_to_output(struct file *fl,char *buf,int bytes)
{
	struct term_data *td=&(terms[fl->idata]);

	append_to_buffer(td->output,&(td->osize),buf,bytes);

	if(fl->idata==active_term_index) 
	{
		one();
		st_show(td);
		all();		
	}
	
	return 0;
}

static int st_pixel_add_to_output(struct file *fl,char *buf,int bytes)
{
	struct term_data *td;
	char x,y,symbol,color;

	if(bytes!=4) return -1;

	if(fl->idata>MAX_NOF_TERMS || terms[fl->idata].used==0) return -1;

	one();

	td=terms+fl->idata;

	x=buf[0];y=buf[1];symbol=buf[2];color=buf[3];

	if(y>=25 || x>=80) return -1;

	td->output[y*80*2+x*2]=symbol;
	td->output[y*80*2+x*2+1]=color;
	
	if(fl->idata==active_term_index) st_show(td);
		
	all();
		
	return 0;
}

static int st_add_to_output(struct file *fl,char *buf,int bytes)
{
	struct term_data *td;	
	td=terms+fl->idata;
	
	if(td->output_mode==TERM_IOCTL_OMODE_PIXEL) st_pixel_add_to_output(fl,buf,bytes);
	else st_text_add_to_output(fl,buf,bytes);
		
	return 0;
}

//----------------------------------------------------------------------------------------------------------

static int term_ioctl(struct file *fl,int cmd,void *arg)
{
	struct term_data *td;

	if(fl->idata<0 || fl->idata>=MAX_NOF_TERMS || terms[fl->idata].used==0) return -1;
	td=terms+fl->idata;

	switch(cmd){
	case TERM_IOCTL_CLEAR_INPUT:
		td->iend=0;
		break;
	case TERM_IOCTL_READ_BLOCK:
		td->read_block=arg?1:0;
		break;
	case TERM_IOCTL_READ_LINE:
		td->read_line=arg?1:0;
		break;
	case TERM_IOCTL_OMODE_PIXEL:
		td->output_mode=TERM_IOCTL_OMODE_PIXEL;
		crt_cursor_off();
		st_clear_term(td);
		break;
	case TERM_IOCTL_OMODE_TEXT:
		td->output_mode=TERM_IOCTL_OMODE_TEXT;
		crt_cursor_on();
		st_clear_term(td);	
		break;
	case TERM_IOCTL_SHOW_OFF:
		td->show=0;
		break;
	case TERM_IOCTL_SHOW_ON:
		td->show=1;
		break;
	case TERM_IOCTL_SHOW:
		if(fl->idata==active_term_index) st_show_no_check(terms+fl->idata);
		break;
	case TERM_IOCTL_MODE_TERMINAL:
		td->output_mode=TERM_IOCTL_OMODE_TEXT;
		td->show=1;
		td->read_block=1;
		td->read_line=1;
		crt_cursor_on();
		st_clear_term(td);
		if(fl->idata==active_term_index) st_show_no_check(td);
		break;
	case TERM_IOCTL_MODE_PICTURE:
		td->output_mode=TERM_IOCTL_OMODE_PIXEL;
		td->show=0;
		td->read_block=0;
		td->read_line=0;
		crt_cursor_off();
		st_clear_term(td);
		if(fl->idata==active_term_index) st_show_no_check(td);		
		break;
	default:
		return -1;
	}

	return 0;
}

static int term_read(struct file *fl,char *buf,int bytes)
{
	struct term_data *td;
	int read_bytes;

	if(fl->idata<0 || fl->idata>=MAX_NOF_TERMS || terms[fl->idata].used==0) return -1;

	td=terms+fl->idata;

	if(!td->read_line)
	{
		if(td->iend<bytes)
		{
			if(td->read_block) while(td->iend<bytes);
			else
			{
				if(td->iend==0) return 0;

				bytes=td->iend;
			}
		}

		kmemcpy(buf,td->input,bytes);

		kmemcpy(td->input,td->input+bytes,bytes);

		td->iend-=bytes;

		read_bytes=bytes;
	}
	else
	{
		int i;

		while(1)
		{
			one();

			for(i=0;i<td->iend && td->input[i]!='\n';i++);

			if(i==td->iend)
			{
				if(!td->read_block)
				{
					all();
					return 0;
				}
			}
			else
			{
				all();break;
			}

			all();
		}

		i++; /* include '\n' character */

		if(i>bytes) return -1;

		kmemcpy(buf,td->input,i);
		buf[i]='\0';

		kmemcpy(td->input,td->input+i,i);

		td->iend-=i;

		read_bytes=i;
	}

	st_add_to_output(fl,buf,read_bytes);

	return read_bytes;
}

static int term_write(struct file *fl,char *buf,int bytes)
{
	struct term_data *td;
	int ret=0;

	if(fl->idata<0 || fl->idata>=MAX_NOF_TERMS || terms[fl->idata].used==0) return -1;

	td=terms+fl->idata;

	one();

	ret=st_add_to_output(fl,buf,bytes);
	
	all();

	return ret;
}

static void switch_to_term(int index)
{
	struct term_data *td;
	one();
	if(index>=0 && index<MAX_NOF_TERMS && terms[index].used)
	{
		active_term_index=index;

		td=terms+active_term_index;				
		if(td->output_mode==TERM_IOCTL_OMODE_TEXT) crt_cursor_on();
		else crt_cursor_off();

		st_show(td);
	}
	all();
}

static int term_close(struct file *fl)
{
	struct term_data *td;
	int i;

	one();

	if(fl->idata<0 || fl->idata>MAX_NOF_TERMS || terms[fl->idata].used==0) 
	{
		all();
		return -1;
	}

	td=terms+fl->idata;

	td->used=0;

	for(i=0;i<MAX_NOF_TERMS && !terms[i].used;i++);
	
	if(i<MAX_NOF_TERMS) switch_to_term(i);

	all();

	return 0;
}

/* ---------------------------------------------------------------------------------------------------------- */

static void key_pressed(unsigned char symbol)
{
	if(active_term_index>=0)
	{
		struct term_data *td=terms+active_term_index;

		if(symbol=='\b')
		{
			if(td->iend>0) td->iend--;
			else return;
		}
		else
		{
			if(td->iend>=TERM_INPUT_BSIZE ||
				(td->iend==TERM_INPUT_BSIZE-1 && symbol!='\n')) return;

			td->input[td->iend++]=symbol;
		}

		st_show(td);
	}
}

#define NOF_SCAN_CODES 0x80

#define KB_PRESSED(scan_code) ((scan_code & 0x80)?0:1)
#define KB_SCAN(scan_code) ((scan_code) & 0x7f)

#define SCAN_LSHIFT 0x2a
#define SCAN_RSHIFT 0x36
#define SCAN_CTRL   0x1d
#define SCAN_ALT    0x38

#define SCAN_F1  0x3B
#define SCAN_F10 0x44

char *pressed_keys;

extern char *scan_code_map,*scan_code_map_shift;

static void handle_keyboard_interrupt(int code)
{
	int scan_code=KB_SCAN(code),switch_to_term_index=-1,i;

	if(KB_PRESSED(code))
	{
		pressed_keys[scan_code]=1;

		if(pressed_keys[SCAN_CTRL])
			for(i=SCAN_F1;i<=SCAN_F10;i++)
				if(pressed_keys[i])
				{
					switch_to_term_index=i-SCAN_F1;
					break;
				}

		if(switch_to_term_index>=0)
			switch_to_term(switch_to_term_index);

		if(pressed_keys[SCAN_LSHIFT] || pressed_keys[SCAN_RSHIFT])
		{
			if(scan_code_map_shift[scan_code]!='\0')
				key_pressed(scan_code_map_shift[scan_code]);
		}
		else
		{
			if(scan_code_map[scan_code]!='\0')
				key_pressed(scan_code_map[scan_code]);
		}
	}
	else pressed_keys[KB_SCAN(scan_code)]=0;
}

static int st_init_keyboard()
{
	if((pressed_keys=rgn_alloc(NOF_SCAN_CODES))==0) return -1;

	kmemset(pressed_keys,0,NOF_SCAN_CODES);

	int_set_handler_keyboard(handle_keyboard_interrupt);

	return 0;
}

int term_init_driver(struct driver_descr *d)
{
	d->open=term_open;
	d->read=term_read;
	d->write=term_write;
	d->ioctl=term_ioctl;
	d->close=term_close;
	d->name="terminal";

	if((terms=(struct term_data*)rgn_alloc(MAX_NOF_TERMS*sizeof(struct term_data)))==0)
		return -1;
		
	kmemset(terms,0,MAX_NOF_TERMS*sizeof(struct term_data));

	if(st_init_keyboard()) return -1;

	return 0;
}

