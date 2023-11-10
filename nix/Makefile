C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OPTIONAL = $(wildcard cpu/interrupt.asm)
OBJ = ${C_SOURCES:.c=.o} ${OPTIONAL:.asm=.o}

# Change this if your cross-compiler is somewhere else
CC = i386-elf-gcc
LD = i386-elf-ld

# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding
LIBGCC = -L $(shell ${CC} ${CFLAGS} -print-libgcc-file-name | dirname) -lgcc
LDFLAGS = -nostdlib ${LIBGCC} -Ttext 0x8000

# First rule is run by default
os-image.bin: boot/bootsect.bin kernel.bin
	cat $^ > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} ${LDFLAGS} -o $@ $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} ${LDFLAGS} -o $@ $^

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o
