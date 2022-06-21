/*
 * This header declares the utility functions used by "Gadget Zero", plus
 * interfaces to its two single-configuration function drivers.
 */

#ifndef __G_ZERO_H
#define __G_ZERO_H

#define GZERO_BULK_BUFLEN	4096
#define GZERO_QLEN		32
#define GZERO_ISOC_INTERVAL	4
#define GZERO_ISOC_MAXPACKET	1024
<<<<<<< HEAD
#define GZERO_INT_INTERVAL	1 /* Default interrupt interval = 1 ms */
#define GZERO_INT_MAXPACKET	1024
=======
#define GZERO_SS_BULK_QLEN	1
#define GZERO_SS_ISO_QLEN	8
>>>>>>> v4.9.227

struct usb_zero_options {
	unsigned pattern;
	unsigned isoc_interval;
	unsigned isoc_maxpacket;
	unsigned isoc_mult;
	unsigned isoc_maxburst;
<<<<<<< HEAD
	unsigned int_interval; /* In ms */
	unsigned int_maxpacket;
	unsigned int_mult;
	unsigned int_maxburst;
	unsigned bulk_buflen;
	unsigned qlen;
=======
	unsigned bulk_buflen;
	unsigned qlen;
	unsigned ss_bulk_qlen;
	unsigned ss_iso_qlen;
>>>>>>> v4.9.227
};

struct f_ss_opts {
	struct usb_function_instance func_inst;
	unsigned pattern;
	unsigned isoc_interval;
	unsigned isoc_maxpacket;
	unsigned isoc_mult;
	unsigned isoc_maxburst;
<<<<<<< HEAD
	unsigned int_interval; /* In ms */
	unsigned int_maxpacket;
	unsigned int_mult;
	unsigned int_maxburst;
	unsigned bulk_buflen;
=======
	unsigned bulk_buflen;
	unsigned bulk_qlen;
	unsigned iso_qlen;
>>>>>>> v4.9.227

	/*
	 * Read/write access to configfs attributes is handled by configfs.
	 *
	 * This is to protect the data from concurrent access by read/write
	 * and create symlink/remove symlink.
	 */
	struct mutex			lock;
	int				refcnt;
};

struct f_lb_opts {
	struct usb_function_instance func_inst;
	unsigned bulk_buflen;
	unsigned qlen;

	/*
	 * Read/write access to configfs attributes is handled by configfs.
	 *
	 * This is to protect the data from concurrent access by read/write
	 * and create symlink/remove symlink.
	 */
	struct mutex			lock;
	int				refcnt;
};

void lb_modexit(void);
int lb_modinit(void);

/* common utilities */
void disable_endpoints(struct usb_composite_dev *cdev,
		struct usb_ep *in, struct usb_ep *out,
<<<<<<< HEAD
		struct usb_ep *iso_in, struct usb_ep *iso_out,
		struct usb_ep *int_in, struct usb_ep *int_out);
=======
		struct usb_ep *iso_in, struct usb_ep *iso_out);
>>>>>>> v4.9.227

#endif /* __G_ZERO_H */
