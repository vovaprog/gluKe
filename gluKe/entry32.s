
.text
.code32

jmp entry32
.globl KERNEL_SIZE
KERNEL_SIZE:
.long 0x00000000
.globl KERNEL_CRC
KERNEL_CRC:
.long 0x00000000
