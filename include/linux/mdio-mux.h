/*
 * MDIO bus multiplexer framwork.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2011, 2012 Cavium, Inc.
 */
#ifndef __LINUX_MDIO_MUX_H
#define __LINUX_MDIO_MUX_H
#include <linux/device.h>
<<<<<<< HEAD
=======
#include <linux/phy.h>
>>>>>>> v4.9.227

int mdio_mux_init(struct device *dev,
		  int (*switch_fn) (int cur, int desired, void *data),
		  void **mux_handle,
<<<<<<< HEAD
		  void *data);
=======
		  void *data,
		  struct mii_bus *mux_bus);
>>>>>>> v4.9.227

void mdio_mux_uninit(void *mux_handle);

#endif /* __LINUX_MDIO_MUX_H */
