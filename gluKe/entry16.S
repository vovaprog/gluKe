#define ASSEMBLER
#include <defs.h>

.code16

.text
.globl _start
_start:
	mov %cs,%ax
	mov %ax,%ds

	mov $msg_kernel_greeting,%si
	call print16

#--- copy GDT ---

	mov $GDT,%si

	mov $GDT_BASE_SEGMENT,%ax
	mov %ax,%es
	mov $0,%di

	mov $(4*ENTRY16_GDT_NOF_DESCRS),%ecx
	
	rep movsw 

#--- copy GDT ---

	#enable A20
	in $0x92,%al
	or $0x02,%al
	out %al,$0x92

	cli #stop interrupts

	lgdt gdt_info #load gdt

	#switching to protected mode: set PE bit in cr0
	mov %cr0,%eax
	or $0x01,%al
	mov %eax,%cr0

	#load DS SS ES FS GS ESP
	mov $0x10,%ax
	mov %ax,%ss
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	mov $ENTRY16_BASE,%esp

	#load 32bit code seg into CS: long jump
	.byte 0x66
	.byte 0xea
	.long KERNEL_BASE
	.word 0x08

.include "print16.s"

/*.org ENTRY16_GDT_OFFSET,0*/
.align 4
GDT:
	.word 0x0000
	.word 0x0000
	.word 0x0000
	.word 0x0000

	.word 0xFFFF
	.word 0x0000
	.byte 0x00
	.byte 0x9A
	.byte 0xcf
	.byte 0x00

	.word 0xFFFF
	.word 0x0000
	.byte 0x00
	.byte 0x92
	.byte 0xcf
	.byte 0x00

gdt_info:
	.word ENTRY16_GDT_NOF_DESCRS*8
	.long GDT_BASE
	
/*	ENTRY16_BASE+GDT */

msg_kernel_greeting: .asciz "gluKernel started\r\nSwitching to protected mode...\r\n"

.org 0x100,0

