/*
 *  External connector (extcon) class driver
 *
<<<<<<< HEAD
=======
 * Copyright (C) 2015 Samsung Electronics
 * Author: Chanwoo Choi <cw00.choi@samsung.com>
 *
>>>>>>> v4.9.227
 * Copyright (C) 2012 Samsung Electronics
 * Author: Donggeun Kim <dg77.kim@samsung.com>
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
 *
*/

#ifndef __LINUX_EXTCON_H__
#define __LINUX_EXTCON_H__

#include <linux/device.h>
<<<<<<< HEAD
#include <linux/notifier.h>
#include <linux/sysfs.h>

#define SUPPORTED_CABLE_MAX	32
#define CABLE_NAME_MAX		30

/*
 * The standard cable name is to help support general notifier
 * and notifiee device drivers to share the common names.
 * Please use standard cable names unless your notifier device has
 * a very unique and abnormal cable or
 * the cable type is supposed to be used with only one unique
 * pair of notifier/notifiee devices.
 *
 * Please add any other "standard" cables used with extcon dev.
 *
 * You may add a dot and number to specify version or specification
 * of the specific cable if it is required. (e.g., "Fast-charger.18"
 * and "Fast-charger.10" for 1.8A and 1.0A chargers)
 * However, the notifiee and notifier should be able to handle such
 * string and if the notifiee can negotiate the protocol or identify,
 * you don't need such convention. This convention is helpful when
 * notifier can distinguish but notifiee cannot.
 */
enum extcon_cable_name {
	EXTCON_USB = 0,
	EXTCON_USB_HOST,
	EXTCON_TA,			/* Travel Adaptor */
	EXTCON_FAST_CHARGER,
	EXTCON_SLOW_CHARGER,
	EXTCON_CHARGE_DOWNSTREAM,	/* Charging an external device */
	EXTCON_HDMI,
	EXTCON_MHL,
	EXTCON_DVI,
	EXTCON_VGA,
	EXTCON_DOCK,
	EXTCON_LINE_IN,
	EXTCON_LINE_OUT,
	EXTCON_MIC_IN,
	EXTCON_HEADPHONE_OUT,
	EXTCON_SPDIF_IN,
	EXTCON_SPDIF_OUT,
	EXTCON_VIDEO_IN,
	EXTCON_VIDEO_OUT,
	EXTCON_MECHANICAL,
};
extern const char extcon_cable_name[][CABLE_NAME_MAX + 1];
=======

/*
 * Define the type of supported external connectors
 */
#define EXTCON_TYPE_USB		BIT(0)	/* USB connector */
#define EXTCON_TYPE_CHG		BIT(1)	/* Charger connector */
#define EXTCON_TYPE_JACK	BIT(2)	/* Jack connector */
#define EXTCON_TYPE_DISP	BIT(3)	/* Display connector */
#define EXTCON_TYPE_MISC	BIT(4)	/* Miscellaneous connector */

/*
 * Define the unique id of supported external connectors
 */
#define EXTCON_NONE		0

/* USB external connector */
#define EXTCON_USB		1
#define EXTCON_USB_HOST		2

/* Charging external connector */
#define EXTCON_CHG_USB_SDP	5	/* Standard Downstream Port */
#define EXTCON_CHG_USB_DCP	6	/* Dedicated Charging Port */
#define EXTCON_CHG_USB_CDP	7	/* Charging Downstream Port */
#define EXTCON_CHG_USB_ACA	8	/* Accessory Charger Adapter */
#define EXTCON_CHG_USB_FAST	9
#define EXTCON_CHG_USB_SLOW	10
#define EXTCON_CHG_WPT		11	/* Wireless Power Transfer */

/* Jack external connector */
#define EXTCON_JACK_MICROPHONE	20
#define EXTCON_JACK_HEADPHONE	21
#define EXTCON_JACK_LINE_IN	22
#define EXTCON_JACK_LINE_OUT	23
#define EXTCON_JACK_VIDEO_IN	24
#define EXTCON_JACK_VIDEO_OUT	25
#define EXTCON_JACK_SPDIF_IN	26	/* Sony Philips Digital InterFace */
#define EXTCON_JACK_SPDIF_OUT	27

/* Display external connector */
#define EXTCON_DISP_HDMI	40	/* High-Definition Multimedia Interface */
#define EXTCON_DISP_MHL		41	/* Mobile High-Definition Link */
#define EXTCON_DISP_DVI		42	/* Digital Visual Interface */
#define EXTCON_DISP_VGA		43	/* Video Graphics Array */
#define EXTCON_DISP_DP		44	/* Display Port */
#define EXTCON_DISP_HMD		45	/* Head-Mounted Display */

/* Miscellaneous external connector */
#define EXTCON_DOCK		60
#define EXTCON_JIG		61
#define EXTCON_MECHANICAL	62

#define EXTCON_NUM		63

/*
 * Define the property of supported external connectors.
 *
 * When adding the new extcon property, they *must* have
 * the type/value/default information. Also, you *have to*
 * modify the EXTCON_PROP_[type]_START/END definitions
 * which mean the range of the supported properties
 * for each extcon type.
 *
 * The naming style of property
 * : EXTCON_PROP_[type]_[property name]
 *
 * EXTCON_PROP_USB_[property name]	: USB property
 * EXTCON_PROP_CHG_[property name]	: Charger property
 * EXTCON_PROP_JACK_[property name]	: Jack property
 * EXTCON_PROP_DISP_[property name]	: Display property
 */

/*
 * Properties of EXTCON_TYPE_USB.
 *
 * - EXTCON_PROP_USB_VBUS
 * @type:	integer (intval)
 * @value:	0 (low) or 1 (high)
 * @default:	0 (low)
 * - EXTCON_PROP_USB_TYPEC_POLARITY
 * @type:	integer (intval)
 * @value:	0 (normal) or 1 (flip)
 * @default:	0 (normal)
 * - EXTCON_PROP_USB_SS (SuperSpeed)
 * @type:       integer (intval)
 * @value:      0 (USB/USB2) or 1 (USB3)
 * @default:    0 (USB/USB2)
 *
 */
#define EXTCON_PROP_USB_VBUS		0
#define EXTCON_PROP_USB_TYPEC_POLARITY	1
#define EXTCON_PROP_USB_SS		2

#define EXTCON_PROP_USB_MIN		0
#define EXTCON_PROP_USB_MAX		2
#define EXTCON_PROP_USB_CNT	(EXTCON_PROP_USB_MAX - EXTCON_PROP_USB_MIN + 1)

/* Properties of EXTCON_TYPE_CHG. */
#define EXTCON_PROP_CHG_MIN		50
#define EXTCON_PROP_CHG_MAX		50
#define EXTCON_PROP_CHG_CNT	(EXTCON_PROP_CHG_MAX - EXTCON_PROP_CHG_MIN + 1)

/* Properties of EXTCON_TYPE_JACK. */
#define EXTCON_PROP_JACK_MIN		100
#define EXTCON_PROP_JACK_MAX		100
#define EXTCON_PROP_JACK_CNT (EXTCON_PROP_JACK_MAX - EXTCON_PROP_JACK_MIN + 1)

/*
 * Properties of EXTCON_TYPE_DISP.
 *
 * - EXTCON_PROP_DISP_HPD (Hot Plug Detect)
 * @type:       integer (intval)
 * @value:      0 (no hpd) or 1 (hpd)
 * @default:    0 (no hpd)
 *
 */
#define EXTCON_PROP_DISP_HPD		150

/* Properties of EXTCON_TYPE_DISP. */
#define EXTCON_PROP_DISP_MIN		150
#define EXTCON_PROP_DISP_MAX		151
#define EXTCON_PROP_DISP_CNT (EXTCON_PROP_DISP_MAX - EXTCON_PROP_DISP_MIN + 1)

/*
 * Define the type of property's value.
 *
 * Define the property's value as union type. Because each property
 * would need the different data type to store it.
 */
union extcon_property_value {
	int intval;	/* type : integer (intval) */
};
>>>>>>> v4.9.227

struct extcon_cable;

/**
 * struct extcon_dev - An extcon device represents one external connector.
 * @name:		The name of this extcon device. Parent device name is
 *			used if NULL.
<<<<<<< HEAD
 * @supported_cable:	Array of supported cable names ending with NULL.
=======
 * @supported_cable:	Array of supported cable names ending with EXTCON_NONE.
>>>>>>> v4.9.227
 *			If supported_cable is NULL, cable name related APIs
 *			are disabled.
 * @mutually_exclusive:	Array of mutually exclusive set of cables that cannot
 *			be attached simultaneously. The array should be
 *			ending with NULL or be NULL (no mutually exclusive
 *			cables). For example, if it is { 0x7, 0x30, 0}, then,
 *			{0, 1}, {0, 1, 2}, {0, 2}, {1, 2}, or {4, 5} cannot
 *			be attached simulataneously. {0x7, 0} is equivalent to
 *			{0x3, 0x6, 0x5, 0}. If it is {0xFFFFFFFF, 0}, there
 *			can be no simultaneous connections.
<<<<<<< HEAD
 * @print_name:		An optional callback to override the method to print the
 *			name of the extcon device.
 * @print_state:	An optional callback to override the method to print the
 *			status of the extcon device.
=======
>>>>>>> v4.9.227
 * @dev:		Device of this extcon.
 * @state:		Attach/detach state of this extcon. Do not provide at
 *			register-time.
 * @nh:			Notifier for the state change events from this extcon
<<<<<<< HEAD
 * @entry:		To support list of extcon devices so that users can search
 *			for extcon devices based on the extcon name.
=======
 * @entry:		To support list of extcon devices so that users can
 *			search for extcon devices based on the extcon name.
>>>>>>> v4.9.227
 * @lock:
 * @max_supported:	Internal value to store the number of cables.
 * @extcon_dev_type:	Device_type struct to provide attribute_groups
 *			customized for each extcon device.
 * @cables:		Sysfs subdirectories. Each represents one cable.
 *
 * In most cases, users only need to provide "User initializing data" of
 * this struct when registering an extcon. In some exceptional cases,
 * optional callbacks may be needed. However, the values in "internal data"
 * are overwritten by register function.
 */
struct extcon_dev {
	/* Optional user initializing data */
	const char *name;
<<<<<<< HEAD
	const char **supported_cable;
	const u32 *mutually_exclusive;

	/* Optional callbacks to override class functions */
	ssize_t	(*print_name)(struct extcon_dev *edev, char *buf);
	ssize_t	(*print_state)(struct extcon_dev *edev, char *buf);

	/* Internal data. Please do not set. */
	struct device dev;
	struct raw_notifier_head nh;
=======
	const unsigned int *supported_cable;
	const u32 *mutually_exclusive;

	/* Internal data. Please do not set. */
	struct device dev;
	struct raw_notifier_head *nh;
>>>>>>> v4.9.227
	struct list_head entry;
	int max_supported;
	spinlock_t lock;	/* could be called by irq handler */
	u32 state;

	/* /sys/class/extcon/.../cable.n/... */
	struct device_type extcon_dev_type;
	struct extcon_cable *cables;

	/* /sys/class/extcon/.../mutually_exclusive/... */
	struct attribute_group attr_g_muex;
	struct attribute **attrs_muex;
	struct device_attribute *d_attrs_muex;
};

<<<<<<< HEAD
/**
 * struct extcon_cable - An internal data for each cable of extcon device.
 * @edev:		The extcon device
 * @cable_index:	Index of this cable in the edev
 * @attr_g:		Attribute group for the cable
 * @attr_name:		"name" sysfs entry
 * @attr_state:		"state" sysfs entry
 * @attrs:		Array pointing to attr_name and attr_state for attr_g
 */
struct extcon_cable {
	struct extcon_dev *edev;
	int cable_index;

	struct attribute_group attr_g;
	struct device_attribute attr_name;
	struct device_attribute attr_state;

	struct attribute *attrs[3]; /* to be fed to attr_g.attrs */
};

/**
 * struct extcon_specific_cable_nb - An internal data for
 *				     extcon_register_interest().
 * @internal_nb:	A notifier block bridging extcon notifier
 *			and cable notifier.
 * @user_nb:		user provided notifier block for events from
 *			a specific cable.
 * @cable_index:	the target cable.
 * @edev:		the target extcon device.
 * @previous_value:	the saved previous event value.
 */
struct extcon_specific_cable_nb {
	struct notifier_block internal_nb;
	struct notifier_block *user_nb;
	int cable_index;
	struct extcon_dev *edev;
	unsigned long previous_value;
};

=======
>>>>>>> v4.9.227
#if IS_ENABLED(CONFIG_EXTCON)

/*
 * Following APIs are for notifiers or configurations.
 * Notifiers are the external port and connection devices.
 */
extern int extcon_dev_register(struct extcon_dev *edev);
extern void extcon_dev_unregister(struct extcon_dev *edev);
extern int devm_extcon_dev_register(struct device *dev,
				    struct extcon_dev *edev);
extern void devm_extcon_dev_unregister(struct device *dev,
				       struct extcon_dev *edev);
extern struct extcon_dev *extcon_get_extcon_dev(const char *extcon_name);

/*
 * Following APIs control the memory of extcon device.
 */
<<<<<<< HEAD
extern struct extcon_dev *extcon_dev_allocate(const char **cables);
extern void extcon_dev_free(struct extcon_dev *edev);
extern struct extcon_dev *devm_extcon_dev_allocate(struct device *dev,
						   const char **cables);
extern void devm_extcon_dev_free(struct device *dev, struct extcon_dev *edev);

/*
 * get/set/update_state access the 32b encoded state value, which represents
 * states of all possible cables of the multistate port. For example, if one
 * calls extcon_set_state(edev, 0x7), it may mean that all the three cables
 * are attached to the port.
 */
static inline u32 extcon_get_state(struct extcon_dev *edev)
{
	return edev->state;
}

extern int extcon_set_state(struct extcon_dev *edev, u32 state);
extern int extcon_update_state(struct extcon_dev *edev, u32 mask, u32 state);

/*
 * get/set_cable_state access each bit of the 32b encoded state value.
 * They are used to access the status of each cable based on the cable_name
 * or cable_index, which is retrieved by extcon_find_cable_index
 */
extern int extcon_find_cable_index(struct extcon_dev *sdev,
				   const char *cable_name);
extern int extcon_get_cable_state_(struct extcon_dev *edev, int cable_index);
extern int extcon_set_cable_state_(struct extcon_dev *edev, int cable_index,
				   bool cable_state);

extern int extcon_get_cable_state(struct extcon_dev *edev,
				  const char *cable_name);
extern int extcon_set_cable_state(struct extcon_dev *edev,
				  const char *cable_name, bool cable_state);

/*
 * Following APIs are for notifiees (those who want to be notified)
 * to register a callback for events from a specific cable of the extcon.
 * Notifiees are the connected device drivers wanting to get notified by
 * a specific external port of a connection device.
 */
extern int extcon_register_interest(struct extcon_specific_cable_nb *obj,
				    const char *extcon_name,
				    const char *cable_name,
				    struct notifier_block *nb);
extern int extcon_unregister_interest(struct extcon_specific_cable_nb *nb);
=======
extern struct extcon_dev *extcon_dev_allocate(const unsigned int *cable);
extern void extcon_dev_free(struct extcon_dev *edev);
extern struct extcon_dev *devm_extcon_dev_allocate(struct device *dev,
						   const unsigned int *cable);
extern void devm_extcon_dev_free(struct device *dev, struct extcon_dev *edev);

/*
 * get/set_state access each bit of the 32b encoded state value.
 * They are used to access the status of each cable based on the cable id.
 */
extern int extcon_get_state(struct extcon_dev *edev, unsigned int id);
extern int extcon_set_state(struct extcon_dev *edev, unsigned int id,
				   bool cable_state);
extern int extcon_set_state_sync(struct extcon_dev *edev, unsigned int id,
				bool cable_state);
/*
 * Synchronize the state and property data for a specific external connector.
 */
extern int extcon_sync(struct extcon_dev *edev, unsigned int id);

/*
 * get/set_property access the property value of each external connector.
 * They are used to access the property of each cable based on the property id.
 */
extern int extcon_get_property(struct extcon_dev *edev, unsigned int id,
				unsigned int prop,
				union extcon_property_value *prop_val);
extern int extcon_set_property(struct extcon_dev *edev, unsigned int id,
				unsigned int prop,
				union extcon_property_value prop_val);
extern int extcon_set_property_sync(struct extcon_dev *edev, unsigned int id,
				unsigned int prop,
				union extcon_property_value prop_val);

/*
 * get/set_property_capability set the capability of the property for each
 * external connector. They are used to set the capability of the property
 * of each external connector based on the id and property.
 */
extern int extcon_get_property_capability(struct extcon_dev *edev,
				unsigned int id, unsigned int prop);
extern int extcon_set_property_capability(struct extcon_dev *edev,
				unsigned int id, unsigned int prop);
>>>>>>> v4.9.227

/*
 * Following APIs are to monitor every action of a notifier.
 * Registrar gets notified for every external port of a connection device.
 * Probably this could be used to debug an action of notifier; however,
 * we do not recommend to use this for normal 'notifiee' device drivers who
 * want to be notified by a specific external port of the notifier.
 */
<<<<<<< HEAD
extern int extcon_register_notifier(struct extcon_dev *edev,
				    struct notifier_block *nb);
extern int extcon_unregister_notifier(struct extcon_dev *edev,
				      struct notifier_block *nb);
=======
extern int extcon_register_notifier(struct extcon_dev *edev, unsigned int id,
				    struct notifier_block *nb);
extern int extcon_unregister_notifier(struct extcon_dev *edev, unsigned int id,
				    struct notifier_block *nb);
extern int devm_extcon_register_notifier(struct device *dev,
				struct extcon_dev *edev, unsigned int id,
				struct notifier_block *nb);
extern void devm_extcon_unregister_notifier(struct device *dev,
				struct extcon_dev *edev, unsigned int id,
				struct notifier_block *nb);
>>>>>>> v4.9.227

/*
 * Following API get the extcon device from devicetree.
 * This function use phandle of devicetree to get extcon device directly.
 */
<<<<<<< HEAD
extern struct extcon_dev *extcon_get_edev_by_phandle(struct device *dev, int index);
=======
extern struct extcon_dev *extcon_get_edev_by_phandle(struct device *dev,
						     int index);

/* Following API to get information of extcon device */
extern const char *extcon_get_edev_name(struct extcon_dev *edev);


>>>>>>> v4.9.227
#else /* CONFIG_EXTCON */
static inline int extcon_dev_register(struct extcon_dev *edev)
{
	return 0;
}

static inline void extcon_dev_unregister(struct extcon_dev *edev) { }

static inline int devm_extcon_dev_register(struct device *dev,
					   struct extcon_dev *edev)
{
	return -EINVAL;
}

static inline void devm_extcon_dev_unregister(struct device *dev,
					      struct extcon_dev *edev) { }

<<<<<<< HEAD
static inline struct extcon_dev *extcon_dev_allocate(const char **cables)
=======
static inline struct extcon_dev *extcon_dev_allocate(const unsigned int *cable)
>>>>>>> v4.9.227
{
	return ERR_PTR(-ENOSYS);
}

static inline void extcon_dev_free(struct extcon_dev *edev) { }

static inline struct extcon_dev *devm_extcon_dev_allocate(struct device *dev,
<<<<<<< HEAD
							  const char **cables)
=======
						const unsigned int *cable)
>>>>>>> v4.9.227
{
	return ERR_PTR(-ENOSYS);
}

static inline void devm_extcon_dev_free(struct extcon_dev *edev) { }

<<<<<<< HEAD
static inline u32 extcon_get_state(struct extcon_dev *edev)
=======

static inline int extcon_get_state(struct extcon_dev *edev, unsigned int id)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_set_state(struct extcon_dev *edev, u32 state)
=======
static inline int extcon_set_state(struct extcon_dev *edev, unsigned int id,
				bool cable_state)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_update_state(struct extcon_dev *edev, u32 mask,
				       u32 state)
=======
static inline int extcon_set_state_sync(struct extcon_dev *edev, unsigned int id,
				bool cable_state)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_find_cable_index(struct extcon_dev *edev,
					  const char *cable_name)
=======
static inline int extcon_sync(struct extcon_dev *edev, unsigned int id)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_get_cable_state_(struct extcon_dev *edev,
					  int cable_index)
=======
static inline int extcon_get_property(struct extcon_dev *edev, unsigned int id,
					unsigned int prop,
					union extcon_property_value *prop_val)
{
	return 0;
}
static inline int extcon_set_property(struct extcon_dev *edev, unsigned int id,
					unsigned int prop,
					union extcon_property_value prop_val)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_set_cable_state_(struct extcon_dev *edev,
					  int cable_index, bool cable_state)
=======
static inline int extcon_set_property_sync(struct extcon_dev *edev,
					unsigned int id, unsigned int prop,
					union extcon_property_value prop_val)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_get_cable_state(struct extcon_dev *edev,
			const char *cable_name)
=======
static inline int extcon_get_property_capability(struct extcon_dev *edev,
					unsigned int id, unsigned int prop)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_set_cable_state(struct extcon_dev *edev,
			const char *cable_name, int state)
=======
static inline int extcon_set_property_capability(struct extcon_dev *edev,
					unsigned int id, unsigned int prop)
>>>>>>> v4.9.227
{
	return 0;
}

static inline struct extcon_dev *extcon_get_extcon_dev(const char *extcon_name)
{
	return NULL;
}

static inline int extcon_register_notifier(struct extcon_dev *edev,
<<<<<<< HEAD
					   struct notifier_block *nb)
=======
					unsigned int id,
					struct notifier_block *nb)
>>>>>>> v4.9.227
{
	return 0;
}

static inline int extcon_unregister_notifier(struct extcon_dev *edev,
<<<<<<< HEAD
					     struct notifier_block *nb)
=======
					unsigned int id,
					struct notifier_block *nb)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int extcon_register_interest(struct extcon_specific_cable_nb *obj,
					   const char *extcon_name,
					   const char *cable_name,
					   struct notifier_block *nb)
{
	return 0;
}

static inline int extcon_unregister_interest(struct extcon_specific_cable_nb
						    *obj)
{
	return 0;
}
=======
static inline int devm_extcon_register_notifier(struct device *dev,
				struct extcon_dev *edev, unsigned int id,
				struct notifier_block *nb)
{
	return -ENOSYS;
}

static inline  void devm_extcon_unregister_notifier(struct device *dev,
				struct extcon_dev *edev, unsigned int id,
				struct notifier_block *nb) { }
>>>>>>> v4.9.227

static inline struct extcon_dev *extcon_get_edev_by_phandle(struct device *dev,
							    int index)
{
	return ERR_PTR(-ENODEV);
}
#endif /* CONFIG_EXTCON */
<<<<<<< HEAD
=======

/*
 * Following structure and API are deprecated. EXTCON remains the function
 * definition to prevent the build break.
 */
struct extcon_specific_cable_nb {
       struct notifier_block *user_nb;
       int cable_index;
       struct extcon_dev *edev;
       unsigned long previous_value;
};

static inline int extcon_register_interest(struct extcon_specific_cable_nb *obj,
			const char *extcon_name, const char *cable_name,
			struct notifier_block *nb)
{
	return -EINVAL;
}

static inline int extcon_unregister_interest(struct extcon_specific_cable_nb
						    *obj)
{
	return -EINVAL;
}

static inline int extcon_get_cable_state_(struct extcon_dev *edev, unsigned int id)
{
	return extcon_get_state(edev, id);
}

static inline int extcon_set_cable_state_(struct extcon_dev *edev, unsigned int id,
				   bool cable_state)
{
	return extcon_set_state_sync(edev, id, cable_state);
}
>>>>>>> v4.9.227
#endif /* __LINUX_EXTCON_H__ */
