.set FPEXC_EN,		0x40000000		/* FPU enable bit, (1 << 30) */

.global _start
_start:
    ldr r12, =stack_top     /* Retrieve initial stack address */
    mov sp, r12             /* Set stack address */

    /* Enabling Advanced SIMD and floating-point support */
    /* https://developer.arm.com/documentation/ddi0409/i/programmers-model/about-this-programmers-model/enabling-advanced-simd-and-floating-point-support?lang=en */
    mrc  p15,0,r0,c1,c0,2   /* Read CPACR into r0 */
    orr  r0,r0,#(3<<20)     /* OR in User and Privileged access for CP10 */
    orr  r0,r0,#(3<<22)     /* OR in User and Privileged access for CP11 */
    bic  r0, r0, #(3<<30)   /* Clear ASEDIS/D32DIS if set */
    mcr  p15,0,r0,c1,c0,2   /* Store new access permissions into CPACR */
    isb                     /* Ensure side-effect of CPACR is visible */

    /* Enable vfp */
    vmrs r1, FPEXC          /* read the exception register */
    orr r1,r1, #FPEXC_EN    /* set VFP enable bit, leave the others in orig state */
    vmsr FPEXC, r1          /* write back the exception register */

    bl main                 /* Branch to main() */

.global system_off
system_off:
    ldr r0, =0x84000008     /* SYSTEM_OFF function ID */
    hvc #0                  /* Hypervisor call */

sleep:                      /* This point should not be reached */
    wfi                     /* Wait for interrupt */
    b sleep                 /* Endless loop */

