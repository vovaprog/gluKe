#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#define USAGE \
"USAGE\n\
   patcher file_name offset value [offset value ...]\n\
DESCRIPTION\n\
   Writes bytes to specified file at specified offset.\n\
OPTIONS\n\
   file_name - patch this file.\n\
   offset    - bytes will be written starting at this offset\n\
               from beginning of file. Offset must be specified as\n\
               hexidecimal number (for ex.: \"12ab\"). Use small letters.\n\
   value     - string, where every two symbols are hexidecimal byte.\n\
               value. Bytes will be written in order, they go in command line.\n\
               To repeat bytes use hex number in braces at beginning of line:\n\
               \"(b)1234\" - write this bytes 11 times.\n"

void die(char *text,...)
{
	if(text) 
	{
		va_list ap;
		va_start(ap,text);
		vprintf(text,ap);
		va_end(ap);		
		printf("\n");
	}
	else printf("program died\n");
	exit(-1);
}

char* parse_patch_string(char *patch_string,unsigned int *buf_size)
{
	unsigned int slen,i,repeat=1,byte_count;
	char *patch_bytes,temp;

	if(patch_string==0 || buf_size==0) die("error occured");

	if(sscanf(patch_string,"(%x)",&repeat)==1)
		for(i=0;patch_string[i]!='\0';i++) if(patch_string[i]==')'){patch_string+=(i+1);break;}

	slen=strlen(patch_string);
	if(slen % 2 || slen==0 || repeat==0) die(USAGE);

	if((patch_bytes=(char*)malloc(*buf_size=repeat*slen/2))==0) die("can't allocate memory");

	for(byte_count=0;repeat--;)
		for(i=0;i<slen;i+=2,byte_count++)
		{
		   temp=patch_string[i+2];
		   patch_string[i+2]='\0';
		   if(sscanf(patch_string+i,"%x",patch_bytes+byte_count)!=1) die(USAGE);
		   patch_string[i+2]=temp;
		}
	return patch_bytes;
}

int main(int argc,char **argv)
{
	unsigned int offset,i,nof_patch_bytes;
	char fname[300],patch_string[1000],*patch_bytes;

	int fd;

	if(argc<4) die(USAGE);
	
	if(sscanf(argv[1]," %s ",&fname)!=1) die("error can't get file name\r\n");

	if((fd=open(fname,O_WRONLY))==-1) die("Can't open file %s for writing.\r\n",fname);
		
	for(i=2;i+1<argc;i+=2)
	{
		if(sscanf(argv[i]," %x ",&offset)!=1) die(USAGE);
		if(sscanf(argv[i+1]," %s ",&patch_string)!=1) die(USAGE);

		patch_bytes=parse_patch_string(patch_string,&nof_patch_bytes);

		if(lseek(fd,offset,SEEK_SET)==-1) die("Can't change position in file %s\n",fname);
		if((write(fd,patch_bytes,nof_patch_bytes))==-1) printf("Can't write to file %s\n",fname);

		free(patch_bytes);
	}

	close(fd);

	return 0;
}
