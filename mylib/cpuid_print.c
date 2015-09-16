#include <cpuid.h>

#define NUMBER_OF_BRAND_STRINGS 0x16

char *brand_strings[]={
"This processor does not support the brand identification feature",
"Intel Celeron processor",
"Intel Pentium III processor",
"Intel Pentium III XeonTM processor",
"Intel Pentium III processor",
0,
"Mobile Intel Pentium III processor-M",
"Mobile Intel Celeron processor",
"Intel Pentium 4 processor",
"Intel Pentium 4 processor",
"Intel Celeron processor",
"Intel XeonTM processor",
"Intel XeonTM processor MP",
0,
"Mobile Intel Pentium 4 processor-M",
0,
"Mobile Intel Celeron processor",
0,
0,
0,
"Mobile Intel Celeron processor",
0,
0,
"Intel Pentium M processor"};

#ifndef GLUKE

#   include <stdio.h>

#   define PRINTF printf
#   define MORE

    void cpuid_print()
#else

#   include <defs.h>
#   include <utils.h>
#   include <sys_utils.h>

#   define PRINTF(...) kfprintf(fl,__VA_ARGS__)
#   define MORE {char c;read_line(fl,"--- continue (press Enter) ---",&c,1);}

    void cpuid_print(int fl)
#endif
{
	struct four_regs regs;	
	char print_string[48],*s;
	unsigned int temp_int,extended_model_info=0,max_basic_query_number;
	int iter,i,j;
	enum IA32_family family;

	if(cpuid(0,&regs))
	{
		PRINTF("cpuid processor command not supported\n");
		MORE
		return;
	}

   PRINTF("max query number for basic info: %u\n",regs.eax);
   max_basic_query_number=regs.eax;

	((unsigned int*)print_string)[0]=regs.ebx;
	((unsigned int*)print_string)[1]=regs.edx;
	((unsigned int*)print_string)[2]=regs.ecx;
	print_string[12]=0;

	PRINTF("vendor: %s\n",print_string);

	PRINTF("\n");

	if(cpuid(1,&regs))
	{
		PRINTF("'cpuid' failed\n");
		MORE
		return;
	}

   PRINTF("modification: %u\n",regs.eax & 0x0000000f);
	
	PRINTF("model: %u\n",(regs.eax>>4)&0x0000000f);

	switch(family=IA32_family(&regs)){
	case IA32_386:s="386";break;
	case IA32_486:s="486";break;
	case IA32_Pentium:s="Pentium";break;
	case IA32_PentiumPro:s="Pentium Pro";break;
	case IA32_Pentium4:s="Pentium 4";extended_model_info=1;break;
	default:s="unknown";
	}

	PRINTF("family: %s\n",s);

	switch((regs.eax>>12)&0x00000003){
	case 0x00:s="Original OEM Processor";break;
	case 0x01:s="Intel OverDrive Processor";break;
	case 0x02:s="Dual Processor";break;
	case 0x03:s="Reserved processor type(unknown)";break;
	}

	PRINTF("processor type: %s\n",s);

	if(extended_model_info)
	{
		PRINTF("extended model: %u\n",(regs.eax>>16)&0x0000000f);
		PRINTF("extended family: %u\n",(regs.eax>>20)&0x000000ff);
	}

	PRINTF("\n");

/*--- EBX register ---*/
	temp_int=regs.ebx & 0x000000ff;

	if(temp_int<=NUMBER_OF_BRAND_STRINGS)
	{
		if(brand_strings[temp_int]) PRINTF("brand: %s\n",brand_strings[temp_int]);
		else PRINTF("brand: unknown\n");
	}
	else PRINTF("brand: unknown\n");
	
	if(family==IA32_Pentium4)
	{
		PRINTF("CFLUSH instruction cache line size in bytes: %u\n",((regs.ebx>>8)&0x000000ff)*8);
		PRINTF("Local APIC ID: %u\n",(regs.ebx>>24)&0x000000ff);
	}
	
	PRINTF("logical processors in package: %u\n",(int)((regs.ebx>>16)&0x000000ff));
/*--- EBX register ---*/

	PRINTF("\n");
	
/*--- ECX register ---*/
   PRINTF("SSE3 extensions: %s\n",regs.ecx & 0x01?"yes":"no");
   PRINTF("MONITOR/MWAIT: %s\n",regs.ecx & 0x08?"yes":"no");
   PRINTF("CPL Qualified Debug Store: %s\n",regs.ecx & (1<<4)?"yes":"no");
	PRINTF("Enhanced Intel SpeedStep Technology: %s\n",regs.ecx & 0x00000080?"yes":"no");
	PRINTF("Thermal monitor 2: %s\n",regs.ecx & 0x00000100?"yes":"no");
	PRINTF("L1ContextID support: %s\n",regs.ecx & 0x00000400?"yes":"no");
/*--- ECX register ---*/

	PRINTF("\n");
	MORE

/*--- EDX register ---*/
	PRINTF("FPU on chip: %s\n",regs.edx & 0x00000001?"yes":"no");
	PRINTF("Virtual 8086 mode enhancements: %s\n",regs.edx & 0x00000002?"yes":"no");
	PRINTF("Debugging extensions: %s\n",regs.edx & 0x00000004?"yes":"no");
	PRINTF("Page size extension: %s\n",regs.edx & 0x00000008?"yes":"no");
	PRINTF("Time step counter: %s\n",regs.edx & 0x00000010?"yes":"no");
	PRINTF("RDMSR and WRMSR instructions: %s\n",regs.edx & 0x00000020?"yes":"no");
	PRINTF("Physical address extension: %s\n",regs.edx & 0x00000040?"yes":"no");
	PRINTF("Machine check exception: %s\n",regs.edx & 0x00000080?"yes":"no");
	PRINTF("CMPXCHG8B instruction: %s\n",regs.edx & 0x00000100?"yes":"no");
	PRINTF("APIC on chip: %s\n",regs.edx & 0x00000200?"yes":"no");
	PRINTF("SYSENTER and SYSEXIT instructions: %s\n",regs.edx & 0x00000800?"yes":"no");
	PRINTF("Memory type range registers: %s\n",regs.edx & 0x00001000?"yes":"no");
	PRINTF("PTE global bit: %s\n",regs.edx & 0x00002000?"yes":"no");
	PRINTF("Machine check architecture: %s\n",regs.edx & 0x00004000?"yes":"no");
	PRINTF("Conditional move instructions: %s\n",regs.edx & 0x00008000?"yes":"no");
	PRINTF("Page attribute table: %s\n",regs.edx & 0x00010000?"yes":"no");

	MORE

	PRINTF("32-bit page size extension: %s\n",regs.edx & 0x00020000?"yes":"no");
	PRINTF("Processor serial number support: %s\n",regs.edx & 0x00040000?"yes":"no");	
	PRINTF("CLFLUSH instruction: %s\n",regs.edx & 0x00080000?"yes":"no");
	PRINTF("Debug store: %s\n",regs.edx & 0x00200000?"yes":"no");
	PRINTF("Thermal monitor and software controlled clock facilities: %s\n",regs.edx & 0x00400000?"yes":"no");
	PRINTF("Intel MMX technology: %s\n",regs.edx & 0x00800000?"yes":"no");
	PRINTF("FXSAVE and FXRSTOR instructions: %s\n",regs.edx & 0x01000000?"yes":"no");
	PRINTF("SSE: %s\n",regs.edx & 0x02000000?"yes":"no");
	PRINTF("SSE2: %s\n",regs.edx & 0x04000000?"yes":"no");
	PRINTF("Self snoop: %s\n",regs.edx & 0x08000000?"yes":"no");
	PRINTF("Hyper-Threading: %s\n",regs.edx & 0x10000000?"yes":"no");
	PRINTF("Thermal monitor: %s\n",regs.edx & 0x20000000?"yes":"no");
	PRINTF("Pending break enable: %s\n",regs.edx & 0x80000000?"yes":"no");
/*--- EDX register ---*/

	PRINTF("\n");

	if(max_basic_query_number<2) goto perform_extended_query;

	MORE

	for(iter=0;;iter++)
	{
		if(cpuid(2,&regs))
		{
			PRINTF("'cpuid' failed\n");
			MORE
			return;
		}

		if(iter==0) PRINTF("number of cache and tlb info queries: %u\n",regs.eax & 0xff);

		if(iter>=(regs.eax & 0xff)) break;

		for(i=0;i<4;i++)
		{
			char *chp=0;
			
			switch(i){
			case 0:chp=(char*)&regs.eax;break;
			case 1:chp=(char*)&regs.ebx;break;
			case 2:chp=(char*)&regs.ecx;break;
			case 3:chp=(char*)&regs.edx;break;
			}	

			for(j=(i==0?1:0);j<4;j++)
			{
				switch((int)chp[j]){
				case 0x00:s=0;break;
				case 0x01:s="Instruction TLB: 4K-Byte Pages, 4-way set associative, 32 entries";break;
				case 0x02:s="Instruction TLB: 4M-Byte Pages, 4-way set associative, 2 entries";break;
				case 0x03:s="Data TLB: 4K-Byte Pages, 4-way set associative, 64 entries";break;
				case 0x04:s="Data TLB: 4M-Byte Pages, 4-way set associative, 8 entries";break;
				case 0x06:s="1st-level instruction cache: 8K Bytes, 4-way set associative, 32byte line size";break;
				case 0x08:s="1st-level instruction cache: 16K Bytes, 4-way set associative, 32 byte line size";break;
				case 0x0A:s="1st-level data cache: 8K Bytes, 2-way set associative, 32 byte line size";break;
				case 0x0C:s="1st-level data cache: 16K Bytes, 4-way set associative, 32 byte line size";break;
				case 0x22:s="3rd-level cache: 512K Bytes, 4-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x23:s="3rd-level cache: 1M Bytes, 8-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x25:s="3rd-level cache: 2M Bytes, 8-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x2C:s="1st-level data cache: 32K Bytes, 8-way set associative, 64 byte line size";break;
				case 0x30:s="1st-level instruction cache: 32K Bytes, 8-way set associative, 64 byte line size";break;
				case 0x40:s="No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache";break;
				case 0x41:s="2nd-level cache: 128K Bytes, 4-way set associative, 32 byte line size";break;
				case 0x42:s="2nd-level cache: 256K Bytes, 4-way set associative, 32 byte line size";break;
				case 0x43:s="2nd-level cache: 512K Bytes, 4-way set associative, 32 byte line size";break;
				case 0x44:s="2nd-level cache: 1M Byte, 4-way set associative, 32 byte line size";break;
				case 0x45:s="2nd-level cache: 2M Byte, 4-way set associative, 32 byte line size";break;
				case 0x50:s="Instruction TLB: 4-KByte and 2-MByte or 4-MByte pages, 64 entries";break;
				case 0x51:s="Instruction TLB: 4-KByte and 2-MByte or 4-MByte pages, 128 entries";break;
				case 0x52:s="Instruction TLB: 4-KByte and 2-MByte or 4-MByte pages, 256 entries";break;
				case 0x5B:s="Data TLB: 4-KByte and 4-MByte pages, 64 entries";break;
				case 0x5C:s="Data TLB: 4-KByte and 4-MByte pages,128 entries";break;
				case 0x5D:s="Data TLB: 4-KByte and 4-MByte pages,256 entries";break;
				case 0x66:s="1st-level data cache: 8KB, 4-way set associative, 64 byte line size";break;
				case 0x67:s="1st-level data cache: 16KB, 4-way set associative, 64 byte line size";break;
				case 0x68:s="1st-level data cache: 32KB, 4-way set associative, 64 byte line size";break;
				case 0x70:s="Trace cache: 12K-uop, 8-way set associative";break;
				case 0x71:s="Trace cache: 16K-uop, 8-way set associative";break;
				case 0x72:s="Trace cache: 32K-uop, 8-way set associative";break;
				case 0x78:s="2nd-level cache: 1M Byte, 8-way set associative, 64byte line size";break;
				case 0x79:s="2nd-level cache: 128KB, 8-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x7A:s="2nd-level cache: 256KB, 8-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x7B:s="2nd-level cache: 512KB, 8-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x7C:s="2nd-level cache: 1MB, 8-way set associative, 64 byte line size, 128 byte sector size";break;
				case 0x7D:s="2nd-level cache: 2M Byte, 8-way set associative, 64byte line size";break;
				case 0x82:s="2nd-level cache: 256K Byte, 8-way set associative, 32 byte line size";break;
				case 0x83:s="2nd-level cache: 512K Byte, 8-way set associative, 32 byte line size";break; 
				case 0x84:s="2nd-level cache: 1M Byte, 8-way set associative, 32 byte line size";break;
				case 0x85:s="2nd-level cache: 2M Byte, 8-way set associative, 32 byte line size";break;
				case 0x86:s="2nd-level cache: 512K Byte, 4-way set associative, 64 byte line size";break;
				case 0x87:s="2nd-level cache: 1M Byte, 8-way set associative, 64 byte line size";break;
				case 0xB0:s="Instruction TLB: 4M-Byte Pages, 4-way set associative, 128 entries";break;
				case 0xB3:s="Data TLB: 4M-Byte Pages, 4-way set associative, 128 entries";break;
				default:s=0;
				}
				if(s!=0) PRINTF("%s\n",s);
			}
		}
	}

	PRINTF("\n");

perform_extended_query:

	if(family!=IA32_Pentium4) 
	{
		MORE
		return;
	}

	if(cpuid(0x80000000,&regs))
	{
		PRINTF("'cpuid' failed\n");
		MORE
		return;
	}

	PRINTF("maximum input value for extended info: 0x%x\n",regs.eax);

	if(cpuid_brand_string(print_string)==0)
		PRINTF("brand string: \"%s\"\n",print_string);

	MORE
	return;
}

