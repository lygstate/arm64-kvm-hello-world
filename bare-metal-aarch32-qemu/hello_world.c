#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

volatile uint32_t *const GICD_CTLR = (uint32_t *)0x080A0000;
volatile extern const char _start[];

void smc_call_bare(uint32_t *input, uint32_t *output)
{
  __asm__ volatile(
      "stmdb     sp!, {r0-r7} \n"
      "ldr       r12, [sp, #(4 * 0)] \n"
      "ldmia     r12, {r0-r7} \n"
      "smc       #0 \n"
      "ldr       r12, [sp, #(4 * 1)] \n"
      "stmia     r12, {r0-r7} \n"
      "ldmfd     sp!, {r0-r7} \n");
}

void smc_call(uint32_t *input, uint32_t *output)
{
  smc_call_bare(input, output);
  __asm__ __volatile__("isb; dsb");
}

/* https://stackoverflow.com/questions/58399436/qemu-aarch64-virt-machine-smp-cpus-starting-in-running-vs-halted-state */
int32_t start_slave_core_smc(int cpu_pos)
{
  uint32_t input[8] = {0};
  int32_t output[8] = {0};
  uint64_t physic_addr_start = (uint64_t)(uintptr_t)_start;
  input[0] = 0x84000003;
  input[1] = cpu_pos;
  input[2] = (uint32_t)(physic_addr_start & 0xFFFFFFFF);

  smc_call(input, output);

#if 0
  printf("start_slave_core_smc output 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    output[0], output[1], output[2], output[3],
    output[4], output[5], output[6], output[7]);
#endif
  return output[0];
}

void hvc_call_bare(uint32_t *input, uint32_t *output)
{
  __asm__ volatile(
      "stmdb     sp!, {r0-r7} \n"
      "ldr       r12, [sp, #(4 * 0)] \n"
      "ldmia     r12, {r0-r7} \n"
      "hvc       #0 \n"
      "ldr       r12, [sp, #(4 * 1)] \n"
      "stmia     r12, {r0-r7} \n"
      "ldmfd     sp!, {r0-r7} \n");
}

void hvc_call(uint32_t *input, uint32_t *output)
{
  hvc_call_bare(input, output);
  __asm__ __volatile__("isb; dsb");
}


/* https://stackoverflow.com/questions/20055754/arm-start-wakeup-bringup-the-other-cpu-cores-aps-and-pass-execution-start-addre/53473447#53473447 */
int32_t start_slave_core_hvc(int cpu_pos)
{
  uint32_t input[8] = {0};
  int32_t output[8] = {0};
  uint64_t physic_addr_start = (uint64_t)(uintptr_t)_start;
  input[0] = 0x84000003;
  input[1] = cpu_pos;
  input[2] = (uint32_t)(physic_addr_start & 0xFFFFFFFF);
  input[3] = 0;
  // printf("start_slave_core input:0x%x %d 0x%x!\n", input[0], input[1], input[2]);

  hvc_call(input, output);
#if 0
  printf("start_slave_core_hvc output 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    output[0], output[1], output[2], output[3],
    output[4], output[5], output[6], output[7]);
#endif
  return output[0];
}
int start_core(int core_id)
{
  int result = -2;
  int32_t smc_result = start_slave_core_smc(core_id);
  if (smc_result == -1)
  {
    result = start_slave_core_hvc(core_id);
  }
  else
  {
    result = 0;
  }
  return result;
}

/* https://developer.arm.com/documentation/ddi0500/d/system-control/aarch32-register-descriptions/multiprocessor-affinity-register */
uint32_t arm_mpidr_get(void) {
  uint32_t rval;
  asm volatile("mrc p15,0,%0,c0,c0,5" : "=r"(rval));
  return rval & 0xFF;
}

void system_off()
{
  uint32_t input[8] = {0};
  uint32_t output[8] = {0};
  input[0] = 0x84000008; /* SYSTEM_OFF function ID */
  hvc_call(input, output);
}



bool main_done = false;

void main()
{
  uint32_t gicd_ctlr_value = *GICD_CTLR;
  uint32_t process_id = arm_mpidr_get();
  if (process_id) {
    while (!main_done);
  }
  printf("Hello World gicd_ctlr_value: 0x%08x process_id: 0x%08x\n", gicd_ctlr_value, process_id);
  if (process_id == 0) {

    printf("start ret:0x%08x\n", start_core(1));
  }
  main_done = true;
  for (;;) {
  }
  system_off();
  printf("System off finished!\n");
}
