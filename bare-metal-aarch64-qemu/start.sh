SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

qemu-system-aarch64 \
-cpu host -accel kvm \
-M virt,gic-version=3 -nographic \
-d int \
-monitor none \
-serial stdio \
-kernel $SCRIPT_DIR/hello_world.elf
