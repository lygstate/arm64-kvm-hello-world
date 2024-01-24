#!/usr/bin/env bash

echo $BASH_SOURCE
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export PATH=$(realpath "$SCRIPT_DIR/../../qemu-origin/build"):$PATH
echo $PATH
which qemu-system-aarch64

export ACCEL_ARGS="-cpu cortex-a15 -accel tcg,thread=multi"
export ACCEL_ARGS="-cpu host,aarch64=off -accel kvm"
export QEMU_EXEC=xemu-core-system-aarch64 
export QEMU_EXEC=qemu-system-aarch64
export QEMU_DEBUG="-d int -s -S"
export QEMU_DEBUG="-s -S"
export QEMU_DEBUG=
export QEMU_GDB_SERVER="gdbserver :1235"
export QEMU_GDB_SERVER=

$QEMU_GDB_SERVER \
$QEMU_EXEC \
$ACCEL_ARGS \
$QEMU_DEBUG \
-m 1024M \
-smp 2 \
-M virt,gic-version=3 -nographic \
-monitor none \
-serial stdio \
-kernel $SCRIPT_DIR/hello_world.elf
