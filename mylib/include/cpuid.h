#ifndef CPUID_H_INCLUDED
#define CPUID_H_INCLUDED

struct four_regs{
	unsigned int eax,ebx,ecx,edx;
};

enum IA32_family{IA32_386,IA32_486,IA32_Pentium,IA32_PentiumPro,IA32_Pentium4,IA32_FAMILY_UNKNOWN,IA32_FAMILY_CPUID_NOT_SUPPORTED};


int cpuid_s(int num,unsigned int *output);

int cpuid(int num,struct four_regs *regs);

enum IA32_family IA32_family(struct four_regs *regs);

char* IA32_family_string(char *s);

int cpuid_brand_string(char *s);

#ifndef GLUKE
void cpuid_print();
#else
void cpuid_print(int fl);
#endif

#endif

