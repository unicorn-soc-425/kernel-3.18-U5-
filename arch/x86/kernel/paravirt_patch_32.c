#include <asm/paravirt.h>

DEF_NATIVE(pv_irq_ops, irq_disable, "cli");
DEF_NATIVE(pv_irq_ops, irq_enable, "sti");
DEF_NATIVE(pv_irq_ops, restore_fl, "push %eax; popf");
DEF_NATIVE(pv_irq_ops, save_fl, "pushf; pop %eax");
DEF_NATIVE(pv_cpu_ops, iret, "iret");
<<<<<<< HEAD
DEF_NATIVE(pv_cpu_ops, irq_enable_sysexit, "sti; sysexit");
=======
>>>>>>> v4.9.227
DEF_NATIVE(pv_mmu_ops, read_cr2, "mov %cr2, %eax");
DEF_NATIVE(pv_mmu_ops, write_cr3, "mov %eax, %cr3");
DEF_NATIVE(pv_mmu_ops, read_cr3, "mov %cr3, %eax");
DEF_NATIVE(pv_cpu_ops, clts, "clts");
<<<<<<< HEAD
DEF_NATIVE(pv_cpu_ops, read_tsc, "rdtsc");
=======

#if defined(CONFIG_PARAVIRT_SPINLOCKS)
DEF_NATIVE(pv_lock_ops, queued_spin_unlock, "movb $0, (%eax)");
#endif
>>>>>>> v4.9.227

unsigned paravirt_patch_ident_32(void *insnbuf, unsigned len)
{
	/* arg in %eax, return in %eax */
	return 0;
}

unsigned paravirt_patch_ident_64(void *insnbuf, unsigned len)
{
	/* arg in %edx:%eax, return in %edx:%eax */
	return 0;
}

<<<<<<< HEAD
=======
extern bool pv_is_native_spin_unlock(void);

>>>>>>> v4.9.227
unsigned native_patch(u8 type, u16 clobbers, void *ibuf,
		      unsigned long addr, unsigned len)
{
	const unsigned char *start, *end;
	unsigned ret;

#define PATCH_SITE(ops, x)					\
		case PARAVIRT_PATCH(ops.x):			\
			start = start_##ops##_##x;		\
			end = end_##ops##_##x;			\
			goto patch_site
	switch (type) {
		PATCH_SITE(pv_irq_ops, irq_disable);
		PATCH_SITE(pv_irq_ops, irq_enable);
		PATCH_SITE(pv_irq_ops, restore_fl);
		PATCH_SITE(pv_irq_ops, save_fl);
		PATCH_SITE(pv_cpu_ops, iret);
<<<<<<< HEAD
		PATCH_SITE(pv_cpu_ops, irq_enable_sysexit);
=======
>>>>>>> v4.9.227
		PATCH_SITE(pv_mmu_ops, read_cr2);
		PATCH_SITE(pv_mmu_ops, read_cr3);
		PATCH_SITE(pv_mmu_ops, write_cr3);
		PATCH_SITE(pv_cpu_ops, clts);
<<<<<<< HEAD
		PATCH_SITE(pv_cpu_ops, read_tsc);

	patch_site:
		ret = paravirt_patch_insns(ibuf, len, start, end);
		break;
=======
#if defined(CONFIG_PARAVIRT_SPINLOCKS)
		case PARAVIRT_PATCH(pv_lock_ops.queued_spin_unlock):
			if (pv_is_native_spin_unlock()) {
				start = start_pv_lock_ops_queued_spin_unlock;
				end   = end_pv_lock_ops_queued_spin_unlock;
				goto patch_site;
			}
#endif
>>>>>>> v4.9.227

	default:
		ret = paravirt_patch_default(type, clobbers, ibuf, addr, len);
		break;
<<<<<<< HEAD
=======

patch_site:
		ret = paravirt_patch_insns(ibuf, len, start, end);
		break;
>>>>>>> v4.9.227
	}
#undef PATCH_SITE
	return ret;
}
