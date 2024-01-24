ENTRY(_start)

MEMORY
{
    cod (RX)  : ORIGIN = 0x40000000 , LENGTH = 0x00800000
    stack (RW) : ORIGIN = 0x40800000, LENGTH = 0x00100000
    mem (RW)  : ORIGIN = 0x40900000 , LENGTH = 0x00100000
}

SECTIONS
{
	. = 0x40000000;
	.startup . : { startup.o(.text) } > cod
	.text : { *(.text) } > cod
	.data : { *(.data) } > mem
	.bss : { *(.bss COMMON) } > mem
	. = ALIGN(16);
	. = 0x40800000;
	. = . + 0x01000000; /* 0x01000000 of stack memory */
	stack_top = .;
	heap_start = 0x40A00000;
	heap_stop = heap_start + 0x0F600000;
}

