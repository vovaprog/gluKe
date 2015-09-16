#ifndef CRC_INCLUDED
#define CRC_INCLUDED

#ifndef GLUKE

short int ipcmp_crc(char *packet,unsigned int size);

#endif

unsigned int crc_sum(unsigned char *buf,unsigned int size);

#endif

