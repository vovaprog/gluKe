sfol=.
ifol=.
ofol=./output

MYLIB=../mylib
MYLIBI=$(MYLIB)/include
MYLIBO=$(MYLIB)/output

CINC=-I$(ifol) -I$(MYLIBI) -I$(sfol)/ktasks -I$(sfol)/drivers
CFLAGS=$(CINC) -Wall
OPTIM=-O2

all: create_output_folder mylib $(ofol)/boot.bin $(ofol)/entry16.bin $(ofol)/entry32.bin $(sfol)/Makefile
	./build_utils/0patch_kernel.pl

	cat $(ofol)/boot.bin > $(ofol)/gluKe.bin
	cat $(ofol)/entry16.bin >> $(ofol)/gluKe.bin
	cat $(ofol)/entry32.bin >> $(ofol)/gluKe.bin

	ls -l ./output/gluKe.bin

$(ofol)/boot.bin: $(sfol)/boot.s $(sfol)/Makefile
	as $(sfol)/boot.s     -o $(ofol)/boot.o
	ld $(ofol)/boot.o --oformat binary -Ttext 0x0000 -o $(ofol)/boot.bin

$(ofol)/entry16.bin: $(sfol)/entry16.S $(ifol)/defs.h $(sfol)/Makefile
	gcc $(CINC) -E $(sfol)/entry16.S -o $(ofol)/entry16.s
	as $(ofol)/entry16.s -o $(ofol)/entry16.o
	ld $(ofol)/entry16.o --oformat binary -Ttext 0x0000 -o $(ofol)/entry16.bin

$(ofol)/entry32.bin: $(sfol)/*.[sS]  $(sfol)/*.c $(ifol)/*.h $(MYLIB)/*.c $(MYLIBI)/*.h $(sfol)/Makefile \
                     $(sfol)/ktasks/* $(sfol)/drivers/*
	as                        $(sfol)/entry32.s   -o $(ofol)/entry32/entry32_s.o
	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/entry32.c   -o $(ofol)/entry32/entry32_c.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/sys_utils.c -o $(ofol)/entry32/sys_utils.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/int.c       -o $(ofol)/entry32/int_c.o
	gcc -E $(CINC)            $(sfol)/int.S       -o $(ofol)/entry32/int.s
	as $(ASFLAGS)             $(ofol)/entry32/int.s -o $(ofol)/entry32/int_s.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/rgn.c -o $(ofol)/entry32/rgn.o
	
	gcc -E $(CINC)            $(sfol)/utils.S     -o $(ofol)/entry32/utils.s
	as $(ASFLAGS)             $(ofol)/entry32/utils.s -o $(ofol)/entry32/utils_s.o
	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/utils.c     -o $(ofol)/entry32/utils.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/file.c -o $(ofol)/entry32/file.o
	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/handle.c -o $(ofol)/entry32/handle.o	

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/task.c -o $(ofol)/entry32/task.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/plan.c -o $(ofol)/entry32/plan.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/ktasks/work.c -o $(ofol)/entry32/work.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/drivers/term.c -o $(ofol)/entry32/term.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/mutex.c -o $(ofol)/entry32/mutex.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/ktasks/counter.c -o $(ofol)/entry32/counter.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/ktasks/reverse.c -o $(ofol)/entry32/reverse.o

	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/ktasks/ktask_cpuid.c -o $(ofol)/entry32/ktask_cpuid.o
	
	gcc -c $(CFLAGS) $(OPTIM) $(sfol)/ktasks/ktask_life.c -o $(ofol)/entry32/ktask_life.o

	ld  --oformat binary -T ./entry32.lnk -o $(ofol)/entry32.bin

create_output_folder:
	mkdir -p ./output/entry32
	
mylib: cpuid mem_block linked_list mtok predicate crc life

cpuid:
	gcc -I. -I$(MYLIBI) -E $(MYLIB)/cpuid.S -o $(ofol)/entry32/cpuid_preprocessed.s
	as $(ofol)/entry32/cpuid_preprocessed.s -o $(ofol)/entry32/cpuid_s.o
	rm $(ofol)/entry32/cpuid_preprocessed.s
	gcc -c -D GLUKE $(CFLAGS) $(MYLIB)/cpuid.c -o $(ofol)/entry32/cpuid.o
	gcc -c -D GLUKE $(CFLAGS) $(MYLIB)/cpuid_print.c -o $(ofol)/entry32/cpuid_print.o

life:
	gcc -c -D GLUKE $(CFLAGS) $(MYLIB)/life.c -o $(ofol)/entry32/life.o

mem_block:
	gcc -c $(CFLAGS) $(OPTIM) $(MYLIB)/mem_block.c -o $(ofol)/entry32/mem_block.o

linked_list:
	gcc -c $(CFLAGS) $(OPTIM) $(MYLIB)/linked_list.c -o $(ofol)/entry32/linked_list.o

predicate:
	gcc -c $(CFLAGS) $(OPTIM) $(MYLIB)/predicate.c -o $(ofol)/entry32/predicate.o
	
crc:	
	gcc -c $(CFLAGS) $(OPTIM) $(MYLIB)/crc.c -o $(ofol)/entry32/crc.o	
	
mtok:
	gcc -c $(CFLAGS) $(OPTIM) $(MYLIB)/mtok.c -o $(ofol)/entry32/mtok.o	
	
clean:
	find $(ofol) -type f | xargs rm

