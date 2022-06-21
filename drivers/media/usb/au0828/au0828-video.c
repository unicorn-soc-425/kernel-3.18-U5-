/*
 * Auvitek AU0828 USB Bridge (Analog video support)
 *
 * Copyright (C) 2009 Devin Heitmueller <dheitmueller@linuxtv.org>
 * Copyright (C) 2005-2008 Auvitek International, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * As published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/* Developer Notes:
 *
<<<<<<< HEAD
 * VBI support is not yet working
=======
>>>>>>> v4.9.227
 * The hardware scaler supported is unimplemented
 * AC97 audio support is unimplemented (only i2s audio mode)
 *
 */

#include "au0828.h"
<<<<<<< HEAD
=======
#include "au8522.h"
>>>>>>> v4.9.227

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/device.h>
#include <media/v4l2-common.h>
<<<<<<< HEAD
=======
#include <media/v4l2-mc.h>
>>>>>>> v4.9.227
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include <media/tuner.h>
#include "au0828-reg.h"

static DEFINE_MUTEX(au0828_sysfs_lock);

/* ------------------------------------------------------------------
	Videobuf operations
   ------------------------------------------------------------------*/

static unsigned int isoc_debug;
module_param(isoc_debug, int, 0644);
MODULE_PARM_DESC(isoc_debug, "enable debug messages [isoc transfers]");

#define au0828_isocdbg(fmt, arg...) \
do {\
	if (isoc_debug) { \
		pr_info("au0828 %s :"fmt, \
		       __func__ , ##arg);	   \
	} \
  } while (0)

static inline void i2c_gate_ctrl(struct au0828_dev *dev, int val)
{
	if (dev->dvb.frontend && dev->dvb.frontend->ops.analog_ops.i2c_gate_ctrl)
		dev->dvb.frontend->ops.analog_ops.i2c_gate_ctrl(dev->dvb.frontend, val);
}

static inline void print_err_status(struct au0828_dev *dev,
				    int packet, int status)
{
	char *errmsg = "Unknown";

	switch (status) {
	case -ENOENT:
		errmsg = "unlinked synchronuously";
		break;
	case -ECONNRESET:
		errmsg = "unlinked asynchronuously";
		break;
	case -ENOSR:
		errmsg = "Buffer error (overrun)";
		break;
	case -EPIPE:
		errmsg = "Stalled (device not responding)";
		break;
	case -EOVERFLOW:
		errmsg = "Babble (bad cable?)";
		break;
	case -EPROTO:
		errmsg = "Bit-stuff error (bad cable?)";
		break;
	case -EILSEQ:
		errmsg = "CRC/Timeout (could be anything)";
		break;
	case -ETIME:
		errmsg = "Device does not respond";
		break;
	}
	if (packet < 0) {
		au0828_isocdbg("URB status %d [%s].\n",	status, errmsg);
	} else {
		au0828_isocdbg("URB packet %d, status %d [%s].\n",
			       packet, status, errmsg);
	}
}

static int check_dev(struct au0828_dev *dev)
{
<<<<<<< HEAD
	if (dev->dev_state & DEV_DISCONNECTED) {
=======
	if (test_bit(DEV_DISCONNECTED, &dev->dev_state)) {
>>>>>>> v4.9.227
		pr_info("v4l2 ioctl: device not present\n");
		return -ENODEV;
	}

<<<<<<< HEAD
	if (dev->dev_state & DEV_MISCONFIGURED) {
		pr_info("v4l2 ioctl: device is misconfigured; "
		       "close and open it again\n");
=======
	if (test_bit(DEV_MISCONFIGURED, &dev->dev_state)) {
		pr_info("v4l2 ioctl: device is misconfigured; close and open it again\n");
>>>>>>> v4.9.227
		return -EIO;
	}
	return 0;
}

/*
 * IRQ callback, called by URB callback
 */
static void au0828_irq_callback(struct urb *urb)
{
	struct au0828_dmaqueue  *dma_q = urb->context;
	struct au0828_dev *dev = container_of(dma_q, struct au0828_dev, vidq);
	unsigned long flags = 0;
	int i;

	switch (urb->status) {
	case 0:             /* success */
	case -ETIMEDOUT:    /* NAK */
		break;
	case -ECONNRESET:   /* kill */
	case -ENOENT:
	case -ESHUTDOWN:
		au0828_isocdbg("au0828_irq_callback called: status kill\n");
		return;
	default:            /* unknown error */
		au0828_isocdbg("urb completition error %d.\n", urb->status);
		break;
	}

	/* Copy data from URB */
	spin_lock_irqsave(&dev->slock, flags);
	dev->isoc_ctl.isoc_copy(dev, urb);
	spin_unlock_irqrestore(&dev->slock, flags);

	/* Reset urb buffers */
	for (i = 0; i < urb->number_of_packets; i++) {
		urb->iso_frame_desc[i].status = 0;
		urb->iso_frame_desc[i].actual_length = 0;
	}
	urb->status = 0;

	urb->status = usb_submit_urb(urb, GFP_ATOMIC);
	if (urb->status) {
		au0828_isocdbg("urb resubmit failed (error=%i)\n",
			       urb->status);
	}
	dev->stream_state = STREAM_ON;
}

/*
 * Stop and Deallocate URBs
 */
static void au0828_uninit_isoc(struct au0828_dev *dev)
{
	struct urb *urb;
	int i;

	au0828_isocdbg("au0828: called au0828_uninit_isoc\n");

	dev->isoc_ctl.nfields = -1;
	for (i = 0; i < dev->isoc_ctl.num_bufs; i++) {
		urb = dev->isoc_ctl.urb[i];
		if (urb) {
			if (!irqs_disabled())
				usb_kill_urb(urb);
			else
				usb_unlink_urb(urb);

			if (dev->isoc_ctl.transfer_buffer[i]) {
				usb_free_coherent(dev->usbdev,
					urb->transfer_buffer_length,
					dev->isoc_ctl.transfer_buffer[i],
					urb->transfer_dma);
			}
			usb_free_urb(urb);
			dev->isoc_ctl.urb[i] = NULL;
		}
		dev->isoc_ctl.transfer_buffer[i] = NULL;
	}

	kfree(dev->isoc_ctl.urb);
	kfree(dev->isoc_ctl.transfer_buffer);

	dev->isoc_ctl.urb = NULL;
	dev->isoc_ctl.transfer_buffer = NULL;
	dev->isoc_ctl.num_bufs = 0;

	dev->stream_state = STREAM_OFF;
}

/*
 * Allocate URBs and start IRQ
 */
static int au0828_init_isoc(struct au0828_dev *dev, int max_packets,
			    int num_bufs, int max_pkt_size,
			    int (*isoc_copy) (struct au0828_dev *dev, struct urb *urb))
{
	struct au0828_dmaqueue *dma_q = &dev->vidq;
	int i;
	int sb_size, pipe;
	struct urb *urb;
	int j, k;
	int rc;

	au0828_isocdbg("au0828: called au0828_prepare_isoc\n");

<<<<<<< HEAD
	/* De-allocates all pending stuff */
	au0828_uninit_isoc(dev);

=======
>>>>>>> v4.9.227
	dev->isoc_ctl.isoc_copy = isoc_copy;
	dev->isoc_ctl.num_bufs = num_bufs;

	dev->isoc_ctl.urb = kzalloc(sizeof(void *)*num_bufs,  GFP_KERNEL);
	if (!dev->isoc_ctl.urb) {
		au0828_isocdbg("cannot alloc memory for usb buffers\n");
		return -ENOMEM;
	}

	dev->isoc_ctl.transfer_buffer = kzalloc(sizeof(void *)*num_bufs,
					      GFP_KERNEL);
	if (!dev->isoc_ctl.transfer_buffer) {
		au0828_isocdbg("cannot allocate memory for usb transfer\n");
		kfree(dev->isoc_ctl.urb);
		return -ENOMEM;
	}

	dev->isoc_ctl.max_pkt_size = max_pkt_size;
	dev->isoc_ctl.buf = NULL;

	sb_size = max_packets * dev->isoc_ctl.max_pkt_size;

	/* allocate urbs and transfer buffers */
	for (i = 0; i < dev->isoc_ctl.num_bufs; i++) {
		urb = usb_alloc_urb(max_packets, GFP_KERNEL);
		if (!urb) {
<<<<<<< HEAD
			au0828_isocdbg("cannot alloc isoc_ctl.urb %i\n", i);
=======
>>>>>>> v4.9.227
			au0828_uninit_isoc(dev);
			return -ENOMEM;
		}
		dev->isoc_ctl.urb[i] = urb;

		dev->isoc_ctl.transfer_buffer[i] = usb_alloc_coherent(dev->usbdev,
			sb_size, GFP_KERNEL, &urb->transfer_dma);
		if (!dev->isoc_ctl.transfer_buffer[i]) {
			printk("unable to allocate %i bytes for transfer"
					" buffer %i%s\n",
					sb_size, i,
					in_interrupt() ? " while in int" : "");
			au0828_uninit_isoc(dev);
			return -ENOMEM;
		}
		memset(dev->isoc_ctl.transfer_buffer[i], 0, sb_size);

		pipe = usb_rcvisocpipe(dev->usbdev,
				       dev->isoc_in_endpointaddr),

		usb_fill_int_urb(urb, dev->usbdev, pipe,
				 dev->isoc_ctl.transfer_buffer[i], sb_size,
				 au0828_irq_callback, dma_q, 1);

		urb->number_of_packets = max_packets;
		urb->transfer_flags = URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;

		k = 0;
		for (j = 0; j < max_packets; j++) {
			urb->iso_frame_desc[j].offset = k;
			urb->iso_frame_desc[j].length =
						dev->isoc_ctl.max_pkt_size;
			k += dev->isoc_ctl.max_pkt_size;
		}
	}

<<<<<<< HEAD
	init_waitqueue_head(&dma_q->wq);

=======
>>>>>>> v4.9.227
	/* submit urbs and enables IRQ */
	for (i = 0; i < dev->isoc_ctl.num_bufs; i++) {
		rc = usb_submit_urb(dev->isoc_ctl.urb[i], GFP_ATOMIC);
		if (rc) {
			au0828_isocdbg("submit of urb %i failed (error=%i)\n",
				       i, rc);
			au0828_uninit_isoc(dev);
			return rc;
		}
	}

	return 0;
}

/*
 * Announces that a buffer were filled and request the next
 */
static inline void buffer_filled(struct au0828_dev *dev,
<<<<<<< HEAD
				  struct au0828_dmaqueue *dma_q,
				  struct au0828_buffer *buf)
{
	/* Advice that buffer was filled */
	au0828_isocdbg("[%p/%d] wakeup\n", buf, buf->vb.i);

	buf->vb.state = VIDEOBUF_DONE;
	buf->vb.field_count++;
	v4l2_get_timestamp(&buf->vb.ts);

	dev->isoc_ctl.buf = NULL;

	list_del(&buf->vb.queue);
	wake_up(&buf->vb.done);
}

static inline void vbi_buffer_filled(struct au0828_dev *dev,
				     struct au0828_dmaqueue *dma_q,
				     struct au0828_buffer *buf)
{
	/* Advice that buffer was filled */
	au0828_isocdbg("[%p/%d] wakeup\n", buf, buf->vb.i);

	buf->vb.state = VIDEOBUF_DONE;
	buf->vb.field_count++;
	v4l2_get_timestamp(&buf->vb.ts);

	dev->isoc_ctl.vbi_buf = NULL;

	list_del(&buf->vb.queue);
	wake_up(&buf->vb.done);
=======
				 struct au0828_dmaqueue *dma_q,
				 struct au0828_buffer *buf)
{
	struct vb2_v4l2_buffer *vb = &buf->vb;
	struct vb2_queue *q = vb->vb2_buf.vb2_queue;

	/* Advice that buffer was filled */
	au0828_isocdbg("[%p/%d] wakeup\n", buf, buf->top_field);

	if (q->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		vb->sequence = dev->frame_count++;
	else
		vb->sequence = dev->vbi_frame_count++;

	vb->field = V4L2_FIELD_INTERLACED;
	vb->vb2_buf.timestamp = ktime_get_ns();
	vb2_buffer_done(&vb->vb2_buf, VB2_BUF_STATE_DONE);
>>>>>>> v4.9.227
}

/*
 * Identify the buffer header type and properly handles
 */
static void au0828_copy_video(struct au0828_dev *dev,
			      struct au0828_dmaqueue  *dma_q,
			      struct au0828_buffer *buf,
			      unsigned char *p,
			      unsigned char *outp, unsigned long len)
{
	void *fieldstart, *startwrite, *startread;
	int  linesdone, currlinedone, offset, lencopy, remain;
	int bytesperline = dev->width << 1; /* Assumes 16-bit depth @@@@ */

	if (len == 0)
		return;

<<<<<<< HEAD
	if (dma_q->pos + len > buf->vb.size)
		len = buf->vb.size - dma_q->pos;
=======
	if (dma_q->pos + len > buf->length)
		len = buf->length - dma_q->pos;
>>>>>>> v4.9.227

	startread = p;
	remain = len;

	/* Interlaces frame */
	if (buf->top_field)
		fieldstart = outp;
	else
		fieldstart = outp + bytesperline;

	linesdone = dma_q->pos / bytesperline;
	currlinedone = dma_q->pos % bytesperline;
	offset = linesdone * bytesperline * 2 + currlinedone;
	startwrite = fieldstart + offset;
	lencopy = bytesperline - currlinedone;
	lencopy = lencopy > remain ? remain : lencopy;

<<<<<<< HEAD
	if ((char *)startwrite + lencopy > (char *)outp + buf->vb.size) {
		au0828_isocdbg("Overflow of %zi bytes past buffer end (1)\n",
			       ((char *)startwrite + lencopy) -
			       ((char *)outp + buf->vb.size));
		remain = (char *)outp + buf->vb.size - (char *)startwrite;
=======
	if ((char *)startwrite + lencopy > (char *)outp + buf->length) {
		au0828_isocdbg("Overflow of %zi bytes past buffer end (1)\n",
			       ((char *)startwrite + lencopy) -
			       ((char *)outp + buf->length));
		remain = (char *)outp + buf->length - (char *)startwrite;
>>>>>>> v4.9.227
		lencopy = remain;
	}
	if (lencopy <= 0)
		return;
	memcpy(startwrite, startread, lencopy);

	remain -= lencopy;

	while (remain > 0) {
		startwrite += lencopy + bytesperline;
		startread += lencopy;
		if (bytesperline > remain)
			lencopy = remain;
		else
			lencopy = bytesperline;

		if ((char *)startwrite + lencopy > (char *)outp +
<<<<<<< HEAD
		    buf->vb.size) {
			au0828_isocdbg("Overflow %zi bytes past buf end (2)\n",
				       ((char *)startwrite + lencopy) -
				       ((char *)outp + buf->vb.size));
			lencopy = remain = (char *)outp + buf->vb.size -
=======
		    buf->length) {
			au0828_isocdbg("Overflow %zi bytes past buf end (2)\n",
				       ((char *)startwrite + lencopy) -
				       ((char *)outp + buf->length));
			lencopy = remain = (char *)outp + buf->length -
>>>>>>> v4.9.227
					   (char *)startwrite;
		}
		if (lencopy <= 0)
			break;

		memcpy(startwrite, startread, lencopy);

		remain -= lencopy;
	}

	if (offset > 1440) {
		/* We have enough data to check for greenscreen */
		if (outp[0] < 0x60 && outp[1440] < 0x60)
			dev->greenscreen_detected = 1;
	}

	dma_q->pos += len;
}

/*
 * video-buf generic routine to get the next available buffer
 */
static inline void get_next_buf(struct au0828_dmaqueue *dma_q,
				struct au0828_buffer **buf)
{
	struct au0828_dev *dev = container_of(dma_q, struct au0828_dev, vidq);

	if (list_empty(&dma_q->active)) {
		au0828_isocdbg("No active queue to serve\n");
		dev->isoc_ctl.buf = NULL;
		*buf = NULL;
		return;
	}

	/* Get the next buffer */
<<<<<<< HEAD
	*buf = list_entry(dma_q->active.next, struct au0828_buffer, vb.queue);
=======
	*buf = list_entry(dma_q->active.next, struct au0828_buffer, list);
	/* Cleans up buffer - Useful for testing for frame/URB loss */
	list_del(&(*buf)->list);
	dma_q->pos = 0;
	(*buf)->vb_buf = (*buf)->mem;
>>>>>>> v4.9.227
	dev->isoc_ctl.buf = *buf;

	return;
}

static void au0828_copy_vbi(struct au0828_dev *dev,
			      struct au0828_dmaqueue  *dma_q,
			      struct au0828_buffer *buf,
			      unsigned char *p,
			      unsigned char *outp, unsigned long len)
{
	unsigned char *startwrite, *startread;
	int bytesperline;
	int i, j = 0;

	if (dev == NULL) {
		au0828_isocdbg("dev is null\n");
		return;
	}

	if (dma_q == NULL) {
		au0828_isocdbg("dma_q is null\n");
		return;
	}
	if (buf == NULL)
		return;
	if (p == NULL) {
		au0828_isocdbg("p is null\n");
		return;
	}
	if (outp == NULL) {
		au0828_isocdbg("outp is null\n");
		return;
	}

	bytesperline = dev->vbi_width;

<<<<<<< HEAD
	if (dma_q->pos + len > buf->vb.size)
		len = buf->vb.size - dma_q->pos;
=======
	if (dma_q->pos + len > buf->length)
		len = buf->length - dma_q->pos;
>>>>>>> v4.9.227

	startread = p;
	startwrite = outp + (dma_q->pos / 2);

	/* Make sure the bottom field populates the second half of the frame */
	if (buf->top_field == 0)
		startwrite += bytesperline * dev->vbi_height;

	for (i = 0; i < len; i += 2)
		startwrite[j++] = startread[i+1];

	dma_q->pos += len;
}


/*
 * video-buf generic routine to get the next available VBI buffer
 */
static inline void vbi_get_next_buf(struct au0828_dmaqueue *dma_q,
				    struct au0828_buffer **buf)
{
	struct au0828_dev *dev = container_of(dma_q, struct au0828_dev, vbiq);
<<<<<<< HEAD
	char *outp;
=======
>>>>>>> v4.9.227

	if (list_empty(&dma_q->active)) {
		au0828_isocdbg("No active queue to serve\n");
		dev->isoc_ctl.vbi_buf = NULL;
		*buf = NULL;
		return;
	}

	/* Get the next buffer */
<<<<<<< HEAD
	*buf = list_entry(dma_q->active.next, struct au0828_buffer, vb.queue);
	/* Cleans up buffer - Useful for testing for frame/URB loss */
	outp = videobuf_to_vmalloc(&(*buf)->vb);
	memset(outp, 0x00, (*buf)->vb.size);

	dev->isoc_ctl.vbi_buf = *buf;

=======
	*buf = list_entry(dma_q->active.next, struct au0828_buffer, list);
	/* Cleans up buffer - Useful for testing for frame/URB loss */
	list_del(&(*buf)->list);
	dma_q->pos = 0;
	(*buf)->vb_buf = (*buf)->mem;
	dev->isoc_ctl.vbi_buf = *buf;
>>>>>>> v4.9.227
	return;
}

/*
 * Controls the isoc copy of each urb packet
 */
static inline int au0828_isoc_copy(struct au0828_dev *dev, struct urb *urb)
{
	struct au0828_buffer    *buf;
	struct au0828_buffer    *vbi_buf;
	struct au0828_dmaqueue  *dma_q = urb->context;
	struct au0828_dmaqueue  *vbi_dma_q = &dev->vbiq;
	unsigned char *outp = NULL;
	unsigned char *vbioutp = NULL;
	int i, len = 0, rc = 1;
	unsigned char *p;
	unsigned char fbyte;
	unsigned int vbi_field_size;
	unsigned int remain, lencopy;

	if (!dev)
		return 0;

<<<<<<< HEAD
	if ((dev->dev_state & DEV_DISCONNECTED) ||
	    (dev->dev_state & DEV_MISCONFIGURED))
=======
	if (test_bit(DEV_DISCONNECTED, &dev->dev_state) ||
	    test_bit(DEV_MISCONFIGURED, &dev->dev_state))
>>>>>>> v4.9.227
		return 0;

	if (urb->status < 0) {
		print_err_status(dev, -1, urb->status);
		if (urb->status == -ENOENT)
			return 0;
	}

	buf = dev->isoc_ctl.buf;
	if (buf != NULL)
<<<<<<< HEAD
		outp = videobuf_to_vmalloc(&buf->vb);

	vbi_buf = dev->isoc_ctl.vbi_buf;
	if (vbi_buf != NULL)
		vbioutp = videobuf_to_vmalloc(&vbi_buf->vb);
=======
		outp = vb2_plane_vaddr(&buf->vb.vb2_buf, 0);

	vbi_buf = dev->isoc_ctl.vbi_buf;
	if (vbi_buf != NULL)
		vbioutp = vb2_plane_vaddr(&vbi_buf->vb.vb2_buf, 0);
>>>>>>> v4.9.227

	for (i = 0; i < urb->number_of_packets; i++) {
		int status = urb->iso_frame_desc[i].status;

		if (status < 0) {
			print_err_status(dev, i, status);
			if (urb->iso_frame_desc[i].status != -EPROTO)
				continue;
		}

		if (urb->iso_frame_desc[i].actual_length <= 0)
			continue;

		if (urb->iso_frame_desc[i].actual_length >
						dev->max_pkt_size) {
			au0828_isocdbg("packet bigger than packet size");
			continue;
		}

		p = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
		fbyte = p[0];
		len = urb->iso_frame_desc[i].actual_length - 4;
		p += 4;

		if (fbyte & 0x80) {
			len -= 4;
			p += 4;
			au0828_isocdbg("Video frame %s\n",
				       (fbyte & 0x40) ? "odd" : "even");
			if (fbyte & 0x40) {
				/* VBI */
				if (vbi_buf != NULL)
<<<<<<< HEAD
					vbi_buffer_filled(dev,
							  vbi_dma_q,
							  vbi_buf);
=======
					buffer_filled(dev, vbi_dma_q, vbi_buf);
>>>>>>> v4.9.227
				vbi_get_next_buf(vbi_dma_q, &vbi_buf);
				if (vbi_buf == NULL)
					vbioutp = NULL;
				else
<<<<<<< HEAD
					vbioutp = videobuf_to_vmalloc(
						&vbi_buf->vb);
=======
					vbioutp = vb2_plane_vaddr(
						&vbi_buf->vb.vb2_buf, 0);
>>>>>>> v4.9.227

				/* Video */
				if (buf != NULL)
					buffer_filled(dev, dma_q, buf);
				get_next_buf(dma_q, &buf);
				if (buf == NULL)
					outp = NULL;
				else
<<<<<<< HEAD
					outp = videobuf_to_vmalloc(&buf->vb);
=======
					outp = vb2_plane_vaddr(
						&buf->vb.vb2_buf, 0);
>>>>>>> v4.9.227

				/* As long as isoc traffic is arriving, keep
				   resetting the timer */
				if (dev->vid_timeout_running)
					mod_timer(&dev->vid_timeout,
						  jiffies + (HZ / 10));
				if (dev->vbi_timeout_running)
					mod_timer(&dev->vbi_timeout,
						  jiffies + (HZ / 10));
			}

			if (buf != NULL) {
				if (fbyte & 0x40)
					buf->top_field = 1;
				else
					buf->top_field = 0;
			}

			if (vbi_buf != NULL) {
				if (fbyte & 0x40)
					vbi_buf->top_field = 1;
				else
					vbi_buf->top_field = 0;
			}

			dev->vbi_read = 0;
			vbi_dma_q->pos = 0;
			dma_q->pos = 0;
		}

		vbi_field_size = dev->vbi_width * dev->vbi_height * 2;
		if (dev->vbi_read < vbi_field_size) {
			remain  = vbi_field_size - dev->vbi_read;
			if (len < remain)
				lencopy = len;
			else
				lencopy = remain;

			if (vbi_buf != NULL)
				au0828_copy_vbi(dev, vbi_dma_q, vbi_buf, p,
						vbioutp, len);

			len -= lencopy;
			p += lencopy;
			dev->vbi_read += lencopy;
		}

		if (dev->vbi_read >= vbi_field_size && buf != NULL)
			au0828_copy_video(dev, dma_q, buf, p, outp, len);
	}
	return rc;
}

<<<<<<< HEAD
static int
buffer_setup(struct videobuf_queue *vq, unsigned int *count,
	     unsigned int *size)
{
	struct au0828_fh *fh = vq->priv_data;
	*size = (fh->dev->width * fh->dev->height * 16 + 7) >> 3;

	if (0 == *count)
		*count = AU0828_DEF_BUF;

	if (*count < AU0828_MIN_BUF)
		*count = AU0828_MIN_BUF;
	return 0;
}

/* This is called *without* dev->slock held; please keep it that way */
static void free_buffer(struct videobuf_queue *vq, struct au0828_buffer *buf)
{
	struct au0828_fh     *fh  = vq->priv_data;
	struct au0828_dev    *dev = fh->dev;
	unsigned long flags = 0;
	if (in_interrupt())
		BUG();

	/* We used to wait for the buffer to finish here, but this didn't work
	   because, as we were keeping the state as VIDEOBUF_QUEUED,
	   videobuf_queue_cancel marked it as finished for us.
	   (Also, it could wedge forever if the hardware was misconfigured.)

	   This should be safe; by the time we get here, the buffer isn't
	   queued anymore. If we ever start marking the buffers as
	   VIDEOBUF_ACTIVE, it won't be, though.
	*/
	spin_lock_irqsave(&dev->slock, flags);
	if (dev->isoc_ctl.buf == buf)
		dev->isoc_ctl.buf = NULL;
	spin_unlock_irqrestore(&dev->slock, flags);

	videobuf_vmalloc_free(&buf->vb);
	buf->vb.state = VIDEOBUF_NEEDS_INIT;
}

static int
buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb,
						enum v4l2_field field)
{
	struct au0828_fh     *fh  = vq->priv_data;
	struct au0828_buffer *buf = container_of(vb, struct au0828_buffer, vb);
	struct au0828_dev    *dev = fh->dev;
	int                  rc = 0, urb_init = 0;

	buf->vb.size = (fh->dev->width * fh->dev->height * 16 + 7) >> 3;

	if (0 != buf->vb.baddr  &&  buf->vb.bsize < buf->vb.size)
		return -EINVAL;

	buf->vb.width  = dev->width;
	buf->vb.height = dev->height;
	buf->vb.field  = field;

	if (VIDEOBUF_NEEDS_INIT == buf->vb.state) {
		rc = videobuf_iolock(vq, &buf->vb, NULL);
		if (rc < 0) {
			pr_info("videobuf_iolock failed\n");
			goto fail;
		}
	}

	if (!dev->isoc_ctl.num_bufs)
		urb_init = 1;

	if (urb_init) {
		rc = au0828_init_isoc(dev, AU0828_ISO_PACKETS_PER_URB,
				      AU0828_MAX_ISO_BUFS, dev->max_pkt_size,
				      au0828_isoc_copy);
		if (rc < 0) {
			pr_info("au0828_init_isoc failed\n");
			goto fail;
		}
	}

	buf->vb.state = VIDEOBUF_PREPARED;
	return 0;

fail:
	free_buffer(vq, buf);
	return rc;
}

static void
buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
	struct au0828_buffer    *buf     = container_of(vb,
							struct au0828_buffer,
							vb);
	struct au0828_fh        *fh      = vq->priv_data;
	struct au0828_dev       *dev     = fh->dev;
	struct au0828_dmaqueue  *vidq    = &dev->vidq;

	buf->vb.state = VIDEOBUF_QUEUED;
	list_add_tail(&buf->vb.queue, &vidq->active);
}

static void buffer_release(struct videobuf_queue *vq,
				struct videobuf_buffer *vb)
{
	struct au0828_buffer   *buf  = container_of(vb,
						    struct au0828_buffer,
						    vb);

	free_buffer(vq, buf);
}

static struct videobuf_queue_ops au0828_video_qops = {
	.buf_setup      = buffer_setup,
	.buf_prepare    = buffer_prepare,
	.buf_queue      = buffer_queue,
	.buf_release    = buffer_release,
};

/* ------------------------------------------------------------------
   V4L2 interface
   ------------------------------------------------------------------*/

=======
void au0828_usb_v4l2_media_release(struct au0828_dev *dev)
{
#ifdef CONFIG_MEDIA_CONTROLLER
	int i;

	for (i = 0; i < AU0828_MAX_INPUT; i++) {
		if (AUVI_INPUT(i).type == AU0828_VMUX_UNDEFINED)
			return;
		media_device_unregister_entity(&dev->input_ent[i]);
	}
#endif
}

static void au0828_usb_v4l2_release(struct v4l2_device *v4l2_dev)
{
	struct au0828_dev *dev =
		container_of(v4l2_dev, struct au0828_dev, v4l2_dev);

	v4l2_ctrl_handler_free(&dev->v4l2_ctrl_hdl);
	v4l2_device_unregister(&dev->v4l2_dev);
	au0828_usb_v4l2_media_release(dev);
	au0828_usb_release(dev);
}

int au0828_v4l2_device_register(struct usb_interface *interface,
				struct au0828_dev *dev)
{
	int retval;

	if (AUVI_INPUT(0).type == AU0828_VMUX_UNDEFINED)
		return 0;

	/* Create the v4l2_device */
#ifdef CONFIG_MEDIA_CONTROLLER
	dev->v4l2_dev.mdev = dev->media_dev;
#endif
	retval = v4l2_device_register(&interface->dev, &dev->v4l2_dev);
	if (retval) {
		pr_err("%s() v4l2_device_register failed\n",
		       __func__);
		return retval;
	}

	dev->v4l2_dev.release = au0828_usb_v4l2_release;

	/* This control handler will inherit the controls from au8522 */
	retval = v4l2_ctrl_handler_init(&dev->v4l2_ctrl_hdl, 4);
	if (retval) {
		pr_err("%s() v4l2_ctrl_handler_init failed\n",
		       __func__);
		return retval;
	}
	dev->v4l2_dev.ctrl_handler = &dev->v4l2_ctrl_hdl;

	return 0;
}

static int queue_setup(struct vb2_queue *vq,
		       unsigned int *nbuffers, unsigned int *nplanes,
		       unsigned int sizes[], struct device *alloc_devs[])
{
	struct au0828_dev *dev = vb2_get_drv_priv(vq);
	unsigned long size = dev->height * dev->bytesperline;

	if (*nplanes)
		return sizes[0] < size ? -EINVAL : 0;
	*nplanes = 1;
	sizes[0] = size;
	return 0;
}

static int
buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct au0828_buffer *buf = container_of(vbuf,
				struct au0828_buffer, vb);
	struct au0828_dev    *dev = vb2_get_drv_priv(vb->vb2_queue);

	buf->length = dev->height * dev->bytesperline;

	if (vb2_plane_size(vb, 0) < buf->length) {
		pr_err("%s data will not fit into plane (%lu < %lu)\n",
			__func__, vb2_plane_size(vb, 0), buf->length);
		return -EINVAL;
	}
	vb2_set_plane_payload(&buf->vb.vb2_buf, 0, buf->length);
	return 0;
}

static void
buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct au0828_buffer    *buf     = container_of(vbuf,
							struct au0828_buffer,
							vb);
	struct au0828_dev       *dev     = vb2_get_drv_priv(vb->vb2_queue);
	struct au0828_dmaqueue  *vidq    = &dev->vidq;
	unsigned long flags = 0;

	buf->mem = vb2_plane_vaddr(vb, 0);
	buf->length = vb2_plane_size(vb, 0);

	spin_lock_irqsave(&dev->slock, flags);
	list_add_tail(&buf->list, &vidq->active);
	spin_unlock_irqrestore(&dev->slock, flags);
}

>>>>>>> v4.9.227
static int au0828_i2s_init(struct au0828_dev *dev)
{
	/* Enable i2s mode */
	au0828_writereg(dev, AU0828_AUDIOCTRL_50C, 0x01);
	return 0;
}

/*
 * Auvitek au0828 analog stream enable
 */
static int au0828_analog_stream_enable(struct au0828_dev *d)
{
	struct usb_interface *iface;
	int ret, h, w;

	dprintk(1, "au0828_analog_stream_enable called\n");

<<<<<<< HEAD
=======
	if (test_bit(DEV_DISCONNECTED, &d->dev_state))
		return -ENODEV;

>>>>>>> v4.9.227
	iface = usb_ifnum_to_if(d->usbdev, 0);
	if (iface && iface->cur_altsetting->desc.bAlternateSetting != 5) {
		dprintk(1, "Changing intf#0 to alt 5\n");
		/* set au0828 interface0 to AS5 here again */
		ret = usb_set_interface(d->usbdev, 0, 5);
		if (ret < 0) {
			pr_info("Au0828 can't set alt setting to 5!\n");
			return -EBUSY;
		}
	}

	h = d->height / 2 + 2;
	w = d->width * 2;

	au0828_writereg(d, AU0828_SENSORCTRL_VBI_103, 0x00);
	au0828_writereg(d, 0x106, 0x00);
	/* set x position */
	au0828_writereg(d, 0x110, 0x00);
	au0828_writereg(d, 0x111, 0x00);
	au0828_writereg(d, 0x114, w & 0xff);
	au0828_writereg(d, 0x115, w >> 8);
	/* set y position */
	au0828_writereg(d, 0x112, 0x00);
	au0828_writereg(d, 0x113, 0x00);
	au0828_writereg(d, 0x116, h & 0xff);
	au0828_writereg(d, 0x117, h >> 8);
	au0828_writereg(d, AU0828_SENSORCTRL_100, 0xb3);

	return 0;
}

<<<<<<< HEAD
int au0828_analog_stream_disable(struct au0828_dev *d)
=======
static int au0828_analog_stream_disable(struct au0828_dev *d)
>>>>>>> v4.9.227
{
	dprintk(1, "au0828_analog_stream_disable called\n");
	au0828_writereg(d, AU0828_SENSORCTRL_100, 0x0);
	return 0;
}

static void au0828_analog_stream_reset(struct au0828_dev *dev)
{
	dprintk(1, "au0828_analog_stream_reset called\n");
	au0828_writereg(dev, AU0828_SENSORCTRL_100, 0x0);
	mdelay(30);
	au0828_writereg(dev, AU0828_SENSORCTRL_100, 0xb3);
}

/*
 * Some operations needs to stop current streaming
 */
static int au0828_stream_interrupt(struct au0828_dev *dev)
{
	int ret = 0;

	dev->stream_state = STREAM_INTERRUPT;
<<<<<<< HEAD
	if (dev->dev_state == DEV_DISCONNECTED)
		return -ENODEV;
	else if (ret) {
		dev->dev_state = DEV_MISCONFIGURED;
=======
	if (test_bit(DEV_DISCONNECTED, &dev->dev_state))
		return -ENODEV;
	else if (ret) {
		set_bit(DEV_MISCONFIGURED, &dev->dev_state);
>>>>>>> v4.9.227
		dprintk(1, "%s device is misconfigured!\n", __func__);
		return ret;
	}
	return 0;
}

<<<<<<< HEAD
/*
 * au0828_release_resources
 * unregister v4l2 devices
 */
void au0828_analog_unregister(struct au0828_dev *dev)
{
	dprintk(1, "au0828_release_resources called\n");
	mutex_lock(&au0828_sysfs_lock);

	if (dev->vdev)
		video_unregister_device(dev->vdev);
	if (dev->vbi_dev)
		video_unregister_device(dev->vbi_dev);

	mutex_unlock(&au0828_sysfs_lock);
}


/* Usage lock check functions */
static int res_get(struct au0828_fh *fh, unsigned int bit)
{
	struct au0828_dev    *dev = fh->dev;

	if (fh->resources & bit)
		/* have it already allocated */
		return 1;

	/* is it free? */
	if (dev->resources & bit) {
		/* no, someone else uses it */
		return 0;
	}
	/* it's free, grab it */
	fh->resources  |= bit;
	dev->resources |= bit;
	dprintk(1, "res: get %d\n", bit);
=======
int au0828_start_analog_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct au0828_dev *dev = vb2_get_drv_priv(vq);
	int rc = 0;

	dprintk(1, "au0828_start_analog_streaming called %d\n",
		dev->streaming_users);

	if (vq->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		dev->frame_count = 0;
	else
		dev->vbi_frame_count = 0;

	if (dev->streaming_users == 0) {
		/* If we were doing ac97 instead of i2s, it would go here...*/
		au0828_i2s_init(dev);
		rc = au0828_init_isoc(dev, AU0828_ISO_PACKETS_PER_URB,
				   AU0828_MAX_ISO_BUFS, dev->max_pkt_size,
				   au0828_isoc_copy);
		if (rc < 0) {
			pr_info("au0828_init_isoc failed\n");
			return rc;
		}

		v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_stream, 1);

		if (vq->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			dev->vid_timeout_running = 1;
			mod_timer(&dev->vid_timeout, jiffies + (HZ / 10));
		} else if (vq->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
			dev->vbi_timeout_running = 1;
			mod_timer(&dev->vbi_timeout, jiffies + (HZ / 10));
		}
	}
	dev->streaming_users++;
	return rc;
}

static void au0828_stop_streaming(struct vb2_queue *vq)
{
	struct au0828_dev *dev = vb2_get_drv_priv(vq);
	struct au0828_dmaqueue *vidq = &dev->vidq;
	unsigned long flags = 0;

	dprintk(1, "au0828_stop_streaming called %d\n", dev->streaming_users);

	if (dev->streaming_users-- == 1) {
		au0828_uninit_isoc(dev);
		v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_stream, 0);
	}

	dev->vid_timeout_running = 0;
	del_timer_sync(&dev->vid_timeout);

	spin_lock_irqsave(&dev->slock, flags);
	if (dev->isoc_ctl.buf != NULL) {
		vb2_buffer_done(&dev->isoc_ctl.buf->vb.vb2_buf,
				VB2_BUF_STATE_ERROR);
		dev->isoc_ctl.buf = NULL;
	}
	while (!list_empty(&vidq->active)) {
		struct au0828_buffer *buf;

		buf = list_entry(vidq->active.next, struct au0828_buffer, list);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
		list_del(&buf->list);
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}

void au0828_stop_vbi_streaming(struct vb2_queue *vq)
{
	struct au0828_dev *dev = vb2_get_drv_priv(vq);
	struct au0828_dmaqueue *vbiq = &dev->vbiq;
	unsigned long flags = 0;

	dprintk(1, "au0828_stop_vbi_streaming called %d\n",
		dev->streaming_users);

	if (dev->streaming_users-- == 1) {
		au0828_uninit_isoc(dev);
		v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_stream, 0);
	}

	spin_lock_irqsave(&dev->slock, flags);
	if (dev->isoc_ctl.vbi_buf != NULL) {
		vb2_buffer_done(&dev->isoc_ctl.vbi_buf->vb.vb2_buf,
				VB2_BUF_STATE_ERROR);
		dev->isoc_ctl.vbi_buf = NULL;
	}
	while (!list_empty(&vbiq->active)) {
		struct au0828_buffer *buf;

		buf = list_entry(vbiq->active.next, struct au0828_buffer, list);
		list_del(&buf->list);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&dev->slock, flags);

	dev->vbi_timeout_running = 0;
	del_timer_sync(&dev->vbi_timeout);
}

static const struct vb2_ops au0828_video_qops = {
	.queue_setup     = queue_setup,
	.buf_prepare     = buffer_prepare,
	.buf_queue       = buffer_queue,
	.start_streaming = au0828_start_analog_streaming,
	.stop_streaming  = au0828_stop_streaming,
	.wait_prepare    = vb2_ops_wait_prepare,
	.wait_finish     = vb2_ops_wait_finish,
};

/* ------------------------------------------------------------------
   V4L2 interface
   ------------------------------------------------------------------*/
/*
 * au0828_analog_unregister
 * unregister v4l2 devices
 */
int au0828_analog_unregister(struct au0828_dev *dev)
{
	dprintk(1, "au0828_analog_unregister called\n");

	/* No analog TV */
	if (AUVI_INPUT(0).type == AU0828_VMUX_UNDEFINED)
		return 0;

	mutex_lock(&au0828_sysfs_lock);
	video_unregister_device(&dev->vdev);
	video_unregister_device(&dev->vbi_dev);
	mutex_unlock(&au0828_sysfs_lock);

	v4l2_device_disconnect(&dev->v4l2_dev);
	v4l2_device_put(&dev->v4l2_dev);
>>>>>>> v4.9.227

	return 1;
}

<<<<<<< HEAD
static int res_check(struct au0828_fh *fh, unsigned int bit)
{
	return fh->resources & bit;
}

static int res_locked(struct au0828_dev *dev, unsigned int bit)
{
	return dev->resources & bit;
}

static void res_free(struct au0828_fh *fh, unsigned int bits)
{
	struct au0828_dev    *dev = fh->dev;

	BUG_ON((fh->resources & bits) != bits);

	fh->resources  &= ~bits;
	dev->resources &= ~bits;
	dprintk(1, "res: put %d\n", bits);
}

static int get_ressource(struct au0828_fh *fh)
{
	switch (fh->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		return AU0828_RESOURCE_VIDEO;
	case V4L2_BUF_TYPE_VBI_CAPTURE:
		return AU0828_RESOURCE_VBI;
	default:
		BUG();
		return 0;
	}
}

=======
>>>>>>> v4.9.227
/* This function ensures that video frames continue to be delivered even if
   the ITU-656 input isn't receiving any data (thereby preventing applications
   such as tvtime from hanging) */
static void au0828_vid_buffer_timeout(unsigned long data)
{
	struct au0828_dev *dev = (struct au0828_dev *) data;
	struct au0828_dmaqueue *dma_q = &dev->vidq;
	struct au0828_buffer *buf;
	unsigned char *vid_data;
	unsigned long flags = 0;

	spin_lock_irqsave(&dev->slock, flags);

	buf = dev->isoc_ctl.buf;
	if (buf != NULL) {
<<<<<<< HEAD
		vid_data = videobuf_to_vmalloc(&buf->vb);
		memset(vid_data, 0x00, buf->vb.size); /* Blank green frame */
=======
		vid_data = vb2_plane_vaddr(&buf->vb.vb2_buf, 0);
		memset(vid_data, 0x00, buf->length); /* Blank green frame */
>>>>>>> v4.9.227
		buffer_filled(dev, dma_q, buf);
	}
	get_next_buf(dma_q, &buf);

	if (dev->vid_timeout_running == 1)
		mod_timer(&dev->vid_timeout, jiffies + (HZ / 10));

	spin_unlock_irqrestore(&dev->slock, flags);
}

static void au0828_vbi_buffer_timeout(unsigned long data)
{
	struct au0828_dev *dev = (struct au0828_dev *) data;
	struct au0828_dmaqueue *dma_q = &dev->vbiq;
	struct au0828_buffer *buf;
	unsigned char *vbi_data;
	unsigned long flags = 0;

	spin_lock_irqsave(&dev->slock, flags);

	buf = dev->isoc_ctl.vbi_buf;
	if (buf != NULL) {
<<<<<<< HEAD
		vbi_data = videobuf_to_vmalloc(&buf->vb);
		memset(vbi_data, 0x00, buf->vb.size);
		vbi_buffer_filled(dev, dma_q, buf);
=======
		vbi_data = vb2_plane_vaddr(&buf->vb.vb2_buf, 0);
		memset(vbi_data, 0x00, buf->length);
		buffer_filled(dev, dma_q, buf);
>>>>>>> v4.9.227
	}
	vbi_get_next_buf(dma_q, &buf);

	if (dev->vbi_timeout_running == 1)
		mod_timer(&dev->vbi_timeout, jiffies + (HZ / 10));
	spin_unlock_irqrestore(&dev->slock, flags);
}

<<<<<<< HEAD

static int au0828_v4l2_open(struct file *filp)
{
	int ret = 0;
	struct video_device *vdev = video_devdata(filp);
	struct au0828_dev *dev = video_drvdata(filp);
	struct au0828_fh *fh;
	int type;

	switch (vdev->vfl_type) {
	case VFL_TYPE_GRABBER:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		break;
	case VFL_TYPE_VBI:
		type = V4L2_BUF_TYPE_VBI_CAPTURE;
		break;
	default:
		return -EINVAL;
	}

	fh = kzalloc(sizeof(struct au0828_fh), GFP_KERNEL);
	if (NULL == fh) {
		dprintk(1, "Failed allocate au0828_fh struct!\n");
		return -ENOMEM;
	}

	fh->type = type;
	fh->dev = dev;
	v4l2_fh_init(&fh->fh, vdev);
	filp->private_data = fh;

	if (mutex_lock_interruptible(&dev->lock)) {
		kfree(fh);
		return -ERESTARTSYS;
	}
	if (dev->users == 0) {
		au0828_analog_stream_enable(dev);
		au0828_analog_stream_reset(dev);

		/* If we were doing ac97 instead of i2s, it would go here...*/
		au0828_i2s_init(dev);

		dev->stream_state = STREAM_OFF;
		dev->dev_state |= DEV_INITIALIZED;
	}

	dev->users++;
	mutex_unlock(&dev->lock);

	videobuf_queue_vmalloc_init(&fh->vb_vidq, &au0828_video_qops,
				    NULL, &dev->slock,
				    V4L2_BUF_TYPE_VIDEO_CAPTURE,
				    V4L2_FIELD_INTERLACED,
				    sizeof(struct au0828_buffer), fh,
				    &dev->lock);

	/* VBI Setup */
	videobuf_queue_vmalloc_init(&fh->vb_vbiq, &au0828_vbi_qops,
				    NULL, &dev->slock,
				    V4L2_BUF_TYPE_VBI_CAPTURE,
				    V4L2_FIELD_SEQ_TB,
				    sizeof(struct au0828_buffer), fh,
				    &dev->lock);
	v4l2_fh_add(&fh->fh);
=======
static int au0828_v4l2_open(struct file *filp)
{
	struct au0828_dev *dev = video_drvdata(filp);
	int ret;

	dprintk(1,
		"%s called std_set %d dev_state %ld stream users %d users %d\n",
		__func__, dev->std_set_in_tuner_core, dev->dev_state,
		dev->streaming_users, dev->users);

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	ret = v4l2_fh_open(filp);
	if (ret) {
		au0828_isocdbg("%s: v4l2_fh_open() returned error %d\n",
				__func__, ret);
		mutex_unlock(&dev->lock);
		return ret;
	}

	if (dev->users == 0) {
		au0828_analog_stream_enable(dev);
		au0828_analog_stream_reset(dev);
		dev->stream_state = STREAM_OFF;
		set_bit(DEV_INITIALIZED, &dev->dev_state);
	}
	dev->users++;
	mutex_unlock(&dev->lock);
>>>>>>> v4.9.227
	return ret;
}

static int au0828_v4l2_close(struct file *filp)
{
	int ret;
<<<<<<< HEAD
	struct au0828_fh *fh = filp->private_data;
	struct au0828_dev *dev = fh->dev;

	v4l2_fh_del(&fh->fh);
	v4l2_fh_exit(&fh->fh);
	mutex_lock(&dev->lock);
	if (res_check(fh, AU0828_RESOURCE_VIDEO)) {
		/* Cancel timeout thread in case they didn't call streamoff */
		dev->vid_timeout_running = 0;
		del_timer_sync(&dev->vid_timeout);

		videobuf_stop(&fh->vb_vidq);
		res_free(fh, AU0828_RESOURCE_VIDEO);
	}

	if (res_check(fh, AU0828_RESOURCE_VBI)) {
		/* Cancel timeout thread in case they didn't call streamoff */
		dev->vbi_timeout_running = 0;
		del_timer_sync(&dev->vbi_timeout);

		videobuf_stop(&fh->vb_vbiq);
		res_free(fh, AU0828_RESOURCE_VBI);
	}

	if (dev->users == 1 && video_is_registered(video_devdata(filp))) {
		au0828_analog_stream_disable(dev);

		au0828_uninit_isoc(dev);

		/* Save some power by putting tuner to sleep */
		v4l2_device_call_all(&dev->v4l2_dev, 0, core, s_power, 0);
=======
	struct au0828_dev *dev = video_drvdata(filp);
	struct video_device *vdev = video_devdata(filp);

	dprintk(1,
		"%s called std_set %d dev_state %ld stream users %d users %d\n",
		__func__, dev->std_set_in_tuner_core, dev->dev_state,
		dev->streaming_users, dev->users);

	mutex_lock(&dev->lock);
	if (vdev->vfl_type == VFL_TYPE_GRABBER && dev->vid_timeout_running) {
		/* Cancel timeout thread in case they didn't call streamoff */
		dev->vid_timeout_running = 0;
		del_timer_sync(&dev->vid_timeout);
	} else if (vdev->vfl_type == VFL_TYPE_VBI &&
			dev->vbi_timeout_running) {
		/* Cancel timeout thread in case they didn't call streamoff */
		dev->vbi_timeout_running = 0;
		del_timer_sync(&dev->vbi_timeout);
	}

	if (test_bit(DEV_DISCONNECTED, &dev->dev_state))
		goto end;

	if (dev->users == 1) {
		/*
		 * Avoid putting tuner in sleep if DVB or ALSA are
		 * streaming.
		 *
		 * On most USB devices  like au0828 the tuner can
		 * be safely put in sleep stare here if ALSA isn't
		 * streaming. Exceptions are some very old USB tuner
		 * models such as em28xx-based WinTV USB2 which have
		 * a separate audio output jack. The devices that have
		 * a separate audio output jack have analog tuners,
		 * like Philips FM1236. Those devices are always on,
		 * so the s_power callback are silently ignored.
		 * So, the current logic here does the following:
		 * Disable (put tuner to sleep) when
		 * - ALSA and DVB aren't not streaming;
		 * - the last V4L2 file handler is closed.
		 *
		 * FIXME:
		 *
		 * Additionally, this logic could be improved to
		 * disable the media source if the above conditions
		 * are met and if the device:
		 * - doesn't have a separate audio out plug (or
		 * - doesn't use a silicon tuner like xc2028/3028/4000/5000).
		 *
		 * Once this additional logic is in place, a callback
		 * is needed to enable the media source and power on
		 * the tuner, for radio to work.
		*/
		ret = v4l_enable_media_source(vdev);
		if (ret == 0)
			v4l2_device_call_all(&dev->v4l2_dev, 0, core,
					     s_power, 0);
>>>>>>> v4.9.227
		dev->std_set_in_tuner_core = 0;

		/* When close the device, set the usb intf0 into alt0 to free
		   USB bandwidth */
		ret = usb_set_interface(dev->usbdev, 0, 0);
		if (ret < 0)
			pr_info("Au0828 can't set alternate to 0!\n");
	}
<<<<<<< HEAD
	mutex_unlock(&dev->lock);

	videobuf_mmap_free(&fh->vb_vidq);
	videobuf_mmap_free(&fh->vb_vbiq);
	kfree(fh);
	dev->users--;
	wake_up_interruptible_nr(&dev->open, 1);
=======
end:
	_vb2_fop_release(filp, NULL);
	dev->users--;
	mutex_unlock(&dev->lock);
>>>>>>> v4.9.227
	return 0;
}

/* Must be called with dev->lock held */
static void au0828_init_tuner(struct au0828_dev *dev)
{
	struct v4l2_frequency f = {
		.frequency = dev->ctrl_freq,
		.type = V4L2_TUNER_ANALOG_TV,
	};

<<<<<<< HEAD
=======
	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	if (dev->std_set_in_tuner_core)
		return;
	dev->std_set_in_tuner_core = 1;
	i2c_gate_ctrl(dev, 1);
	/* If we've never sent the standard in tuner core, do so now.
	   We don't do this at device probe because we don't want to
	   incur the cost of a firmware load */
	v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_std, dev->std);
	v4l2_device_call_all(&dev->v4l2_dev, 0, tuner, s_frequency, &f);
	i2c_gate_ctrl(dev, 0);
}

<<<<<<< HEAD
static ssize_t au0828_v4l2_read(struct file *filp, char __user *buf,
				size_t count, loff_t *pos)
{
	struct au0828_fh *fh = filp->private_data;
	struct au0828_dev *dev = fh->dev;
	int rc;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	au0828_init_tuner(dev);
	mutex_unlock(&dev->lock);

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		if (res_locked(dev, AU0828_RESOURCE_VIDEO))
			return -EBUSY;

		return videobuf_read_stream(&fh->vb_vidq, buf, count, pos, 0,
					filp->f_flags & O_NONBLOCK);
	}

	if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
		if (!res_get(fh, AU0828_RESOURCE_VBI))
			return -EBUSY;

		if (dev->vbi_timeout_running == 0) {
			/* Handle case where caller tries to read without
			   calling streamon first */
			dev->vbi_timeout_running = 1;
			mod_timer(&dev->vbi_timeout, jiffies + (HZ / 10));
		}

		return videobuf_read_stream(&fh->vb_vbiq, buf, count, pos, 0,
					    filp->f_flags & O_NONBLOCK);
	}

	return 0;
}

static unsigned int au0828_v4l2_poll(struct file *filp, poll_table *wait)
{
	struct au0828_fh *fh = filp->private_data;
	struct au0828_dev *dev = fh->dev;
	unsigned long req_events = poll_requested_events(wait);
	unsigned int res;

	if (check_dev(dev) < 0)
		return POLLERR;

	res = v4l2_ctrl_poll(filp, wait);
	if (!(req_events & (POLLIN | POLLRDNORM)))
		return res;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	au0828_init_tuner(dev);
	mutex_unlock(&dev->lock);

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		if (!res_get(fh, AU0828_RESOURCE_VIDEO))
			return POLLERR;
		return res | videobuf_poll_stream(filp, &fh->vb_vidq, wait);
	}
	if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
		if (!res_get(fh, AU0828_RESOURCE_VBI))
			return POLLERR;
		return res | videobuf_poll_stream(filp, &fh->vb_vbiq, wait);
	}
	return POLLERR;
}

static int au0828_v4l2_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct au0828_fh *fh    = filp->private_data;
	struct au0828_dev *dev   = fh->dev;
	int		 rc;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		rc = videobuf_mmap_mapper(&fh->vb_vidq, vma);
	else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE)
		rc = videobuf_mmap_mapper(&fh->vb_vbiq, vma);

	return rc;
}

=======
>>>>>>> v4.9.227
static int au0828_set_format(struct au0828_dev *dev, unsigned int cmd,
			     struct v4l2_format *format)
{
	int ret;
	int width = format->fmt.pix.width;
	int height = format->fmt.pix.height;

	/* If they are demanding a format other than the one we support,
	   bail out (tvtime asks for UYVY and then retries with YUYV) */
	if (format->fmt.pix.pixelformat != V4L2_PIX_FMT_UYVY)
		return -EINVAL;

	/* format->fmt.pix.width only support 720 and height 480 */
	if (width != 720)
		width = 720;
	if (height != 480)
		height = 480;

	format->fmt.pix.width = width;
	format->fmt.pix.height = height;
	format->fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	format->fmt.pix.bytesperline = width * 2;
	format->fmt.pix.sizeimage = width * height * 2;
	format->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	format->fmt.pix.field = V4L2_FIELD_INTERLACED;
	format->fmt.pix.priv = 0;

	if (cmd == VIDIOC_TRY_FMT)
		return 0;

	/* maybe set new image format, driver current only support 720*480 */
	dev->width = width;
	dev->height = height;
	dev->frame_size = width * height * 2;
	dev->field_size = width * height;
	dev->bytesperline = width * 2;

	if (dev->stream_state == STREAM_ON) {
		dprintk(1, "VIDIOC_SET_FMT: interrupting stream!\n");
		ret = au0828_stream_interrupt(dev);
		if (ret != 0) {
			dprintk(1, "error interrupting video stream!\n");
			return ret;
		}
	}

	au0828_analog_stream_enable(dev);

	return 0;
}

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
static int vidioc_querycap(struct file *file, void  *priv,
			   struct v4l2_capability *cap)
{
	struct video_device *vdev = video_devdata(file);
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	strlcpy(cap->driver, "au0828", sizeof(cap->driver));
	strlcpy(cap->card, dev->board.name, sizeof(cap->card));
	usb_make_path(dev->usbdev, cap->bus_info, sizeof(cap->bus_info));

	/* set the device capabilities */
	cap->device_caps = V4L2_CAP_AUDIO |
		V4L2_CAP_READWRITE |
		V4L2_CAP_STREAMING |
		V4L2_CAP_TUNER;
	if (vdev->vfl_type == VFL_TYPE_GRABBER)
		cap->device_caps |= V4L2_CAP_VIDEO_CAPTURE;
	else
		cap->device_caps |= V4L2_CAP_VBI_CAPTURE;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS |
		V4L2_CAP_VBI_CAPTURE | V4L2_CAP_VIDEO_CAPTURE;
	return 0;
}

static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index)
		return -EINVAL;

<<<<<<< HEAD
=======
	dprintk(1, "%s called\n", __func__);

>>>>>>> v4.9.227
	f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	strcpy(f->description, "Packed YUV2");

	f->flags = 0;
	f->pixelformat = V4L2_PIX_FMT_UYVY;

	return 0;
}

static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
<<<<<<< HEAD
	struct au0828_fh *fh  = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	f->fmt.pix.width = dev->width;
	f->fmt.pix.height = dev->height;
	f->fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	f->fmt.pix.bytesperline = dev->bytesperline;
	f->fmt.pix.sizeimage = dev->frame_size;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M; /* NTSC/PAL */
	f->fmt.pix.field = V4L2_FIELD_INTERLACED;
	f->fmt.pix.priv = 0;
	return 0;
}

static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				  struct v4l2_format *f)
{
<<<<<<< HEAD
	struct au0828_fh *fh  = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	return au0828_set_format(dev, VIDIOC_TRY_FMT, f);
}

static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
<<<<<<< HEAD
	struct au0828_fh *fh  = priv;
	struct au0828_dev *dev = fh->dev;
	int rc;

=======
	struct au0828_dev *dev = video_drvdata(file);
	int rc;

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	rc = check_dev(dev);
	if (rc < 0)
		return rc;

<<<<<<< HEAD
	if (videobuf_queue_is_busy(&fh->vb_vidq)) {
=======
	if (vb2_is_busy(&dev->vb_vidq)) {
>>>>>>> v4.9.227
		pr_info("%s queue busy\n", __func__);
		rc = -EBUSY;
		goto out;
	}

	rc = au0828_set_format(dev, VIDIOC_S_FMT, f);
out:
	return rc;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id norm)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

	if (norm == dev->std)
		return 0;

	if (dev->streaming_users > 0)
		return -EBUSY;
>>>>>>> v4.9.227

	dev->std = norm;

	au0828_init_tuner(dev);

	i2c_gate_ctrl(dev, 1);

	/*
	 * FIXME: when we support something other than 60Hz standards,
	 * we are going to have to make the au0828 bridge adjust the size
	 * of its capture buffer, which is currently hardcoded at 720x480
	 */

	v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_std, norm);

	i2c_gate_ctrl(dev, 0);

	return 0;
}

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *norm)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	*norm = dev->std;
	return 0;
}

static int vidioc_enum_input(struct file *file, void *priv,
				struct v4l2_input *input)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);
>>>>>>> v4.9.227
	unsigned int tmp;

	static const char *inames[] = {
		[AU0828_VMUX_UNDEFINED] = "Undefined",
		[AU0828_VMUX_COMPOSITE] = "Composite",
		[AU0828_VMUX_SVIDEO] = "S-Video",
		[AU0828_VMUX_CABLE] = "Cable TV",
		[AU0828_VMUX_TELEVISION] = "Television",
		[AU0828_VMUX_DVB] = "DVB",
<<<<<<< HEAD
		[AU0828_VMUX_DEBUG] = "tv debug"
	};

=======
	};

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	tmp = input->index;

	if (tmp >= AU0828_MAX_INPUT)
		return -EINVAL;
	if (AUVI_INPUT(tmp).type == 0)
		return -EINVAL;

	input->index = tmp;
	strcpy(input->name, inames[AUVI_INPUT(tmp).type]);
	if ((AUVI_INPUT(tmp).type == AU0828_VMUX_TELEVISION) ||
	    (AUVI_INPUT(tmp).type == AU0828_VMUX_CABLE)) {
		input->type |= V4L2_INPUT_TYPE_TUNER;
		input->audioset = 1;
	} else {
		input->type |= V4L2_INPUT_TYPE_CAMERA;
		input->audioset = 2;
	}

<<<<<<< HEAD
	input->std = dev->vdev->tvnorms;
=======
	input->std = dev->vdev.tvnorms;
>>>>>>> v4.9.227

	return 0;
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	*i = dev->ctrl_input;
	return 0;
}

static void au0828_s_input(struct au0828_dev *dev, int index)
{
	int i;

<<<<<<< HEAD
=======
	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	switch (AUVI_INPUT(index).type) {
	case AU0828_VMUX_SVIDEO:
		dev->input_type = AU0828_VMUX_SVIDEO;
		dev->ctrl_ainput = 1;
		break;
	case AU0828_VMUX_COMPOSITE:
		dev->input_type = AU0828_VMUX_COMPOSITE;
		dev->ctrl_ainput = 1;
		break;
	case AU0828_VMUX_TELEVISION:
		dev->input_type = AU0828_VMUX_TELEVISION;
		dev->ctrl_ainput = 0;
		break;
	default:
		dprintk(1, "unknown input type set [%d]\n",
			AUVI_INPUT(index).type);
<<<<<<< HEAD
		break;
	}

=======
		return;
	}

	dev->ctrl_input = index;

>>>>>>> v4.9.227
	v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_routing,
			AUVI_INPUT(index).vmux, 0, 0);

	for (i = 0; i < AU0828_MAX_INPUT; i++) {
		int enable = 0;
		if (AUVI_INPUT(i).audio_setup == NULL)
			continue;

		if (i == index)
			enable = 1;
		else
			enable = 0;
		if (enable) {
			(AUVI_INPUT(i).audio_setup)(dev, enable);
		} else {
			/* Make sure we leave it turned on if some
			   other input is routed to this callback */
			if ((AUVI_INPUT(i).audio_setup) !=
			    ((AUVI_INPUT(index).audio_setup))) {
				(AUVI_INPUT(i).audio_setup)(dev, enable);
			}
		}
	}

	v4l2_device_call_all(&dev->v4l2_dev, 0, audio, s_routing,
			AUVI_INPUT(index).amux, 0, 0);
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int index)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);
	struct video_device *vfd = video_devdata(file);
>>>>>>> v4.9.227

	dprintk(1, "VIDIOC_S_INPUT in function %s, input=%d\n", __func__,
		index);
	if (index >= AU0828_MAX_INPUT)
		return -EINVAL;
	if (AUVI_INPUT(index).type == 0)
		return -EINVAL;
<<<<<<< HEAD
	dev->ctrl_input = index;
	au0828_s_input(dev, index);
	return 0;
=======

	if (dev->ctrl_input == index)
		return 0;

	au0828_s_input(dev, index);

	/*
	 * Input has been changed. Disable the media source
	 * associated with the old input and enable source
	 * for the newly set input
	 */
	v4l_disable_media_source(vfd);
	return v4l_enable_media_source(vfd);
>>>>>>> v4.9.227
}

static int vidioc_enumaudio(struct file *file, void *priv, struct v4l2_audio *a)
{
	if (a->index > 1)
		return -EINVAL;

<<<<<<< HEAD
=======
	dprintk(1, "%s called\n", __func__);

>>>>>>> v4.9.227
	if (a->index == 0)
		strcpy(a->name, "Television");
	else
		strcpy(a->name, "Line in");

	a->capability = V4L2_AUDCAP_STEREO;
	return 0;
}

static int vidioc_g_audio(struct file *file, void *priv, struct v4l2_audio *a)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	a->index = dev->ctrl_ainput;
	if (a->index == 0)
		strcpy(a->name, "Television");
	else
		strcpy(a->name, "Line in");

	a->capability = V4L2_AUDCAP_STEREO;
	return 0;
}

static int vidioc_s_audio(struct file *file, void *priv, const struct v4l2_audio *a)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;

	if (a->index != dev->ctrl_ainput)
		return -EINVAL;
=======
	struct au0828_dev *dev = video_drvdata(file);

	if (a->index != dev->ctrl_ainput)
		return -EINVAL;

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227
	return 0;
}

static int vidioc_g_tuner(struct file *file, void *priv, struct v4l2_tuner *t)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);
	struct video_device *vfd = video_devdata(file);
	int ret;
>>>>>>> v4.9.227

	if (t->index != 0)
		return -EINVAL;

<<<<<<< HEAD
=======
	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	strcpy(t->name, "Auvitek tuner");

	au0828_init_tuner(dev);
	i2c_gate_ctrl(dev, 1);
	v4l2_device_call_all(&dev->v4l2_dev, 0, tuner, g_tuner, t);
	i2c_gate_ctrl(dev, 0);
	return 0;
}

static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);
>>>>>>> v4.9.227

	if (t->index != 0)
		return -EINVAL;

<<<<<<< HEAD
=======
	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	au0828_init_tuner(dev);
	i2c_gate_ctrl(dev, 1);
	v4l2_device_call_all(&dev->v4l2_dev, 0, tuner, s_tuner, t);
	i2c_gate_ctrl(dev, 0);

	dprintk(1, "VIDIOC_S_TUNER: signal = %x, afc = %x\n", t->signal,
		t->afc);

	return 0;

}

static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *freq)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;

	if (freq->tuner != 0)
		return -EINVAL;
=======
	struct au0828_dev *dev = video_drvdata(file);

	if (freq->tuner != 0)
		return -EINVAL;
	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227
	freq->frequency = dev->ctrl_freq;
	return 0;
}

static int vidioc_s_frequency(struct file *file, void *priv,
				const struct v4l2_frequency *freq)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);
>>>>>>> v4.9.227
	struct v4l2_frequency new_freq = *freq;

	if (freq->tuner != 0)
		return -EINVAL;

<<<<<<< HEAD
=======
	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	au0828_init_tuner(dev);
	i2c_gate_ctrl(dev, 1);

	v4l2_device_call_all(&dev->v4l2_dev, 0, tuner, s_frequency, freq);
	/* Get the actual set (and possibly clamped) frequency */
	v4l2_device_call_all(&dev->v4l2_dev, 0, tuner, g_frequency, &new_freq);
	dev->ctrl_freq = new_freq.frequency;

	i2c_gate_ctrl(dev, 0);

	au0828_analog_stream_reset(dev);

	return 0;
}


/* RAW VBI ioctls */

static int vidioc_g_fmt_vbi_cap(struct file *file, void *priv,
				struct v4l2_format *format)
{
<<<<<<< HEAD
	struct au0828_fh      *fh  = priv;
	struct au0828_dev     *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	format->fmt.vbi.samples_per_line = dev->vbi_width;
	format->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
	format->fmt.vbi.offset = 0;
	format->fmt.vbi.flags = 0;
	format->fmt.vbi.sampling_rate = 6750000 * 4 / 2;

	format->fmt.vbi.count[0] = dev->vbi_height;
	format->fmt.vbi.count[1] = dev->vbi_height;
	format->fmt.vbi.start[0] = 21;
	format->fmt.vbi.start[1] = 284;
	memset(format->fmt.vbi.reserved, 0, sizeof(format->fmt.vbi.reserved));

	return 0;
}

static int vidioc_cropcap(struct file *file, void *priv,
			  struct v4l2_cropcap *cc)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);
>>>>>>> v4.9.227

	if (cc->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

<<<<<<< HEAD
=======
	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);

>>>>>>> v4.9.227
	cc->bounds.left = 0;
	cc->bounds.top = 0;
	cc->bounds.width = dev->width;
	cc->bounds.height = dev->height;

	cc->defrect = cc->bounds;

	cc->pixelaspect.numerator = 54;
	cc->pixelaspect.denominator = 59;

	return 0;
}

<<<<<<< HEAD
static int vidioc_streamon(struct file *file, void *priv,
			   enum v4l2_buf_type type)
{
	struct au0828_fh      *fh  = priv;
	struct au0828_dev     *dev = fh->dev;
	int                   rc = -EINVAL;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (unlikely(type != fh->type))
		return -EINVAL;

	dprintk(1, "vidioc_streamon fh=%p t=%d fh->res=%d dev->res=%d\n",
		fh, type, fh->resources, dev->resources);

	if (unlikely(!res_get(fh, get_ressource(fh))))
		return -EBUSY;

	au0828_init_tuner(dev);
	if (type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		au0828_analog_stream_enable(dev);
		v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_stream, 1);
	}

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		rc = videobuf_streamon(&fh->vb_vidq);
		dev->vid_timeout_running = 1;
		mod_timer(&dev->vid_timeout, jiffies + (HZ / 10));
	} else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
		rc = videobuf_streamon(&fh->vb_vbiq);
		dev->vbi_timeout_running = 1;
		mod_timer(&dev->vbi_timeout, jiffies + (HZ / 10));
	}

	return rc;
}

static int vidioc_streamoff(struct file *file, void *priv,
			    enum v4l2_buf_type type)
{
	struct au0828_fh      *fh  = priv;
	struct au0828_dev     *dev = fh->dev;
	int                   rc;
	int                   i;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (fh->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
	    fh->type != V4L2_BUF_TYPE_VBI_CAPTURE)
		return -EINVAL;
	if (type != fh->type)
		return -EINVAL;

	dprintk(1, "vidioc_streamoff fh=%p t=%d fh->res=%d dev->res=%d\n",
		fh, type, fh->resources, dev->resources);

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		dev->vid_timeout_running = 0;
		del_timer_sync(&dev->vid_timeout);

		au0828_analog_stream_disable(dev);
		v4l2_device_call_all(&dev->v4l2_dev, 0, video, s_stream, 0);
		rc = au0828_stream_interrupt(dev);
		if (rc != 0)
			return rc;

		for (i = 0; i < AU0828_MAX_INPUT; i++) {
			if (AUVI_INPUT(i).audio_setup == NULL)
				continue;
			(AUVI_INPUT(i).audio_setup)(dev, 0);
		}

		if (res_check(fh, AU0828_RESOURCE_VIDEO)) {
			videobuf_streamoff(&fh->vb_vidq);
			res_free(fh, AU0828_RESOURCE_VIDEO);
		}
	} else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
		dev->vbi_timeout_running = 0;
		del_timer_sync(&dev->vbi_timeout);

		if (res_check(fh, AU0828_RESOURCE_VBI)) {
			videobuf_streamoff(&fh->vb_vbiq);
			res_free(fh, AU0828_RESOURCE_VBI);
		}
	}

	return 0;
}

=======
>>>>>>> v4.9.227
#ifdef CONFIG_VIDEO_ADV_DEBUG
static int vidioc_g_register(struct file *file, void *priv,
			     struct v4l2_dbg_register *reg)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	reg->val = au0828_read(dev, reg->reg);
	reg->size = 1;
	return 0;
}

static int vidioc_s_register(struct file *file, void *priv,
			     const struct v4l2_dbg_register *reg)
{
<<<<<<< HEAD
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
=======
	struct au0828_dev *dev = video_drvdata(file);

	dprintk(1, "%s called std_set %d dev_state %ld\n", __func__,
		dev->std_set_in_tuner_core, dev->dev_state);
>>>>>>> v4.9.227

	return au0828_writereg(dev, reg->reg, reg->val);
}
#endif

static int vidioc_log_status(struct file *file, void *fh)
{
	struct video_device *vdev = video_devdata(file);

<<<<<<< HEAD
=======
	dprintk(1, "%s called\n", __func__);

>>>>>>> v4.9.227
	v4l2_ctrl_log_status(file, fh);
	v4l2_device_call_all(vdev->v4l2_dev, 0, core, log_status);
	return 0;
}

<<<<<<< HEAD
static int vidioc_reqbufs(struct file *file, void *priv,
			  struct v4l2_requestbuffers *rb)
{
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
	int rc;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		rc = videobuf_reqbufs(&fh->vb_vidq, rb);
	else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE)
		rc = videobuf_reqbufs(&fh->vb_vbiq, rb);

	return rc;
}

static int vidioc_querybuf(struct file *file, void *priv,
			   struct v4l2_buffer *b)
{
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
	int rc;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		rc = videobuf_querybuf(&fh->vb_vidq, b);
	else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE)
		rc = videobuf_querybuf(&fh->vb_vbiq, b);

	return rc;
}

static int vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *b)
{
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
	int rc;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		rc = videobuf_qbuf(&fh->vb_vidq, b);
	else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE)
		rc = videobuf_qbuf(&fh->vb_vbiq, b);

	return rc;
}

static int vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *b)
{
	struct au0828_fh *fh = priv;
	struct au0828_dev *dev = fh->dev;
	int rc;

	rc = check_dev(dev);
	if (rc < 0)
		return rc;

	/* Workaround for a bug in the au0828 hardware design that sometimes
	   results in the colorspace being inverted */
	if (dev->greenscreen_detected == 1) {
		dprintk(1, "Detected green frame.  Resetting stream...\n");
		au0828_analog_stream_reset(dev);
		dev->greenscreen_detected = 0;
	}

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		rc = videobuf_dqbuf(&fh->vb_vidq, b, file->f_flags & O_NONBLOCK);
	else if (fh->type == V4L2_BUF_TYPE_VBI_CAPTURE)
		rc = videobuf_dqbuf(&fh->vb_vbiq, b, file->f_flags & O_NONBLOCK);

	return rc;
}

=======
>>>>>>> v4.9.227
void au0828_v4l2_suspend(struct au0828_dev *dev)
{
	struct urb *urb;
	int i;

	pr_info("stopping V4L2\n");

	if (dev->stream_state == STREAM_ON) {
		pr_info("stopping V4L2 active URBs\n");
		au0828_analog_stream_disable(dev);
		/* stop urbs */
		for (i = 0; i < dev->isoc_ctl.num_bufs; i++) {
			urb = dev->isoc_ctl.urb[i];
			if (urb) {
				if (!irqs_disabled())
					usb_kill_urb(urb);
				else
					usb_unlink_urb(urb);
			}
		}
	}

	if (dev->vid_timeout_running)
		del_timer_sync(&dev->vid_timeout);
	if (dev->vbi_timeout_running)
		del_timer_sync(&dev->vbi_timeout);
}

void au0828_v4l2_resume(struct au0828_dev *dev)
{
	int i, rc;

	pr_info("restarting V4L2\n");

	if (dev->stream_state == STREAM_ON) {
		au0828_stream_interrupt(dev);
		au0828_init_tuner(dev);
	}

	if (dev->vid_timeout_running)
		mod_timer(&dev->vid_timeout, jiffies + (HZ / 10));
	if (dev->vbi_timeout_running)
		mod_timer(&dev->vbi_timeout, jiffies + (HZ / 10));

	/* If we were doing ac97 instead of i2s, it would go here...*/
	au0828_i2s_init(dev);

	au0828_analog_stream_enable(dev);

	if (!(dev->stream_state == STREAM_ON)) {
		au0828_analog_stream_reset(dev);
		/* submit urbs */
		for (i = 0; i < dev->isoc_ctl.num_bufs; i++) {
			rc = usb_submit_urb(dev->isoc_ctl.urb[i], GFP_ATOMIC);
			if (rc) {
				au0828_isocdbg("submit of urb %i failed (error=%i)\n",
					       i, rc);
				au0828_uninit_isoc(dev);
			}
		}
	}
}

static struct v4l2_file_operations au0828_v4l_fops = {
	.owner      = THIS_MODULE,
	.open       = au0828_v4l2_open,
	.release    = au0828_v4l2_close,
<<<<<<< HEAD
	.read       = au0828_v4l2_read,
	.poll       = au0828_v4l2_poll,
	.mmap       = au0828_v4l2_mmap,
=======
	.read       = vb2_fop_read,
	.poll       = vb2_fop_poll,
	.mmap       = vb2_fop_mmap,
>>>>>>> v4.9.227
	.unlocked_ioctl = video_ioctl2,
};

static const struct v4l2_ioctl_ops video_ioctl_ops = {
	.vidioc_querycap            = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap    = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap       = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap     = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap       = vidioc_s_fmt_vid_cap,
	.vidioc_g_fmt_vbi_cap       = vidioc_g_fmt_vbi_cap,
	.vidioc_try_fmt_vbi_cap     = vidioc_g_fmt_vbi_cap,
	.vidioc_s_fmt_vbi_cap       = vidioc_g_fmt_vbi_cap,
	.vidioc_enumaudio           = vidioc_enumaudio,
	.vidioc_g_audio             = vidioc_g_audio,
	.vidioc_s_audio             = vidioc_s_audio,
	.vidioc_cropcap             = vidioc_cropcap,
<<<<<<< HEAD
	.vidioc_reqbufs             = vidioc_reqbufs,
	.vidioc_querybuf            = vidioc_querybuf,
	.vidioc_qbuf                = vidioc_qbuf,
	.vidioc_dqbuf               = vidioc_dqbuf,
=======

	.vidioc_reqbufs             = vb2_ioctl_reqbufs,
	.vidioc_create_bufs         = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf         = vb2_ioctl_prepare_buf,
	.vidioc_querybuf            = vb2_ioctl_querybuf,
	.vidioc_qbuf                = vb2_ioctl_qbuf,
	.vidioc_dqbuf               = vb2_ioctl_dqbuf,
	.vidioc_expbuf               = vb2_ioctl_expbuf,

>>>>>>> v4.9.227
	.vidioc_s_std               = vidioc_s_std,
	.vidioc_g_std               = vidioc_g_std,
	.vidioc_enum_input          = vidioc_enum_input,
	.vidioc_g_input             = vidioc_g_input,
	.vidioc_s_input             = vidioc_s_input,
<<<<<<< HEAD
	.vidioc_streamon            = vidioc_streamon,
	.vidioc_streamoff           = vidioc_streamoff,
=======

	.vidioc_streamon            = vb2_ioctl_streamon,
	.vidioc_streamoff           = vb2_ioctl_streamoff,

>>>>>>> v4.9.227
	.vidioc_g_tuner             = vidioc_g_tuner,
	.vidioc_s_tuner             = vidioc_s_tuner,
	.vidioc_g_frequency         = vidioc_g_frequency,
	.vidioc_s_frequency         = vidioc_s_frequency,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_register          = vidioc_g_register,
	.vidioc_s_register          = vidioc_s_register,
#endif
	.vidioc_log_status	    = vidioc_log_status,
	.vidioc_subscribe_event     = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event   = v4l2_event_unsubscribe,
};

static const struct video_device au0828_video_template = {
	.fops                       = &au0828_v4l_fops,
<<<<<<< HEAD
	.release                    = video_device_release,
=======
	.release                    = video_device_release_empty,
>>>>>>> v4.9.227
	.ioctl_ops 		    = &video_ioctl_ops,
	.tvnorms                    = V4L2_STD_NTSC_M | V4L2_STD_PAL_M,
};

<<<<<<< HEAD
=======
static int au0828_vb2_setup(struct au0828_dev *dev)
{
	int rc;
	struct vb2_queue *q;

	/* Setup Videobuf2 for Video capture */
	q = &dev->vb_vidq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_READ | VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct au0828_buffer);
	q->ops = &au0828_video_qops;
	q->mem_ops = &vb2_vmalloc_memops;

	rc = vb2_queue_init(q);
	if (rc < 0)
		return rc;

	/* Setup Videobuf2 for VBI capture */
	q = &dev->vb_vbiq;
	q->type = V4L2_BUF_TYPE_VBI_CAPTURE;
	q->io_modes = VB2_READ | VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct au0828_buffer);
	q->ops = &au0828_vbi_qops;
	q->mem_ops = &vb2_vmalloc_memops;

	rc = vb2_queue_init(q);
	if (rc < 0)
		return rc;

	return 0;
}

static void au0828_analog_create_entities(struct au0828_dev *dev)
{
#if defined(CONFIG_MEDIA_CONTROLLER)
	static const char * const inames[] = {
		[AU0828_VMUX_COMPOSITE] = "Composite",
		[AU0828_VMUX_SVIDEO] = "S-Video",
		[AU0828_VMUX_CABLE] = "Cable TV",
		[AU0828_VMUX_TELEVISION] = "Television",
		[AU0828_VMUX_DVB] = "DVB",
	};
	int ret, i;

	/* Initialize Video and VBI pads */
	dev->video_pad.flags = MEDIA_PAD_FL_SINK;
	ret = media_entity_pads_init(&dev->vdev.entity, 1, &dev->video_pad);
	if (ret < 0)
		pr_err("failed to initialize video media entity!\n");

	dev->vbi_pad.flags = MEDIA_PAD_FL_SINK;
	ret = media_entity_pads_init(&dev->vbi_dev.entity, 1, &dev->vbi_pad);
	if (ret < 0)
		pr_err("failed to initialize vbi media entity!\n");

	/* Create entities for each input connector */
	for (i = 0; i < AU0828_MAX_INPUT; i++) {
		struct media_entity *ent = &dev->input_ent[i];

		if (AUVI_INPUT(i).type == AU0828_VMUX_UNDEFINED)
			break;

		ent->name = inames[AUVI_INPUT(i).type];
		ent->flags = MEDIA_ENT_FL_CONNECTOR;
		dev->input_pad[i].flags = MEDIA_PAD_FL_SOURCE;

		switch (AUVI_INPUT(i).type) {
		case AU0828_VMUX_COMPOSITE:
			ent->function = MEDIA_ENT_F_CONN_COMPOSITE;
			break;
		case AU0828_VMUX_SVIDEO:
			ent->function = MEDIA_ENT_F_CONN_SVIDEO;
			break;
		case AU0828_VMUX_CABLE:
		case AU0828_VMUX_TELEVISION:
		case AU0828_VMUX_DVB:
		default: /* Just to shut up a warning */
			ent->function = MEDIA_ENT_F_CONN_RF;
			break;
		}

		ret = media_entity_pads_init(ent, 1, &dev->input_pad[i]);
		if (ret < 0)
			pr_err("failed to initialize input pad[%d]!\n", i);

		ret = media_device_register_entity(dev->media_dev, ent);
		if (ret < 0)
			pr_err("failed to register input entity %d!\n", i);
	}
#endif
}

>>>>>>> v4.9.227
/**************************************************************************/

int au0828_analog_register(struct au0828_dev *dev,
			   struct usb_interface *interface)
{
	int retval = -ENOMEM;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i, ret;

	dprintk(1, "au0828_analog_register called for intf#%d!\n",
		interface->cur_altsetting->desc.bInterfaceNumber);

<<<<<<< HEAD
=======
	/* No analog TV */
	if (AUVI_INPUT(0).type == AU0828_VMUX_UNDEFINED)
		return 0;

>>>>>>> v4.9.227
	/* set au0828 usb interface0 to as5 */
	retval = usb_set_interface(dev->usbdev,
			interface->cur_altsetting->desc.bInterfaceNumber, 5);
	if (retval != 0) {
		pr_info("Failure setting usb interface0 to as5\n");
		return retval;
	}

	/* Figure out which endpoint has the isoc interface */
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
		endpoint = &iface_desc->endpoint[i].desc;
		if (((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK)
		     == USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
		     == USB_ENDPOINT_XFER_ISOC)) {

			/* we find our isoc in endpoint */
			u16 tmp = le16_to_cpu(endpoint->wMaxPacketSize);
			dev->max_pkt_size = (tmp & 0x07ff) *
				(((tmp & 0x1800) >> 11) + 1);
			dev->isoc_in_endpointaddr = endpoint->bEndpointAddress;
			dprintk(1,
				"Found isoc endpoint 0x%02x, max size = %d\n",
				dev->isoc_in_endpointaddr, dev->max_pkt_size);
		}
	}
	if (!(dev->isoc_in_endpointaddr)) {
		pr_info("Could not locate isoc endpoint\n");
<<<<<<< HEAD
		kfree(dev);
=======
>>>>>>> v4.9.227
		return -ENODEV;
	}

	init_waitqueue_head(&dev->open);
	spin_lock_init(&dev->slock);

	/* init video dma queues */
	INIT_LIST_HEAD(&dev->vidq.active);
<<<<<<< HEAD
	INIT_LIST_HEAD(&dev->vidq.queued);
	INIT_LIST_HEAD(&dev->vbiq.active);
	INIT_LIST_HEAD(&dev->vbiq.queued);

	dev->vid_timeout.function = au0828_vid_buffer_timeout;
	dev->vid_timeout.data = (unsigned long) dev;
	init_timer(&dev->vid_timeout);

	dev->vbi_timeout.function = au0828_vbi_buffer_timeout;
	dev->vbi_timeout.data = (unsigned long) dev;
	init_timer(&dev->vbi_timeout);
=======
	INIT_LIST_HEAD(&dev->vbiq.active);

	setup_timer(&dev->vid_timeout, au0828_vid_buffer_timeout,
		    (unsigned long)dev);
	setup_timer(&dev->vbi_timeout, au0828_vbi_buffer_timeout,
		    (unsigned long)dev);
>>>>>>> v4.9.227

	dev->width = NTSC_STD_W;
	dev->height = NTSC_STD_H;
	dev->field_size = dev->width * dev->height;
	dev->frame_size = dev->field_size << 1;
	dev->bytesperline = dev->width << 1;
	dev->vbi_width = 720;
	dev->vbi_height = 1;
	dev->ctrl_ainput = 0;
	dev->ctrl_freq = 960;
	dev->std = V4L2_STD_NTSC_M;
<<<<<<< HEAD
	au0828_s_input(dev, 0);

	/* allocate and fill v4l2 video struct */
	dev->vdev = video_device_alloc();
	if (NULL == dev->vdev) {
		dprintk(1, "Can't allocate video_device.\n");
		return -ENOMEM;
	}

	/* allocate the VBI struct */
	dev->vbi_dev = video_device_alloc();
	if (NULL == dev->vbi_dev) {
		dprintk(1, "Can't allocate vbi_device.\n");
		ret = -ENOMEM;
		goto err_vdev;
	}

	/* Fill the video capture device struct */
	*dev->vdev = au0828_video_template;
	dev->vdev->v4l2_dev = &dev->v4l2_dev;
	dev->vdev->lock = &dev->lock;
	strcpy(dev->vdev->name, "au0828a video");

	/* Setup the VBI device */
	*dev->vbi_dev = au0828_video_template;
	dev->vbi_dev->v4l2_dev = &dev->v4l2_dev;
	dev->vbi_dev->lock = &dev->lock;
	strcpy(dev->vbi_dev->name, "au0828a vbi");

	/* Register the v4l2 device */
	video_set_drvdata(dev->vdev, dev);
	retval = video_register_device(dev->vdev, VFL_TYPE_GRABBER, -1);
=======
	/* Default input is TV Tuner */
	au0828_s_input(dev, 0);

	mutex_init(&dev->vb_queue_lock);
	mutex_init(&dev->vb_vbi_queue_lock);

	/* Fill the video capture device struct */
	dev->vdev = au0828_video_template;
	dev->vdev.v4l2_dev = &dev->v4l2_dev;
	dev->vdev.lock = &dev->lock;
	dev->vdev.queue = &dev->vb_vidq;
	dev->vdev.queue->lock = &dev->vb_queue_lock;
	strcpy(dev->vdev.name, "au0828a video");

	/* Setup the VBI device */
	dev->vbi_dev = au0828_video_template;
	dev->vbi_dev.v4l2_dev = &dev->v4l2_dev;
	dev->vbi_dev.lock = &dev->lock;
	dev->vbi_dev.queue = &dev->vb_vbiq;
	dev->vbi_dev.queue->lock = &dev->vb_vbi_queue_lock;
	strcpy(dev->vbi_dev.name, "au0828a vbi");

	/* Init entities at the Media Controller */
	au0828_analog_create_entities(dev);

	/* initialize videobuf2 stuff */
	retval = au0828_vb2_setup(dev);
	if (retval != 0) {
		dprintk(1, "unable to setup videobuf2 queues (error = %d).\n",
			retval);
		return -ENODEV;
	}

	/* Register the v4l2 device */
	video_set_drvdata(&dev->vdev, dev);
	retval = video_register_device(&dev->vdev, VFL_TYPE_GRABBER, -1);
>>>>>>> v4.9.227
	if (retval != 0) {
		dprintk(1, "unable to register video device (error = %d).\n",
			retval);
		ret = -ENODEV;
<<<<<<< HEAD
		goto err_vbi_dev;
	}

	/* Register the vbi device */
	video_set_drvdata(dev->vbi_dev, dev);
	retval = video_register_device(dev->vbi_dev, VFL_TYPE_VBI, -1);
=======
		goto err_reg_vdev;
	}

	/* Register the vbi device */
	video_set_drvdata(&dev->vbi_dev, dev);
	retval = video_register_device(&dev->vbi_dev, VFL_TYPE_VBI, -1);
>>>>>>> v4.9.227
	if (retval != 0) {
		dprintk(1, "unable to register vbi device (error = %d).\n",
			retval);
		ret = -ENODEV;
<<<<<<< HEAD
		goto err_vbi_dev;
	}

=======
		goto err_reg_vbi_dev;
	}

#ifdef CONFIG_MEDIA_CONTROLLER
	retval = v4l2_mc_create_media_graph(dev->media_dev);
	if (retval) {
		pr_err("%s() au0282_dev_register failed to create graph\n",
			__func__);
		ret = -ENODEV;
		goto err_reg_vbi_dev;
	}
#endif

>>>>>>> v4.9.227
	dprintk(1, "%s completed!\n", __func__);

	return 0;

<<<<<<< HEAD
err_vbi_dev:
	video_device_release(dev->vbi_dev);
err_vdev:
	video_device_release(dev->vdev);
=======
err_reg_vbi_dev:
	video_unregister_device(&dev->vdev);
err_reg_vdev:
	vb2_queue_release(&dev->vb_vidq);
	vb2_queue_release(&dev->vb_vbiq);
>>>>>>> v4.9.227
	return ret;
}

