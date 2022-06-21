/*
 * Generic OPP Interface
 *
 * Copyright (C) 2009-2010 Texas Instruments Incorporated.
 *	Nishanth Menon
 *	Romit Dasgupta
 *	Kevin Hilman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DRIVER_OPP_H__
#define __DRIVER_OPP_H__

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/list.h>
<<<<<<< HEAD
#include <linux/pm_opp.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/limits.h>
=======
#include <linux/limits.h>
#include <linux/pm_opp.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
>>>>>>> v4.9.227

struct clk;
struct regulator;

/* Lock to allow exclusive modification to the device and opp lists */
<<<<<<< HEAD
extern struct mutex dev_opp_list_lock;
=======
extern struct mutex opp_table_lock;

extern struct list_head opp_tables;
>>>>>>> v4.9.227

/*
 * Internal data structure organization with the OPP layer library is as
 * follows:
<<<<<<< HEAD
 * dev_opp_list (root)
=======
 * opp_tables (root)
>>>>>>> v4.9.227
 *	|- device 1 (represents voltage domain 1)
 *	|	|- opp 1 (availability, freq, voltage)
 *	|	|- opp 2 ..
 *	...	...
 *	|	`- opp n ..
 *	|- device 2 (represents the next voltage domain)
 *	...
 *	`- device m (represents mth voltage domain)
<<<<<<< HEAD
 * device 1, 2.. are represented by dev_opp structure while each opp
=======
 * device 1, 2.. are represented by opp_table structure while each opp
>>>>>>> v4.9.227
 * is represented by the opp structure.
 */

/**
 * struct dev_pm_opp - Generic OPP description structure
<<<<<<< HEAD
 * @node:	opp list node. The nodes are maintained throughout the lifetime
 *		of boot. It is expected only an optimal set of OPPs are
 *		added to the library by the SoC framework.
 *		RCU usage: opp list is traversed with RCU locks. node
 *		modification is possible realtime, hence the modifications
 *		are protected by the dev_opp_list_lock for integrity.
=======
 * @node:	opp table node. The nodes are maintained throughout the lifetime
 *		of boot. It is expected only an optimal set of OPPs are
 *		added to the library by the SoC framework.
 *		RCU usage: opp table is traversed with RCU locks. node
 *		modification is possible realtime, hence the modifications
 *		are protected by the opp_table_lock for integrity.
>>>>>>> v4.9.227
 *		IMPORTANT: the opp nodes should be maintained in increasing
 *		order.
 * @available:	true/false - marks if this OPP as available or not
 * @dynamic:	not-created from static DT entries.
 * @turbo:	true if turbo (boost) OPP
<<<<<<< HEAD
=======
 * @suspend:	true if suspend OPP
>>>>>>> v4.9.227
 * @rate:	Frequency in hertz
 * @u_volt:	Target voltage in microvolts corresponding to this OPP
 * @u_volt_min:	Minimum voltage in microvolts corresponding to this OPP
 * @u_volt_max:	Maximum voltage in microvolts corresponding to this OPP
 * @u_amp:	Maximum current drawn by the device in microamperes
 * @clock_latency_ns: Latency (in nanoseconds) of switching to this OPP's
 *		frequency from any other OPP's frequency.
<<<<<<< HEAD
 * @dev_opp:	points back to the device_opp struct this opp belongs to
 * @rcu_head:	RCU callback head used for deferred freeing
 * @np:		OPP's device node.
=======
 * @opp_table:	points back to the opp_table struct this opp belongs to
 * @rcu_head:	RCU callback head used for deferred freeing
 * @np:		OPP's device node.
 * @dentry:	debugfs dentry pointer (per opp)
>>>>>>> v4.9.227
 *
 * This structure stores the OPP information for a given device.
 */
struct dev_pm_opp {
	struct list_head node;

	bool available;
	bool dynamic;
	bool turbo;
<<<<<<< HEAD
=======
	bool suspend;
>>>>>>> v4.9.227
	unsigned long rate;

	unsigned long u_volt;
	unsigned long u_volt_min;
	unsigned long u_volt_max;
	unsigned long u_amp;
	unsigned long clock_latency_ns;

<<<<<<< HEAD
	struct device_opp *dev_opp;
	struct rcu_head rcu_head;

	struct device_node *np;
};

/**
 * struct device_list_opp - devices managed by 'struct device_opp'
 * @node:	list node
 * @dev:	device to which the struct object belongs
 * @rcu_head:	RCU callback head used for deferred freeing
 *
 * This is an internal data structure maintaining the list of devices that are
 * managed by 'struct device_opp'.
 */
struct device_list_opp {
	struct list_head node;
	const struct device *dev;
	struct rcu_head rcu_head;
};

/**
 * struct device_opp - Device opp structure
 * @node:	list node - contains the devices with OPPs that
 *		have been registered. Nodes once added are not modified in this
 *		list.
 *		RCU usage: nodes are not modified in the list of device_opp,
 *		however addition is possible and is secured by dev_opp_list_lock
 * @srcu_head:	notifier head to notify the OPP availability changes.
 * @rcu_head:	RCU callback head used for deferred freeing
 * @dev_list:	list of devices that share these OPPs
 * @opp_list:	list of opps
=======
	struct opp_table *opp_table;
	struct rcu_head rcu_head;

	struct device_node *np;

#ifdef CONFIG_DEBUG_FS
	struct dentry *dentry;
#endif
};

/**
 * struct opp_device - devices managed by 'struct opp_table'
 * @node:	list node
 * @dev:	device to which the struct object belongs
 * @rcu_head:	RCU callback head used for deferred freeing
 * @dentry:	debugfs dentry pointer (per device)
 *
 * This is an internal data structure maintaining the devices that are managed
 * by 'struct opp_table'.
 */
struct opp_device {
	struct list_head node;
	const struct device *dev;
	struct rcu_head rcu_head;

#ifdef CONFIG_DEBUG_FS
	struct dentry *dentry;
#endif
};

enum opp_table_access {
	OPP_TABLE_ACCESS_UNKNOWN = 0,
	OPP_TABLE_ACCESS_EXCLUSIVE = 1,
	OPP_TABLE_ACCESS_SHARED = 2,
};

/**
 * struct opp_table - Device opp structure
 * @node:	table node - contains the devices with OPPs that
 *		have been registered. Nodes once added are not modified in this
 *		table.
 *		RCU usage: nodes are not modified in the table of opp_table,
 *		however addition is possible and is secured by opp_table_lock
 * @srcu_head:	notifier head to notify the OPP availability changes.
 * @rcu_head:	RCU callback head used for deferred freeing
 * @dev_list:	list of devices that share these OPPs
 * @opp_list:	table of opps
>>>>>>> v4.9.227
 * @np:		struct device_node pointer for opp's DT node.
 * @clock_latency_ns_max: Max clock latency in nanoseconds.
 * @shared_opp: OPP is shared between multiple devices.
 * @suspend_opp: Pointer to OPP to be used during device suspend.
 * @supported_hw: Array of version number to support.
 * @supported_hw_count: Number of elements in supported_hw array.
 * @prop_name: A name to postfix to many DT properties, while parsing them.
 * @clk: Device's clock handle
 * @regulator: Supply regulator
 * @dentry:	debugfs dentry pointer of the real device directory (not links).
 * @dentry_name: Name of the real dentry.
 *
 * @voltage_tolerance_v1: In percentage, for v1 bindings only.
 *
 * This is an internal data structure maintaining the link to opps attached to
 * a device. This structure is not meant to be shared to users as it is
 * meant for book keeping and private to OPP library.
 *
 * Because the opp structures can be used from both rcu and srcu readers, we
 * need to wait for the grace period of both of them before freeing any
 * resources. And so we have used kfree_rcu() from within call_srcu() handlers.
 */
<<<<<<< HEAD
struct device_opp {
=======
struct opp_table {
>>>>>>> v4.9.227
	struct list_head node;

	struct srcu_notifier_head srcu_head;
	struct rcu_head rcu_head;
	struct list_head dev_list;
	struct list_head opp_list;

	struct device_node *np;
	unsigned long clock_latency_ns_max;

	/* For backward compatibility with v1 bindings */
	unsigned int voltage_tolerance_v1;

<<<<<<< HEAD
	bool shared_opp;
=======
	enum opp_table_access shared_opp;
>>>>>>> v4.9.227
	struct dev_pm_opp *suspend_opp;

	unsigned int *supported_hw;
	unsigned int supported_hw_count;
	const char *prop_name;
	struct clk *clk;
	struct regulator *regulator;

<<<<<<< HEAD
};

/* Routines internal to opp core */
struct device_opp *_find_device_opp(struct device *dev);
struct device_list_opp *_add_list_dev(const struct device *dev,
				      struct device_opp *dev_opp);
struct device_node *_of_get_opp_desc_node(struct device *dev);
=======
#ifdef CONFIG_DEBUG_FS
	struct dentry *dentry;
	char dentry_name[NAME_MAX];
#endif
};

/* Routines internal to opp core */
struct opp_table *_find_opp_table(struct device *dev);
struct opp_device *_add_opp_dev(const struct device *dev, struct opp_table *opp_table);
struct device_node *_of_get_opp_desc_node(struct device *dev);
void _dev_pm_opp_remove_table(struct device *dev, bool remove_all);
struct dev_pm_opp *_allocate_opp(struct device *dev, struct opp_table **opp_table);
int _opp_add(struct device *dev, struct dev_pm_opp *new_opp, struct opp_table *opp_table);
void _opp_remove(struct opp_table *opp_table, struct dev_pm_opp *opp, bool notify);
int _opp_add_v1(struct device *dev, unsigned long freq, long u_volt, bool dynamic);
void _dev_pm_opp_cpumask_remove_table(const struct cpumask *cpumask, bool of);

#ifdef CONFIG_OF
void _of_init_opp_table(struct opp_table *opp_table, struct device *dev);
#else
static inline void _of_init_opp_table(struct opp_table *opp_table, struct device *dev) {}
#endif

#ifdef CONFIG_DEBUG_FS
void opp_debug_remove_one(struct dev_pm_opp *opp);
int opp_debug_create_one(struct dev_pm_opp *opp, struct opp_table *opp_table);
int opp_debug_register(struct opp_device *opp_dev, struct opp_table *opp_table);
void opp_debug_unregister(struct opp_device *opp_dev, struct opp_table *opp_table);
#else
static inline void opp_debug_remove_one(struct dev_pm_opp *opp) {}

static inline int opp_debug_create_one(struct dev_pm_opp *opp,
				       struct opp_table *opp_table)
{ return 0; }
static inline int opp_debug_register(struct opp_device *opp_dev,
				     struct opp_table *opp_table)
{ return 0; }

static inline void opp_debug_unregister(struct opp_device *opp_dev,
					struct opp_table *opp_table)
{ }
#endif		/* DEBUG_FS */
>>>>>>> v4.9.227

#endif		/* __DRIVER_OPP_H__ */
