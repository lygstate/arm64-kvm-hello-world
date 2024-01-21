TOOL_CHAIN_PREFIX ?= aarch64-none-elf-

CC = $(TOOL_CHAIN_PREFIX)gcc
CXX = $(TOOL_CHAIN_PREFIX)g++
CPP = $(TOOL_CHAIN_PREFIX)gcc -E -P -x c
STRIP = $(TOOL_CHAIN_PREFIX)strip --strip-unneeded
OBJCOPY = $(TOOL_CHAIN_PREFIX)objcopy
LD = $(TOOL_CHAIN_PREFIX)ld
AR = $(TOOL_CHAIN_PREFIX)ar rcs
AS = $(TOOL_CHAIN_PREFIX)as
NM = $(TOOL_CHAIN_PREFIX)nm
OD = $(TOOL_CHAIN_PREFIX)objdump
FILTER = $(TOOL_CHAIN_PREFIX)c++filt

all: hello_world.elf

hello_world.elf: link_script.ld startup.o hello_world.o
	# Link the two object files to an elf file
	$(LD) -T link_script.ld startup.o hello_world.o -o hello_world.elf

startup.o: startup.s
	$(AS) -o startup.o startup.s

hello_world.o: hello_world.c
	$(CC) -c -o hello_world.o hello_world.c

clean:
	rm -f hello_world.o startup.o hello_world.elf
