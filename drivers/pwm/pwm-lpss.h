/*
 * Intel Low Power Subsystem PWM controller driver
 *
 * Copyright (C) 2014, Intel Corporation
 *
 * Derived from the original pwm-lpss.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PWM_LPSS_H
#define __PWM_LPSS_H

#include <linux/device.h>
#include <linux/pwm.h>

struct pwm_lpss_chip;

struct pwm_lpss_boardinfo {
	unsigned long clk_rate;
<<<<<<< HEAD
=======
	unsigned int npwm;
	unsigned long base_unit_bits;
>>>>>>> v4.9.227
};

extern const struct pwm_lpss_boardinfo pwm_lpss_byt_info;
extern const struct pwm_lpss_boardinfo pwm_lpss_bsw_info;
<<<<<<< HEAD
=======
extern const struct pwm_lpss_boardinfo pwm_lpss_bxt_info;
>>>>>>> v4.9.227

struct pwm_lpss_chip *pwm_lpss_probe(struct device *dev, struct resource *r,
				     const struct pwm_lpss_boardinfo *info);
int pwm_lpss_remove(struct pwm_lpss_chip *lpwm);
<<<<<<< HEAD
=======
int pwm_lpss_suspend(struct device *dev);
int pwm_lpss_resume(struct device *dev);
>>>>>>> v4.9.227

#endif	/* __PWM_LPSS_H */
