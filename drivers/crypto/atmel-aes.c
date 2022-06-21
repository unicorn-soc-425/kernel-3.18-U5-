/*
 * Cryptographic API.
 *
 * Support for ATMEL AES HW acceleration.
 *
 * Copyright (c) 2012 Eukréa Electromatique - ATMEL
 * Author: Nicolas Royer <nicolas@eukrea.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Some ideas are from omap-aes.c driver.
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/hw_random.h>
#include <linux/platform_device.h>

#include <linux/device.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#include <linux/crypto.h>
<<<<<<< HEAD
#include <linux/cryptohash.h>
#include <crypto/scatterwalk.h>
#include <crypto/algapi.h>
#include <crypto/aes.h>
#include <crypto/hash.h>
#include <crypto/internal/hash.h>
=======
#include <crypto/scatterwalk.h>
#include <crypto/algapi.h>
#include <crypto/aes.h>
#include <crypto/internal/aead.h>
>>>>>>> v4.9.227
#include <linux/platform_data/crypto-atmel.h>
#include <dt-bindings/dma/at91.h>
#include "atmel-aes-regs.h"

<<<<<<< HEAD
=======
#define ATMEL_AES_PRIORITY	300

#define ATMEL_AES_BUFFER_ORDER	2
#define ATMEL_AES_BUFFER_SIZE	(PAGE_SIZE << ATMEL_AES_BUFFER_ORDER)

>>>>>>> v4.9.227
#define CFB8_BLOCK_SIZE		1
#define CFB16_BLOCK_SIZE	2
#define CFB32_BLOCK_SIZE	4
#define CFB64_BLOCK_SIZE	8

<<<<<<< HEAD
/* AES flags */
#define AES_FLAGS_MODE_MASK	0x03ff
#define AES_FLAGS_ENCRYPT	BIT(0)
#define AES_FLAGS_CBC		BIT(1)
#define AES_FLAGS_CFB		BIT(2)
#define AES_FLAGS_CFB8		BIT(3)
#define AES_FLAGS_CFB16		BIT(4)
#define AES_FLAGS_CFB32		BIT(5)
#define AES_FLAGS_CFB64		BIT(6)
#define AES_FLAGS_CFB128	BIT(7)
#define AES_FLAGS_OFB		BIT(8)
#define AES_FLAGS_CTR		BIT(9)

#define AES_FLAGS_INIT		BIT(16)
#define AES_FLAGS_DMA		BIT(17)
#define AES_FLAGS_BUSY		BIT(18)
#define AES_FLAGS_FAST		BIT(19)

#define ATMEL_AES_QUEUE_LENGTH	50

#define ATMEL_AES_DMA_THRESHOLD		16


struct atmel_aes_caps {
	bool	has_dualbuff;
	bool	has_cfb64;
	u32		max_burst_size;
=======
#define SIZE_IN_WORDS(x)	((x) >> 2)

/* AES flags */
/* Reserve bits [18:16] [14:12] [1:0] for mode (same as for AES_MR) */
#define AES_FLAGS_ENCRYPT	AES_MR_CYPHER_ENC
#define AES_FLAGS_GTAGEN	AES_MR_GTAGEN
#define AES_FLAGS_OPMODE_MASK	(AES_MR_OPMOD_MASK | AES_MR_CFBS_MASK)
#define AES_FLAGS_ECB		AES_MR_OPMOD_ECB
#define AES_FLAGS_CBC		AES_MR_OPMOD_CBC
#define AES_FLAGS_OFB		AES_MR_OPMOD_OFB
#define AES_FLAGS_CFB128	(AES_MR_OPMOD_CFB | AES_MR_CFBS_128b)
#define AES_FLAGS_CFB64		(AES_MR_OPMOD_CFB | AES_MR_CFBS_64b)
#define AES_FLAGS_CFB32		(AES_MR_OPMOD_CFB | AES_MR_CFBS_32b)
#define AES_FLAGS_CFB16		(AES_MR_OPMOD_CFB | AES_MR_CFBS_16b)
#define AES_FLAGS_CFB8		(AES_MR_OPMOD_CFB | AES_MR_CFBS_8b)
#define AES_FLAGS_CTR		AES_MR_OPMOD_CTR
#define AES_FLAGS_GCM		AES_MR_OPMOD_GCM

#define AES_FLAGS_MODE_MASK	(AES_FLAGS_OPMODE_MASK |	\
				 AES_FLAGS_ENCRYPT |		\
				 AES_FLAGS_GTAGEN)

#define AES_FLAGS_INIT		BIT(2)
#define AES_FLAGS_BUSY		BIT(3)
#define AES_FLAGS_DUMP_REG	BIT(4)

#define AES_FLAGS_PERSISTENT	(AES_FLAGS_INIT | AES_FLAGS_BUSY)

#define ATMEL_AES_QUEUE_LENGTH	50

#define ATMEL_AES_DMA_THRESHOLD		256


struct atmel_aes_caps {
	bool			has_dualbuff;
	bool			has_cfb64;
	bool			has_gcm;
	u32			max_burst_size;
>>>>>>> v4.9.227
};

struct atmel_aes_dev;

<<<<<<< HEAD
struct atmel_aes_ctx {
	struct atmel_aes_dev *dd;

	int		keylen;
	u32		key[AES_KEYSIZE_256 / sizeof(u32)];

	u16		block_size;
};

struct atmel_aes_reqctx {
	unsigned long mode;
};

struct atmel_aes_dma {
	struct dma_chan			*chan;
	struct dma_slave_config dma_conf;
=======

typedef int (*atmel_aes_fn_t)(struct atmel_aes_dev *);


struct atmel_aes_base_ctx {
	struct atmel_aes_dev	*dd;
	atmel_aes_fn_t		start;
	int			keylen;
	u32			key[AES_KEYSIZE_256 / sizeof(u32)];
	u16			block_size;
};

struct atmel_aes_ctx {
	struct atmel_aes_base_ctx	base;
};

struct atmel_aes_ctr_ctx {
	struct atmel_aes_base_ctx	base;

	u32			iv[AES_BLOCK_SIZE / sizeof(u32)];
	size_t			offset;
	struct scatterlist	src[2];
	struct scatterlist	dst[2];
};

struct atmel_aes_gcm_ctx {
	struct atmel_aes_base_ctx	base;

	struct scatterlist	src[2];
	struct scatterlist	dst[2];

	u32			j0[AES_BLOCK_SIZE / sizeof(u32)];
	u32			tag[AES_BLOCK_SIZE / sizeof(u32)];
	u32			ghash[AES_BLOCK_SIZE / sizeof(u32)];
	size_t			textlen;

	const u32		*ghash_in;
	u32			*ghash_out;
	atmel_aes_fn_t		ghash_resume;
};

struct atmel_aes_reqctx {
	unsigned long		mode;
};

struct atmel_aes_dma {
	struct dma_chan		*chan;
	struct scatterlist	*sg;
	int			nents;
	unsigned int		remainder;
	unsigned int		sg_len;
>>>>>>> v4.9.227
};

struct atmel_aes_dev {
	struct list_head	list;
	unsigned long		phys_base;
	void __iomem		*io_base;

<<<<<<< HEAD
	struct atmel_aes_ctx	*ctx;
	struct device		*dev;
	struct clk		*iclk;
	int	irq;

	unsigned long		flags;
	int	err;
=======
	struct crypto_async_request	*areq;
	struct atmel_aes_base_ctx	*ctx;

	bool			is_async;
	atmel_aes_fn_t		resume;
	atmel_aes_fn_t		cpu_transfer_complete;

	struct device		*dev;
	struct clk		*iclk;
	int			irq;

	unsigned long		flags;
>>>>>>> v4.9.227

	spinlock_t		lock;
	struct crypto_queue	queue;

	struct tasklet_struct	done_task;
	struct tasklet_struct	queue_task;

<<<<<<< HEAD
	struct ablkcipher_request	*req;
	size_t	total;

	struct scatterlist	*in_sg;
	unsigned int		nb_in_sg;
	size_t				in_offset;
	struct scatterlist	*out_sg;
	unsigned int		nb_out_sg;
	size_t				out_offset;

	size_t	bufcnt;
	size_t	buflen;
	size_t	dma_size;

	void	*buf_in;
	int		dma_in;
	dma_addr_t	dma_addr_in;
	struct atmel_aes_dma	dma_lch_in;

	void	*buf_out;
	int		dma_out;
	dma_addr_t	dma_addr_out;
	struct atmel_aes_dma	dma_lch_out;

	struct atmel_aes_caps	caps;

	u32	hw_version;
=======
	size_t			total;
	size_t			datalen;
	u32			*data;

	struct atmel_aes_dma	src;
	struct atmel_aes_dma	dst;

	size_t			buflen;
	void			*buf;
	struct scatterlist	aligned_sg;
	struct scatterlist	*real_dst;

	struct atmel_aes_caps	caps;

	u32			hw_version;
>>>>>>> v4.9.227
};

struct atmel_aes_drv {
	struct list_head	dev_list;
	spinlock_t		lock;
};

static struct atmel_aes_drv atmel_aes = {
	.dev_list = LIST_HEAD_INIT(atmel_aes.dev_list),
	.lock = __SPIN_LOCK_UNLOCKED(atmel_aes.lock),
};

<<<<<<< HEAD
static int atmel_aes_sg_length(struct ablkcipher_request *req,
			struct scatterlist *sg)
{
	unsigned int total = req->nbytes;
	int sg_nb;
	unsigned int len;
	struct scatterlist *sg_list;

	sg_nb = 0;
	sg_list = sg;
	total = req->nbytes;

	while (total) {
		len = min(sg_list->length, total);

		sg_nb++;
		total -= len;

		sg_list = sg_next(sg_list);
		if (!sg_list)
			total = 0;
	}

	return sg_nb;
}

static int atmel_aes_sg_copy(struct scatterlist **sg, size_t *offset,
			void *buf, size_t buflen, size_t total, int out)
{
	unsigned int count, off = 0;

	while (buflen && total) {
		count = min((*sg)->length - *offset, total);
		count = min(count, buflen);

		if (!count)
			return off;

		scatterwalk_map_and_copy(buf + off, *sg, *offset, count, out);

		off += count;
		buflen -= count;
		*offset += count;
		total -= count;

		if (*offset == (*sg)->length) {
			*sg = sg_next(*sg);
			if (*sg)
				*offset = 0;
			else
				total = 0;
		}
	}

	return off;
}

static inline u32 atmel_aes_read(struct atmel_aes_dev *dd, u32 offset)
{
	return readl_relaxed(dd->io_base + offset);
=======
#ifdef VERBOSE_DEBUG
static const char *atmel_aes_reg_name(u32 offset, char *tmp, size_t sz)
{
	switch (offset) {
	case AES_CR:
		return "CR";

	case AES_MR:
		return "MR";

	case AES_ISR:
		return "ISR";

	case AES_IMR:
		return "IMR";

	case AES_IER:
		return "IER";

	case AES_IDR:
		return "IDR";

	case AES_KEYWR(0):
	case AES_KEYWR(1):
	case AES_KEYWR(2):
	case AES_KEYWR(3):
	case AES_KEYWR(4):
	case AES_KEYWR(5):
	case AES_KEYWR(6):
	case AES_KEYWR(7):
		snprintf(tmp, sz, "KEYWR[%u]", (offset - AES_KEYWR(0)) >> 2);
		break;

	case AES_IDATAR(0):
	case AES_IDATAR(1):
	case AES_IDATAR(2):
	case AES_IDATAR(3):
		snprintf(tmp, sz, "IDATAR[%u]", (offset - AES_IDATAR(0)) >> 2);
		break;

	case AES_ODATAR(0):
	case AES_ODATAR(1):
	case AES_ODATAR(2):
	case AES_ODATAR(3):
		snprintf(tmp, sz, "ODATAR[%u]", (offset - AES_ODATAR(0)) >> 2);
		break;

	case AES_IVR(0):
	case AES_IVR(1):
	case AES_IVR(2):
	case AES_IVR(3):
		snprintf(tmp, sz, "IVR[%u]", (offset - AES_IVR(0)) >> 2);
		break;

	case AES_AADLENR:
		return "AADLENR";

	case AES_CLENR:
		return "CLENR";

	case AES_GHASHR(0):
	case AES_GHASHR(1):
	case AES_GHASHR(2):
	case AES_GHASHR(3):
		snprintf(tmp, sz, "GHASHR[%u]", (offset - AES_GHASHR(0)) >> 2);
		break;

	case AES_TAGR(0):
	case AES_TAGR(1):
	case AES_TAGR(2):
	case AES_TAGR(3):
		snprintf(tmp, sz, "TAGR[%u]", (offset - AES_TAGR(0)) >> 2);
		break;

	case AES_CTRR:
		return "CTRR";

	case AES_GCMHR(0):
	case AES_GCMHR(1):
	case AES_GCMHR(2):
	case AES_GCMHR(3):
		snprintf(tmp, sz, "GCMHR[%u]", (offset - AES_GCMHR(0)) >> 2);
		break;

	default:
		snprintf(tmp, sz, "0x%02x", offset);
		break;
	}

	return tmp;
}
#endif /* VERBOSE_DEBUG */

/* Shared functions */

static inline u32 atmel_aes_read(struct atmel_aes_dev *dd, u32 offset)
{
	u32 value = readl_relaxed(dd->io_base + offset);

#ifdef VERBOSE_DEBUG
	if (dd->flags & AES_FLAGS_DUMP_REG) {
		char tmp[16];

		dev_vdbg(dd->dev, "read 0x%08x from %s\n", value,
			 atmel_aes_reg_name(offset, tmp, sizeof(tmp)));
	}
#endif /* VERBOSE_DEBUG */

	return value;
>>>>>>> v4.9.227
}

static inline void atmel_aes_write(struct atmel_aes_dev *dd,
					u32 offset, u32 value)
{
<<<<<<< HEAD
=======
#ifdef VERBOSE_DEBUG
	if (dd->flags & AES_FLAGS_DUMP_REG) {
		char tmp[16];

		dev_vdbg(dd->dev, "write 0x%08x into %s\n", value,
			 atmel_aes_reg_name(offset, tmp));
	}
#endif /* VERBOSE_DEBUG */

>>>>>>> v4.9.227
	writel_relaxed(value, dd->io_base + offset);
}

static void atmel_aes_read_n(struct atmel_aes_dev *dd, u32 offset,
					u32 *value, int count)
{
	for (; count--; value++, offset += 4)
		*value = atmel_aes_read(dd, offset);
}

static void atmel_aes_write_n(struct atmel_aes_dev *dd, u32 offset,
<<<<<<< HEAD
					u32 *value, int count)
=======
			      const u32 *value, int count)
>>>>>>> v4.9.227
{
	for (; count--; value++, offset += 4)
		atmel_aes_write(dd, offset, *value);
}

<<<<<<< HEAD
static struct atmel_aes_dev *atmel_aes_find_dev(struct atmel_aes_ctx *ctx)
=======
static inline void atmel_aes_read_block(struct atmel_aes_dev *dd, u32 offset,
					u32 *value)
{
	atmel_aes_read_n(dd, offset, value, SIZE_IN_WORDS(AES_BLOCK_SIZE));
}

static inline void atmel_aes_write_block(struct atmel_aes_dev *dd, u32 offset,
					 const u32 *value)
{
	atmel_aes_write_n(dd, offset, value, SIZE_IN_WORDS(AES_BLOCK_SIZE));
}

static inline int atmel_aes_wait_for_data_ready(struct atmel_aes_dev *dd,
						atmel_aes_fn_t resume)
{
	u32 isr = atmel_aes_read(dd, AES_ISR);

	if (unlikely(isr & AES_INT_DATARDY))
		return resume(dd);

	dd->resume = resume;
	atmel_aes_write(dd, AES_IER, AES_INT_DATARDY);
	return -EINPROGRESS;
}

static inline size_t atmel_aes_padlen(size_t len, size_t block_size)
{
	len &= block_size - 1;
	return len ? block_size - len : 0;
}

static struct atmel_aes_dev *atmel_aes_find_dev(struct atmel_aes_base_ctx *ctx)
>>>>>>> v4.9.227
{
	struct atmel_aes_dev *aes_dd = NULL;
	struct atmel_aes_dev *tmp;

	spin_lock_bh(&atmel_aes.lock);
	if (!ctx->dd) {
		list_for_each_entry(tmp, &atmel_aes.dev_list, list) {
			aes_dd = tmp;
			break;
		}
		ctx->dd = aes_dd;
	} else {
		aes_dd = ctx->dd;
	}

	spin_unlock_bh(&atmel_aes.lock);

	return aes_dd;
}

static int atmel_aes_hw_init(struct atmel_aes_dev *dd)
{
<<<<<<< HEAD
	clk_prepare_enable(dd->iclk);
=======
	int err;

	err = clk_enable(dd->iclk);
	if (err)
		return err;
>>>>>>> v4.9.227

	if (!(dd->flags & AES_FLAGS_INIT)) {
		atmel_aes_write(dd, AES_CR, AES_CR_SWRST);
		atmel_aes_write(dd, AES_MR, 0xE << AES_MR_CKEY_OFFSET);
		dd->flags |= AES_FLAGS_INIT;
<<<<<<< HEAD
		dd->err = 0;
=======
>>>>>>> v4.9.227
	}

	return 0;
}

static inline unsigned int atmel_aes_get_version(struct atmel_aes_dev *dd)
{
	return atmel_aes_read(dd, AES_HW_VERSION) & 0x00000fff;
}

<<<<<<< HEAD
static void atmel_aes_hw_version_init(struct atmel_aes_dev *dd)
{
	atmel_aes_hw_init(dd);

	dd->hw_version = atmel_aes_get_version(dd);

	dev_info(dd->dev,
			"version: 0x%x\n", dd->hw_version);

	clk_disable_unprepare(dd->iclk);
}

static void atmel_aes_finish_req(struct atmel_aes_dev *dd, int err)
{
	struct ablkcipher_request *req = dd->req;

	clk_disable_unprepare(dd->iclk);
	dd->flags &= ~AES_FLAGS_BUSY;

	req->base.complete(&req->base, err);
}

static void atmel_aes_dma_callback(void *data)
{
	struct atmel_aes_dev *dd = data;

	/* dma_lch_out - completed */
	tasklet_schedule(&dd->done_task);
}

static int atmel_aes_crypt_dma(struct atmel_aes_dev *dd,
		dma_addr_t dma_addr_in, dma_addr_t dma_addr_out, int length)
{
	struct scatterlist sg[2];
	struct dma_async_tx_descriptor	*in_desc, *out_desc;

	dd->dma_size = length;

	if (!(dd->flags & AES_FLAGS_FAST)) {
		dma_sync_single_for_device(dd->dev, dma_addr_in, length,
					   DMA_TO_DEVICE);
	}

	if (dd->flags & AES_FLAGS_CFB8) {
		dd->dma_lch_in.dma_conf.dst_addr_width =
			DMA_SLAVE_BUSWIDTH_1_BYTE;
		dd->dma_lch_out.dma_conf.src_addr_width =
			DMA_SLAVE_BUSWIDTH_1_BYTE;
	} else if (dd->flags & AES_FLAGS_CFB16) {
		dd->dma_lch_in.dma_conf.dst_addr_width =
			DMA_SLAVE_BUSWIDTH_2_BYTES;
		dd->dma_lch_out.dma_conf.src_addr_width =
			DMA_SLAVE_BUSWIDTH_2_BYTES;
	} else {
		dd->dma_lch_in.dma_conf.dst_addr_width =
			DMA_SLAVE_BUSWIDTH_4_BYTES;
		dd->dma_lch_out.dma_conf.src_addr_width =
			DMA_SLAVE_BUSWIDTH_4_BYTES;
	}

	if (dd->flags & (AES_FLAGS_CFB8 | AES_FLAGS_CFB16 |
			AES_FLAGS_CFB32 | AES_FLAGS_CFB64)) {
		dd->dma_lch_in.dma_conf.src_maxburst = 1;
		dd->dma_lch_in.dma_conf.dst_maxburst = 1;
		dd->dma_lch_out.dma_conf.src_maxburst = 1;
		dd->dma_lch_out.dma_conf.dst_maxburst = 1;
	} else {
		dd->dma_lch_in.dma_conf.src_maxburst = dd->caps.max_burst_size;
		dd->dma_lch_in.dma_conf.dst_maxburst = dd->caps.max_burst_size;
		dd->dma_lch_out.dma_conf.src_maxburst = dd->caps.max_burst_size;
		dd->dma_lch_out.dma_conf.dst_maxburst = dd->caps.max_burst_size;
	}

	dmaengine_slave_config(dd->dma_lch_in.chan, &dd->dma_lch_in.dma_conf);
	dmaengine_slave_config(dd->dma_lch_out.chan, &dd->dma_lch_out.dma_conf);

	dd->flags |= AES_FLAGS_DMA;

	sg_init_table(&sg[0], 1);
	sg_dma_address(&sg[0]) = dma_addr_in;
	sg_dma_len(&sg[0]) = length;

	sg_init_table(&sg[1], 1);
	sg_dma_address(&sg[1]) = dma_addr_out;
	sg_dma_len(&sg[1]) = length;

	in_desc = dmaengine_prep_slave_sg(dd->dma_lch_in.chan, &sg[0],
				1, DMA_MEM_TO_DEV,
				DMA_PREP_INTERRUPT  |  DMA_CTRL_ACK);
	if (!in_desc)
		return -EINVAL;

	out_desc = dmaengine_prep_slave_sg(dd->dma_lch_out.chan, &sg[1],
				1, DMA_DEV_TO_MEM,
				DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!out_desc)
		return -EINVAL;

	out_desc->callback = atmel_aes_dma_callback;
	out_desc->callback_param = dd;

	dmaengine_submit(out_desc);
	dma_async_issue_pending(dd->dma_lch_out.chan);

	dmaengine_submit(in_desc);
	dma_async_issue_pending(dd->dma_lch_in.chan);

	return 0;
}

static int atmel_aes_crypt_cpu_start(struct atmel_aes_dev *dd)
{
	dd->flags &= ~AES_FLAGS_DMA;

	/* use cache buffers */
	dd->nb_in_sg = atmel_aes_sg_length(dd->req, dd->in_sg);
	if (!dd->nb_in_sg)
		return -EINVAL;

	dd->nb_out_sg = atmel_aes_sg_length(dd->req, dd->out_sg);
	if (!dd->nb_out_sg)
		return -EINVAL;

	dd->bufcnt = sg_copy_to_buffer(dd->in_sg, dd->nb_in_sg,
					dd->buf_in, dd->total);

	if (!dd->bufcnt)
		return -EINVAL;

	dd->total -= dd->bufcnt;

	atmel_aes_write(dd, AES_IER, AES_INT_DATARDY);
	atmel_aes_write_n(dd, AES_IDATAR(0), (u32 *) dd->buf_in,
				dd->bufcnt >> 2);

	return 0;
}

static int atmel_aes_crypt_dma_start(struct atmel_aes_dev *dd)
{
	int err, fast = 0, in, out;
	size_t count;
	dma_addr_t addr_in, addr_out;

	if ((!dd->in_offset) && (!dd->out_offset)) {
		/* check for alignment */
		in = IS_ALIGNED((u32)dd->in_sg->offset, sizeof(u32)) &&
			IS_ALIGNED(dd->in_sg->length, dd->ctx->block_size);
		out = IS_ALIGNED((u32)dd->out_sg->offset, sizeof(u32)) &&
			IS_ALIGNED(dd->out_sg->length, dd->ctx->block_size);
		fast = in && out;

		if (sg_dma_len(dd->in_sg) != sg_dma_len(dd->out_sg))
			fast = 0;
	}


	if (fast)  {
		count = min(dd->total, sg_dma_len(dd->in_sg));
		count = min(count, sg_dma_len(dd->out_sg));

		err = dma_map_sg(dd->dev, dd->in_sg, 1, DMA_TO_DEVICE);
		if (!err) {
			dev_err(dd->dev, "dma_map_sg() error\n");
			return -EINVAL;
		}

		err = dma_map_sg(dd->dev, dd->out_sg, 1,
				DMA_FROM_DEVICE);
		if (!err) {
			dev_err(dd->dev, "dma_map_sg() error\n");
			dma_unmap_sg(dd->dev, dd->in_sg, 1,
				DMA_TO_DEVICE);
			return -EINVAL;
		}

		addr_in = sg_dma_address(dd->in_sg);
		addr_out = sg_dma_address(dd->out_sg);

		dd->flags |= AES_FLAGS_FAST;

	} else {
		/* use cache buffers */
		count = atmel_aes_sg_copy(&dd->in_sg, &dd->in_offset,
				dd->buf_in, dd->buflen, dd->total, 0);

		addr_in = dd->dma_addr_in;
		addr_out = dd->dma_addr_out;

		dd->flags &= ~AES_FLAGS_FAST;
	}

	dd->total -= count;

	err = atmel_aes_crypt_dma(dd, addr_in, addr_out, count);

	if (err && (dd->flags & AES_FLAGS_FAST)) {
		dma_unmap_sg(dd->dev, dd->in_sg, 1, DMA_TO_DEVICE);
		dma_unmap_sg(dd->dev, dd->out_sg, 1, DMA_TO_DEVICE);
	}
=======
static int atmel_aes_hw_version_init(struct atmel_aes_dev *dd)
{
	int err;

	err = atmel_aes_hw_init(dd);
	if (err)
		return err;

	dd->hw_version = atmel_aes_get_version(dd);

	dev_info(dd->dev, "version: 0x%x\n", dd->hw_version);

	clk_disable(dd->iclk);
	return 0;
}

static inline void atmel_aes_set_mode(struct atmel_aes_dev *dd,
				      const struct atmel_aes_reqctx *rctx)
{
	/* Clear all but persistent flags and set request flags. */
	dd->flags = (dd->flags & AES_FLAGS_PERSISTENT) | rctx->mode;
}

static inline bool atmel_aes_is_encrypt(const struct atmel_aes_dev *dd)
{
	return (dd->flags & AES_FLAGS_ENCRYPT);
}

static inline int atmel_aes_complete(struct atmel_aes_dev *dd, int err)
{
	clk_disable(dd->iclk);
	dd->flags &= ~AES_FLAGS_BUSY;

	if (dd->is_async)
		dd->areq->complete(dd->areq, err);

	tasklet_schedule(&dd->queue_task);
>>>>>>> v4.9.227

	return err;
}

<<<<<<< HEAD
static int atmel_aes_write_ctrl(struct atmel_aes_dev *dd)
{
	int err;
	u32 valcr = 0, valmr = 0;

	err = atmel_aes_hw_init(dd);

	if (err)
		return err;
=======
static void atmel_aes_write_ctrl(struct atmel_aes_dev *dd, bool use_dma,
				 const u32 *iv)
{
	u32 valmr = 0;
>>>>>>> v4.9.227

	/* MR register must be set before IV registers */
	if (dd->ctx->keylen == AES_KEYSIZE_128)
		valmr |= AES_MR_KEYSIZE_128;
	else if (dd->ctx->keylen == AES_KEYSIZE_192)
		valmr |= AES_MR_KEYSIZE_192;
	else
		valmr |= AES_MR_KEYSIZE_256;

<<<<<<< HEAD
	if (dd->flags & AES_FLAGS_CBC) {
		valmr |= AES_MR_OPMOD_CBC;
	} else if (dd->flags & AES_FLAGS_CFB) {
		valmr |= AES_MR_OPMOD_CFB;
		if (dd->flags & AES_FLAGS_CFB8)
			valmr |= AES_MR_CFBS_8b;
		else if (dd->flags & AES_FLAGS_CFB16)
			valmr |= AES_MR_CFBS_16b;
		else if (dd->flags & AES_FLAGS_CFB32)
			valmr |= AES_MR_CFBS_32b;
		else if (dd->flags & AES_FLAGS_CFB64)
			valmr |= AES_MR_CFBS_64b;
		else if (dd->flags & AES_FLAGS_CFB128)
			valmr |= AES_MR_CFBS_128b;
	} else if (dd->flags & AES_FLAGS_OFB) {
		valmr |= AES_MR_OPMOD_OFB;
	} else if (dd->flags & AES_FLAGS_CTR) {
		valmr |= AES_MR_OPMOD_CTR;
	} else {
		valmr |= AES_MR_OPMOD_ECB;
	}

	if (dd->flags & AES_FLAGS_ENCRYPT)
		valmr |= AES_MR_CYPHER_ENC;

	if (dd->total > ATMEL_AES_DMA_THRESHOLD) {
=======
	valmr |= dd->flags & AES_FLAGS_MODE_MASK;

	if (use_dma) {
>>>>>>> v4.9.227
		valmr |= AES_MR_SMOD_IDATAR0;
		if (dd->caps.has_dualbuff)
			valmr |= AES_MR_DUALBUFF;
	} else {
		valmr |= AES_MR_SMOD_AUTO;
	}

<<<<<<< HEAD
	atmel_aes_write(dd, AES_CR, valcr);
	atmel_aes_write(dd, AES_MR, valmr);

	atmel_aes_write_n(dd, AES_KEYWR(0), dd->ctx->key,
						dd->ctx->keylen >> 2);

	if (((dd->flags & AES_FLAGS_CBC) || (dd->flags & AES_FLAGS_CFB) ||
	   (dd->flags & AES_FLAGS_OFB) || (dd->flags & AES_FLAGS_CTR)) &&
	   dd->req->info) {
		atmel_aes_write_n(dd, AES_IVR(0), dd->req->info, 4);
=======
	atmel_aes_write(dd, AES_MR, valmr);

	atmel_aes_write_n(dd, AES_KEYWR(0), dd->ctx->key,
			  SIZE_IN_WORDS(dd->ctx->keylen));

	if (iv && (valmr & AES_MR_OPMOD_MASK) != AES_MR_OPMOD_ECB)
		atmel_aes_write_block(dd, AES_IVR(0), iv);
}


/* CPU transfer */

static int atmel_aes_cpu_transfer(struct atmel_aes_dev *dd)
{
	int err = 0;
	u32 isr;

	for (;;) {
		atmel_aes_read_block(dd, AES_ODATAR(0), dd->data);
		dd->data += 4;
		dd->datalen -= AES_BLOCK_SIZE;

		if (dd->datalen < AES_BLOCK_SIZE)
			break;

		atmel_aes_write_block(dd, AES_IDATAR(0), dd->data);

		isr = atmel_aes_read(dd, AES_ISR);
		if (!(isr & AES_INT_DATARDY)) {
			dd->resume = atmel_aes_cpu_transfer;
			atmel_aes_write(dd, AES_IER, AES_INT_DATARDY);
			return -EINPROGRESS;
		}
	}

	if (!sg_copy_from_buffer(dd->real_dst, sg_nents(dd->real_dst),
				 dd->buf, dd->total))
		err = -EINVAL;

	if (err)
		return atmel_aes_complete(dd, err);

	return dd->cpu_transfer_complete(dd);
}

static int atmel_aes_cpu_start(struct atmel_aes_dev *dd,
			       struct scatterlist *src,
			       struct scatterlist *dst,
			       size_t len,
			       atmel_aes_fn_t resume)
{
	size_t padlen = atmel_aes_padlen(len, AES_BLOCK_SIZE);

	if (unlikely(len == 0))
		return -EINVAL;

	sg_copy_to_buffer(src, sg_nents(src), dd->buf, len);

	dd->total = len;
	dd->real_dst = dst;
	dd->cpu_transfer_complete = resume;
	dd->datalen = len + padlen;
	dd->data = (u32 *)dd->buf;
	atmel_aes_write_block(dd, AES_IDATAR(0), dd->data);
	return atmel_aes_wait_for_data_ready(dd, atmel_aes_cpu_transfer);
}


/* DMA transfer */

static void atmel_aes_dma_callback(void *data);

static bool atmel_aes_check_aligned(struct atmel_aes_dev *dd,
				    struct scatterlist *sg,
				    size_t len,
				    struct atmel_aes_dma *dma)
{
	int nents;

	if (!IS_ALIGNED(len, dd->ctx->block_size))
		return false;

	for (nents = 0; sg; sg = sg_next(sg), ++nents) {
		if (!IS_ALIGNED(sg->offset, sizeof(u32)))
			return false;

		if (len <= sg->length) {
			if (!IS_ALIGNED(len, dd->ctx->block_size))
				return false;

			dma->nents = nents+1;
			dma->remainder = sg->length - len;
			sg->length = len;
			return true;
		}

		if (!IS_ALIGNED(sg->length, dd->ctx->block_size))
			return false;

		len -= sg->length;
	}

	return false;
}

static inline void atmel_aes_restore_sg(const struct atmel_aes_dma *dma)
{
	struct scatterlist *sg = dma->sg;
	int nents = dma->nents;

	if (!dma->remainder)
		return;

	while (--nents > 0 && sg)
		sg = sg_next(sg);

	if (!sg)
		return;

	sg->length += dma->remainder;
}

static int atmel_aes_map(struct atmel_aes_dev *dd,
			 struct scatterlist *src,
			 struct scatterlist *dst,
			 size_t len)
{
	bool src_aligned, dst_aligned;
	size_t padlen;

	dd->total = len;
	dd->src.sg = src;
	dd->dst.sg = dst;
	dd->real_dst = dst;

	src_aligned = atmel_aes_check_aligned(dd, src, len, &dd->src);
	if (src == dst)
		dst_aligned = src_aligned;
	else
		dst_aligned = atmel_aes_check_aligned(dd, dst, len, &dd->dst);
	if (!src_aligned || !dst_aligned) {
		padlen = atmel_aes_padlen(len, dd->ctx->block_size);

		if (dd->buflen < len + padlen)
			return -ENOMEM;

		if (!src_aligned) {
			sg_copy_to_buffer(src, sg_nents(src), dd->buf, len);
			dd->src.sg = &dd->aligned_sg;
			dd->src.nents = 1;
			dd->src.remainder = 0;
		}

		if (!dst_aligned) {
			dd->dst.sg = &dd->aligned_sg;
			dd->dst.nents = 1;
			dd->dst.remainder = 0;
		}

		sg_init_table(&dd->aligned_sg, 1);
		sg_set_buf(&dd->aligned_sg, dd->buf, len + padlen);
	}

	if (dd->src.sg == dd->dst.sg) {
		dd->src.sg_len = dma_map_sg(dd->dev, dd->src.sg, dd->src.nents,
					    DMA_BIDIRECTIONAL);
		dd->dst.sg_len = dd->src.sg_len;
		if (!dd->src.sg_len)
			return -EFAULT;
	} else {
		dd->src.sg_len = dma_map_sg(dd->dev, dd->src.sg, dd->src.nents,
					    DMA_TO_DEVICE);
		if (!dd->src.sg_len)
			return -EFAULT;

		dd->dst.sg_len = dma_map_sg(dd->dev, dd->dst.sg, dd->dst.nents,
					    DMA_FROM_DEVICE);
		if (!dd->dst.sg_len) {
			dma_unmap_sg(dd->dev, dd->src.sg, dd->src.nents,
				     DMA_TO_DEVICE);
			return -EFAULT;
		}
>>>>>>> v4.9.227
	}

	return 0;
}

<<<<<<< HEAD
static int atmel_aes_handle_queue(struct atmel_aes_dev *dd,
			       struct ablkcipher_request *req)
{
	struct crypto_async_request *async_req, *backlog;
	struct atmel_aes_ctx *ctx;
	struct atmel_aes_reqctx *rctx;
=======
static void atmel_aes_unmap(struct atmel_aes_dev *dd)
{
	if (dd->src.sg == dd->dst.sg) {
		dma_unmap_sg(dd->dev, dd->src.sg, dd->src.nents,
			     DMA_BIDIRECTIONAL);

		if (dd->src.sg != &dd->aligned_sg)
			atmel_aes_restore_sg(&dd->src);
	} else {
		dma_unmap_sg(dd->dev, dd->dst.sg, dd->dst.nents,
			     DMA_FROM_DEVICE);

		if (dd->dst.sg != &dd->aligned_sg)
			atmel_aes_restore_sg(&dd->dst);

		dma_unmap_sg(dd->dev, dd->src.sg, dd->src.nents,
			     DMA_TO_DEVICE);

		if (dd->src.sg != &dd->aligned_sg)
			atmel_aes_restore_sg(&dd->src);
	}

	if (dd->dst.sg == &dd->aligned_sg)
		sg_copy_from_buffer(dd->real_dst, sg_nents(dd->real_dst),
				    dd->buf, dd->total);
}

static int atmel_aes_dma_transfer_start(struct atmel_aes_dev *dd,
					enum dma_slave_buswidth addr_width,
					enum dma_transfer_direction dir,
					u32 maxburst)
{
	struct dma_async_tx_descriptor *desc;
	struct dma_slave_config config;
	dma_async_tx_callback callback;
	struct atmel_aes_dma *dma;
	int err;

	memset(&config, 0, sizeof(config));
	config.direction = dir;
	config.src_addr_width = addr_width;
	config.dst_addr_width = addr_width;
	config.src_maxburst = maxburst;
	config.dst_maxburst = maxburst;

	switch (dir) {
	case DMA_MEM_TO_DEV:
		dma = &dd->src;
		callback = NULL;
		config.dst_addr = dd->phys_base + AES_IDATAR(0);
		break;

	case DMA_DEV_TO_MEM:
		dma = &dd->dst;
		callback = atmel_aes_dma_callback;
		config.src_addr = dd->phys_base + AES_ODATAR(0);
		break;

	default:
		return -EINVAL;
	}

	err = dmaengine_slave_config(dma->chan, &config);
	if (err)
		return err;

	desc = dmaengine_prep_slave_sg(dma->chan, dma->sg, dma->sg_len, dir,
				       DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc)
		return -ENOMEM;

	desc->callback = callback;
	desc->callback_param = dd;
	dmaengine_submit(desc);
	dma_async_issue_pending(dma->chan);

	return 0;
}

static void atmel_aes_dma_transfer_stop(struct atmel_aes_dev *dd,
					enum dma_transfer_direction dir)
{
	struct atmel_aes_dma *dma;

	switch (dir) {
	case DMA_MEM_TO_DEV:
		dma = &dd->src;
		break;

	case DMA_DEV_TO_MEM:
		dma = &dd->dst;
		break;

	default:
		return;
	}

	dmaengine_terminate_all(dma->chan);
}

static int atmel_aes_dma_start(struct atmel_aes_dev *dd,
			       struct scatterlist *src,
			       struct scatterlist *dst,
			       size_t len,
			       atmel_aes_fn_t resume)
{
	enum dma_slave_buswidth addr_width;
	u32 maxburst;
	int err;

	switch (dd->ctx->block_size) {
	case CFB8_BLOCK_SIZE:
		addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
		maxburst = 1;
		break;

	case CFB16_BLOCK_SIZE:
		addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
		maxburst = 1;
		break;

	case CFB32_BLOCK_SIZE:
	case CFB64_BLOCK_SIZE:
		addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		maxburst = 1;
		break;

	case AES_BLOCK_SIZE:
		addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		maxburst = dd->caps.max_burst_size;
		break;

	default:
		err = -EINVAL;
		goto exit;
	}

	err = atmel_aes_map(dd, src, dst, len);
	if (err)
		goto exit;

	dd->resume = resume;

	/* Set output DMA transfer first */
	err = atmel_aes_dma_transfer_start(dd, addr_width, DMA_DEV_TO_MEM,
					   maxburst);
	if (err)
		goto unmap;

	/* Then set input DMA transfer */
	err = atmel_aes_dma_transfer_start(dd, addr_width, DMA_MEM_TO_DEV,
					   maxburst);
	if (err)
		goto output_transfer_stop;

	return -EINPROGRESS;

output_transfer_stop:
	atmel_aes_dma_transfer_stop(dd, DMA_DEV_TO_MEM);
unmap:
	atmel_aes_unmap(dd);
exit:
	return atmel_aes_complete(dd, err);
}

static void atmel_aes_dma_stop(struct atmel_aes_dev *dd)
{
	atmel_aes_dma_transfer_stop(dd, DMA_MEM_TO_DEV);
	atmel_aes_dma_transfer_stop(dd, DMA_DEV_TO_MEM);
	atmel_aes_unmap(dd);
}

static void atmel_aes_dma_callback(void *data)
{
	struct atmel_aes_dev *dd = data;

	atmel_aes_dma_stop(dd);
	dd->is_async = true;
	(void)dd->resume(dd);
}

static int atmel_aes_handle_queue(struct atmel_aes_dev *dd,
				  struct crypto_async_request *new_areq)
{
	struct crypto_async_request *areq, *backlog;
	struct atmel_aes_base_ctx *ctx;
>>>>>>> v4.9.227
	unsigned long flags;
	int err, ret = 0;

	spin_lock_irqsave(&dd->lock, flags);
<<<<<<< HEAD
	if (req)
		ret = ablkcipher_enqueue_request(&dd->queue, req);
=======
	if (new_areq)
		ret = crypto_enqueue_request(&dd->queue, new_areq);
>>>>>>> v4.9.227
	if (dd->flags & AES_FLAGS_BUSY) {
		spin_unlock_irqrestore(&dd->lock, flags);
		return ret;
	}
	backlog = crypto_get_backlog(&dd->queue);
<<<<<<< HEAD
	async_req = crypto_dequeue_request(&dd->queue);
	if (async_req)
		dd->flags |= AES_FLAGS_BUSY;
	spin_unlock_irqrestore(&dd->lock, flags);

	if (!async_req)
=======
	areq = crypto_dequeue_request(&dd->queue);
	if (areq)
		dd->flags |= AES_FLAGS_BUSY;
	spin_unlock_irqrestore(&dd->lock, flags);

	if (!areq)
>>>>>>> v4.9.227
		return ret;

	if (backlog)
		backlog->complete(backlog, -EINPROGRESS);

<<<<<<< HEAD
	req = ablkcipher_request_cast(async_req);

	/* assign new request to device */
	dd->req = req;
	dd->total = req->nbytes;
	dd->in_offset = 0;
	dd->in_sg = req->src;
	dd->out_offset = 0;
	dd->out_sg = req->dst;

	rctx = ablkcipher_request_ctx(req);
	ctx = crypto_ablkcipher_ctx(crypto_ablkcipher_reqtfm(req));
	rctx->mode &= AES_FLAGS_MODE_MASK;
	dd->flags = (dd->flags & ~AES_FLAGS_MODE_MASK) | rctx->mode;
	dd->ctx = ctx;
	ctx->dd = dd;

	err = atmel_aes_write_ctrl(dd);
	if (!err) {
		if (dd->total > ATMEL_AES_DMA_THRESHOLD)
			err = atmel_aes_crypt_dma_start(dd);
		else
			err = atmel_aes_crypt_cpu_start(dd);
	}
	if (err) {
		/* aes_task will not finish it, so do it here */
		atmel_aes_finish_req(dd, err);
		tasklet_schedule(&dd->queue_task);
	}

	return ret;
}

static int atmel_aes_crypt_dma_stop(struct atmel_aes_dev *dd)
{
	int err = -EINVAL;
	size_t count;

	if (dd->flags & AES_FLAGS_DMA) {
		err = 0;
		if  (dd->flags & AES_FLAGS_FAST) {
			dma_unmap_sg(dd->dev, dd->out_sg, 1, DMA_FROM_DEVICE);
			dma_unmap_sg(dd->dev, dd->in_sg, 1, DMA_TO_DEVICE);
		} else {
			dma_sync_single_for_device(dd->dev, dd->dma_addr_out,
				dd->dma_size, DMA_FROM_DEVICE);

			/* copy data */
			count = atmel_aes_sg_copy(&dd->out_sg, &dd->out_offset,
				dd->buf_out, dd->buflen, dd->dma_size, 1);
			if (count != dd->dma_size) {
				err = -EINVAL;
				pr_err("not all data converted: %u\n", count);
			}
		}
	}

	return err;
}


static int atmel_aes_buff_init(struct atmel_aes_dev *dd)
{
	int err = -ENOMEM;

	dd->buf_in = (void *)__get_free_pages(GFP_KERNEL, 0);
	dd->buf_out = (void *)__get_free_pages(GFP_KERNEL, 0);
	dd->buflen = PAGE_SIZE;
	dd->buflen &= ~(AES_BLOCK_SIZE - 1);

	if (!dd->buf_in || !dd->buf_out) {
		dev_err(dd->dev, "unable to alloc pages.\n");
		goto err_alloc;
	}

	/* MAP here */
	dd->dma_addr_in = dma_map_single(dd->dev, dd->buf_in,
					dd->buflen, DMA_TO_DEVICE);
	if (dma_mapping_error(dd->dev, dd->dma_addr_in)) {
		dev_err(dd->dev, "dma %d bytes error\n", dd->buflen);
		err = -EINVAL;
		goto err_map_in;
	}

	dd->dma_addr_out = dma_map_single(dd->dev, dd->buf_out,
					dd->buflen, DMA_FROM_DEVICE);
	if (dma_mapping_error(dd->dev, dd->dma_addr_out)) {
		dev_err(dd->dev, "dma %d bytes error\n", dd->buflen);
		err = -EINVAL;
		goto err_map_out;
	}

	return 0;

err_map_out:
	dma_unmap_single(dd->dev, dd->dma_addr_in, dd->buflen,
		DMA_TO_DEVICE);
err_map_in:
	free_page((unsigned long)dd->buf_out);
	free_page((unsigned long)dd->buf_in);
err_alloc:
	if (err)
		pr_err("error: %d\n", err);
	return err;
}

static void atmel_aes_buff_cleanup(struct atmel_aes_dev *dd)
{
	dma_unmap_single(dd->dev, dd->dma_addr_out, dd->buflen,
			 DMA_FROM_DEVICE);
	dma_unmap_single(dd->dev, dd->dma_addr_in, dd->buflen,
		DMA_TO_DEVICE);
	free_page((unsigned long)dd->buf_out);
	free_page((unsigned long)dd->buf_in);
=======
	ctx = crypto_tfm_ctx(areq->tfm);

	dd->areq = areq;
	dd->ctx = ctx;
	dd->is_async = (areq != new_areq);

	err = ctx->start(dd);
	return (dd->is_async) ? ret : err;
}


/* AES async block ciphers */

static int atmel_aes_transfer_complete(struct atmel_aes_dev *dd)
{
	return atmel_aes_complete(dd, 0);
}

static int atmel_aes_start(struct atmel_aes_dev *dd)
{
	struct ablkcipher_request *req = ablkcipher_request_cast(dd->areq);
	struct atmel_aes_reqctx *rctx = ablkcipher_request_ctx(req);
	bool use_dma = (req->nbytes >= ATMEL_AES_DMA_THRESHOLD ||
			dd->ctx->block_size != AES_BLOCK_SIZE);
	int err;

	atmel_aes_set_mode(dd, rctx);

	err = atmel_aes_hw_init(dd);
	if (err)
		return atmel_aes_complete(dd, err);

	atmel_aes_write_ctrl(dd, use_dma, req->info);
	if (use_dma)
		return atmel_aes_dma_start(dd, req->src, req->dst, req->nbytes,
					   atmel_aes_transfer_complete);

	return atmel_aes_cpu_start(dd, req->src, req->dst, req->nbytes,
				   atmel_aes_transfer_complete);
}

static inline struct atmel_aes_ctr_ctx *
atmel_aes_ctr_ctx_cast(struct atmel_aes_base_ctx *ctx)
{
	return container_of(ctx, struct atmel_aes_ctr_ctx, base);
}

static int atmel_aes_ctr_transfer(struct atmel_aes_dev *dd)
{
	struct atmel_aes_ctr_ctx *ctx = atmel_aes_ctr_ctx_cast(dd->ctx);
	struct ablkcipher_request *req = ablkcipher_request_cast(dd->areq);
	struct scatterlist *src, *dst;
	size_t datalen;
	u32 ctr;
	u16 blocks, start, end;
	bool use_dma, fragmented = false;

	/* Check for transfer completion. */
	ctx->offset += dd->total;
	if (ctx->offset >= req->nbytes)
		return atmel_aes_transfer_complete(dd);

	/* Compute data length. */
	datalen = req->nbytes - ctx->offset;
	blocks = DIV_ROUND_UP(datalen, AES_BLOCK_SIZE);
	ctr = be32_to_cpu(ctx->iv[3]);

	/* Check 16bit counter overflow. */
	start = ctr & 0xffff;
	end = start + blocks - 1;

	if (blocks >> 16 || end < start) {
		ctr |= 0xffff;
		datalen = AES_BLOCK_SIZE * (0x10000 - start);
		fragmented = true;
	}

	use_dma = (datalen >= ATMEL_AES_DMA_THRESHOLD);

	/* Jump to offset. */
	src = scatterwalk_ffwd(ctx->src, req->src, ctx->offset);
	dst = ((req->src == req->dst) ? src :
	       scatterwalk_ffwd(ctx->dst, req->dst, ctx->offset));

	/* Configure hardware. */
	atmel_aes_write_ctrl(dd, use_dma, ctx->iv);
	if (unlikely(fragmented)) {
		/*
		 * Increment the counter manually to cope with the hardware
		 * counter overflow.
		 */
		ctx->iv[3] = cpu_to_be32(ctr);
		crypto_inc((u8 *)ctx->iv, AES_BLOCK_SIZE);
	}

	if (use_dma)
		return atmel_aes_dma_start(dd, src, dst, datalen,
					   atmel_aes_ctr_transfer);

	return atmel_aes_cpu_start(dd, src, dst, datalen,
				   atmel_aes_ctr_transfer);
}

static int atmel_aes_ctr_start(struct atmel_aes_dev *dd)
{
	struct atmel_aes_ctr_ctx *ctx = atmel_aes_ctr_ctx_cast(dd->ctx);
	struct ablkcipher_request *req = ablkcipher_request_cast(dd->areq);
	struct atmel_aes_reqctx *rctx = ablkcipher_request_ctx(req);
	int err;

	atmel_aes_set_mode(dd, rctx);

	err = atmel_aes_hw_init(dd);
	if (err)
		return atmel_aes_complete(dd, err);

	memcpy(ctx->iv, req->info, AES_BLOCK_SIZE);
	ctx->offset = 0;
	dd->total = 0;
	return atmel_aes_ctr_transfer(dd);
>>>>>>> v4.9.227
}

static int atmel_aes_crypt(struct ablkcipher_request *req, unsigned long mode)
{
<<<<<<< HEAD
	struct atmel_aes_ctx *ctx = crypto_ablkcipher_ctx(
			crypto_ablkcipher_reqtfm(req));
	struct atmel_aes_reqctx *rctx = ablkcipher_request_ctx(req);
	struct atmel_aes_dev *dd;

	if (mode & AES_FLAGS_CFB8) {
		if (!IS_ALIGNED(req->nbytes, CFB8_BLOCK_SIZE)) {
			pr_err("request size is not exact amount of CFB8 blocks\n");
			return -EINVAL;
		}
		ctx->block_size = CFB8_BLOCK_SIZE;
	} else if (mode & AES_FLAGS_CFB16) {
		if (!IS_ALIGNED(req->nbytes, CFB16_BLOCK_SIZE)) {
			pr_err("request size is not exact amount of CFB16 blocks\n");
			return -EINVAL;
		}
		ctx->block_size = CFB16_BLOCK_SIZE;
	} else if (mode & AES_FLAGS_CFB32) {
		if (!IS_ALIGNED(req->nbytes, CFB32_BLOCK_SIZE)) {
			pr_err("request size is not exact amount of CFB32 blocks\n");
			return -EINVAL;
		}
		ctx->block_size = CFB32_BLOCK_SIZE;
	} else if (mode & AES_FLAGS_CFB64) {
		if (!IS_ALIGNED(req->nbytes, CFB64_BLOCK_SIZE)) {
			pr_err("request size is not exact amount of CFB64 blocks\n");
			return -EINVAL;
		}
		ctx->block_size = CFB64_BLOCK_SIZE;
	} else {
		if (!IS_ALIGNED(req->nbytes, AES_BLOCK_SIZE)) {
			pr_err("request size is not exact amount of AES blocks\n");
			return -EINVAL;
		}
		ctx->block_size = AES_BLOCK_SIZE;
=======
	struct atmel_aes_base_ctx *ctx;
	struct atmel_aes_reqctx *rctx;
	struct atmel_aes_dev *dd;

	ctx = crypto_ablkcipher_ctx(crypto_ablkcipher_reqtfm(req));
	switch (mode & AES_FLAGS_OPMODE_MASK) {
	case AES_FLAGS_CFB8:
		ctx->block_size = CFB8_BLOCK_SIZE;
		break;

	case AES_FLAGS_CFB16:
		ctx->block_size = CFB16_BLOCK_SIZE;
		break;

	case AES_FLAGS_CFB32:
		ctx->block_size = CFB32_BLOCK_SIZE;
		break;

	case AES_FLAGS_CFB64:
		ctx->block_size = CFB64_BLOCK_SIZE;
		break;

	default:
		ctx->block_size = AES_BLOCK_SIZE;
		break;
>>>>>>> v4.9.227
	}

	dd = atmel_aes_find_dev(ctx);
	if (!dd)
		return -ENODEV;

<<<<<<< HEAD
	rctx->mode = mode;

	return atmel_aes_handle_queue(dd, req);
}

static bool atmel_aes_filter(struct dma_chan *chan, void *slave)
{
	struct at_dma_slave	*sl = slave;

	if (sl && sl->dma_dev == chan->device->dev) {
		chan->private = sl;
		return true;
	} else {
		return false;
	}
}

static int atmel_aes_dma_init(struct atmel_aes_dev *dd,
	struct crypto_platform_data *pdata)
{
	int err = -ENOMEM;
	dma_cap_mask_t mask;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	/* Try to grab 2 DMA channels */
	dd->dma_lch_in.chan = dma_request_slave_channel_compat(mask,
			atmel_aes_filter, &pdata->dma_slave->rxdata, dd->dev, "tx");
	if (!dd->dma_lch_in.chan)
		goto err_dma_in;

	dd->dma_lch_in.dma_conf.direction = DMA_MEM_TO_DEV;
	dd->dma_lch_in.dma_conf.dst_addr = dd->phys_base +
		AES_IDATAR(0);
	dd->dma_lch_in.dma_conf.src_maxburst = dd->caps.max_burst_size;
	dd->dma_lch_in.dma_conf.src_addr_width =
		DMA_SLAVE_BUSWIDTH_4_BYTES;
	dd->dma_lch_in.dma_conf.dst_maxburst = dd->caps.max_burst_size;
	dd->dma_lch_in.dma_conf.dst_addr_width =
		DMA_SLAVE_BUSWIDTH_4_BYTES;
	dd->dma_lch_in.dma_conf.device_fc = false;

	dd->dma_lch_out.chan = dma_request_slave_channel_compat(mask,
			atmel_aes_filter, &pdata->dma_slave->txdata, dd->dev, "rx");
	if (!dd->dma_lch_out.chan)
		goto err_dma_out;

	dd->dma_lch_out.dma_conf.direction = DMA_DEV_TO_MEM;
	dd->dma_lch_out.dma_conf.src_addr = dd->phys_base +
		AES_ODATAR(0);
	dd->dma_lch_out.dma_conf.src_maxburst = dd->caps.max_burst_size;
	dd->dma_lch_out.dma_conf.src_addr_width =
		DMA_SLAVE_BUSWIDTH_4_BYTES;
	dd->dma_lch_out.dma_conf.dst_maxburst = dd->caps.max_burst_size;
	dd->dma_lch_out.dma_conf.dst_addr_width =
		DMA_SLAVE_BUSWIDTH_4_BYTES;
	dd->dma_lch_out.dma_conf.device_fc = false;

	return 0;

err_dma_out:
	dma_release_channel(dd->dma_lch_in.chan);
err_dma_in:
	dev_warn(dd->dev, "no DMA channel available\n");
	return err;
}

static void atmel_aes_dma_cleanup(struct atmel_aes_dev *dd)
{
	dma_release_channel(dd->dma_lch_in.chan);
	dma_release_channel(dd->dma_lch_out.chan);
=======
	rctx = ablkcipher_request_ctx(req);
	rctx->mode = mode;

	return atmel_aes_handle_queue(dd, &req->base);
>>>>>>> v4.9.227
}

static int atmel_aes_setkey(struct crypto_ablkcipher *tfm, const u8 *key,
			   unsigned int keylen)
{
<<<<<<< HEAD
	struct atmel_aes_ctx *ctx = crypto_ablkcipher_ctx(tfm);

	if (keylen != AES_KEYSIZE_128 && keylen != AES_KEYSIZE_192 &&
		   keylen != AES_KEYSIZE_256) {
=======
	struct atmel_aes_base_ctx *ctx = crypto_ablkcipher_ctx(tfm);

	if (keylen != AES_KEYSIZE_128 &&
	    keylen != AES_KEYSIZE_192 &&
	    keylen != AES_KEYSIZE_256) {
>>>>>>> v4.9.227
		crypto_ablkcipher_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}

	memcpy(ctx->key, key, keylen);
	ctx->keylen = keylen;

	return 0;
}

static int atmel_aes_ecb_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT);
=======
	return atmel_aes_crypt(req, AES_FLAGS_ECB | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_ecb_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		0);
=======
	return atmel_aes_crypt(req, AES_FLAGS_ECB);
>>>>>>> v4.9.227
}

static int atmel_aes_cbc_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_CBC);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CBC | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_cbc_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CBC);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CBC);
>>>>>>> v4.9.227
}

static int atmel_aes_ofb_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_OFB);
=======
	return atmel_aes_crypt(req, AES_FLAGS_OFB | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_ofb_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_OFB);
=======
	return atmel_aes_crypt(req, AES_FLAGS_OFB);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_CFB | AES_FLAGS_CFB128);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB128 | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CFB | AES_FLAGS_CFB128);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB128);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb64_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_CFB | AES_FLAGS_CFB64);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB64 | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb64_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CFB | AES_FLAGS_CFB64);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB64);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb32_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_CFB | AES_FLAGS_CFB32);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB32 | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb32_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CFB | AES_FLAGS_CFB32);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB32);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb16_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_CFB | AES_FLAGS_CFB16);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB16 | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb16_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CFB | AES_FLAGS_CFB16);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB16);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb8_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT |	AES_FLAGS_CFB | AES_FLAGS_CFB8);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB8 | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_cfb8_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CFB | AES_FLAGS_CFB8);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CFB8);
>>>>>>> v4.9.227
}

static int atmel_aes_ctr_encrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_ENCRYPT | AES_FLAGS_CTR);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CTR | AES_FLAGS_ENCRYPT);
>>>>>>> v4.9.227
}

static int atmel_aes_ctr_decrypt(struct ablkcipher_request *req)
{
<<<<<<< HEAD
	return atmel_aes_crypt(req,
		AES_FLAGS_CTR);
=======
	return atmel_aes_crypt(req, AES_FLAGS_CTR);
>>>>>>> v4.9.227
}

static int atmel_aes_cra_init(struct crypto_tfm *tfm)
{
<<<<<<< HEAD
	tfm->crt_ablkcipher.reqsize = sizeof(struct atmel_aes_reqctx);
=======
	struct atmel_aes_ctx *ctx = crypto_tfm_ctx(tfm);

	tfm->crt_ablkcipher.reqsize = sizeof(struct atmel_aes_reqctx);
	ctx->base.start = atmel_aes_start;

	return 0;
}

static int atmel_aes_ctr_cra_init(struct crypto_tfm *tfm)
{
	struct atmel_aes_ctx *ctx = crypto_tfm_ctx(tfm);

	tfm->crt_ablkcipher.reqsize = sizeof(struct atmel_aes_reqctx);
	ctx->base.start = atmel_aes_ctr_start;
>>>>>>> v4.9.227

	return 0;
}

static void atmel_aes_cra_exit(struct crypto_tfm *tfm)
{
}

static struct crypto_alg aes_algs[] = {
{
	.cra_name		= "ecb(aes)",
	.cra_driver_name	= "atmel-ecb-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= AES_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0xf,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_ecb_encrypt,
		.decrypt	= atmel_aes_ecb_decrypt,
	}
},
{
	.cra_name		= "cbc(aes)",
	.cra_driver_name	= "atmel-cbc-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= AES_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0xf,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_cbc_encrypt,
		.decrypt	= atmel_aes_cbc_decrypt,
	}
},
{
	.cra_name		= "ofb(aes)",
	.cra_driver_name	= "atmel-ofb-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= AES_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0xf,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_ofb_encrypt,
		.decrypt	= atmel_aes_ofb_decrypt,
	}
},
{
	.cra_name		= "cfb(aes)",
	.cra_driver_name	= "atmel-cfb-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= AES_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0xf,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_cfb_encrypt,
		.decrypt	= atmel_aes_cfb_decrypt,
	}
},
{
	.cra_name		= "cfb32(aes)",
	.cra_driver_name	= "atmel-cfb32-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= CFB32_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0x3,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_cfb32_encrypt,
		.decrypt	= atmel_aes_cfb32_decrypt,
	}
},
{
	.cra_name		= "cfb16(aes)",
	.cra_driver_name	= "atmel-cfb16-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= CFB16_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0x1,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_cfb16_encrypt,
		.decrypt	= atmel_aes_cfb16_decrypt,
	}
},
{
	.cra_name		= "cfb8(aes)",
	.cra_driver_name	= "atmel-cfb8-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= CFB8_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0x0,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_cfb8_encrypt,
		.decrypt	= atmel_aes_cfb8_decrypt,
	}
},
{
	.cra_name		= "ctr(aes)",
	.cra_driver_name	= "atmel-ctr-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= AES_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0xf,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= 1,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctr_ctx),
	.cra_alignmask		= 0xf,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_ctr_cra_init,
>>>>>>> v4.9.227
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_ctr_encrypt,
		.decrypt	= atmel_aes_ctr_decrypt,
	}
},
};

static struct crypto_alg aes_cfb64_alg = {
	.cra_name		= "cfb64(aes)",
	.cra_driver_name	= "atmel-cfb64-aes",
<<<<<<< HEAD
	.cra_priority		= 100,
=======
	.cra_priority		= ATMEL_AES_PRIORITY,
>>>>>>> v4.9.227
	.cra_flags		= CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC,
	.cra_blocksize		= CFB64_BLOCK_SIZE,
	.cra_ctxsize		= sizeof(struct atmel_aes_ctx),
	.cra_alignmask		= 0x7,
	.cra_type		= &crypto_ablkcipher_type,
	.cra_module		= THIS_MODULE,
	.cra_init		= atmel_aes_cra_init,
	.cra_exit		= atmel_aes_cra_exit,
	.cra_u.ablkcipher = {
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.setkey		= atmel_aes_setkey,
		.encrypt	= atmel_aes_cfb64_encrypt,
		.decrypt	= atmel_aes_cfb64_decrypt,
	}
};

<<<<<<< HEAD
=======

/* gcm aead functions */

static int atmel_aes_gcm_ghash(struct atmel_aes_dev *dd,
			       const u32 *data, size_t datalen,
			       const u32 *ghash_in, u32 *ghash_out,
			       atmel_aes_fn_t resume);
static int atmel_aes_gcm_ghash_init(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_ghash_finalize(struct atmel_aes_dev *dd);

static int atmel_aes_gcm_start(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_process(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_length(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_data(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_tag_init(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_tag(struct atmel_aes_dev *dd);
static int atmel_aes_gcm_finalize(struct atmel_aes_dev *dd);

static inline struct atmel_aes_gcm_ctx *
atmel_aes_gcm_ctx_cast(struct atmel_aes_base_ctx *ctx)
{
	return container_of(ctx, struct atmel_aes_gcm_ctx, base);
}

static int atmel_aes_gcm_ghash(struct atmel_aes_dev *dd,
			       const u32 *data, size_t datalen,
			       const u32 *ghash_in, u32 *ghash_out,
			       atmel_aes_fn_t resume)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);

	dd->data = (u32 *)data;
	dd->datalen = datalen;
	ctx->ghash_in = ghash_in;
	ctx->ghash_out = ghash_out;
	ctx->ghash_resume = resume;

	atmel_aes_write_ctrl(dd, false, NULL);
	return atmel_aes_wait_for_data_ready(dd, atmel_aes_gcm_ghash_init);
}

static int atmel_aes_gcm_ghash_init(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);

	/* Set the data length. */
	atmel_aes_write(dd, AES_AADLENR, dd->total);
	atmel_aes_write(dd, AES_CLENR, 0);

	/* If needed, overwrite the GCM Intermediate Hash Word Registers */
	if (ctx->ghash_in)
		atmel_aes_write_block(dd, AES_GHASHR(0), ctx->ghash_in);

	return atmel_aes_gcm_ghash_finalize(dd);
}

static int atmel_aes_gcm_ghash_finalize(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	u32 isr;

	/* Write data into the Input Data Registers. */
	while (dd->datalen > 0) {
		atmel_aes_write_block(dd, AES_IDATAR(0), dd->data);
		dd->data += 4;
		dd->datalen -= AES_BLOCK_SIZE;

		isr = atmel_aes_read(dd, AES_ISR);
		if (!(isr & AES_INT_DATARDY)) {
			dd->resume = atmel_aes_gcm_ghash_finalize;
			atmel_aes_write(dd, AES_IER, AES_INT_DATARDY);
			return -EINPROGRESS;
		}
	}

	/* Read the computed hash from GHASHRx. */
	atmel_aes_read_block(dd, AES_GHASHR(0), ctx->ghash_out);

	return ctx->ghash_resume(dd);
}


static int atmel_aes_gcm_start(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	struct aead_request *req = aead_request_cast(dd->areq);
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct atmel_aes_reqctx *rctx = aead_request_ctx(req);
	size_t ivsize = crypto_aead_ivsize(tfm);
	size_t datalen, padlen;
	const void *iv = req->iv;
	u8 *data = dd->buf;
	int err;

	atmel_aes_set_mode(dd, rctx);

	err = atmel_aes_hw_init(dd);
	if (err)
		return atmel_aes_complete(dd, err);

	if (likely(ivsize == 12)) {
		memcpy(ctx->j0, iv, ivsize);
		ctx->j0[3] = cpu_to_be32(1);
		return atmel_aes_gcm_process(dd);
	}

	padlen = atmel_aes_padlen(ivsize, AES_BLOCK_SIZE);
	datalen = ivsize + padlen + AES_BLOCK_SIZE;
	if (datalen > dd->buflen)
		return atmel_aes_complete(dd, -EINVAL);

	memcpy(data, iv, ivsize);
	memset(data + ivsize, 0, padlen + sizeof(u64));
	((u64 *)(data + datalen))[-1] = cpu_to_be64(ivsize * 8);

	return atmel_aes_gcm_ghash(dd, (const u32 *)data, datalen,
				   NULL, ctx->j0, atmel_aes_gcm_process);
}

static int atmel_aes_gcm_process(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	struct aead_request *req = aead_request_cast(dd->areq);
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	bool enc = atmel_aes_is_encrypt(dd);
	u32 authsize;

	/* Compute text length. */
	authsize = crypto_aead_authsize(tfm);
	ctx->textlen = req->cryptlen - (enc ? 0 : authsize);

	/*
	 * According to tcrypt test suite, the GCM Automatic Tag Generation
	 * fails when both the message and its associated data are empty.
	 */
	if (likely(req->assoclen != 0 || ctx->textlen != 0))
		dd->flags |= AES_FLAGS_GTAGEN;

	atmel_aes_write_ctrl(dd, false, NULL);
	return atmel_aes_wait_for_data_ready(dd, atmel_aes_gcm_length);
}

static int atmel_aes_gcm_length(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	struct aead_request *req = aead_request_cast(dd->areq);
	u32 j0_lsw, *j0 = ctx->j0;
	size_t padlen;

	/* Write incr32(J0) into IV. */
	j0_lsw = j0[3];
	j0[3] = cpu_to_be32(be32_to_cpu(j0[3]) + 1);
	atmel_aes_write_block(dd, AES_IVR(0), j0);
	j0[3] = j0_lsw;

	/* Set aad and text lengths. */
	atmel_aes_write(dd, AES_AADLENR, req->assoclen);
	atmel_aes_write(dd, AES_CLENR, ctx->textlen);

	/* Check whether AAD are present. */
	if (unlikely(req->assoclen == 0)) {
		dd->datalen = 0;
		return atmel_aes_gcm_data(dd);
	}

	/* Copy assoc data and add padding. */
	padlen = atmel_aes_padlen(req->assoclen, AES_BLOCK_SIZE);
	if (unlikely(req->assoclen + padlen > dd->buflen))
		return atmel_aes_complete(dd, -EINVAL);
	sg_copy_to_buffer(req->src, sg_nents(req->src), dd->buf, req->assoclen);

	/* Write assoc data into the Input Data register. */
	dd->data = (u32 *)dd->buf;
	dd->datalen = req->assoclen + padlen;
	return atmel_aes_gcm_data(dd);
}

static int atmel_aes_gcm_data(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	struct aead_request *req = aead_request_cast(dd->areq);
	bool use_dma = (ctx->textlen >= ATMEL_AES_DMA_THRESHOLD);
	struct scatterlist *src, *dst;
	u32 isr, mr;

	/* Write AAD first. */
	while (dd->datalen > 0) {
		atmel_aes_write_block(dd, AES_IDATAR(0), dd->data);
		dd->data += 4;
		dd->datalen -= AES_BLOCK_SIZE;

		isr = atmel_aes_read(dd, AES_ISR);
		if (!(isr & AES_INT_DATARDY)) {
			dd->resume = atmel_aes_gcm_data;
			atmel_aes_write(dd, AES_IER, AES_INT_DATARDY);
			return -EINPROGRESS;
		}
	}

	/* GMAC only. */
	if (unlikely(ctx->textlen == 0))
		return atmel_aes_gcm_tag_init(dd);

	/* Prepare src and dst scatter lists to transfer cipher/plain texts */
	src = scatterwalk_ffwd(ctx->src, req->src, req->assoclen);
	dst = ((req->src == req->dst) ? src :
	       scatterwalk_ffwd(ctx->dst, req->dst, req->assoclen));

	if (use_dma) {
		/* Update the Mode Register for DMA transfers. */
		mr = atmel_aes_read(dd, AES_MR);
		mr &= ~(AES_MR_SMOD_MASK | AES_MR_DUALBUFF);
		mr |= AES_MR_SMOD_IDATAR0;
		if (dd->caps.has_dualbuff)
			mr |= AES_MR_DUALBUFF;
		atmel_aes_write(dd, AES_MR, mr);

		return atmel_aes_dma_start(dd, src, dst, ctx->textlen,
					   atmel_aes_gcm_tag_init);
	}

	return atmel_aes_cpu_start(dd, src, dst, ctx->textlen,
				   atmel_aes_gcm_tag_init);
}

static int atmel_aes_gcm_tag_init(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	struct aead_request *req = aead_request_cast(dd->areq);
	u64 *data = dd->buf;

	if (likely(dd->flags & AES_FLAGS_GTAGEN)) {
		if (!(atmel_aes_read(dd, AES_ISR) & AES_INT_TAGRDY)) {
			dd->resume = atmel_aes_gcm_tag_init;
			atmel_aes_write(dd, AES_IER, AES_INT_TAGRDY);
			return -EINPROGRESS;
		}

		return atmel_aes_gcm_finalize(dd);
	}

	/* Read the GCM Intermediate Hash Word Registers. */
	atmel_aes_read_block(dd, AES_GHASHR(0), ctx->ghash);

	data[0] = cpu_to_be64(req->assoclen * 8);
	data[1] = cpu_to_be64(ctx->textlen * 8);

	return atmel_aes_gcm_ghash(dd, (const u32 *)data, AES_BLOCK_SIZE,
				   ctx->ghash, ctx->ghash, atmel_aes_gcm_tag);
}

static int atmel_aes_gcm_tag(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	unsigned long flags;

	/*
	 * Change mode to CTR to complete the tag generation.
	 * Use J0 as Initialization Vector.
	 */
	flags = dd->flags;
	dd->flags &= ~(AES_FLAGS_OPMODE_MASK | AES_FLAGS_GTAGEN);
	dd->flags |= AES_FLAGS_CTR;
	atmel_aes_write_ctrl(dd, false, ctx->j0);
	dd->flags = flags;

	atmel_aes_write_block(dd, AES_IDATAR(0), ctx->ghash);
	return atmel_aes_wait_for_data_ready(dd, atmel_aes_gcm_finalize);
}

static int atmel_aes_gcm_finalize(struct atmel_aes_dev *dd)
{
	struct atmel_aes_gcm_ctx *ctx = atmel_aes_gcm_ctx_cast(dd->ctx);
	struct aead_request *req = aead_request_cast(dd->areq);
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	bool enc = atmel_aes_is_encrypt(dd);
	u32 offset, authsize, itag[4], *otag = ctx->tag;
	int err;

	/* Read the computed tag. */
	if (likely(dd->flags & AES_FLAGS_GTAGEN))
		atmel_aes_read_block(dd, AES_TAGR(0), ctx->tag);
	else
		atmel_aes_read_block(dd, AES_ODATAR(0), ctx->tag);

	offset = req->assoclen + ctx->textlen;
	authsize = crypto_aead_authsize(tfm);
	if (enc) {
		scatterwalk_map_and_copy(otag, req->dst, offset, authsize, 1);
		err = 0;
	} else {
		scatterwalk_map_and_copy(itag, req->src, offset, authsize, 0);
		err = crypto_memneq(itag, otag, authsize) ? -EBADMSG : 0;
	}

	return atmel_aes_complete(dd, err);
}

static int atmel_aes_gcm_crypt(struct aead_request *req,
			       unsigned long mode)
{
	struct atmel_aes_base_ctx *ctx;
	struct atmel_aes_reqctx *rctx;
	struct atmel_aes_dev *dd;

	ctx = crypto_aead_ctx(crypto_aead_reqtfm(req));
	ctx->block_size = AES_BLOCK_SIZE;

	dd = atmel_aes_find_dev(ctx);
	if (!dd)
		return -ENODEV;

	rctx = aead_request_ctx(req);
	rctx->mode = AES_FLAGS_GCM | mode;

	return atmel_aes_handle_queue(dd, &req->base);
}

static int atmel_aes_gcm_setkey(struct crypto_aead *tfm, const u8 *key,
				unsigned int keylen)
{
	struct atmel_aes_base_ctx *ctx = crypto_aead_ctx(tfm);

	if (keylen != AES_KEYSIZE_256 &&
	    keylen != AES_KEYSIZE_192 &&
	    keylen != AES_KEYSIZE_128) {
		crypto_aead_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}

	memcpy(ctx->key, key, keylen);
	ctx->keylen = keylen;

	return 0;
}

static int atmel_aes_gcm_setauthsize(struct crypto_aead *tfm,
				     unsigned int authsize)
{
	/* Same as crypto_gcm_authsize() from crypto/gcm.c */
	switch (authsize) {
	case 4:
	case 8:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int atmel_aes_gcm_encrypt(struct aead_request *req)
{
	return atmel_aes_gcm_crypt(req, AES_FLAGS_ENCRYPT);
}

static int atmel_aes_gcm_decrypt(struct aead_request *req)
{
	return atmel_aes_gcm_crypt(req, 0);
}

static int atmel_aes_gcm_init(struct crypto_aead *tfm)
{
	struct atmel_aes_gcm_ctx *ctx = crypto_aead_ctx(tfm);

	crypto_aead_set_reqsize(tfm, sizeof(struct atmel_aes_reqctx));
	ctx->base.start = atmel_aes_gcm_start;

	return 0;
}

static void atmel_aes_gcm_exit(struct crypto_aead *tfm)
{

}

static struct aead_alg aes_gcm_alg = {
	.setkey		= atmel_aes_gcm_setkey,
	.setauthsize	= atmel_aes_gcm_setauthsize,
	.encrypt	= atmel_aes_gcm_encrypt,
	.decrypt	= atmel_aes_gcm_decrypt,
	.init		= atmel_aes_gcm_init,
	.exit		= atmel_aes_gcm_exit,
	.ivsize		= 12,
	.maxauthsize	= AES_BLOCK_SIZE,

	.base = {
		.cra_name		= "gcm(aes)",
		.cra_driver_name	= "atmel-gcm-aes",
		.cra_priority		= ATMEL_AES_PRIORITY,
		.cra_flags		= CRYPTO_ALG_ASYNC,
		.cra_blocksize		= 1,
		.cra_ctxsize		= sizeof(struct atmel_aes_gcm_ctx),
		.cra_alignmask		= 0xf,
		.cra_module		= THIS_MODULE,
	},
};


/* Probe functions */

static int atmel_aes_buff_init(struct atmel_aes_dev *dd)
{
	dd->buf = (void *)__get_free_pages(GFP_KERNEL, ATMEL_AES_BUFFER_ORDER);
	dd->buflen = ATMEL_AES_BUFFER_SIZE;
	dd->buflen &= ~(AES_BLOCK_SIZE - 1);

	if (!dd->buf) {
		dev_err(dd->dev, "unable to alloc pages.\n");
		return -ENOMEM;
	}

	return 0;
}

static void atmel_aes_buff_cleanup(struct atmel_aes_dev *dd)
{
	free_page((unsigned long)dd->buf);
}

static bool atmel_aes_filter(struct dma_chan *chan, void *slave)
{
	struct at_dma_slave	*sl = slave;

	if (sl && sl->dma_dev == chan->device->dev) {
		chan->private = sl;
		return true;
	} else {
		return false;
	}
}

static int atmel_aes_dma_init(struct atmel_aes_dev *dd,
			      struct crypto_platform_data *pdata)
{
	struct at_dma_slave *slave;
	int err = -ENOMEM;
	dma_cap_mask_t mask;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	/* Try to grab 2 DMA channels */
	slave = &pdata->dma_slave->rxdata;
	dd->src.chan = dma_request_slave_channel_compat(mask, atmel_aes_filter,
							slave, dd->dev, "tx");
	if (!dd->src.chan)
		goto err_dma_in;

	slave = &pdata->dma_slave->txdata;
	dd->dst.chan = dma_request_slave_channel_compat(mask, atmel_aes_filter,
							slave, dd->dev, "rx");
	if (!dd->dst.chan)
		goto err_dma_out;

	return 0;

err_dma_out:
	dma_release_channel(dd->src.chan);
err_dma_in:
	dev_warn(dd->dev, "no DMA channel available\n");
	return err;
}

static void atmel_aes_dma_cleanup(struct atmel_aes_dev *dd)
{
	dma_release_channel(dd->dst.chan);
	dma_release_channel(dd->src.chan);
}

>>>>>>> v4.9.227
static void atmel_aes_queue_task(unsigned long data)
{
	struct atmel_aes_dev *dd = (struct atmel_aes_dev *)data;

	atmel_aes_handle_queue(dd, NULL);
}

static void atmel_aes_done_task(unsigned long data)
{
<<<<<<< HEAD
	struct atmel_aes_dev *dd = (struct atmel_aes_dev *) data;
	int err;

	if (!(dd->flags & AES_FLAGS_DMA)) {
		atmel_aes_read_n(dd, AES_ODATAR(0), (u32 *) dd->buf_out,
				dd->bufcnt >> 2);

		if (sg_copy_from_buffer(dd->out_sg, dd->nb_out_sg,
			dd->buf_out, dd->bufcnt))
			err = 0;
		else
			err = -EINVAL;

		goto cpu_end;
	}

	err = atmel_aes_crypt_dma_stop(dd);

	err = dd->err ? : err;

	if (dd->total && !err) {
		if (dd->flags & AES_FLAGS_FAST) {
			dd->in_sg = sg_next(dd->in_sg);
			dd->out_sg = sg_next(dd->out_sg);
			if (!dd->in_sg || !dd->out_sg)
				err = -EINVAL;
		}
		if (!err)
			err = atmel_aes_crypt_dma_start(dd);
		if (!err)
			return; /* DMA started. Not fininishing. */
	}

cpu_end:
	atmel_aes_finish_req(dd, err);
	atmel_aes_handle_queue(dd, NULL);
=======
	struct atmel_aes_dev *dd = (struct atmel_aes_dev *)data;

	dd->is_async = true;
	(void)dd->resume(dd);
>>>>>>> v4.9.227
}

static irqreturn_t atmel_aes_irq(int irq, void *dev_id)
{
	struct atmel_aes_dev *aes_dd = dev_id;
	u32 reg;

	reg = atmel_aes_read(aes_dd, AES_ISR);
	if (reg & atmel_aes_read(aes_dd, AES_IMR)) {
		atmel_aes_write(aes_dd, AES_IDR, reg);
		if (AES_FLAGS_BUSY & aes_dd->flags)
			tasklet_schedule(&aes_dd->done_task);
		else
			dev_warn(aes_dd->dev, "AES interrupt when no active requests.\n");
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static void atmel_aes_unregister_algs(struct atmel_aes_dev *dd)
{
	int i;

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(aes_algs); i++)
		crypto_unregister_alg(&aes_algs[i]);
	if (dd->caps.has_cfb64)
		crypto_unregister_alg(&aes_cfb64_alg);
=======
	if (dd->caps.has_gcm)
		crypto_unregister_aead(&aes_gcm_alg);

	if (dd->caps.has_cfb64)
		crypto_unregister_alg(&aes_cfb64_alg);

	for (i = 0; i < ARRAY_SIZE(aes_algs); i++)
		crypto_unregister_alg(&aes_algs[i]);
>>>>>>> v4.9.227
}

static int atmel_aes_register_algs(struct atmel_aes_dev *dd)
{
	int err, i, j;

	for (i = 0; i < ARRAY_SIZE(aes_algs); i++) {
		err = crypto_register_alg(&aes_algs[i]);
		if (err)
			goto err_aes_algs;
	}

	if (dd->caps.has_cfb64) {
		err = crypto_register_alg(&aes_cfb64_alg);
		if (err)
			goto err_aes_cfb64_alg;
	}

<<<<<<< HEAD
	return 0;

=======
	if (dd->caps.has_gcm) {
		err = crypto_register_aead(&aes_gcm_alg);
		if (err)
			goto err_aes_gcm_alg;
	}

	return 0;

err_aes_gcm_alg:
	crypto_unregister_alg(&aes_cfb64_alg);
>>>>>>> v4.9.227
err_aes_cfb64_alg:
	i = ARRAY_SIZE(aes_algs);
err_aes_algs:
	for (j = 0; j < i; j++)
		crypto_unregister_alg(&aes_algs[j]);

	return err;
}

static void atmel_aes_get_cap(struct atmel_aes_dev *dd)
{
	dd->caps.has_dualbuff = 0;
	dd->caps.has_cfb64 = 0;
<<<<<<< HEAD
=======
	dd->caps.has_gcm = 0;
>>>>>>> v4.9.227
	dd->caps.max_burst_size = 1;

	/* keep only major version number */
	switch (dd->hw_version & 0xff0) {
<<<<<<< HEAD
=======
	case 0x500:
		dd->caps.has_dualbuff = 1;
		dd->caps.has_cfb64 = 1;
		dd->caps.has_gcm = 1;
		dd->caps.max_burst_size = 4;
		break;
	case 0x200:
		dd->caps.has_dualbuff = 1;
		dd->caps.has_cfb64 = 1;
		dd->caps.has_gcm = 1;
		dd->caps.max_burst_size = 4;
		break;
>>>>>>> v4.9.227
	case 0x130:
		dd->caps.has_dualbuff = 1;
		dd->caps.has_cfb64 = 1;
		dd->caps.max_burst_size = 4;
		break;
	case 0x120:
		break;
	default:
		dev_warn(dd->dev,
				"Unmanaged aes version, set minimum capabilities\n");
		break;
	}
}

#if defined(CONFIG_OF)
static const struct of_device_id atmel_aes_dt_ids[] = {
	{ .compatible = "atmel,at91sam9g46-aes" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, atmel_aes_dt_ids);

static struct crypto_platform_data *atmel_aes_of_init(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct crypto_platform_data *pdata;

	if (!np) {
		dev_err(&pdev->dev, "device node not found\n");
		return ERR_PTR(-EINVAL);
	}

	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		dev_err(&pdev->dev, "could not allocate memory for pdata\n");
		return ERR_PTR(-ENOMEM);
	}

	pdata->dma_slave = devm_kzalloc(&pdev->dev,
					sizeof(*(pdata->dma_slave)),
					GFP_KERNEL);
	if (!pdata->dma_slave) {
		dev_err(&pdev->dev, "could not allocate memory for dma_slave\n");
		devm_kfree(&pdev->dev, pdata);
		return ERR_PTR(-ENOMEM);
	}

	return pdata;
}
#else
static inline struct crypto_platform_data *atmel_aes_of_init(struct platform_device *pdev)
{
	return ERR_PTR(-EINVAL);
}
#endif

static int atmel_aes_probe(struct platform_device *pdev)
{
	struct atmel_aes_dev *aes_dd;
	struct crypto_platform_data *pdata;
	struct device *dev = &pdev->dev;
	struct resource *aes_res;
<<<<<<< HEAD
	unsigned long aes_phys_size;
=======
>>>>>>> v4.9.227
	int err;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		pdata = atmel_aes_of_init(pdev);
		if (IS_ERR(pdata)) {
			err = PTR_ERR(pdata);
			goto aes_dd_err;
		}
	}

	if (!pdata->dma_slave) {
		err = -ENXIO;
		goto aes_dd_err;
	}

<<<<<<< HEAD
	aes_dd = kzalloc(sizeof(struct atmel_aes_dev), GFP_KERNEL);
=======
	aes_dd = devm_kzalloc(&pdev->dev, sizeof(*aes_dd), GFP_KERNEL);
>>>>>>> v4.9.227
	if (aes_dd == NULL) {
		dev_err(dev, "unable to alloc data struct.\n");
		err = -ENOMEM;
		goto aes_dd_err;
	}

	aes_dd->dev = dev;

	platform_set_drvdata(pdev, aes_dd);

	INIT_LIST_HEAD(&aes_dd->list);
<<<<<<< HEAD
=======
	spin_lock_init(&aes_dd->lock);
>>>>>>> v4.9.227

	tasklet_init(&aes_dd->done_task, atmel_aes_done_task,
					(unsigned long)aes_dd);
	tasklet_init(&aes_dd->queue_task, atmel_aes_queue_task,
					(unsigned long)aes_dd);

	crypto_init_queue(&aes_dd->queue, ATMEL_AES_QUEUE_LENGTH);

	aes_dd->irq = -1;

	/* Get the base address */
	aes_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!aes_res) {
		dev_err(dev, "no MEM resource info\n");
		err = -ENODEV;
		goto res_err;
	}
	aes_dd->phys_base = aes_res->start;
<<<<<<< HEAD
	aes_phys_size = resource_size(aes_res);
=======
>>>>>>> v4.9.227

	/* Get the IRQ */
	aes_dd->irq = platform_get_irq(pdev,  0);
	if (aes_dd->irq < 0) {
		dev_err(dev, "no IRQ resource info\n");
		err = aes_dd->irq;
<<<<<<< HEAD
		goto aes_irq_err;
	}

	err = request_irq(aes_dd->irq, atmel_aes_irq, IRQF_SHARED, "atmel-aes",
						aes_dd);
	if (err) {
		dev_err(dev, "unable to request aes irq.\n");
		goto aes_irq_err;
	}

	/* Initializing the clock */
	aes_dd->iclk = clk_get(&pdev->dev, "aes_clk");
	if (IS_ERR(aes_dd->iclk)) {
		dev_err(dev, "clock intialization failed.\n");
		err = PTR_ERR(aes_dd->iclk);
		goto clk_err;
	}

	aes_dd->io_base = ioremap(aes_dd->phys_base, aes_phys_size);
	if (!aes_dd->io_base) {
		dev_err(dev, "can't ioremap\n");
		err = -ENOMEM;
		goto aes_io_err;
	}

	atmel_aes_hw_version_init(aes_dd);
=======
		goto res_err;
	}

	err = devm_request_irq(&pdev->dev, aes_dd->irq, atmel_aes_irq,
			       IRQF_SHARED, "atmel-aes", aes_dd);
	if (err) {
		dev_err(dev, "unable to request aes irq.\n");
		goto res_err;
	}

	/* Initializing the clock */
	aes_dd->iclk = devm_clk_get(&pdev->dev, "aes_clk");
	if (IS_ERR(aes_dd->iclk)) {
		dev_err(dev, "clock initialization failed.\n");
		err = PTR_ERR(aes_dd->iclk);
		goto res_err;
	}

	aes_dd->io_base = devm_ioremap_resource(&pdev->dev, aes_res);
	if (IS_ERR(aes_dd->io_base)) {
		dev_err(dev, "can't ioremap\n");
		err = PTR_ERR(aes_dd->io_base);
		goto res_err;
	}

	err = clk_prepare(aes_dd->iclk);
	if (err)
		goto res_err;

	err = atmel_aes_hw_version_init(aes_dd);
	if (err)
		goto iclk_unprepare;
>>>>>>> v4.9.227

	atmel_aes_get_cap(aes_dd);

	err = atmel_aes_buff_init(aes_dd);
	if (err)
		goto err_aes_buff;

	err = atmel_aes_dma_init(aes_dd, pdata);
	if (err)
		goto err_aes_dma;

	spin_lock(&atmel_aes.lock);
	list_add_tail(&aes_dd->list, &atmel_aes.dev_list);
	spin_unlock(&atmel_aes.lock);

	err = atmel_aes_register_algs(aes_dd);
	if (err)
		goto err_algs;

	dev_info(dev, "Atmel AES - Using %s, %s for DMA transfers\n",
<<<<<<< HEAD
			dma_chan_name(aes_dd->dma_lch_in.chan),
			dma_chan_name(aes_dd->dma_lch_out.chan));
=======
			dma_chan_name(aes_dd->src.chan),
			dma_chan_name(aes_dd->dst.chan));
>>>>>>> v4.9.227

	return 0;

err_algs:
	spin_lock(&atmel_aes.lock);
	list_del(&aes_dd->list);
	spin_unlock(&atmel_aes.lock);
	atmel_aes_dma_cleanup(aes_dd);
err_aes_dma:
	atmel_aes_buff_cleanup(aes_dd);
err_aes_buff:
<<<<<<< HEAD
	iounmap(aes_dd->io_base);
aes_io_err:
	clk_put(aes_dd->iclk);
clk_err:
	free_irq(aes_dd->irq, aes_dd);
aes_irq_err:
res_err:
	tasklet_kill(&aes_dd->done_task);
	tasklet_kill(&aes_dd->queue_task);
	kfree(aes_dd);
	aes_dd = NULL;
=======
iclk_unprepare:
	clk_unprepare(aes_dd->iclk);
res_err:
	tasklet_kill(&aes_dd->done_task);
	tasklet_kill(&aes_dd->queue_task);
>>>>>>> v4.9.227
aes_dd_err:
	dev_err(dev, "initialization failed.\n");

	return err;
}

static int atmel_aes_remove(struct platform_device *pdev)
{
	static struct atmel_aes_dev *aes_dd;

	aes_dd = platform_get_drvdata(pdev);
	if (!aes_dd)
		return -ENODEV;
	spin_lock(&atmel_aes.lock);
	list_del(&aes_dd->list);
	spin_unlock(&atmel_aes.lock);

	atmel_aes_unregister_algs(aes_dd);

	tasklet_kill(&aes_dd->done_task);
	tasklet_kill(&aes_dd->queue_task);

	atmel_aes_dma_cleanup(aes_dd);
<<<<<<< HEAD

	iounmap(aes_dd->io_base);

	clk_put(aes_dd->iclk);

	if (aes_dd->irq > 0)
		free_irq(aes_dd->irq, aes_dd);

	kfree(aes_dd);
	aes_dd = NULL;
=======
	atmel_aes_buff_cleanup(aes_dd);

	clk_unprepare(aes_dd->iclk);
>>>>>>> v4.9.227

	return 0;
}

static struct platform_driver atmel_aes_driver = {
	.probe		= atmel_aes_probe,
	.remove		= atmel_aes_remove,
	.driver		= {
		.name	= "atmel_aes",
<<<<<<< HEAD
		.owner	= THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = of_match_ptr(atmel_aes_dt_ids),
	},
};

module_platform_driver(atmel_aes_driver);

MODULE_DESCRIPTION("Atmel AES hw acceleration support.");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Nicolas Royer - Eukréa Electromatique");
