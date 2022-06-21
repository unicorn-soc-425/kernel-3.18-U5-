/*
 * drivers/input/tablet/wacom_wac.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef WACOM_WAC_H
#define WACOM_WAC_H

#include <linux/types.h>
<<<<<<< HEAD

/* maximum packet length for USB devices */
#define WACOM_PKGLEN_MAX	68

#define WACOM_NAME_MAX		64

/* packet length for individual models */
#define WACOM_PKGLEN_PENPRTN	 7
#define WACOM_PKGLEN_GRAPHIRE	 8
#define WACOM_PKGLEN_BBFUN	 9
#define WACOM_PKGLEN_INTUOS	10
=======
#include <linux/hid.h>

/* maximum packet length for USB devices */
#define WACOM_PKGLEN_MAX	192

#define WACOM_NAME_MAX		64
#define WACOM_MAX_REMOTES	5
#define WACOM_STATUS_UNKNOWN	255

/* packet length for individual models */
#define WACOM_PKGLEN_BBFUN	 9
>>>>>>> v4.9.227
#define WACOM_PKGLEN_TPC1FG	 5
#define WACOM_PKGLEN_TPC1FG_B	10
#define WACOM_PKGLEN_TPC2FG	14
#define WACOM_PKGLEN_BBTOUCH	20
#define WACOM_PKGLEN_BBTOUCH3	64
#define WACOM_PKGLEN_BBPEN	10
#define WACOM_PKGLEN_WIRELESS	32
<<<<<<< HEAD
#define WACOM_PKGLEN_MTOUCH	62
#define WACOM_PKGLEN_MTTPC	40
#define WACOM_PKGLEN_DTUS	68
#define WACOM_PKGLEN_PENABLED	 8
=======
#define WACOM_PKGLEN_PENABLED	 8
#define WACOM_PKGLEN_BPAD_TOUCH	32
#define WACOM_PKGLEN_BPAD_TOUCH_USB	64
>>>>>>> v4.9.227

/* wacom data size per MT contact */
#define WACOM_BYTES_PER_MT_PACKET	11
#define WACOM_BYTES_PER_24HDT_PACKET	14
<<<<<<< HEAD
=======
#define WACOM_BYTES_PER_QHDTHID_PACKET	 6
>>>>>>> v4.9.227

/* device IDs */
#define STYLUS_DEVICE_ID	0x02
#define TOUCH_DEVICE_ID		0x03
#define CURSOR_DEVICE_ID	0x06
#define ERASER_DEVICE_ID	0x0A
#define PAD_DEVICE_ID		0x0F

/* wacom data packet report IDs */
#define WACOM_REPORT_PENABLED		2
#define WACOM_REPORT_PENABLED_BT	3
<<<<<<< HEAD
#define WACOM_REPORT_INTUOSREAD		5
#define WACOM_REPORT_INTUOSWRITE	6
=======
#define WACOM_REPORT_INTUOS_ID1		5
#define WACOM_REPORT_INTUOS_ID2		6
>>>>>>> v4.9.227
#define WACOM_REPORT_INTUOSPAD		12
#define WACOM_REPORT_INTUOS5PAD		3
#define WACOM_REPORT_DTUSPAD		21
#define WACOM_REPORT_TPC1FG		6
#define WACOM_REPORT_TPC2FG		13
#define WACOM_REPORT_TPCMT		13
#define WACOM_REPORT_TPCMT2		3
#define WACOM_REPORT_TPCHID		15
<<<<<<< HEAD
=======
#define WACOM_REPORT_CINTIQ		16
#define WACOM_REPORT_CINTIQPAD		17
>>>>>>> v4.9.227
#define WACOM_REPORT_TPCST		16
#define WACOM_REPORT_DTUS		17
#define WACOM_REPORT_TPC1FGE		18
#define WACOM_REPORT_24HDT		1
#define WACOM_REPORT_WL			128
#define WACOM_REPORT_USB		192
<<<<<<< HEAD

/* device quirks */
#define WACOM_QUIRK_MULTI_INPUT		0x0001
#define WACOM_QUIRK_BBTOUCH_LOWRES	0x0002
#define WACOM_QUIRK_NO_INPUT		0x0004
#define WACOM_QUIRK_MONITOR		0x0008
#define WACOM_QUIRK_BATTERY		0x0010
=======
#define WACOM_REPORT_BPAD_PEN		3
#define WACOM_REPORT_BPAD_TOUCH		16
#define WACOM_REPORT_DEVICE_LIST	16
#define WACOM_REPORT_INTUOS_PEN		16
#define WACOM_REPORT_REMOTE		17
#define WACOM_REPORT_INTUOSHT2_ID	8

/* device quirks */
#define WACOM_QUIRK_BBTOUCH_LOWRES	0x0001
#define WACOM_QUIRK_BATTERY		0x0008

/* device types */
#define WACOM_DEVICETYPE_NONE           0x0000
#define WACOM_DEVICETYPE_PEN            0x0001
#define WACOM_DEVICETYPE_TOUCH          0x0002
#define WACOM_DEVICETYPE_PAD            0x0004
#define WACOM_DEVICETYPE_WL_MONITOR     0x0008
#define WACOM_DEVICETYPE_DIRECT         0x0010

#define WACOM_VENDORDEFINED_PEN		0xff0d0001
#define WACOM_G9_PAGE			0xff090000
#define WACOM_G9_DIGITIZER		(WACOM_G9_PAGE | 0x02)
#define WACOM_G9_TOUCHSCREEN		(WACOM_G9_PAGE | 0x11)
#define WACOM_G11_PAGE			0xff110000
#define WACOM_G11_DIGITIZER		(WACOM_G11_PAGE | 0x02)
#define WACOM_G11_TOUCHSCREEN		(WACOM_G11_PAGE | 0x11)

#define WACOM_PEN_FIELD(f)	(((f)->logical == HID_DG_STYLUS) || \
				 ((f)->physical == HID_DG_STYLUS) || \
				 ((f)->physical == HID_DG_PEN) || \
				 ((f)->application == HID_DG_PEN) || \
				 ((f)->application == HID_DG_DIGITIZER) || \
				 ((f)->application == WACOM_VENDORDEFINED_PEN))
#define WACOM_FINGER_FIELD(f)	(((f)->logical == HID_DG_FINGER) || \
				 ((f)->physical == HID_DG_FINGER) || \
				 ((f)->application == HID_DG_TOUCHSCREEN))
>>>>>>> v4.9.227

enum {
	PENPARTNER = 0,
	GRAPHIRE,
	GRAPHIRE_BT,
	WACOM_G4,
	PTU,
	PL,
	DTU,
	DTUS,
<<<<<<< HEAD
=======
	DTUSX,
>>>>>>> v4.9.227
	INTUOS,
	INTUOS3S,
	INTUOS3,
	INTUOS3L,
	INTUOS4S,
	INTUOS4,
	INTUOS4WL,
	INTUOS4L,
	INTUOS5S,
	INTUOS5,
	INTUOS5L,
	INTUOSPS,
	INTUOSPM,
	INTUOSPL,
<<<<<<< HEAD
	INTUOSHT,
=======
>>>>>>> v4.9.227
	WACOM_21UX2,
	WACOM_22HD,
	DTK,
	WACOM_24HD,
<<<<<<< HEAD
	CINTIQ_HYBRID,
=======
	WACOM_27QHD,
	CINTIQ_HYBRID,
	CINTIQ_COMPANION_2,
>>>>>>> v4.9.227
	CINTIQ,
	WACOM_BEE,
	WACOM_13HD,
	WACOM_MO,
<<<<<<< HEAD
	WIRELESS,
	BAMBOO_PT,
	WACOM_24HDT,
=======
	BAMBOO_PEN,
	INTUOSHT,
	INTUOSHT2,
	BAMBOO_TOUCH,
	BAMBOO_PT,
	WACOM_24HDT,
	WACOM_27QHDT,
	BAMBOO_PAD,
	WIRELESS,
	REMOTE,
>>>>>>> v4.9.227
	TABLETPC,   /* add new TPC below */
	TABLETPCE,
	TABLETPC2FG,
	MTSCREEN,
	MTTPC,
	MTTPC_B,
	HID_GENERIC,
	MAX_TYPE
};

struct wacom_features {
	const char *name;
	int x_max;
	int y_max;
	int pressure_max;
	int distance_max;
	int type;
	int x_resolution;
	int y_resolution;
<<<<<<< HEAD
=======
	int numbered_buttons;
>>>>>>> v4.9.227
	int x_min;
	int y_min;
	int device_type;
	int x_phy;
	int y_phy;
	unsigned unit;
	int unitExpo;
	int x_fuzz;
	int y_fuzz;
	int pressure_fuzz;
	int distance_fuzz;
<<<<<<< HEAD
=======
	int tilt_fuzz;
>>>>>>> v4.9.227
	unsigned quirks;
	unsigned touch_max;
	int oVid;
	int oPid;
	int pktlen;
	bool check_for_hid_type;
	int hid_type;
};

struct wacom_shared {
	bool stylus_in_proximity;
	bool touch_down;
	/* for wireless device to access USB interfaces */
	unsigned touch_max;
	int type;
	struct input_dev *touch_input;
<<<<<<< HEAD
=======
	struct hid_device *pen;
	struct hid_device *touch;
>>>>>>> v4.9.227
};

struct hid_data {
	__s16 inputmode;	/* InputMode HID feature, -1 if non-existent */
	__s16 inputmode_index;	/* InputMode HID feature index in the report */
	bool inrange_state;
	bool invert_state;
	bool tipswitch;
	int x;
	int y;
	int pressure;
	int width;
	int height;
	int id;
<<<<<<< HEAD
=======
	int cc_report;
	int cc_index;
	int cc_value_index;
	int last_slot_field;
	int num_expected;
	int num_received;
};

struct wacom_remote_data {
	struct {
		u32 serial;
		bool connected;
	} remote[WACOM_MAX_REMOTES];
>>>>>>> v4.9.227
};

struct wacom_wac {
	char name[WACOM_NAME_MAX];
<<<<<<< HEAD
	char pad_name[WACOM_NAME_MAX];
	char bat_name[WACOM_NAME_MAX];
	char ac_name[WACOM_NAME_MAX];
=======
	char pen_name[WACOM_NAME_MAX];
	char touch_name[WACOM_NAME_MAX];
	char pad_name[WACOM_NAME_MAX];
>>>>>>> v4.9.227
	unsigned char data[WACOM_PKGLEN_MAX];
	int tool[2];
	int id[2];
	__u32 serial[2];
<<<<<<< HEAD
	struct wacom_features features;
	struct wacom_shared *shared;
	struct input_dev *input;
	struct input_dev *pad_input;
	bool input_registered;
	int pid;
	int battery_capacity;
	int num_contacts_left;
	int bat_charging;
	int ps_connected;
	u8 bt_features;
	u8 bt_high_speed;
=======
	bool reporting_data;
	struct wacom_features features;
	struct wacom_shared *shared;
	struct input_dev *pen_input;
	struct input_dev *touch_input;
	struct input_dev *pad_input;
	int pid;
	int num_contacts_left;
	u8 bt_features;
	u8 bt_high_speed;
	int mode_report;
	int mode_value;
>>>>>>> v4.9.227
	struct hid_data hid_data;
};

#endif
