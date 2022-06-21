/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * KVM/MIPS: Binary Patching for privileged instructions, reduces traps.
 *
 * Copyright (C) 2012  MIPS Technologies, Inc.  All rights reserved.
 * Authors: Sanjay Lal <sanjayl@kymasys.com>
 */

#include <linux/errno.h>
#include <linux/err.h>
<<<<<<< HEAD
#include <linux/kvm_host.h>
#include <linux/module.h>
=======
#include <linux/highmem.h>
#include <linux/kvm_host.h>
>>>>>>> v4.9.227
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/bootmem.h>
#include <asm/cacheflush.h>

#include "commpage.h"

<<<<<<< HEAD
#define SYNCI_TEMPLATE  0x041f0000
#define SYNCI_BASE(x)   (((x) >> 21) & 0x1f)
#define SYNCI_OFFSET    ((x) & 0xffff)

#define LW_TEMPLATE     0x8c000000
#define CLEAR_TEMPLATE  0x00000020
#define SW_TEMPLATE     0xac000000

int kvm_mips_trans_cache_index(uint32_t inst, uint32_t *opc,
			       struct kvm_vcpu *vcpu)
{
	int result = 0;
	unsigned long kseg0_opc;
	uint32_t synci_inst = 0x0;

	/* Replace the CACHE instruction, with a NOP */
	kseg0_opc =
	    CKSEG0ADDR(kvm_mips_translate_guest_kseg0_to_hpa
		       (vcpu, (unsigned long) opc));
	memcpy((void *)kseg0_opc, (void *)&synci_inst, sizeof(uint32_t));
	local_flush_icache_range(kseg0_opc, kseg0_opc + 32);

	return result;
=======
/**
 * kvm_mips_trans_replace() - Replace trapping instruction in guest memory.
 * @vcpu:	Virtual CPU.
 * @opc:	PC of instruction to replace.
 * @replace:	Instruction to write
 */
static int kvm_mips_trans_replace(struct kvm_vcpu *vcpu, u32 *opc,
				  union mips_instruction replace)
{
	unsigned long paddr, flags;
	void *vaddr;

	if (KVM_GUEST_KSEGX((unsigned long)opc) == KVM_GUEST_KSEG0) {
		paddr = kvm_mips_translate_guest_kseg0_to_hpa(vcpu,
							    (unsigned long)opc);
		vaddr = kmap_atomic(pfn_to_page(PHYS_PFN(paddr)));
		vaddr += paddr & ~PAGE_MASK;
		memcpy(vaddr, (void *)&replace, sizeof(u32));
		local_flush_icache_range((unsigned long)vaddr,
					 (unsigned long)vaddr + 32);
		kunmap_atomic(vaddr);
	} else if (KVM_GUEST_KSEGX((unsigned long) opc) == KVM_GUEST_KSEG23) {
		local_irq_save(flags);
		memcpy((void *)opc, (void *)&replace, sizeof(u32));
		__local_flush_icache_user_range((unsigned long)opc,
						(unsigned long)opc + 32);
		local_irq_restore(flags);
	} else {
		kvm_err("%s: Invalid address: %p\n", __func__, opc);
		return -EFAULT;
	}

	return 0;
}

int kvm_mips_trans_cache_index(union mips_instruction inst, u32 *opc,
			       struct kvm_vcpu *vcpu)
{
	union mips_instruction nop_inst = { 0 };

	/* Replace the CACHE instruction, with a NOP */
	return kvm_mips_trans_replace(vcpu, opc, nop_inst);
>>>>>>> v4.9.227
}

/*
 * Address based CACHE instructions are transformed into synci(s). A little
 * heavy for just D-cache invalidates, but avoids an expensive trap
 */
<<<<<<< HEAD
int kvm_mips_trans_cache_va(uint32_t inst, uint32_t *opc,
			    struct kvm_vcpu *vcpu)
{
	int result = 0;
	unsigned long kseg0_opc;
	uint32_t synci_inst = SYNCI_TEMPLATE, base, offset;

	base = (inst >> 21) & 0x1f;
	offset = inst & 0xffff;
	synci_inst |= (base << 21);
	synci_inst |= offset;

	kseg0_opc =
	    CKSEG0ADDR(kvm_mips_translate_guest_kseg0_to_hpa
		       (vcpu, (unsigned long) opc));
	memcpy((void *)kseg0_opc, (void *)&synci_inst, sizeof(uint32_t));
	local_flush_icache_range(kseg0_opc, kseg0_opc + 32);

	return result;
}

int kvm_mips_trans_mfc0(uint32_t inst, uint32_t *opc, struct kvm_vcpu *vcpu)
{
	int32_t rt, rd, sel;
	uint32_t mfc0_inst;
	unsigned long kseg0_opc, flags;

	rt = (inst >> 16) & 0x1f;
	rd = (inst >> 11) & 0x1f;
	sel = inst & 0x7;

	if ((rd == MIPS_CP0_ERRCTL) && (sel == 0)) {
		mfc0_inst = CLEAR_TEMPLATE;
		mfc0_inst |= ((rt & 0x1f) << 16);
	} else {
		mfc0_inst = LW_TEMPLATE;
		mfc0_inst |= ((rt & 0x1f) << 16);
		mfc0_inst |=
		    offsetof(struct mips_coproc,
			     reg[rd][sel]) + offsetof(struct kvm_mips_commpage,
						      cop0);
	}

	if (KVM_GUEST_KSEGX(opc) == KVM_GUEST_KSEG0) {
		kseg0_opc =
		    CKSEG0ADDR(kvm_mips_translate_guest_kseg0_to_hpa
			       (vcpu, (unsigned long) opc));
		memcpy((void *)kseg0_opc, (void *)&mfc0_inst, sizeof(uint32_t));
		local_flush_icache_range(kseg0_opc, kseg0_opc + 32);
	} else if (KVM_GUEST_KSEGX((unsigned long) opc) == KVM_GUEST_KSEG23) {
		local_irq_save(flags);
		memcpy((void *)opc, (void *)&mfc0_inst, sizeof(uint32_t));
		local_flush_icache_range((unsigned long)opc,
					 (unsigned long)opc + 32);
		local_irq_restore(flags);
	} else {
		kvm_err("%s: Invalid address: %p\n", __func__, opc);
		return -EFAULT;
	}

	return 0;
}

int kvm_mips_trans_mtc0(uint32_t inst, uint32_t *opc, struct kvm_vcpu *vcpu)
{
	int32_t rt, rd, sel;
	uint32_t mtc0_inst = SW_TEMPLATE;
	unsigned long kseg0_opc, flags;

	rt = (inst >> 16) & 0x1f;
	rd = (inst >> 11) & 0x1f;
	sel = inst & 0x7;

	mtc0_inst |= ((rt & 0x1f) << 16);
	mtc0_inst |=
	    offsetof(struct mips_coproc,
		     reg[rd][sel]) + offsetof(struct kvm_mips_commpage, cop0);

	if (KVM_GUEST_KSEGX(opc) == KVM_GUEST_KSEG0) {
		kseg0_opc =
		    CKSEG0ADDR(kvm_mips_translate_guest_kseg0_to_hpa
			       (vcpu, (unsigned long) opc));
		memcpy((void *)kseg0_opc, (void *)&mtc0_inst, sizeof(uint32_t));
		local_flush_icache_range(kseg0_opc, kseg0_opc + 32);
	} else if (KVM_GUEST_KSEGX((unsigned long) opc) == KVM_GUEST_KSEG23) {
		local_irq_save(flags);
		memcpy((void *)opc, (void *)&mtc0_inst, sizeof(uint32_t));
		local_flush_icache_range((unsigned long)opc,
					 (unsigned long)opc + 32);
		local_irq_restore(flags);
	} else {
		kvm_err("%s: Invalid address: %p\n", __func__, opc);
		return -EFAULT;
	}

	return 0;
=======
int kvm_mips_trans_cache_va(union mips_instruction inst, u32 *opc,
			    struct kvm_vcpu *vcpu)
{
	union mips_instruction synci_inst = { 0 };

	synci_inst.i_format.opcode = bcond_op;
	synci_inst.i_format.rs = inst.i_format.rs;
	synci_inst.i_format.rt = synci_op;
	if (cpu_has_mips_r6)
		synci_inst.i_format.simmediate = inst.spec3_format.simmediate;
	else
		synci_inst.i_format.simmediate = inst.i_format.simmediate;

	return kvm_mips_trans_replace(vcpu, opc, synci_inst);
}

int kvm_mips_trans_mfc0(union mips_instruction inst, u32 *opc,
			struct kvm_vcpu *vcpu)
{
	union mips_instruction mfc0_inst = { 0 };
	u32 rd, sel;

	rd = inst.c0r_format.rd;
	sel = inst.c0r_format.sel;

	if (rd == MIPS_CP0_ERRCTL && sel == 0) {
		mfc0_inst.r_format.opcode = spec_op;
		mfc0_inst.r_format.rd = inst.c0r_format.rt;
		mfc0_inst.r_format.func = add_op;
	} else {
		mfc0_inst.i_format.opcode = lw_op;
		mfc0_inst.i_format.rt = inst.c0r_format.rt;
		mfc0_inst.i_format.simmediate = KVM_GUEST_COMMPAGE_ADDR |
			offsetof(struct kvm_mips_commpage, cop0.reg[rd][sel]);
#ifdef CONFIG_CPU_BIG_ENDIAN
		if (sizeof(vcpu->arch.cop0->reg[0][0]) == 8)
			mfc0_inst.i_format.simmediate |= 4;
#endif
	}

	return kvm_mips_trans_replace(vcpu, opc, mfc0_inst);
}

int kvm_mips_trans_mtc0(union mips_instruction inst, u32 *opc,
			struct kvm_vcpu *vcpu)
{
	union mips_instruction mtc0_inst = { 0 };
	u32 rd, sel;

	rd = inst.c0r_format.rd;
	sel = inst.c0r_format.sel;

	mtc0_inst.i_format.opcode = sw_op;
	mtc0_inst.i_format.rt = inst.c0r_format.rt;
	mtc0_inst.i_format.simmediate = KVM_GUEST_COMMPAGE_ADDR |
		offsetof(struct kvm_mips_commpage, cop0.reg[rd][sel]);
#ifdef CONFIG_CPU_BIG_ENDIAN
	if (sizeof(vcpu->arch.cop0->reg[0][0]) == 8)
		mtc0_inst.i_format.simmediate |= 4;
#endif

	return kvm_mips_trans_replace(vcpu, opc, mtc0_inst);
>>>>>>> v4.9.227
}
