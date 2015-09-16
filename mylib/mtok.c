#include <mtok.h>

#ifndef GLUKE
#   include <string.h>
#else

char* strchr(char *s,int c)
{
	int i;

	for(i=0;s[i]!=0 && s[i]!=c;i++);

	if(s[i]==0) return 0;

	return s+i;
}

#endif

//int mtok(char *input,unsigned int *cur_pos,char *buf,unsigned int *buf_size,char *usep,char *jsep)
int mtok(char *input, int *cur_pos,char *buf, int *buf_size,char *usep,char *jsep)
{
	int cur_buf_size=0,taking_word=0,i;

	if(input==0 || cur_pos==0 || buf==0 || buf_size==0 || (usep==0 && jsep==0))
		return E_BARG;

	if(*buf_size<2) return E_BARG;

	for(i=*cur_pos;input[i];i++)
	{
		if(jsep?strchr(jsep,input[i]):0)
		{
			if(taking_word) goto return_word;
		}
		else if(usep?strchr(usep,input[i]):0)
		{
			if(taking_word) goto return_word;
			else
			{
				buf[0]=input[i];buf[1]='\0';
				(*cur_pos)++;
				return 0;
			}
		}
		else
		{
			taking_word=1;
			
			/* if buf is not large enough - just count needed size */
			if(cur_buf_size<*buf_size-1) buf[cur_buf_size]=input[i];
			cur_buf_size++;
		}
	}
	if(!taking_word) return MTOK_END;

return_word:
	*cur_pos=i;
	if(cur_buf_size<*buf_size)
	{
		buf[cur_buf_size]='\0';
		return 0;
	}

	*buf_size=cur_buf_size+1;
	return E_BUF_2SMALL;
}

