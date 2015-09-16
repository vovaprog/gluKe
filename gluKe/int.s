#define ASSEMBLER
#include <defs.h>
.text
.code32

 #-------------------------------------------------


.globl get_ESP
get_ESP:
	mov %esp,%eax
	ret


 #-------------------------------------------------


.globl init_PIC
init_PIC:
 #init PIC: place irqs after reserved interrupts
 #---------------------
ICW1=0x11
ICW2_MASTER=0x20 #irqs 0-7 after reserved interrupts
ICW2_SLAVE=0x28  #irqs 8-15 after 0-7
ICW3_MASTER=0x04
ICW3_SLAVE=0x02  #slave is connected at 2
ICW4=0x01        #unbuffered
 #---------------------
	mov $ICW1,%al
	out %al,$0x20
	out %al,$0xA0

	mov $ICW2_MASTER,%al
	out %al,$0x21
	mov $ICW2_SLAVE,%al
	out %al,$0xA1

	mov $ICW3_MASTER,%al
	out %al,$0x21
	mov $ICW3_SLAVE,%al
	out %al,$0xA1

	mov $ICW4,%al
	out %al,$0x21
	out %al,$0xA1
 #---------------------
	ret


 #-------------------------------------------------


.globl get_scan_code
get_scan_code:

	in $0x60,%al #read scan code
	push %eax

	in $0x61,%al
	push %eax
	or $0x80,%al
	out %al,$0x61
	pop %eax
	out %al,$0x61

	pop %eax
	ret


 #-------------------------------------------------


int_handle_address:
.long int_handle

.align 0x10
.globl int_handler_no_err
int_handler_no_err:
	pusha
	pushl $0
	pushl $0
	call *int_handle_address
	add $0x08,%esp
	popa
	iret
	.align 0x10

.globl int_handler_with_err
int_handler_with_err:
	pusha
	pushl $0
	call *int_handle_address
	add $0x08,%esp
	popa
	iret
	.align 0x10


 #-------------------------------------------------


.globl IDT_info_main
IDT_info_main:
	.word 0x0
	.long 0x0

