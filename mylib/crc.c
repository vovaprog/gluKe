#include <defs.h>

#include <crc.h>

#ifndef GLUKE

#ifdef OS_WINDOWS
#	include <Winsock2.h>
#else
#	include <netinet/in.h>
#endif

short int ipcmp_crc(char *packet,unsigned int size)
{
	short int *p=(short int*)packet;
	int sum=0;
	unsigned int i,q;

	if(p==0) return 0;

	q=size/2;

	for(i=0;i<q;i++) sum+=(unsigned short int)ntohs(p[i]);

	if(size % 2) sum+=((char*)(p+i))[0] << 8;

	sum=((unsigned int)sum>>16)+(sum & 0xffff);

	return htons(0xffff-(sum & 0xffff));
}

#endif

unsigned int crc_sum(unsigned char *buf,unsigned int size)
{
	unsigned int i,sum=0;

	for(i=0;i<size;i++) sum+=(unsigned int)buf[i];

	return sum;
}



