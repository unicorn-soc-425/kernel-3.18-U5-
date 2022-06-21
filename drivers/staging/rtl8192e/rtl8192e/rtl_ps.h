/******************************************************************************
 * Copyright(c) 2008 - 2010 Realtek Corporation. All rights reserved.
 *
 * Based on the r8180 driver, which is:
 * Copyright 2004-2005 Andrea Merello <andrea.merello@gmail.com>, et al.
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
<<<<<<< HEAD
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
=======
>>>>>>> v4.9.227
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 ******************************************************************************/
#ifndef _RTL_PS_H
#define _RTL_PS_H

#include <linux/types.h>

struct net_device;

#define RT_CHECK_FOR_HANG_PERIOD 2
<<<<<<< HEAD
#define INIT_DEFAULT_CHAN	 1

void rtl8192_hw_wakeup(struct net_device *dev);
void rtl8192_hw_to_sleep(struct net_device *dev, u64 time);
void rtllib_ips_leave_wq(struct net_device *dev);
void rtllib_ips_leave(struct net_device *dev);
void IPSLeave_wq(void *data);

void IPSEnter(struct net_device *dev);
void IPSLeave(struct net_device *dev);

void LeisurePSEnter(struct net_device *dev);
void LeisurePSLeave(struct net_device *dev);
=======

void rtl92e_hw_wakeup(struct net_device *dev);
void rtl92e_enter_sleep(struct net_device *dev, u64 time);
void rtl92e_rtllib_ips_leave_wq(struct net_device *dev);
void rtl92e_rtllib_ips_leave(struct net_device *dev);
void rtl92e_ips_leave_wq(void *data);

void rtl92e_ips_enter(struct net_device *dev);
void rtl92e_ips_leave(struct net_device *dev);

void rtl92e_leisure_ps_enter(struct net_device *dev);
void rtl92e_leisure_ps_leave(struct net_device *dev);
>>>>>>> v4.9.227

#endif
