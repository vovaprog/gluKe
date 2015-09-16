#include <cpuid.h>

#ifdef GLUKE
#   include <gluKe_stubs.h>
#else
#   include <string.h>
#endif

int cpuid(int num,struct four_regs *regs)
{
	unsigned int buf[4];

	if(cpuid_s(num,buf)) return -1;

	regs->eax=buf[0];
	regs->ebx=buf[1];
	regs->ecx=buf[2];
	regs->edx=buf[3];

	return 0;
}

enum IA32_family IA32_family(struct four_regs *arg_regs)
{
   struct four_regs *regs,loc_regs;
   
   if(arg_regs==0)
   {
   	if(cpuid(1,&loc_regs)) return IA32_FAMILY_CPUID_NOT_SUPPORTED;
   	regs=&loc_regs;
	}
	else regs=arg_regs;
	                  	
	switch((regs->eax>>8) & 0x0000000f){
	case 0x03:return IA32_386;break;
	case 0x04:return IA32_486;break;
	case 0x05:return IA32_Pentium;break;
	case 0x06:return IA32_PentiumPro;break;
	case 0x0f:return IA32_Pentium4;break;
	}
	return IA32_FAMILY_UNKNOWN;
}

char* IA32_family_string(char *s)
{
	enum IA32_family fam;
	
	fam=IA32_family(0);
	
	switch(fam){
	case IA32_386: strcpy(s,"386");break;
	case IA32_486: strcpy(s,"486");break;
	case IA32_Pentium: strcpy(s,"Pentium");break;
	case IA32_PentiumPro: strcpy(s,"PentiumPro");break;
	case IA32_Pentium4: strcpy(s,"Pentium4");break;	
	case IA32_FAMILY_CPUID_NOT_SUPPORTED: strcpy(s,"cpuid not supported");
	default: strcpy(s,"unknown");break;
	}
	return s;
}

int cpuid_brand_string(char *s)
{
   int i;
	struct four_regs regs;

	if(IA32_family(0)!=IA32_Pentium4) return IA32_family(0);

	if(cpuid(0x80000000,&regs)) return -1;

	if(regs.eax<0x80000004) return -1;

	for(i=0;i<3;i++)	
	{
	   if(cpuid(0x80000000+i+2,&regs)) return -1;

		((unsigned int*)s)[i*4]=regs.eax;
		((unsigned int*)s)[i*4+1]=regs.ebx;
		((unsigned int*)s)[i*4+2]=regs.ecx;
		((unsigned int*)s)[i*4+3]=regs.edx;
	}

	s[47]=0; /* is not really needed */

	return 0;
}

