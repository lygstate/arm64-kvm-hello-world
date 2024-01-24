# arm-none-eabi-gdb -iex "set auto-load safe-path $PWD"
# GDB may have ./.gdbinit loading disabled by default.  In that case you can
# follow the instructions it prints.  They boil down to adding the following to
# your home directory's ~/.gdbinit file:
#
#   add-auto-load-safe-path /path/to/qemu/.gdbinit

# Load QEMU-specific sub-commands and settings
# source scripts/qemu-gdb.py
set pagination off
set confirm off
add-symbol-file -readnow ./hello_world.elf
target remote 127.0.0.1:1234
# hbreak lmain
# hbreak acCoreIdGet
# hbreak arm_undef_exception_reentrant
# hbreak arm_data_abort_exception_reentrant
# hbreak ___sysstart_veneer
# hbreak _sysstart
# hbreak *0x8000b020
# hbreak *0x8000b0e4
# hbreak *0x8000b0f4
# hbreak *0x8000b0e4
# hbreak *0x8000b0e4
# hbreak lmain
# hbreak _startup
# hbreak DataAbortHandler
# hbreak DataAbortHandler
# hbreak f_vprintf
# hbreak FtOut64
# hbreak DataAbortHandler
# hbreak FreeRTOS_SWI_Handler
hbreak _start
hbreak hvc_call_bare
hbreak start_slave_core_hvc