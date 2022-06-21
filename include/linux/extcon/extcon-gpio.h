/*
<<<<<<< HEAD
 *  External connector (extcon) class generic GPIO driver
=======
 * Single-state GPIO extcon driver based on extcon class
>>>>>>> v4.9.227
 *
 * Copyright (C) 2012 Samsung Electronics
 * Author: MyungJoo Ham <myungjoo.ham@samsung.com>
 *
 * based on switch class driver
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
<<<<<<< HEAD
 *
*/
=======
 */
>>>>>>> v4.9.227
#ifndef __EXTCON_GPIO_H__
#define __EXTCON_GPIO_H__ __FILE__

#include <linux/extcon.h>

/**
<<<<<<< HEAD
 * struct gpio_extcon_platform_data - A simple GPIO-controlled extcon device.
 * @name:		The name of this GPIO extcon device.
=======
 * struct gpio_extcon_pdata - A simple GPIO-controlled extcon device.
 * @extcon_id:		The unique id of specific external connector.
>>>>>>> v4.9.227
 * @gpio:		Corresponding GPIO.
 * @gpio_active_low:	Boolean describing whether gpio active state is 1 or 0
 *			If true, low state of gpio means active.
 *			If false, high state of gpio means active.
 * @debounce:		Debounce time for GPIO IRQ in ms.
 * @irq_flags:		IRQ Flags (e.g., IRQF_TRIGGER_LOW).
<<<<<<< HEAD
 * @state_on:		print_state is overriden with state_on if attached.
 *			If NULL, default method of extcon class is used.
 * @state_off:		print_state is overriden with state_off if detached.
 *			If NUll, default method of extcon class is used.
 * @check_on_resume:	Boolean describing whether to check the state of gpio
 *			while resuming from sleep.
 *
 * Note that in order for state_on or state_off to be valid, both state_on
 * and state_off should be not NULL. If at least one of them is NULL,
 * the print_state is not overriden.
 */
struct gpio_extcon_platform_data {
	const char *name;
=======
 * @check_on_resume:	Boolean describing whether to check the state of gpio
 *			while resuming from sleep.
 */
struct gpio_extcon_pdata {
	unsigned int extcon_id;
>>>>>>> v4.9.227
	unsigned gpio;
	bool gpio_active_low;
	unsigned long debounce;
	unsigned long irq_flags;

<<<<<<< HEAD
	/* if NULL, "0" or "1" will be printed */
	const char *state_on;
	const char *state_off;
=======
>>>>>>> v4.9.227
	bool check_on_resume;
};

#endif /* __EXTCON_GPIO_H__ */
