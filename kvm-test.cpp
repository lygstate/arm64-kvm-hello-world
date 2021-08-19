#include <sys/ioctl.h>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <cerrno>
#include <fcntl.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

int kvm, vmfd, vcpufd;

int ioctl_exit_on_error(int file_descriptor, unsigned long request, void *argument, string name) {
    int ret = ioctl(file_descriptor, request, argument);
    if (ret < 0) {
        printf("System call '%s' failed: %s\n", name.c_str(), strerror(errno));
        exit(ret);
    }
    return ret;
}

int get_register(uint64_t id, uint64_t *val, string name) {
    struct kvm_one_reg reg {
        .id = id,
        .addr = (uintptr_t)val,
    };
    printf("Reading register %s\n", name.c_str());
    ioctl_exit_on_error(vcpufd, KVM_GET_ONE_REG, &reg, "KVM_GET_ONE_REG");
    printf("%s: %ld\n", name.c_str(), *val);
    return 0;
}

int set_register(uint64_t id, uint64_t *val, string name) {
    struct kvm_one_reg reg {
        .id = id,
        .addr = (uintptr_t)val,
    };
    printf("Set register %s to %ld\n", name.c_str(), *val);
    ioctl_exit_on_error(vcpufd, KVM_SET_ONE_REG, &reg, "KVM_SET_ONE_REG");
    return 0;
}

int main() {
    kvm_regs kvm_regs;
    int ret;
    const uint32_t code[] = {
            /* Add two registers */
            0xd2800281, /* mov	x1, #0x14 */
            0xd28002c2, /* mov	x2, #0x16 */
            0x8b020023, /* add	x3, x1, x2 */

            /* Supervisor Call Exit */
            //0xd2800000, /* mov x0, #0x0 */
            //0x52800ba8, /* mov w8, #0x5d */
            //0xd4000001, /* svc #0x0 */

            /* Wait for Interrupt */
            0xd503207f, /* wfi */
    };
    uint8_t *mem;
    size_t mmap_size;
    struct kvm_run *run;

    /* Get the KVM file descriptor */
    kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
    if (kvm < 0) {
        printf("Cannot open '/dev/kvm': %s", strerror(errno));
        return kvm;
    }

    /* Make sure we have the stable version of the API */
    ret = ioctl(kvm, KVM_GET_API_VERSION, NULL);
    if (ret < 0) {
        printf("System call 'KVM_GET_API_VERSION' failed: %s", strerror(errno));
        return ret;
    }
    if (ret != 12) {
        printf("expected KVM API Version 12 got: %d", ret);
        return -1;
    }

    /* Create a VM and receive the VM file descriptor */
    printf("Creating VM\n");
    vmfd = ioctl_exit_on_error(kvm, KVM_CREATE_VM, (unsigned long) 0, "KVM_CREATE_VM");

    /* Allocate one aligned page of guest memory to hold the code. */
    printf("Setting up memory\n");
    void *void_mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mem = static_cast<uint8_t *>(void_mem);
    if (!mem)
        printf("Error while allocating guest memory");
    memcpy(mem, code, sizeof(code));
    printf("Size of code: %ld B\n", sizeof(code));

    struct kvm_userspace_memory_region region = {
            .slot = 0,
            .guest_phys_addr = 0x0000,
            .memory_size = 0x1000,
            .userspace_addr = (uint64_t) mem,
    };
    ioctl_exit_on_error(vmfd, KVM_SET_USER_MEMORY_REGION, &region, "KVM_SET_USER_MEMORY_REGION");

    /* Create a virtual CPU and receive its file descriptor */
    printf("Creating VCPU\n");
    vcpufd = ioctl_exit_on_error(vmfd, KVM_CREATE_VCPU, (unsigned long) 0, "KVM_CREATE_VCPU");

    /* Get CPU information for VCPU init*/
    printf("Retrieving CPU information\n");
    struct kvm_vcpu_init preferred_target;
    ioctl_exit_on_error(vmfd, KVM_ARM_PREFERRED_TARGET, &preferred_target, "KVM_ARM_PREFERRED_TARGET");

    /* Initialize VCPU */
    printf("Initializing VCPU\n");
    ioctl_exit_on_error(vcpufd, KVM_ARM_VCPU_INIT, &preferred_target, "KVM_ARM_VCPU_INIT");

    /* Map the shared kvm_run structure and following data. */
    ret = ioctl_exit_on_error(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL, "KVM_GET_VCPU_MMAP_SIZE");
    mmap_size = ret;
    if (mmap_size < sizeof(*run))
        printf("KVM_GET_VCPU_MMAP_SIZE unexpectedly small");
    void_mem = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);
    run = static_cast<kvm_run *>(void_mem);
    if (!run)
        printf("Error while mmap vcpu");

    /* Get register list. Somehow this is necessary */
    struct kvm_reg_list list;
    ret = ioctl(vcpufd, KVM_GET_REG_LIST, &list);
    if (ret < 0) {
        printf("System call 'KVM_GET_REG_LIST' failed: %s\n", strerror(errno));
        return ret;
    }

    /* Read register PSTATE (just for fun) */
    uint64_t val;
    get_register(kvm_regs.regs.pstate, &val, "PSTATE");

    /* Read register PC (just for fun) */
    get_register(kvm_regs.regs.pc, &val, "PC");

    /* Set register x2 to 42 and read it for verification (just for fun) */
    val = 42;
    set_register(kvm_regs.regs.regs[2], &val, "x2");
    val = 0;
    get_register(kvm_regs.regs.regs[2], &val, "x2");

    /* Repeatedly run code and handle VM exits. */
    printf("Running code\n");
    bool done = false;
    for (int i = 0; i < 10 && !done; i++) {
        printf("Loop %d\n", i);
        ret = ioctl(vcpufd, KVM_RUN, NULL);
        printf("Loop %d\n", i);
        if (ret < 0) {
            printf("System call 'KVM_RUN' failed: %s\n", strerror(errno));
            return ret;
        }

        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("KVM_EXIT_HLT\n");
                done = true;
                break;
            case KVM_EXIT_IO:
                printf("KVM_EXIT_IO\n");
                break;
            case KVM_EXIT_FAIL_ENTRY:
                printf("KVM_EXIT_FAIL_ENTRY\n");
                break;
            case KVM_EXIT_INTERNAL_ERROR:
                printf("KVM_EXIT_INTERNAL_ERROR\n");
                break;
            default:
                printf("KVM_EXIT other\n");
        }
    }

    /* Read register PC should not be 0 anymore */
    get_register(kvm_regs.regs.pc, &val, "PC");

    return 0;
}
