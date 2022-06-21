/*
 * Copyright (C) 2014 Linaro Ltd. <ard.biesheuvel@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_CPUFEATURE_H
#define __ASM_CPUFEATURE_H

<<<<<<< HEAD
=======
#include <asm/cpucaps.h>
>>>>>>> v4.9.227
#include <asm/hwcap.h>
#include <asm/sysreg.h>

/*
 * In the arm64 world (as in the ARM world), elf_hwcap is used both internally
 * in the kernel and for user space to keep track of which optional features
 * are supported by the current system. So let's map feature 'x' to HWCAP_x.
 * Note that HWCAP_x constants are bit fields so we need to take the log.
 */

#define MAX_CPU_FEATURES	(8 * sizeof(elf_hwcap))
#define cpu_feature(x)		ilog2(HWCAP_ ## x)

<<<<<<< HEAD
#define ARM64_WORKAROUND_CLEAN_CACHE		0
#define ARM64_WORKAROUND_DEVICE_LOAD_ACQUIRE	1
#define ARM64_WORKAROUND_845719			2
#define ARM64_HAS_SYSREG_GIC_CPUIF		3
#define ARM64_HAS_PAN				4
#define ARM64_HAS_UAO				5
#define ARM64_ALT_PAN_NOT_UAO			6
#define ARM64_HARDEN_BRANCH_PREDICTOR		7
#define ARM64_UNMAP_KERNEL_AT_EL0		23

#define ARM64_NCAPS				24

#ifndef __ASSEMBLY__

=======
#ifndef __ASSEMBLY__

#include <linux/bug.h>
#include <linux/jump_label.h>
>>>>>>> v4.9.227
#include <linux/kernel.h>

/* CPU feature register tracking */
enum ftr_type {
<<<<<<< HEAD
	FTR_EXACT,	/* Use a predefined safe value */
	FTR_LOWER_SAFE,	/* Smaller value is safe */
	FTR_HIGHER_SAFE,/* Bigger value is safe */
=======
	FTR_EXACT,			/* Use a predefined safe value */
	FTR_LOWER_SAFE,			/* Smaller value is safe */
	FTR_HIGHER_SAFE,		/* Bigger value is safe */
	FTR_HIGHER_OR_ZERO_SAFE,	/* Bigger value is safe, but 0 is biggest */
>>>>>>> v4.9.227
};

#define FTR_STRICT	true	/* SANITY check strict matching required */
#define FTR_NONSTRICT	false	/* SANITY check ignored */

#define FTR_SIGNED	true	/* Value should be treated as signed */
#define FTR_UNSIGNED	false	/* Value should be treated as unsigned */

struct arm64_ftr_bits {
	bool		sign;	/* Value is signed ? */
	bool		strict;	/* CPU Sanity check: strict matching required ? */
	enum ftr_type	type;
	u8		shift;
	u8		width;
<<<<<<< HEAD
	s64		safe_val; /* safe value for discrete features */
=======
	s64		safe_val; /* safe value for FTR_EXACT features */
>>>>>>> v4.9.227
};

/*
 * @arm64_ftr_reg - Feature register
 * @strict_mask		Bits which should match across all CPUs for sanity.
 * @sys_val		Safe value across the CPUs (system view)
 */
struct arm64_ftr_reg {
<<<<<<< HEAD
	u32			sys_id;
	const char		*name;
	u64			strict_mask;
	u64			sys_val;
	struct arm64_ftr_bits	*ftr_bits;
=======
	const char			*name;
	u64				strict_mask;
	u64				sys_val;
	const struct arm64_ftr_bits	*ftr_bits;
};

extern struct arm64_ftr_reg arm64_ftr_reg_ctrel0;

/* scope of capability check */
enum {
	SCOPE_SYSTEM,
	SCOPE_LOCAL_CPU,
>>>>>>> v4.9.227
};

struct arm64_cpu_capabilities {
	const char *desc;
	u16 capability;
<<<<<<< HEAD
	bool (*matches)(const struct arm64_cpu_capabilities *);
	void (*enable)(void *);		/* Called on all active CPUs */
=======
	int def_scope;			/* default scope */
	bool (*matches)(const struct arm64_cpu_capabilities *caps, int scope);
	int (*enable)(void *);		/* Called on all active CPUs */
>>>>>>> v4.9.227
	union {
		struct {	/* To be used for erratum handling only */
			u32 midr_model;
			u32 midr_range_min, midr_range_max;
		};

		struct {	/* Feature register checking */
			u32 sys_reg;
<<<<<<< HEAD
			int field_pos;
			int min_field_value;
			int hwcap_type;
=======
			u8 field_pos;
			u8 min_field_value;
			u8 hwcap_type;
			bool sign;
>>>>>>> v4.9.227
			unsigned long hwcap;
		};
	};
};

extern DECLARE_BITMAP(cpu_hwcaps, ARM64_NCAPS);
<<<<<<< HEAD
=======
extern struct static_key_false cpu_hwcap_keys[ARM64_NCAPS];
extern struct static_key_false arm64_const_caps_ready;

bool this_cpu_has_cap(unsigned int cap);
>>>>>>> v4.9.227

static inline bool cpu_have_feature(unsigned int num)
{
	return elf_hwcap & (1UL << num);
}

<<<<<<< HEAD
=======
/* System capability check for constant caps */
static inline bool __cpus_have_const_cap(int num)
{
	if (num >= ARM64_NCAPS)
		return false;
	return static_branch_unlikely(&cpu_hwcap_keys[num]);
}

>>>>>>> v4.9.227
static inline bool cpus_have_cap(unsigned int num)
{
	if (num >= ARM64_NCAPS)
		return false;
	return test_bit(num, cpu_hwcaps);
}

<<<<<<< HEAD
static inline void cpus_set_cap(unsigned int num)
{
	if (num >= ARM64_NCAPS)
		pr_warn("Attempt to set an illegal CPU capability (%d >= %d)\n",
			num, ARM64_NCAPS);
	else
		__set_bit(num, cpu_hwcaps);
}

static inline int __attribute_const__
cpuid_feature_extract_field_width(u64 features, int field, int width)
=======
static inline bool cpus_have_const_cap(int num)
{
	if (static_branch_likely(&arm64_const_caps_ready))
		return __cpus_have_const_cap(num);
	else
		return cpus_have_cap(num);
}

static inline void cpus_set_cap(unsigned int num)
{
	if (num >= ARM64_NCAPS) {
		pr_warn("Attempt to set an illegal CPU capability (%d >= %d)\n",
			num, ARM64_NCAPS);
	} else {
		__set_bit(num, cpu_hwcaps);
	}
}

static inline int __attribute_const__
cpuid_feature_extract_signed_field_width(u64 features, int field, int width)
>>>>>>> v4.9.227
{
	return (s64)(features << (64 - width - field)) >> (64 - width);
}

static inline int __attribute_const__
<<<<<<< HEAD
cpuid_feature_extract_field(u64 features, int field)
{
	return cpuid_feature_extract_field_width(features, field, 4);
=======
cpuid_feature_extract_signed_field(u64 features, int field)
{
	return cpuid_feature_extract_signed_field_width(features, field, 4);
>>>>>>> v4.9.227
}

static inline unsigned int __attribute_const__
cpuid_feature_extract_unsigned_field_width(u64 features, int field, int width)
{
	return (u64)(features << (64 - width - field)) >> (64 - width);
}

static inline unsigned int __attribute_const__
cpuid_feature_extract_unsigned_field(u64 features, int field)
{
	return cpuid_feature_extract_unsigned_field_width(features, field, 4);
}

<<<<<<< HEAD
static inline u64 arm64_ftr_mask(struct arm64_ftr_bits *ftrp)
=======
static inline u64 arm64_ftr_mask(const struct arm64_ftr_bits *ftrp)
>>>>>>> v4.9.227
{
	return (u64)GENMASK(ftrp->shift + ftrp->width - 1, ftrp->shift);
}

<<<<<<< HEAD
static inline s64 arm64_ftr_value(struct arm64_ftr_bits *ftrp, u64 val)
{
	return ftrp->sign ?
		cpuid_feature_extract_field_width(val, ftrp->shift, ftrp->width) :
		cpuid_feature_extract_unsigned_field_width(val, ftrp->shift, ftrp->width);
=======
static inline int __attribute_const__
cpuid_feature_extract_field(u64 features, int field, bool sign)
{
	return (sign) ?
		cpuid_feature_extract_signed_field(features, field) :
		cpuid_feature_extract_unsigned_field(features, field);
}

static inline s64 arm64_ftr_value(const struct arm64_ftr_bits *ftrp, u64 val)
{
	return (s64)cpuid_feature_extract_field(val, ftrp->shift, ftrp->sign);
>>>>>>> v4.9.227
}

static inline bool id_aa64mmfr0_mixed_endian_el0(u64 mmfr0)
{
<<<<<<< HEAD
	return cpuid_feature_extract_field(mmfr0, ID_AA64MMFR0_BIGENDEL_SHIFT) == 0x1 ||
		cpuid_feature_extract_field(mmfr0, ID_AA64MMFR0_BIGENDEL0_SHIFT) == 0x1;
=======
	return cpuid_feature_extract_unsigned_field(mmfr0, ID_AA64MMFR0_BIGENDEL_SHIFT) == 0x1 ||
		cpuid_feature_extract_unsigned_field(mmfr0, ID_AA64MMFR0_BIGENDEL0_SHIFT) == 0x1;
}

static inline bool id_aa64pfr0_32bit_el0(u64 pfr0)
{
	u32 val = cpuid_feature_extract_unsigned_field(pfr0, ID_AA64PFR0_EL0_SHIFT);

	return val == ID_AA64PFR0_EL0_32BIT_64BIT;
>>>>>>> v4.9.227
}

void __init setup_cpu_features(void);

void update_cpu_capabilities(const struct arm64_cpu_capabilities *caps,
			    const char *info);
void enable_cpu_capabilities(const struct arm64_cpu_capabilities *caps);
<<<<<<< HEAD
void check_local_cpu_errata(void);
void __init enable_errata_workarounds(void);

#ifdef CONFIG_HOTPLUG_CPU
void verify_local_cpu_capabilities(void);
#else
static inline void verify_local_cpu_capabilities(void)
{
}
#endif
=======
void check_local_cpu_capabilities(void);

void update_cpu_errata_workarounds(void);
void __init enable_errata_workarounds(void);
void verify_local_cpu_errata_workarounds(void);
>>>>>>> v4.9.227

u64 read_system_reg(u32 id);

static inline bool cpu_supports_mixed_endian_el0(void)
{
<<<<<<< HEAD
	return id_aa64mmfr0_mixed_endian_el0(read_cpuid(SYS_ID_AA64MMFR0_EL1));
=======
	return id_aa64mmfr0_mixed_endian_el0(read_cpuid(ID_AA64MMFR0_EL1));
}

static inline bool system_supports_32bit_el0(void)
{
	return cpus_have_const_cap(ARM64_HAS_32BIT_EL0);
>>>>>>> v4.9.227
}

static inline bool system_supports_mixed_endian_el0(void)
{
	return id_aa64mmfr0_mixed_endian_el0(read_system_reg(SYS_ID_AA64MMFR0_EL1));
}

<<<<<<< HEAD
static inline bool system_uses_ttbr0_pan(void)
{
	return IS_ENABLED(CONFIG_ARM64_SW_TTBR0_PAN) &&
		!cpus_have_cap(ARM64_HAS_PAN);
}

=======
#define ARM64_SSBD_UNKNOWN		-1
#define ARM64_SSBD_FORCE_DISABLE	0
#define ARM64_SSBD_KERNEL		1
#define ARM64_SSBD_FORCE_ENABLE		2
#define ARM64_SSBD_MITIGATED		3

static inline int arm64_get_ssbd_state(void)
{
#ifdef CONFIG_ARM64_SSBD
	extern int ssbd_state;
	return ssbd_state;
#else
	return ARM64_SSBD_UNKNOWN;
#endif
}

#ifdef CONFIG_ARM64_SSBD
void arm64_set_ssbd_mitigation(bool state);
#else
static inline void arm64_set_ssbd_mitigation(bool state) {}
#endif

>>>>>>> v4.9.227
#endif /* __ASSEMBLY__ */

#endif
