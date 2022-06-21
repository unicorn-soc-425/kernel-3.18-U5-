#ifndef __ARCH_ARM_FAULT_H
#define __ARCH_ARM_FAULT_H

/*
 * Fault status register encodings.  We steal bit 31 for our own purposes.
 */
#define FSR_LNX_PF		(1 << 31)
<<<<<<< HEAD
=======
#define FSR_CM			(1 << 13)
>>>>>>> v4.9.227
#define FSR_WRITE		(1 << 11)
#define FSR_FS4			(1 << 10)
#define FSR_FS3_0		(15)
#define FSR_FS5_0		(0x3f)

#ifdef CONFIG_ARM_LPAE
<<<<<<< HEAD
=======
#define FSR_FS_AEA		17

>>>>>>> v4.9.227
static inline int fsr_fs(unsigned int fsr)
{
	return fsr & FSR_FS5_0;
}
#else
<<<<<<< HEAD
=======
#define FSR_FS_AEA		22

>>>>>>> v4.9.227
static inline int fsr_fs(unsigned int fsr)
{
	return (fsr & FSR_FS3_0) | (fsr & FSR_FS4) >> 6;
}
#endif

void do_bad_area(unsigned long addr, unsigned int fsr, struct pt_regs *regs);
<<<<<<< HEAD
unsigned long search_exception_table(unsigned long addr);
=======
void early_abt_enable(void);
>>>>>>> v4.9.227

#endif	/* __ARCH_ARM_FAULT_H */
