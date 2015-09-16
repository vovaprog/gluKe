#define ASSEMBLER
#include <defs.h>

.text
.code32

.globl kmemset
kmemset:
	push %eax
	push %ecx
	push %edi

	movl 16(%esp),%edi
	movl 20(%esp),%eax
	movl 24(%esp),%ecx

	cld
	rep stosb

	pop %edi
	pop %ecx
	pop %eax

	ret

.globl kmemset4
kmemset4:
	push %eax
	push %ecx
	push %edi

	movl 16(%esp),%edi
	movl 20(%esp),%eax
	movl 24(%esp),%ecx

	cld
	rep stosl

	pop %edi
	pop %ecx
	pop %eax

	ret

.globl kstrlen
kstrlen:
	push %ecx
	push %edi

	movl 12(%esp),%edi
	xor %eax,%eax
	xor %ecx,%ecx
	dec %ecx

	cld
	repne scasb

	mov %edi,%eax
	sub 12(%esp),%eax
	dec %eax

	pop %edi
	pop %ecx

	ret

.globl kstrcmp
kstrcmp:
	push %ebx
	push %esi
	push %edi

	movl 16(%esp),%esi
	movl 20(%esp),%edi

	xor %eax,%eax

kstrcmp_loop:
	mov (%esi),%ebx
	cmpb (%edi),%bl

	ja above
	jb below

	cmpb $0,(%esi)
	je equal

	inc %esi
	inc %edi
	jmp kstrcmp_loop

above:
	inc %eax
	jmp kstrcmp_exit
below:
	dec %eax
equal:
kstrcmp_exit:
	pop %edi
	pop %esi
	pop %ebx
	ret

.globl kmemcpy
kmemcpy:
	push %ecx
	push %esi
	push %edi

	movl 16(%esp),%edi
	movl 20(%esp),%esi
	movl 24(%esp),%ecx

	cmp %esi,%edi

	ja kmemcpy_backcopy

	cld
	rep movsb

	jmp kmemcpy_exit

kmemcpy_backcopy:
	add %ecx,%esi
	dec %esi
	add %ecx,%edi
	dec %edi

	std
	rep movsb

kmemcpy_exit:
	pop %edi
	pop %esi
	pop %ecx

	ret

