TOOL_CHAIN_PREFIX ?= arm-none-eabi-

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

ARCH_CPU_FPU := -mfpu=crypto-neon-fp-armv8
ARCH_CPU_FPU += -mfloat-abi=hard

#  Sets the compiler option for the target CPU architecture to ARMv8-A. Specifically
ARCH_CPU_MARCH += -march=armv8-a
ARCH_CPU_MARCH += -mlittle-endian

# Use --print-multi-dir to print the lib directory
CFLAGS := $(ARCHCFLAGS) $(ARCH_OPTIMIZATION) $(ARCH_DEBUG) $(ARCH_CPU_FPU) $(ARCH_CPU_MARCH)
# debug
CFLAGS += -g -O0

GCC_ROOT :=/home/xch/work/xpack-arm-none-eabi-gcc-13.2.1-1.1
LDFLAGS := -nostdlib -T link_script.ld -L$(GCC_ROOT)/arm-none-eabi/lib/thumb/v8-a+simd/hard/ -L$(GCC_ROOT)/lib/gcc/arm-none-eabi/13.2.1/thumb/v8-a+simd/hard/
all: hello_world.elf

hello_world.elf: link_script.ld startup.o hello_world.o nosys_gnu.o
	# Link the two object files to an elf file
	$(LD) $(LDFLAGS) startup.o hello_world.o nosys_gnu.o -lnosys -lgcc -lc_nano -lm -o hello_world.elf

%.o: %.c
	$(CC) $(CFLAGS) -c -MMD $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c -MMD $< -o $@

clean:
	rm -f hello_world.o startup.o hello_world.elf
