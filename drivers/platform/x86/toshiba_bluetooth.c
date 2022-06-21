/*
 * Toshiba Bluetooth Enable Driver
 *
 * Copyright (C) 2009 Jes Sorensen <Jes.Sorensen@gmail.com>
<<<<<<< HEAD
=======
 * Copyright (C) 2015 Azael Avalos <coproscefalo@gmail.com>
>>>>>>> v4.9.227
 *
 * Thanks to Matthew Garrett for background info on ACPI innards which
 * normal people aren't meant to understand :-)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
<<<<<<< HEAD
 *
 * Note the Toshiba Bluetooth RFKill switch seems to be a strange
 * fish. It only provides a BT event when the switch is flipped to
 * the 'on' position. When flipping it to 'off', the USB device is
 * simply pulled away underneath us, without any BT event being
 * delivered.
=======
>>>>>>> v4.9.227
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/acpi.h>
<<<<<<< HEAD
=======
#include <linux/rfkill.h>

#define BT_KILLSWITCH_MASK	0x01
#define BT_PLUGGED_MASK		0x40
#define BT_POWER_MASK		0x80
>>>>>>> v4.9.227

MODULE_AUTHOR("Jes Sorensen <Jes.Sorensen@gmail.com>");
MODULE_DESCRIPTION("Toshiba Laptop ACPI Bluetooth Enable Driver");
MODULE_LICENSE("GPL");

<<<<<<< HEAD
=======
struct toshiba_bluetooth_dev {
	struct acpi_device *acpi_dev;
	struct rfkill *rfk;

	bool killswitch;
	bool plugged;
	bool powered;
};

>>>>>>> v4.9.227
static int toshiba_bt_rfkill_add(struct acpi_device *device);
static int toshiba_bt_rfkill_remove(struct acpi_device *device);
static void toshiba_bt_rfkill_notify(struct acpi_device *device, u32 event);

static const struct acpi_device_id bt_device_ids[] = {
	{ "TOS6205", 0},
	{ "", 0},
};
MODULE_DEVICE_TABLE(acpi, bt_device_ids);

#ifdef CONFIG_PM_SLEEP
static int toshiba_bt_resume(struct device *dev);
#endif
static SIMPLE_DEV_PM_OPS(toshiba_bt_pm, NULL, toshiba_bt_resume);

static struct acpi_driver toshiba_bt_rfkill_driver = {
	.name =		"Toshiba BT",
	.class =	"Toshiba",
	.ids =		bt_device_ids,
	.ops =		{
				.add =		toshiba_bt_rfkill_add,
				.remove =	toshiba_bt_rfkill_remove,
				.notify =	toshiba_bt_rfkill_notify,
			},
	.owner = 	THIS_MODULE,
	.drv.pm =	&toshiba_bt_pm,
};

<<<<<<< HEAD

static int toshiba_bluetooth_enable(acpi_handle handle)
{
	acpi_status res1, res2;
	u64 result;

	/*
	 * Query ACPI to verify RFKill switch is set to 'on'.
	 * If not, we return silently, no need to report it as
	 * an error.
	 */
	res1 = acpi_evaluate_integer(handle, "BTST", NULL, &result);
	if (ACPI_FAILURE(res1))
		return res1;
	if (!(result & 0x01))
		return 0;

	pr_info("Re-enabling Toshiba Bluetooth\n");
	res1 = acpi_evaluate_object(handle, "AUSB", NULL, NULL);
	res2 = acpi_evaluate_object(handle, "BTPO", NULL, NULL);
	if (!ACPI_FAILURE(res1) || !ACPI_FAILURE(res2))
		return 0;

	pr_warn("Failed to re-enable Toshiba Bluetooth\n");

	return -ENODEV;
}

static void toshiba_bt_rfkill_notify(struct acpi_device *device, u32 event)
{
	toshiba_bluetooth_enable(device->handle);
=======
static int toshiba_bluetooth_present(acpi_handle handle)
{
	acpi_status result;
	u64 bt_present;

	/*
	 * Some Toshiba laptops may have a fake TOS6205 device in
	 * their ACPI BIOS, so query the _STA method to see if there
	 * is really anything there.
	 */
	result = acpi_evaluate_integer(handle, "_STA", NULL, &bt_present);
	if (ACPI_FAILURE(result)) {
		pr_err("ACPI call to query Bluetooth presence failed\n");
		return -ENXIO;
	}

	if (!bt_present) {
		pr_info("Bluetooth device not present\n");
		return -ENODEV;
	}

	return 0;
}

static int toshiba_bluetooth_status(acpi_handle handle)
{
	acpi_status result;
	u64 status;

	result = acpi_evaluate_integer(handle, "BTST", NULL, &status);
	if (ACPI_FAILURE(result)) {
		pr_err("Could not get Bluetooth device status\n");
		return -ENXIO;
	}

	return status;
}

static int toshiba_bluetooth_enable(acpi_handle handle)
{
	acpi_status result;

	result = acpi_evaluate_object(handle, "AUSB", NULL, NULL);
	if (ACPI_FAILURE(result)) {
		pr_err("Could not attach USB Bluetooth device\n");
		return -ENXIO;
	}

	result = acpi_evaluate_object(handle, "BTPO", NULL, NULL);
	if (ACPI_FAILURE(result)) {
		pr_err("Could not power ON Bluetooth device\n");
		return -ENXIO;
	}

	return 0;
}

static int toshiba_bluetooth_disable(acpi_handle handle)
{
	acpi_status result;

	result = acpi_evaluate_object(handle, "BTPF", NULL, NULL);
	if (ACPI_FAILURE(result)) {
		pr_err("Could not power OFF Bluetooth device\n");
		return -ENXIO;
	}

	result = acpi_evaluate_object(handle, "DUSB", NULL, NULL);
	if (ACPI_FAILURE(result)) {
		pr_err("Could not detach USB Bluetooth device\n");
		return -ENXIO;
	}

	return 0;
}

/* Helper function */
static int toshiba_bluetooth_sync_status(struct toshiba_bluetooth_dev *bt_dev)
{
	int status;

	status = toshiba_bluetooth_status(bt_dev->acpi_dev->handle);
	if (status < 0) {
		pr_err("Could not sync bluetooth device status\n");
		return status;
	}

	bt_dev->killswitch = (status & BT_KILLSWITCH_MASK) ? true : false;
	bt_dev->plugged = (status & BT_PLUGGED_MASK) ? true : false;
	bt_dev->powered = (status & BT_POWER_MASK) ? true : false;

	pr_debug("Bluetooth status %d killswitch %d plugged %d powered %d\n",
		 status, bt_dev->killswitch, bt_dev->plugged, bt_dev->powered);

	return 0;
}

/* RFKill handlers */
static int bt_rfkill_set_block(void *data, bool blocked)
{
	struct toshiba_bluetooth_dev *bt_dev = data;
	int ret;

	ret = toshiba_bluetooth_sync_status(bt_dev);
	if (ret)
		return ret;

	if (!bt_dev->killswitch)
		return 0;

	if (blocked)
		ret = toshiba_bluetooth_disable(bt_dev->acpi_dev->handle);
	else
		ret = toshiba_bluetooth_enable(bt_dev->acpi_dev->handle);

	return ret;
}

static void bt_rfkill_poll(struct rfkill *rfkill, void *data)
{
	struct toshiba_bluetooth_dev *bt_dev = data;

	if (toshiba_bluetooth_sync_status(bt_dev))
		return;

	/*
	 * Note the Toshiba Bluetooth RFKill switch seems to be a strange
	 * fish. It only provides a BT event when the switch is flipped to
	 * the 'on' position. When flipping it to 'off', the USB device is
	 * simply pulled away underneath us, without any BT event being
	 * delivered.
	 */
	rfkill_set_hw_state(bt_dev->rfk, !bt_dev->killswitch);
}

static const struct rfkill_ops rfk_ops = {
	.set_block = bt_rfkill_set_block,
	.poll = bt_rfkill_poll,
};

/* ACPI driver functions */
static void toshiba_bt_rfkill_notify(struct acpi_device *device, u32 event)
{
	struct toshiba_bluetooth_dev *bt_dev = acpi_driver_data(device);

	if (toshiba_bluetooth_sync_status(bt_dev))
		return;

	rfkill_set_hw_state(bt_dev->rfk, !bt_dev->killswitch);
>>>>>>> v4.9.227
}

#ifdef CONFIG_PM_SLEEP
static int toshiba_bt_resume(struct device *dev)
{
<<<<<<< HEAD
	return toshiba_bluetooth_enable(to_acpi_device(dev)->handle);
=======
	struct toshiba_bluetooth_dev *bt_dev;
	int ret;

	bt_dev = acpi_driver_data(to_acpi_device(dev));

	ret = toshiba_bluetooth_sync_status(bt_dev);
	if (ret)
		return ret;

	rfkill_set_hw_state(bt_dev->rfk, !bt_dev->killswitch);

	return 0;
>>>>>>> v4.9.227
}
#endif

static int toshiba_bt_rfkill_add(struct acpi_device *device)
{
<<<<<<< HEAD
	acpi_status status;
	u64 bt_present;
	int result = -ENODEV;

	/*
	 * Some Toshiba laptops may have a fake TOS6205 device in
	 * their ACPI BIOS, so query the _STA method to see if there
	 * is really anything there, before trying to enable it.
	 */
	status = acpi_evaluate_integer(device->handle, "_STA", NULL,
				       &bt_present);

	if (!ACPI_FAILURE(status) && bt_present) {
		pr_info("Detected Toshiba ACPI Bluetooth device - "
			"installing RFKill handler\n");
		result = toshiba_bluetooth_enable(device->handle);
=======
	struct toshiba_bluetooth_dev *bt_dev;
	int result;

	result = toshiba_bluetooth_present(device->handle);
	if (result)
		return result;

	pr_info("Toshiba ACPI Bluetooth device driver\n");

	bt_dev = kzalloc(sizeof(*bt_dev), GFP_KERNEL);
	if (!bt_dev)
		return -ENOMEM;
	bt_dev->acpi_dev = device;
	device->driver_data = bt_dev;
	dev_set_drvdata(&device->dev, bt_dev);

	result = toshiba_bluetooth_sync_status(bt_dev);
	if (result) {
		kfree(bt_dev);
		return result;
	}

	bt_dev->rfk = rfkill_alloc("Toshiba Bluetooth",
				   &device->dev,
				   RFKILL_TYPE_BLUETOOTH,
				   &rfk_ops,
				   bt_dev);
	if (!bt_dev->rfk) {
		pr_err("Unable to allocate rfkill device\n");
		kfree(bt_dev);
		return -ENOMEM;
	}

	rfkill_set_hw_state(bt_dev->rfk, !bt_dev->killswitch);

	result = rfkill_register(bt_dev->rfk);
	if (result) {
		pr_err("Unable to register rfkill device\n");
		rfkill_destroy(bt_dev->rfk);
		kfree(bt_dev);
>>>>>>> v4.9.227
	}

	return result;
}

static int toshiba_bt_rfkill_remove(struct acpi_device *device)
{
<<<<<<< HEAD
	/* clean up */
	return 0;
=======
	struct toshiba_bluetooth_dev *bt_dev = acpi_driver_data(device);

	/* clean up */
	if (bt_dev->rfk) {
		rfkill_unregister(bt_dev->rfk);
		rfkill_destroy(bt_dev->rfk);
	}

	kfree(bt_dev);

	return toshiba_bluetooth_disable(device->handle);
>>>>>>> v4.9.227
}

module_acpi_driver(toshiba_bt_rfkill_driver);
