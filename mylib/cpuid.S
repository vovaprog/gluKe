#define ASSEMBLER

#include <defs.h>

#ifdef OS_WINDOWS
.globl _cpuid_s
_cpuid_s:
#else
.globl cpuid_s
cpuid_s:
#endif

      pushl %ebp
      mov %esp,%ebp

   	push %esi
   	push %edx
   	push %ecx
   	push %ebx
   	
		pushf
	   orl $0x00200000,(%esp)
	   popf
	   pushf
	   movl (%esp),%eax
	   popf 
	   and $(~0x00200000),%eax 
	   jnz cpuid_supported 

	   mov $0xffffffff,%eax

	   pop %ebx
	   pop %ecx
	   pop %edx
	   pop %esi

  	   pop %ebp
	   ret

cpuid_supported: 

		mov 12(%ebp),%esi

		movl 8(%ebp),%eax

		cpuid
		
		mov %eax,(%esi)
		mov %ebx,4(%esi)
		mov %ecx,8(%esi)
		mov %edx,12(%esi)

		mov $0x0,%eax 

	   pop %ebx
	   pop %ecx
	   pop %edx
	   pop %esi

		pop %ebp
		ret
