<<<<<<< HEAD
/* Copyright (c) 2011-2017, The Linux Foundation. All rights reserved.
=======
/*
 * Copyright (c) 2011-2014, The Linux Foundation. All rights reserved.
 * Copyright (c) 2014,2015, Linaro Ltd.
 *
 * SAW power controller driver
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
<<<<<<< HEAD
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/slab.h>

#include "spm_driver.h"

#define MSM_SPM_PMIC_STATE_IDLE  0

enum {
	MSM_SPM_DEBUG_SHADOW = 1U << 0,
	MSM_SPM_DEBUG_VCTL = 1U << 1,
};

static int msm_spm_debug_mask;
module_param_named(
	debug_mask, msm_spm_debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP
);

struct saw2_data {
	const char *ver_name;
	uint32_t major;
	uint32_t minor;
	uint32_t *spm_reg_offset_ptr;
};

static uint32_t msm_spm_reg_offsets_saw2_v2_1[MSM_SPM_REG_NR] = {
	[MSM_SPM_REG_SAW_SECURE]		= 0x00,
	[MSM_SPM_REG_SAW_ID]			= 0x04,
	[MSM_SPM_REG_SAW_CFG]			= 0x08,
	[MSM_SPM_REG_SAW_SPM_STS]		= 0x0C,
	[MSM_SPM_REG_SAW_AVS_STS]		= 0x10,
	[MSM_SPM_REG_SAW_PMIC_STS]		= 0x14,
	[MSM_SPM_REG_SAW_RST]			= 0x18,
	[MSM_SPM_REG_SAW_VCTL]			= 0x1C,
	[MSM_SPM_REG_SAW_AVS_CTL]		= 0x20,
	[MSM_SPM_REG_SAW_AVS_LIMIT]		= 0x24,
	[MSM_SPM_REG_SAW_AVS_DLY]		= 0x28,
	[MSM_SPM_REG_SAW_AVS_HYSTERESIS]	= 0x2C,
	[MSM_SPM_REG_SAW_SPM_CTL]		= 0x30,
	[MSM_SPM_REG_SAW_SPM_DLY]		= 0x34,
	[MSM_SPM_REG_SAW_PMIC_DATA_0]		= 0x40,
	[MSM_SPM_REG_SAW_PMIC_DATA_1]		= 0x44,
	[MSM_SPM_REG_SAW_PMIC_DATA_2]		= 0x48,
	[MSM_SPM_REG_SAW_PMIC_DATA_3]		= 0x4C,
	[MSM_SPM_REG_SAW_PMIC_DATA_4]		= 0x50,
	[MSM_SPM_REG_SAW_PMIC_DATA_5]		= 0x54,
	[MSM_SPM_REG_SAW_PMIC_DATA_6]		= 0x58,
	[MSM_SPM_REG_SAW_PMIC_DATA_7]		= 0x5C,
	[MSM_SPM_REG_SAW_SEQ_ENTRY]		= 0x80,
	[MSM_SPM_REG_SAW_VERSION]		= 0xFD0,
};

static uint32_t msm_spm_reg_offsets_saw2_v3_0[MSM_SPM_REG_NR] = {
	[MSM_SPM_REG_SAW_SECURE]		= 0x00,
	[MSM_SPM_REG_SAW_ID]			= 0x04,
	[MSM_SPM_REG_SAW_CFG]			= 0x08,
	[MSM_SPM_REG_SAW_SPM_STS]		= 0x0C,
	[MSM_SPM_REG_SAW_AVS_STS]		= 0x10,
	[MSM_SPM_REG_SAW_PMIC_STS]		= 0x14,
	[MSM_SPM_REG_SAW_RST]			= 0x18,
	[MSM_SPM_REG_SAW_VCTL]			= 0x1C,
	[MSM_SPM_REG_SAW_AVS_CTL]		= 0x20,
	[MSM_SPM_REG_SAW_AVS_LIMIT]		= 0x24,
	[MSM_SPM_REG_SAW_AVS_DLY]		= 0x28,
	[MSM_SPM_REG_SAW_AVS_HYSTERESIS]	= 0x2C,
	[MSM_SPM_REG_SAW_SPM_CTL]		= 0x30,
	[MSM_SPM_REG_SAW_SPM_DLY]		= 0x34,
	[MSM_SPM_REG_SAW_STS2]			= 0x38,
	[MSM_SPM_REG_SAW_PMIC_DATA_0]		= 0x40,
	[MSM_SPM_REG_SAW_PMIC_DATA_1]		= 0x44,
	[MSM_SPM_REG_SAW_PMIC_DATA_2]		= 0x48,
	[MSM_SPM_REG_SAW_PMIC_DATA_3]		= 0x4C,
	[MSM_SPM_REG_SAW_PMIC_DATA_4]		= 0x50,
	[MSM_SPM_REG_SAW_PMIC_DATA_5]		= 0x54,
	[MSM_SPM_REG_SAW_PMIC_DATA_6]		= 0x58,
	[MSM_SPM_REG_SAW_PMIC_DATA_7]		= 0x5C,
	[MSM_SPM_REG_SAW_SEQ_ENTRY]		= 0x400,
	[MSM_SPM_REG_SAW_VERSION]		= 0xFD0,
};

static uint32_t msm_spm_reg_offsets_saw2_v4_1[MSM_SPM_REG_NR] = {
	[MSM_SPM_REG_SAW_SECURE]		= 0xC00,
	[MSM_SPM_REG_SAW_ID]			= 0xC04,
	[MSM_SPM_REG_SAW_STS2]			= 0xC10,
	[MSM_SPM_REG_SAW_SPM_STS]		= 0xC0C,
	[MSM_SPM_REG_SAW_AVS_STS]		= 0xC14,
	[MSM_SPM_REG_SAW_PMIC_STS]		= 0xC18,
	[MSM_SPM_REG_SAW_RST]			= 0xC1C,
	[MSM_SPM_REG_SAW_VCTL]			= 0x900,
	[MSM_SPM_REG_SAW_AVS_CTL]		= 0x904,
	[MSM_SPM_REG_SAW_AVS_LIMIT]		= 0x908,
	[MSM_SPM_REG_SAW_AVS_DLY]		= 0x90C,
	[MSM_SPM_REG_SAW_SPM_CTL]		= 0x0,
	[MSM_SPM_REG_SAW_SPM_DLY]		= 0x4,
	[MSM_SPM_REG_SAW_CFG]			= 0x0C,
	[MSM_SPM_REG_SAW_PMIC_DATA_0]		= 0x40,
	[MSM_SPM_REG_SAW_PMIC_DATA_1]		= 0x44,
	[MSM_SPM_REG_SAW_PMIC_DATA_2]		= 0x48,
	[MSM_SPM_REG_SAW_PMIC_DATA_3]		= 0x4C,
	[MSM_SPM_REG_SAW_PMIC_DATA_4]		= 0x50,
	[MSM_SPM_REG_SAW_PMIC_DATA_5]		= 0x54,
	[MSM_SPM_REG_SAW_PMIC_DATA_6]		= 0x58,
	[MSM_SPM_REG_SAW_PMIC_DATA_7]		= 0x5C,
	[MSM_SPM_REG_SAW_SEQ_ENTRY]		= 0x400,
	[MSM_SPM_REG_SAW_VERSION]		= 0xFD0,
};

static struct saw2_data saw2_info[] = {
	[0] = {
		"SAW_v2.1",
		0x2,
		0x1,
		msm_spm_reg_offsets_saw2_v2_1,
	},
	[1] = {
		"SAW_v2.3",
		0x3,
		0x0,
		msm_spm_reg_offsets_saw2_v3_0,
	},
	[2] = {
		"SAW_v3.0",
		0x1,
		0x0,
		msm_spm_reg_offsets_saw2_v3_0,
	},
	[3] = {
		"SAW_v4.0",
		0x4,
		0x1,
		msm_spm_reg_offsets_saw2_v4_1,
	},
};

static uint32_t num_pmic_data;

static void msm_spm_drv_flush_shadow(struct msm_spm_driver_data *dev,
		unsigned int reg_index)
{
	BUG_ON(!dev);

	BUG_ON(!dev->reg_shadow);

	__raw_writel(dev->reg_shadow[reg_index],
		dev->reg_base_addr + dev->reg_offsets[reg_index]);
}

static void msm_spm_drv_load_shadow(struct msm_spm_driver_data *dev,
		unsigned int reg_index)
{
	dev->reg_shadow[reg_index] =
		__raw_readl(dev->reg_base_addr +
				dev->reg_offsets[reg_index]);
}

static inline uint32_t msm_spm_drv_get_num_spm_entry(
		struct msm_spm_driver_data *dev)
{
	BUG_ON(!dev);
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_ID);
	return (dev->reg_shadow[MSM_SPM_REG_SAW_ID] >> 24) & 0xFF;
}

static inline void msm_spm_drv_set_start_addr(
		struct msm_spm_driver_data *dev, uint32_t ctl)
{
	dev->reg_shadow[MSM_SPM_REG_SAW_SPM_CTL] = ctl;
}

static inline bool msm_spm_pmic_arb_present(struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_ID);
	return (dev->reg_shadow[MSM_SPM_REG_SAW_ID] >> 2) & 0x1;
}

static inline void msm_spm_drv_set_vctl2(struct msm_spm_driver_data *dev,
				uint32_t vlevel, uint32_t vctl_port)
{
	unsigned int pmic_data = 0;

	pmic_data |= vlevel;
	pmic_data |= (vctl_port & 0x7) << 16;

	dev->reg_shadow[MSM_SPM_REG_SAW_VCTL] &= ~0x700FF;
	dev->reg_shadow[MSM_SPM_REG_SAW_VCTL] |= pmic_data;

	dev->reg_shadow[MSM_SPM_REG_SAW_PMIC_DATA_3] &= ~0x700FF;
	dev->reg_shadow[MSM_SPM_REG_SAW_PMIC_DATA_3] |= pmic_data;

	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_VCTL);
	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_PMIC_DATA_3);
}

static inline uint32_t msm_spm_drv_get_num_pmic_data(
		struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_ID);
	mb();
	return (dev->reg_shadow[MSM_SPM_REG_SAW_ID] >> 4) & 0x7;
}

static inline uint32_t msm_spm_drv_get_sts_pmic_state(
		struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_PMIC_STS);
	return (dev->reg_shadow[MSM_SPM_REG_SAW_PMIC_STS] >> 16) &
				0x03;
}

uint32_t msm_spm_drv_get_sts_curr_pmic_data(
		struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_PMIC_STS);
	return dev->reg_shadow[MSM_SPM_REG_SAW_PMIC_STS] & 0x300FF;
}

static inline void msm_spm_drv_get_saw2_ver(struct msm_spm_driver_data *dev,
		uint32_t *major, uint32_t *minor)
{
	uint32_t val = 0;

	dev->reg_shadow[MSM_SPM_REG_SAW_VERSION] =
			__raw_readl(dev->reg_base_addr + dev->ver_reg);

	val = dev->reg_shadow[MSM_SPM_REG_SAW_VERSION];

	*major = (val >> 28) & 0xF;
	*minor = (val >> 16) & 0xFFF;
}

inline int msm_spm_drv_set_spm_enable(
		struct msm_spm_driver_data *dev, bool enable)
{
	uint32_t value = enable ? 0x01 : 0x00;

	if (!dev)
		return -EINVAL;

	if ((dev->reg_shadow[MSM_SPM_REG_SAW_SPM_CTL] & 0x01) ^ value) {

		dev->reg_shadow[MSM_SPM_REG_SAW_SPM_CTL] &= ~0x1;
		dev->reg_shadow[MSM_SPM_REG_SAW_SPM_CTL] |= value;

		msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_SPM_CTL);
		wmb();
	}
	return 0;
}

int msm_spm_drv_get_avs_enable(struct msm_spm_driver_data *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] & 0x01;
}

int msm_spm_drv_set_avs_enable(struct msm_spm_driver_data *dev,
		 bool enable)
{
	uint32_t value = enable ? 0x1 : 0x0;

	if (!dev)
		return -EINVAL;

	if ((dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] & 0x1) ^ value) {
		dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] &= ~0x1;
		dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] |= value;

		msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
	}

	return 0;
}

int msm_spm_drv_set_avs_limit(struct msm_spm_driver_data *dev,
		uint32_t min_lvl, uint32_t max_lvl)
{
	uint32_t value = (max_lvl & 0xff) << 16 | (min_lvl & 0xff);

	if (!dev)
		return -EINVAL;

	dev->reg_shadow[MSM_SPM_REG_SAW_AVS_LIMIT] = value;

	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_LIMIT);

	return 0;
}

static int msm_spm_drv_avs_irq_mask(enum msm_spm_avs_irq irq)
{
	switch (irq) {
	case MSM_SPM_AVS_IRQ_MIN:
		return BIT(1);
	case MSM_SPM_AVS_IRQ_MAX:
		return BIT(2);
	default:
		return -EINVAL;
	}
}

int msm_spm_drv_set_avs_irq_enable(struct msm_spm_driver_data *dev,
		enum msm_spm_avs_irq irq, bool enable)
{
	int mask = msm_spm_drv_avs_irq_mask(irq);
	uint32_t value;

	if (!dev)
		return -EINVAL;
	else if (mask < 0)
		return mask;

	value = enable ? mask : 0;

	if ((dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] & mask) ^ value) {
		dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] &= ~mask;
		dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] |= value;
		msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
	}

	return 0;
}

int msm_spm_drv_avs_clear_irq(struct msm_spm_driver_data *dev,
		enum msm_spm_avs_irq irq)
{
	int mask = msm_spm_drv_avs_irq_mask(irq);

	if (!dev)
		return -EINVAL;
	else if (mask < 0)
		return mask;

	if (dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] & mask) {
		/*
		 * The interrupt status is cleared by disabling and then
		 * re-enabling the interrupt.
		 */
		dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] &= ~mask;
		msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
		dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] |= mask;
		msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
	}

	return 0;
}

void msm_spm_drv_flush_seq_entry(struct msm_spm_driver_data *dev)
{
	int i;
	int num_spm_entry = msm_spm_drv_get_num_spm_entry(dev);

	if (!dev) {
		__WARN();
		return;
	}

	for (i = 0; i < num_spm_entry; i++) {
		__raw_writel(dev->reg_seq_entry_shadow[i],
			dev->reg_base_addr
			+ dev->reg_offsets[MSM_SPM_REG_SAW_SEQ_ENTRY]
			+ 4 * i);
	}
	mb();
}

void dump_regs(struct msm_spm_driver_data *dev, int cpu)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_SPM_STS);
	mb();
	pr_err("CPU%d: spm register MSM_SPM_REG_SAW_SPM_STS: 0x%x\n", cpu,
			dev->reg_shadow[MSM_SPM_REG_SAW_SPM_STS]);
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_SPM_CTL);
	mb();
	pr_err("CPU%d: spm register MSM_SPM_REG_SAW_SPM_CTL: 0x%x\n", cpu,
			dev->reg_shadow[MSM_SPM_REG_SAW_SPM_CTL]);
}

int msm_spm_drv_write_seq_data(struct msm_spm_driver_data *dev,
		uint8_t *cmd, uint32_t *offset)
{
	uint32_t cmd_w;
	uint32_t offset_w = *offset / 4;
	uint8_t last_cmd;

	if (!cmd)
		return -EINVAL;

	while (1) {
		int i;
		cmd_w = 0;
		last_cmd = 0;
		cmd_w = dev->reg_seq_entry_shadow[offset_w];

		for (i = (*offset % 4); i < 4; i++) {
			last_cmd = *(cmd++);
			cmd_w |=  last_cmd << (i * 8);
			(*offset)++;
			if (last_cmd == 0x0f)
				break;
		}

		dev->reg_seq_entry_shadow[offset_w++] = cmd_w;
		if (last_cmd == 0x0f)
			break;
	}

	return 0;
}

int msm_spm_drv_set_low_power_mode(struct msm_spm_driver_data *dev,
		uint32_t ctl)
{

	/* SPM is configured to reset start address to zero after end of Program
	 */
	if (!dev)
		return -EINVAL;

	msm_spm_drv_set_start_addr(dev, ctl);

	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_SPM_CTL);
	wmb();

	if (msm_spm_debug_mask & MSM_SPM_DEBUG_SHADOW) {
		int i;
		for (i = 0; i < MSM_SPM_REG_NR; i++)
			pr_info("%s: reg %02x = 0x%08x\n", __func__,
				dev->reg_offsets[i], dev->reg_shadow[i]);
	}
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_SPM_STS);

	return 0;
}

uint32_t msm_spm_drv_get_vdd(struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_PMIC_STS);
	return dev->reg_shadow[MSM_SPM_REG_SAW_PMIC_STS] & 0xFF;
}

#ifdef CONFIG_MSM_AVS_HW
static bool msm_spm_drv_is_avs_enabled(struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
	return dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] & BIT(0);
}

static void msm_spm_drv_disable_avs(struct msm_spm_driver_data *dev)
{
	msm_spm_drv_load_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
	dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] &= ~BIT(27);
	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
}

static void msm_spm_drv_enable_avs(struct msm_spm_driver_data *dev)
{
	dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] |= BIT(27);
	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
}

static void msm_spm_drv_set_avs_vlevel(struct msm_spm_driver_data *dev,
		unsigned int vlevel)
{
	vlevel &= 0x3f;
	dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] &= ~0x7efc00;
	dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] |= ((vlevel - 4) << 10);
	dev->reg_shadow[MSM_SPM_REG_SAW_AVS_CTL] |= (vlevel << 17);
	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_AVS_CTL);
}

#else
static bool msm_spm_drv_is_avs_enabled(struct msm_spm_driver_data *dev)
{
	return false;
}

static void msm_spm_drv_disable_avs(struct msm_spm_driver_data *dev) { }

static void msm_spm_drv_enable_avs(struct msm_spm_driver_data *dev) { }

static void msm_spm_drv_set_avs_vlevel(struct msm_spm_driver_data *dev,
		unsigned int vlevel) { }
#endif

static inline int msm_spm_drv_validate_data(struct msm_spm_driver_data *dev,
					unsigned int vlevel, int vctl_port)
{
	int timeout_us = dev->vctl_timeout_us;
	uint32_t new_level;

	/* Confirm the voltage we set was what hardware sent and
	 * FSM is idle */
	do {
		udelay(1);
		new_level = msm_spm_drv_get_sts_curr_pmic_data(dev);

		/**
		 * VCTL_PORT has to be 0, for vlevel to be updated.
		 * If port is not 0, check for PMIC_STATE only.
		 */

		if (((new_level & 0x30000) == MSM_SPM_PMIC_STATE_IDLE) &&
				(vctl_port || ((new_level & 0xFF) == vlevel)))
			break;
	} while (--timeout_us);

	if (!timeout_us) {
		pr_err("Wrong level %#x\n", new_level);
		return -EIO;
	}

	if (msm_spm_debug_mask & MSM_SPM_DEBUG_VCTL)
		pr_info("%s: done, remaining timeout %u us\n",
			__func__, timeout_us);

	return 0;
}

int msm_spm_drv_set_vdd(struct msm_spm_driver_data *dev, unsigned int vlevel)
{
	uint32_t vlevel_set = vlevel;
	bool avs_enabled;
	int ret = 0;

	if (!dev)
		return -EINVAL;

	avs_enabled  = msm_spm_drv_is_avs_enabled(dev);

	if (!msm_spm_pmic_arb_present(dev))
		return -ENOSYS;

	if (msm_spm_debug_mask & MSM_SPM_DEBUG_VCTL)
		pr_info("%s: requesting vlevel %#x\n", __func__, vlevel);

	if (avs_enabled)
		msm_spm_drv_disable_avs(dev);

	if (dev->vctl_port_ub >= 0) {
		/**
		 * VCTL can send 8bit voltage level at once.
		 * Send lower 8bit first, vlevel change happens
		 * when upper 8bit is sent.
		 */
		vlevel = vlevel_set & 0xFF;
	}

	/* Kick the state machine back to idle */
	dev->reg_shadow[MSM_SPM_REG_SAW_RST] = 1;
	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_RST);

	msm_spm_drv_set_vctl2(dev, vlevel, dev->vctl_port);

	ret = msm_spm_drv_validate_data(dev, vlevel, dev->vctl_port);
	if (ret)
		goto set_vdd_bail;

	if (dev->vctl_port_ub >= 0) {
		/* Send upper 8bit of voltage level */
		vlevel = (vlevel_set >> 8) & 0xFF;

		/* Kick the state machine back to idle */
		dev->reg_shadow[MSM_SPM_REG_SAW_RST] = 1;
		msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_RST);

		/*
		 * Steps for sending for vctl port other than '0'
		 * Write VCTL register with pmic data and address index
		 * Perform system barrier
		 * Wait for 1us
		 * Read PMIC_STS register to make sure operation is complete
		 */
		msm_spm_drv_set_vctl2(dev, vlevel, dev->vctl_port_ub);

		mb(); /* To make sure data is sent before checking status */

		ret = msm_spm_drv_validate_data(dev, vlevel, dev->vctl_port_ub);
		if (ret)
			goto set_vdd_bail;
	}

	/* Set AVS min/max */
	if (avs_enabled) {
		msm_spm_drv_set_avs_vlevel(dev, vlevel_set);
		msm_spm_drv_enable_avs(dev);
	}

	return ret;

set_vdd_bail:
	if (avs_enabled)
		msm_spm_drv_enable_avs(dev);

	pr_err("%s: failed %#x vlevel setting in timeout %uus\n",
			__func__, vlevel_set, dev->vctl_timeout_us);
	return -EIO;
}

static int msm_spm_drv_get_pmic_port(struct msm_spm_driver_data *dev,
		enum msm_spm_pmic_port port)
{
	int index = -1;

	switch (port) {
	case MSM_SPM_PMIC_VCTL_PORT:
		index = dev->vctl_port;
		break;
	case MSM_SPM_PMIC_PHASE_PORT:
		index = dev->phase_port;
		break;
	case MSM_SPM_PMIC_PFM_PORT:
		index = dev->pfm_port;
		break;
	default:
		break;
	}

	return index;
}

int msm_spm_drv_set_pmic_data(struct msm_spm_driver_data *dev,
		enum msm_spm_pmic_port port, unsigned int data)
{
	unsigned int pmic_data = 0;
	unsigned int timeout_us = 0;
	int index = 0;

	if (!msm_spm_pmic_arb_present(dev))
		return -ENOSYS;

	index = msm_spm_drv_get_pmic_port(dev, port);
	if (index < 0)
		return -ENODEV;

	pmic_data |= data & 0xFF;
	pmic_data |= (index & 0x7) << 16;

	dev->reg_shadow[MSM_SPM_REG_SAW_VCTL] &= ~0x700FF;
	dev->reg_shadow[MSM_SPM_REG_SAW_VCTL] |= pmic_data;
	msm_spm_drv_flush_shadow(dev, MSM_SPM_REG_SAW_VCTL);
	mb();

	timeout_us = dev->vctl_timeout_us;
	/**
	 * Confirm the pmic data set was what hardware sent by
	 * checking the PMIC FSM state.
	 * We cannot use the sts_pmic_data and check it against
	 * the value like we do fot set_vdd, since the PMIC_STS
	 * is only updated for SAW_VCTL sent with port index 0.
	 */
	do {
		if (msm_spm_drv_get_sts_pmic_state(dev) ==
				MSM_SPM_PMIC_STATE_IDLE)
			break;
		udelay(1);
	} while (--timeout_us);

	if (!timeout_us) {
		pr_err("%s: failed, remaining timeout %u us, data %d\n",
				__func__, timeout_us, data);
		return -EIO;
	}

	return 0;
}

void msm_spm_drv_reinit(struct msm_spm_driver_data *dev, bool seq_write)
{
	int i;

	if (seq_write)
		msm_spm_drv_flush_seq_entry(dev);

	for (i = 0; i < MSM_SPM_REG_SAW_PMIC_DATA_0 + num_pmic_data; i++)
		msm_spm_drv_load_shadow(dev, i);

	for (i = MSM_SPM_REG_NR_INITIALIZE + 1; i < MSM_SPM_REG_NR; i++)
		msm_spm_drv_load_shadow(dev, i);
}

int msm_spm_drv_reg_init(struct msm_spm_driver_data *dev,
		struct msm_spm_platform_data *data)
{
	int i;
	bool found = false;

	dev->ver_reg = data->ver_reg;
	dev->reg_base_addr = data->reg_base_addr;
	msm_spm_drv_get_saw2_ver(dev, &dev->major, &dev->minor);
	for (i = 0; i < ARRAY_SIZE(saw2_info); i++)
		if (dev->major == saw2_info[i].major &&
			dev->minor == saw2_info[i].minor) {
			pr_debug("%s: Version found\n",
					saw2_info[i].ver_name);
			dev->reg_offsets = saw2_info[i].spm_reg_offset_ptr;
			found = true;
			break;
		}

	if (!found) {
		pr_err("%s: No SAW version found\n", __func__);
		BUG_ON(!found);
	}
	return 0;
}

void msm_spm_drv_upd_reg_shadow(struct msm_spm_driver_data *dev, int id,
		int val)
{
	dev->reg_shadow[id] = val;
	msm_spm_drv_flush_shadow(dev, id);
	/* Complete the above writes before other accesses */
	mb();
}

int msm_spm_drv_init(struct msm_spm_driver_data *dev,
		struct msm_spm_platform_data *data)
{
	int num_spm_entry;

	BUG_ON(!dev || !data);

	dev->vctl_port = data->vctl_port;
	dev->vctl_port_ub = data->vctl_port_ub;
	dev->phase_port = data->phase_port;
	dev->pfm_port = data->pfm_port;
	dev->reg_base_addr = data->reg_base_addr;
	memcpy(dev->reg_shadow, data->reg_init_values,
			sizeof(data->reg_init_values));

	dev->vctl_timeout_us = data->vctl_timeout_us;


	if (!num_pmic_data)
		num_pmic_data = msm_spm_drv_get_num_pmic_data(dev);

	num_spm_entry = msm_spm_drv_get_num_spm_entry(dev);

	dev->reg_seq_entry_shadow =
		kzalloc(sizeof(*dev->reg_seq_entry_shadow) * num_spm_entry,
				GFP_KERNEL);

	if (!dev->reg_seq_entry_shadow)
		return -ENOMEM;

	return 0;
}
=======
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/qcom_scm.h>

#include <asm/cpuidle.h>
#include <asm/proc-fns.h>
#include <asm/suspend.h>

#define MAX_PMIC_DATA		2
#define MAX_SEQ_DATA		64
#define SPM_CTL_INDEX		0x7f
#define SPM_CTL_INDEX_SHIFT	4
#define SPM_CTL_EN		BIT(0)

enum pm_sleep_mode {
	PM_SLEEP_MODE_STBY,
	PM_SLEEP_MODE_RET,
	PM_SLEEP_MODE_SPC,
	PM_SLEEP_MODE_PC,
	PM_SLEEP_MODE_NR,
};

enum spm_reg {
	SPM_REG_CFG,
	SPM_REG_SPM_CTL,
	SPM_REG_DLY,
	SPM_REG_PMIC_DLY,
	SPM_REG_PMIC_DATA_0,
	SPM_REG_PMIC_DATA_1,
	SPM_REG_VCTL,
	SPM_REG_SEQ_ENTRY,
	SPM_REG_SPM_STS,
	SPM_REG_PMIC_STS,
	SPM_REG_NR,
};

struct spm_reg_data {
	const u8 *reg_offset;
	u32 spm_cfg;
	u32 spm_dly;
	u32 pmic_dly;
	u32 pmic_data[MAX_PMIC_DATA];
	u8 seq[MAX_SEQ_DATA];
	u8 start_index[PM_SLEEP_MODE_NR];
};

struct spm_driver_data {
	void __iomem *reg_base;
	const struct spm_reg_data *reg_data;
};

static const u8 spm_reg_offset_v2_1[SPM_REG_NR] = {
	[SPM_REG_CFG]		= 0x08,
	[SPM_REG_SPM_CTL]	= 0x30,
	[SPM_REG_DLY]		= 0x34,
	[SPM_REG_SEQ_ENTRY]	= 0x80,
};

/* SPM register data for 8974, 8084 */
static const struct spm_reg_data spm_reg_8974_8084_cpu  = {
	.reg_offset = spm_reg_offset_v2_1,
	.spm_cfg = 0x1,
	.spm_dly = 0x3C102800,
	.seq = { 0x03, 0x0B, 0x0F, 0x00, 0x20, 0x80, 0x10, 0xE8, 0x5B, 0x03,
		0x3B, 0xE8, 0x5B, 0x82, 0x10, 0x0B, 0x30, 0x06, 0x26, 0x30,
		0x0F },
	.start_index[PM_SLEEP_MODE_STBY] = 0,
	.start_index[PM_SLEEP_MODE_SPC] = 3,
};

static const u8 spm_reg_offset_v1_1[SPM_REG_NR] = {
	[SPM_REG_CFG]		= 0x08,
	[SPM_REG_SPM_CTL]	= 0x20,
	[SPM_REG_PMIC_DLY]	= 0x24,
	[SPM_REG_PMIC_DATA_0]	= 0x28,
	[SPM_REG_PMIC_DATA_1]	= 0x2C,
	[SPM_REG_SEQ_ENTRY]	= 0x80,
};

/* SPM register data for 8064 */
static const struct spm_reg_data spm_reg_8064_cpu = {
	.reg_offset = spm_reg_offset_v1_1,
	.spm_cfg = 0x1F,
	.pmic_dly = 0x02020004,
	.pmic_data[0] = 0x0084009C,
	.pmic_data[1] = 0x00A4001C,
	.seq = { 0x03, 0x0F, 0x00, 0x24, 0x54, 0x10, 0x09, 0x03, 0x01,
		0x10, 0x54, 0x30, 0x0C, 0x24, 0x30, 0x0F },
	.start_index[PM_SLEEP_MODE_STBY] = 0,
	.start_index[PM_SLEEP_MODE_SPC] = 2,
};

static DEFINE_PER_CPU(struct spm_driver_data *, cpu_spm_drv);

typedef int (*idle_fn)(void);
static DEFINE_PER_CPU(idle_fn*, qcom_idle_ops);

static inline void spm_register_write(struct spm_driver_data *drv,
					enum spm_reg reg, u32 val)
{
	if (drv->reg_data->reg_offset[reg])
		writel_relaxed(val, drv->reg_base +
				drv->reg_data->reg_offset[reg]);
}

/* Ensure a guaranteed write, before return */
static inline void spm_register_write_sync(struct spm_driver_data *drv,
					enum spm_reg reg, u32 val)
{
	u32 ret;

	if (!drv->reg_data->reg_offset[reg])
		return;

	do {
		writel_relaxed(val, drv->reg_base +
				drv->reg_data->reg_offset[reg]);
		ret = readl_relaxed(drv->reg_base +
				drv->reg_data->reg_offset[reg]);
		if (ret == val)
			break;
		cpu_relax();
	} while (1);
}

static inline u32 spm_register_read(struct spm_driver_data *drv,
					enum spm_reg reg)
{
	return readl_relaxed(drv->reg_base + drv->reg_data->reg_offset[reg]);
}

static void spm_set_low_power_mode(struct spm_driver_data *drv,
					enum pm_sleep_mode mode)
{
	u32 start_index;
	u32 ctl_val;

	start_index = drv->reg_data->start_index[mode];

	ctl_val = spm_register_read(drv, SPM_REG_SPM_CTL);
	ctl_val &= ~(SPM_CTL_INDEX << SPM_CTL_INDEX_SHIFT);
	ctl_val |= start_index << SPM_CTL_INDEX_SHIFT;
	ctl_val |= SPM_CTL_EN;
	spm_register_write_sync(drv, SPM_REG_SPM_CTL, ctl_val);
}

static int qcom_pm_collapse(unsigned long int unused)
{
	qcom_scm_cpu_power_down(QCOM_SCM_CPU_PWR_DOWN_L2_ON);

	/*
	 * Returns here only if there was a pending interrupt and we did not
	 * power down as a result.
	 */
	return -1;
}

static int qcom_cpu_spc(void)
{
	int ret;
	struct spm_driver_data *drv = __this_cpu_read(cpu_spm_drv);

	spm_set_low_power_mode(drv, PM_SLEEP_MODE_SPC);
	ret = cpu_suspend(0, qcom_pm_collapse);
	/*
	 * ARM common code executes WFI without calling into our driver and
	 * if the SPM mode is not reset, then we may accidently power down the
	 * cpu when we intended only to gate the cpu clock.
	 * Ensure the state is set to standby before returning.
	 */
	spm_set_low_power_mode(drv, PM_SLEEP_MODE_STBY);

	return ret;
}

static int qcom_idle_enter(unsigned long index)
{
	return __this_cpu_read(qcom_idle_ops)[index]();
}

static const struct of_device_id qcom_idle_state_match[] __initconst = {
	{ .compatible = "qcom,idle-state-spc", .data = qcom_cpu_spc },
	{ },
};

static int __init qcom_cpuidle_init(struct device_node *cpu_node, int cpu)
{
	const struct of_device_id *match_id;
	struct device_node *state_node;
	int i;
	int state_count = 1;
	idle_fn idle_fns[CPUIDLE_STATE_MAX];
	idle_fn *fns;
	cpumask_t mask;
	bool use_scm_power_down = false;

	for (i = 0; ; i++) {
		state_node = of_parse_phandle(cpu_node, "cpu-idle-states", i);
		if (!state_node)
			break;

		if (!of_device_is_available(state_node))
			continue;

		if (i == CPUIDLE_STATE_MAX) {
			pr_warn("%s: cpuidle states reached max possible\n",
					__func__);
			break;
		}

		match_id = of_match_node(qcom_idle_state_match, state_node);
		if (!match_id)
			return -ENODEV;

		idle_fns[state_count] = match_id->data;

		/* Check if any of the states allow power down */
		if (match_id->data == qcom_cpu_spc)
			use_scm_power_down = true;

		state_count++;
	}

	if (state_count == 1)
		goto check_spm;

	fns = devm_kcalloc(get_cpu_device(cpu), state_count, sizeof(*fns),
			GFP_KERNEL);
	if (!fns)
		return -ENOMEM;

	for (i = 1; i < state_count; i++)
		fns[i] = idle_fns[i];

	if (use_scm_power_down) {
		/* We have atleast one power down mode */
		cpumask_clear(&mask);
		cpumask_set_cpu(cpu, &mask);
		qcom_scm_set_warm_boot_addr(cpu_resume_arm, &mask);
	}

	per_cpu(qcom_idle_ops, cpu) = fns;

	/*
	 * SPM probe for the cpu should have happened by now, if the
	 * SPM device does not exist, return -ENXIO to indicate that the
	 * cpu does not support idle states.
	 */
check_spm:
	return per_cpu(cpu_spm_drv, cpu) ? 0 : -ENXIO;
}

static const struct cpuidle_ops qcom_cpuidle_ops __initconst = {
	.suspend = qcom_idle_enter,
	.init = qcom_cpuidle_init,
};

CPUIDLE_METHOD_OF_DECLARE(qcom_idle_v1, "qcom,kpss-acc-v1", &qcom_cpuidle_ops);
CPUIDLE_METHOD_OF_DECLARE(qcom_idle_v2, "qcom,kpss-acc-v2", &qcom_cpuidle_ops);

static struct spm_driver_data *spm_get_drv(struct platform_device *pdev,
		int *spm_cpu)
{
	struct spm_driver_data *drv = NULL;
	struct device_node *cpu_node, *saw_node;
	int cpu;
	bool found = 0;

	for_each_possible_cpu(cpu) {
		cpu_node = of_cpu_device_node_get(cpu);
		if (!cpu_node)
			continue;
		saw_node = of_parse_phandle(cpu_node, "qcom,saw", 0);
		found = (saw_node == pdev->dev.of_node);
		of_node_put(saw_node);
		of_node_put(cpu_node);
		if (found)
			break;
	}

	if (found) {
		drv = devm_kzalloc(&pdev->dev, sizeof(*drv), GFP_KERNEL);
		if (drv)
			*spm_cpu = cpu;
	}

	return drv;
}

static const struct of_device_id spm_match_table[] = {
	{ .compatible = "qcom,msm8974-saw2-v2.1-cpu",
	  .data = &spm_reg_8974_8084_cpu },
	{ .compatible = "qcom,apq8084-saw2-v2.1-cpu",
	  .data = &spm_reg_8974_8084_cpu },
	{ .compatible = "qcom,apq8064-saw2-v1.1-cpu",
	  .data = &spm_reg_8064_cpu },
	{ },
};

static int spm_dev_probe(struct platform_device *pdev)
{
	struct spm_driver_data *drv;
	struct resource *res;
	const struct of_device_id *match_id;
	void __iomem *addr;
	int cpu;

	drv = spm_get_drv(pdev, &cpu);
	if (!drv)
		return -EINVAL;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	drv->reg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(drv->reg_base))
		return PTR_ERR(drv->reg_base);

	match_id = of_match_node(spm_match_table, pdev->dev.of_node);
	if (!match_id)
		return -ENODEV;

	drv->reg_data = match_id->data;

	/* Write the SPM sequences first.. */
	addr = drv->reg_base + drv->reg_data->reg_offset[SPM_REG_SEQ_ENTRY];
	__iowrite32_copy(addr, drv->reg_data->seq,
			ARRAY_SIZE(drv->reg_data->seq) / 4);

	/*
	 * ..and then the control registers.
	 * On some SoC if the control registers are written first and if the
	 * CPU was held in reset, the reset signal could trigger the SPM state
	 * machine, before the sequences are completely written.
	 */
	spm_register_write(drv, SPM_REG_CFG, drv->reg_data->spm_cfg);
	spm_register_write(drv, SPM_REG_DLY, drv->reg_data->spm_dly);
	spm_register_write(drv, SPM_REG_PMIC_DLY, drv->reg_data->pmic_dly);
	spm_register_write(drv, SPM_REG_PMIC_DATA_0,
				drv->reg_data->pmic_data[0]);
	spm_register_write(drv, SPM_REG_PMIC_DATA_1,
				drv->reg_data->pmic_data[1]);

	/* Set up Standby as the default low power mode */
	spm_set_low_power_mode(drv, PM_SLEEP_MODE_STBY);

	per_cpu(cpu_spm_drv, cpu) = drv;

	return 0;
}

static struct platform_driver spm_driver = {
	.probe = spm_dev_probe,
	.driver = {
		.name = "saw",
		.of_match_table = spm_match_table,
	},
};

builtin_platform_driver(spm_driver);
>>>>>>> v4.9.227
