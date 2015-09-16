#include <glinc.h>

void phex(unsigned char *s,int nof_bytes)
{
	int i=0,j;
	char itos_buf[ITOS_BSIZE];

	while(i<nof_bytes)
	{
		for(j=0;j<16 && i<nof_bytes;i++,j++)
		{
			if(j % 4==0 && j!=0) kwr("| ");
			kwr(itos_ux_nobase_width(s[i],2,itos_buf));
			kwr(" ");
		}
		kwr("\n");
	}
}

void kstrcpy(char *dest,char *src)
{
	int l=kstrlen(src);

	kmemcpy(dest,src,l+1);
}


char* itos(int x,int sign,int base,int show_base,int min_width,char *buf)
{
	char loc_buf[ITOS_BSIZE];	
	unsigned int loc_x=x>0?x:-x;
	int i,buf_pos=0,more_bytes=0;

	loc_buf[buf_pos++]='\0';

	do{
		i=loc_x % base;
		loc_x/=base;
		
		if(i<10) loc_buf[buf_pos++]=i+'0';
		else loc_buf[buf_pos++]=i-0x0A+'A';
		
	}while(loc_x);

	if(base==0x10 && show_base) more_bytes+=2;
	if(sign && x<0) more_bytes+=1;

	for(;buf_pos-1+more_bytes<min_width;buf_pos++) loc_buf[buf_pos]='0';

	if(base==0x10 && show_base)
	{
		loc_buf[buf_pos++]='x';loc_buf[buf_pos++]='0';
	}

	if(sign && x<0)
	{
		loc_buf[buf_pos++]='-';
	}

	for(i=0,buf_pos--;buf_pos>=0;i++,buf_pos--)
		buf[i]=loc_buf[buf_pos];

	buf[i]='\0';

	return buf;
}

char* itos_ud(unsigned int x,char *buf)
{
	return itos(x,0,10,1,0,buf);
}

char* itos_ux(unsigned int x,char *buf)
{
	return itos(x,0,0x10,1,0,buf);
}

char* itos_ux_nobase_width(unsigned int x,int width,char *buf)
{
	return itos(x,0,0x10,0,2,buf);
}

int stois(char *snum,int *result)
{
	int neg=0;

	if(snum==0) return -1;
	if(snum[0]=='-') neg=1;
	if(stoi(snum+neg,result)) return -1;
	if(neg) *result=-*result;
	return 0;
}

int stoi(char *snum,unsigned int *result)
{
	int i,first,slen;
	unsigned int dig,base,mul=1;

	if(snum==0 || result==0) return -1;

	*result=0;

	if((slen=kstrlen(snum))==0) return -1;

	switch(slen){
	case 1: base=10;first=0;break;
	case 2:
		if(snum[0]=='0'){first=1;base=8;}
		else{first=0;base=10;}
		break;
	default:
		if(snum[0]=='0')
		{
			if(snum[1]=='x' || snum[1]=='X'){first=2;base=0x10;}
			else{first=1;base=010;}
		}
		else{first=0;base=10;}
	}

	for(i=slen-1;i>=first;i--)
	{
		dig=(unsigned int)snum[i];
		if(dig>='0' && dig<='9') dig-='0';
		else if(dig>='A' && dig<='F') dig-='A'-10;
		else if(dig>='a' && dig<='f') dig-='a'-10;
		else return -1;
		if(dig>=base) return -1;
		*result+=dig*mul;
		mul*=base;
	}
	return 0;
}

char* kstrcat(char *dest,char *src)
{
	int i,j;		
	for(i=kstrlen(dest),j=0;src[j];i++,j++)
		dest[i]=src[j];
	dest[i]=0;	
	return dest;
}

static int st_printf(char *s,int fl,char *format,unsigned int *vargs)
{
	char itos_buf[ITOS_BSIZE],*chp;
/*	unsigned int *vargs;*/
	int varg_index=0,i,start_i,temp_char;	

	/*asm("mov %%ebp,%0":"=m" (vargs):);
	vargs+=4;*/

	s[0]=0;

	for(i=0;format[i];)
	{
		if(format[i]=='%')
		{
			switch(format[i+1]){
			case 'u':
				itos_ud(vargs[varg_index],itos_buf);
				
				if(fl<0) kstrcat(s,itos_buf);
				else write_string(fl,itos_buf);
				
				varg_index++;
				break;
			case 'x':
				itos_ux(vargs[varg_index],itos_buf);

				if(fl<0) kstrcat(s,itos_buf);
				else write_string(fl,itos_buf);

				varg_index++;
				break;
			case 's':
				chp=(char*)vargs[varg_index];
				
				if(fl<0) kstrcat(s,chp);
				else write_string(fl,chp);				

				varg_index++;

				break;				
			default:return -1;
			}
			i+=2;
		}

		start_i=i;
		for(;format[i] && format[i]!='%';i++);

		if(i!=start_i)
		{
			if(fl<0)
			{
				temp_char=format[i];
				format[i]=0;
				kstrcat(s,format+start_i);
				format[i]=temp_char;
			}
			else write(fl,format+start_i,i-start_i);				
		}
	}
	return 0;
}

int ksprintf(char *s,char *format,...)
{
	unsigned int *vargs;

	asm("mov %%ebp,%0":"=m" (vargs):);
	vargs+=4;

	return st_printf(s,-1,format,vargs);
}
	
int kfprintf(int fl,char *format,...)
{
	unsigned int *vargs;

	asm("mov %%ebp,%0":"=m" (vargs):);
	vargs+=4;

	return st_printf(0,fl,format,vargs);
}
