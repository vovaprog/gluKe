
#si - offset of zero-terminated string in ds based segment.
print16:
	cld           #si will be increased with lodsb
	mov $0,%bx
print16_loop1:
	lodsb         #byte at ds:si to al, si+=1
	test %al,%al
	jz print16_exit
	mov $0x0e,%ah
	int $0x10
	jmp print16_loop1
print16_exit:
	ret

