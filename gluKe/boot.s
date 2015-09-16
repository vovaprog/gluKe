/*
	This code is loaded at 0x7c00
	CS = 0

	mov some_var,%ax   -  move mem at offset some_var to ax
	mov $some_var,%ax  -  move offest some_var to ax
*/

/*
now works only if drive has 2 heads (floppy drive).
*/

READ_BLOCK_SECTORS=0x11
SECTOR_SIZE=0x200

.code16

.text
.globl _start
_start:
	jmp real_start

#-------------------------------------------------------

#byte at offset 0x02 from beg of file
#must be patched with nof sectors to read
SECTORS_TO_READ:
	.byte 0x00

loaded_code:             #long jump to loaded code
	.byte 0xea
	.word 0x0000

#word at offset 0x06 from beg of file must be patched
#with target segment address for loaded code in little endian
#segment means, that address written here:=needed_address >> 4
LOAD_TARGET_SEG_ADDRESS:
	.word 0x0000

#-------------------------------------------------------

real_start:
	mov $0x07c0,%ax
	mov %ax,%ds

	#clear vidio memory
	mov $0xb800,%ax
	mov %ax,%es
	xor %di,%di
	mov $(80*25),%cx
	mov $0x0f00,%ax  #white zero symbol
	rep stosw

	push %ds
	pop %es

	#move cursor to line 0, column 0
	mov $0x02,%ah
	xor %bx,%bx
	xor %dx,%dx
	int $0x10

	mov $msg_boot_greeting,%si
	call print16

	#stack: 0x0000-0xffff
	xor %ax,%ax
	mov %ax,%ss
	#mov $0xffff,%sp
	mov $0x7b00,%sp


#--- reset disk ---
	xor %ah,%ah #function 0
	xor %dl,%dl #drive number
	int $0x13
#--- reset disk ---


#------------------
#find number of sectors per track
#try values 36,18,15,9
#if none works halt
#or put found number to sectors_per_track

	mov $disksizes,%si

probe_loop:

	lodsb
	cbtw
	
	#we have tried all values and none worked
	cmp $0,%ax
	je disk_read_error
		
	mov %ax,%cx
	mov $0x0201,%ax
	mov $0x0200,%bx  #just after boot sector
	int $0x13
	jc probe_loop

	movb %cl,sectors_per_track
#------------------


#------------------
#find bytes_to_read_at_once value
#it will be added to bx
	mov sectors_per_track,%al
	cbtw
	mov $SECTOR_SIZE,%bx
	mul %bx
	mov %ax,bytes_to_read_at_once
#------------------


#------------------
#find number of read operations
#after this code number will be in di
	mov SECTORS_TO_READ,%al
	inc %al
	mov $0x0,%ah
	mov sectors_per_track,%bl
	div %bl

	test %ah,%ah
	jz no_inc
	inc %al
no_inc:
	cbtw
	mov %ax,%di
#------------------


	mov LOAD_TARGET_SEG_ADDRESS,%ax
	mov %ax,%es

	#destination offset
	mov $0x0,%bx


	mov $0,%ch   #track

	mov $0x0000,%dx     #drive number. 0 - first floppy
	                    #and head number

read_loop:

	mov $1,%cl   #1-based sector number

	mov $0x02,%ah               #function 2 - read sector
	mov sectors_per_track,%al   #number of sectors to read

#if reading first time - do not read first sector of track
	cmpb $0,first_time
	je not_first
	inc %cl
	dec %al
not_first:

	int $0x13
	jc disk_read_error

	add bytes_to_read_at_once,%bx

#if reading first time - sub 0x200 from bx
	cmpb $0,first_time
	je not_first2
	sub $0x200,%bx
not_first2:

#next time will not be first
	movb $0x00,first_time



#setup head and track
	cmp $0x0,%dh
	je now_head1

#head was 1, so now it is 0
	mov $0x00,%dh
#increase track
	inc %ch

	jmp after_head_track_setup
now_head1:

#head was 0, so set it to 1
	mov $0x01,%dh

after_head_track_setup:



	dec %di

	jnz read_loop



	mov $msg_jmp_to_kernel,%si
	call print16

	jmp loaded_code


#-------------------------------------------------------

disk_read_error:
	mov $msg_floppy_error,%si
	call print16
disk_read_error_halt:
	jmp disk_read_error_halt

.include "print16.s"

#-------------------------------------------------------

disksizes:
.byte 36,18,15,9,0

first_time: .byte 0xff

bytes_to_read_at_once:.word 0x0000

sectors_per_track: .byte 0x0

msg_boot_greeting: .asciz "Boot sector started\r\n"
msg_jmp_to_kernel: .asciz "Jumping to loaded code...\r\n"
msg_floppy_error:  .asciz "Disk read error\r\n"

.org 510,0
.word 0xAA55 #BIOS boot signature

