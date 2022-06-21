/* Driver for TI CC2520 802.15.4 Wireless-PAN Networking controller
 *
 * Copyright (C) 2014 Varka Bhadram <varkab@cdac.in>
 *		      Md.Jamal Mohiuddin <mjmohiuddin@cdac.in>
 *		      P Sowjanya <sowjanyap@cdac.in>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/spi/cc2520.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
<<<<<<< HEAD
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>

#include <net/mac802154.h>
#include <net/wpan-phy.h>
#include <net/ieee802154.h>
=======
#include <linux/of_gpio.h>
#include <linux/ieee802154.h>
#include <linux/crc-ccitt.h>
#include <asm/unaligned.h>

#include <net/mac802154.h>
#include <net/cfg802154.h>
>>>>>>> v4.9.227

#define	SPI_COMMAND_BUFFER	3
#define	HIGH			1
#define	LOW			0
#define	STATE_IDLE		0
#define	RSSI_VALID		0
#define	RSSI_OFFSET		78

#define	CC2520_RAM_SIZE		640
#define	CC2520_FIFO_SIZE	128

#define	CC2520RAM_TXFIFO	0x100
#define	CC2520RAM_RXFIFO	0x180
#define	CC2520RAM_IEEEADDR	0x3EA
#define	CC2520RAM_PANID		0x3F2
#define	CC2520RAM_SHORTADDR	0x3F4

#define	CC2520_FREG_MASK	0x3F

/* status byte values */
<<<<<<< HEAD
#define	CC2520_STATUS_XOSC32M_STABLE	(1 << 7)
#define	CC2520_STATUS_RSSI_VALID	(1 << 6)
#define	CC2520_STATUS_TX_UNDERFLOW	(1 << 3)
=======
#define	CC2520_STATUS_XOSC32M_STABLE	BIT(7)
#define	CC2520_STATUS_RSSI_VALID	BIT(6)
#define	CC2520_STATUS_TX_UNDERFLOW	BIT(3)
>>>>>>> v4.9.227

/* IEEE-802.15.4 defined constants (2.4 GHz logical channels) */
#define	CC2520_MINCHANNEL		11
#define	CC2520_MAXCHANNEL		26
#define	CC2520_CHANNEL_SPACING		5

/* command strobes */
#define	CC2520_CMD_SNOP			0x00
#define	CC2520_CMD_IBUFLD		0x02
#define	CC2520_CMD_SIBUFEX		0x03
#define	CC2520_CMD_SSAMPLECCA		0x04
#define	CC2520_CMD_SRES			0x0f
#define	CC2520_CMD_MEMORY_MASK		0x0f
#define	CC2520_CMD_MEMORY_READ		0x10
#define	CC2520_CMD_MEMORY_WRITE		0x20
#define	CC2520_CMD_RXBUF		0x30
#define	CC2520_CMD_RXBUFCP		0x38
#define	CC2520_CMD_RXBUFMOV		0x32
#define	CC2520_CMD_TXBUF		0x3A
#define	CC2520_CMD_TXBUFCP		0x3E
#define	CC2520_CMD_RANDOM		0x3C
#define	CC2520_CMD_SXOSCON		0x40
#define	CC2520_CMD_STXCAL		0x41
#define	CC2520_CMD_SRXON		0x42
#define	CC2520_CMD_STXON		0x43
#define	CC2520_CMD_STXONCCA		0x44
#define	CC2520_CMD_SRFOFF		0x45
#define	CC2520_CMD_SXOSCOFF		0x46
#define	CC2520_CMD_SFLUSHRX		0x47
#define	CC2520_CMD_SFLUSHTX		0x48
#define	CC2520_CMD_SACK			0x49
#define	CC2520_CMD_SACKPEND		0x4A
#define	CC2520_CMD_SNACK		0x4B
#define	CC2520_CMD_SRXMASKBITSET	0x4C
#define	CC2520_CMD_SRXMASKBITCLR	0x4D
#define	CC2520_CMD_RXMASKAND		0x4E
#define	CC2520_CMD_RXMASKOR		0x4F
#define	CC2520_CMD_MEMCP		0x50
#define	CC2520_CMD_MEMCPR		0x52
#define	CC2520_CMD_MEMXCP		0x54
#define	CC2520_CMD_MEMXWR		0x56
#define	CC2520_CMD_BCLR			0x58
#define	CC2520_CMD_BSET			0x59
#define	CC2520_CMD_CTR_UCTR		0x60
#define	CC2520_CMD_CBCMAC		0x64
#define	CC2520_CMD_UCBCMAC		0x66
#define	CC2520_CMD_CCM			0x68
#define	CC2520_CMD_UCCM			0x6A
#define	CC2520_CMD_ECB			0x70
#define	CC2520_CMD_ECBO			0x72
#define	CC2520_CMD_ECBX			0x74
#define	CC2520_CMD_INC			0x78
#define	CC2520_CMD_ABORT		0x7F
#define	CC2520_CMD_REGISTER_READ	0x80
#define	CC2520_CMD_REGISTER_WRITE	0xC0

/* status registers */
#define	CC2520_CHIPID			0x40
#define	CC2520_VERSION			0x42
#define	CC2520_EXTCLOCK			0x44
#define	CC2520_MDMCTRL0			0x46
#define	CC2520_MDMCTRL1			0x47
#define	CC2520_FREQEST			0x48
#define	CC2520_RXCTRL			0x4A
#define	CC2520_FSCTRL			0x4C
#define	CC2520_FSCAL0			0x4E
#define	CC2520_FSCAL1			0x4F
#define	CC2520_FSCAL2			0x50
#define	CC2520_FSCAL3			0x51
#define	CC2520_AGCCTRL0			0x52
#define	CC2520_AGCCTRL1			0x53
#define	CC2520_AGCCTRL2			0x54
#define	CC2520_AGCCTRL3			0x55
#define	CC2520_ADCTEST0			0x56
#define	CC2520_ADCTEST1			0x57
#define	CC2520_ADCTEST2			0x58
#define	CC2520_MDMTEST0			0x5A
#define	CC2520_MDMTEST1			0x5B
#define	CC2520_DACTEST0			0x5C
#define	CC2520_DACTEST1			0x5D
#define	CC2520_ATEST			0x5E
#define	CC2520_DACTEST2			0x5F
#define	CC2520_PTEST0			0x60
#define	CC2520_PTEST1			0x61
#define	CC2520_RESERVED			0x62
#define	CC2520_DPUBIST			0x7A
#define	CC2520_ACTBIST			0x7C
#define	CC2520_RAMBIST			0x7E

/* frame registers */
#define	CC2520_FRMFILT0			0x00
#define	CC2520_FRMFILT1			0x01
#define	CC2520_SRCMATCH			0x02
#define	CC2520_SRCSHORTEN0		0x04
#define	CC2520_SRCSHORTEN1		0x05
#define	CC2520_SRCSHORTEN2		0x06
#define	CC2520_SRCEXTEN0		0x08
#define	CC2520_SRCEXTEN1		0x09
#define	CC2520_SRCEXTEN2		0x0A
#define	CC2520_FRMCTRL0			0x0C
#define	CC2520_FRMCTRL1			0x0D
#define	CC2520_RXENABLE0		0x0E
#define	CC2520_RXENABLE1		0x0F
#define	CC2520_EXCFLAG0			0x10
#define	CC2520_EXCFLAG1			0x11
#define	CC2520_EXCFLAG2			0x12
#define	CC2520_EXCMASKA0		0x14
#define	CC2520_EXCMASKA1		0x15
#define	CC2520_EXCMASKA2		0x16
#define	CC2520_EXCMASKB0		0x18
#define	CC2520_EXCMASKB1		0x19
#define	CC2520_EXCMASKB2		0x1A
#define	CC2520_EXCBINDX0		0x1C
#define	CC2520_EXCBINDX1		0x1D
#define	CC2520_EXCBINDY0		0x1E
#define	CC2520_EXCBINDY1		0x1F
#define	CC2520_GPIOCTRL0		0x20
#define	CC2520_GPIOCTRL1		0x21
#define	CC2520_GPIOCTRL2		0x22
#define	CC2520_GPIOCTRL3		0x23
#define	CC2520_GPIOCTRL4		0x24
#define	CC2520_GPIOCTRL5		0x25
#define	CC2520_GPIOPOLARITY		0x26
#define	CC2520_GPIOCTRL			0x28
#define	CC2520_DPUCON			0x2A
#define	CC2520_DPUSTAT			0x2C
#define	CC2520_FREQCTRL			0x2E
#define	CC2520_FREQTUNE			0x2F
#define	CC2520_TXPOWER			0x30
#define	CC2520_TXCTRL			0x31
#define	CC2520_FSMSTAT0			0x32
#define	CC2520_FSMSTAT1			0x33
#define	CC2520_FIFOPCTRL		0x34
#define	CC2520_FSMCTRL			0x35
#define	CC2520_CCACTRL0			0x36
#define	CC2520_CCACTRL1			0x37
#define	CC2520_RSSI			0x38
#define	CC2520_RSSISTAT			0x39
#define	CC2520_RXFIRST			0x3C
#define	CC2520_RXFIFOCNT		0x3E
#define	CC2520_TXFIFOCNT		0x3F

<<<<<<< HEAD
/* Driver private information */
struct cc2520_private {
	struct spi_device *spi;		/* SPI device structure */
	struct ieee802154_dev *dev;	/* IEEE-802.15.4 device */
	u8 *buf;			/* SPI TX/Rx data buffer */
	struct mutex buffer_mutex;	/* SPI buffer mutex */
	bool is_tx;			/* Flag for sync b/w Tx and Rx */
=======
/* CC2520_FRMFILT0 */
#define FRMFILT0_FRAME_FILTER_EN	BIT(0)
#define FRMFILT0_PAN_COORDINATOR	BIT(1)

/* CC2520_FRMCTRL0 */
#define FRMCTRL0_AUTOACK		BIT(5)
#define FRMCTRL0_AUTOCRC		BIT(6)

/* CC2520_FRMCTRL1 */
#define FRMCTRL1_SET_RXENMASK_ON_TX	BIT(0)
#define FRMCTRL1_IGNORE_TX_UNDERF	BIT(1)

/* Driver private information */
struct cc2520_private {
	struct spi_device *spi;		/* SPI device structure */
	struct ieee802154_hw *hw;	/* IEEE-802.15.4 device */
	u8 *buf;			/* SPI TX/Rx data buffer */
	struct mutex buffer_mutex;	/* SPI buffer mutex */
	bool is_tx;			/* Flag for sync b/w Tx and Rx */
	bool amplified;			/* Flag for CC2591 */
>>>>>>> v4.9.227
	int fifo_pin;			/* FIFO GPIO pin number */
	struct work_struct fifop_irqwork;/* Workqueue for FIFOP */
	spinlock_t lock;		/* Lock for is_tx*/
	struct completion tx_complete;	/* Work completion for Tx */
<<<<<<< HEAD
=======
	bool promiscuous;               /* Flag for promiscuous mode */
>>>>>>> v4.9.227
};

/* Generic Functions */
static int
cc2520_cmd_strobe(struct cc2520_private *priv, u8 cmd)
{
	int ret;
	u8 status = 0xff;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 0,
		.tx_buf = priv->buf,
		.rx_buf = priv->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	mutex_lock(&priv->buffer_mutex);
	priv->buf[xfer.len++] = cmd;
	dev_vdbg(&priv->spi->dev,
		 "command strobe buf[0] = %02x\n",
		 priv->buf[0]);

	ret = spi_sync(priv->spi, &msg);
	if (!ret)
		status = priv->buf[0];
	dev_vdbg(&priv->spi->dev,
		 "buf[0] = %02x\n", priv->buf[0]);
	mutex_unlock(&priv->buffer_mutex);

	return ret;
}

static int
cc2520_get_status(struct cc2520_private *priv, u8 *status)
{
	int ret;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 0,
		.tx_buf = priv->buf,
		.rx_buf = priv->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	mutex_lock(&priv->buffer_mutex);
	priv->buf[xfer.len++] = CC2520_CMD_SNOP;
	dev_vdbg(&priv->spi->dev,
		 "get status command buf[0] = %02x\n", priv->buf[0]);

	ret = spi_sync(priv->spi, &msg);
	if (!ret)
		*status = priv->buf[0];
	dev_vdbg(&priv->spi->dev,
		 "buf[0] = %02x\n", priv->buf[0]);
	mutex_unlock(&priv->buffer_mutex);

	return ret;
}

static int
cc2520_write_register(struct cc2520_private *priv, u8 reg, u8 value)
{
	int status;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 0,
		.tx_buf = priv->buf,
		.rx_buf = priv->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	mutex_lock(&priv->buffer_mutex);

	if (reg <= CC2520_FREG_MASK) {
		priv->buf[xfer.len++] = CC2520_CMD_REGISTER_WRITE | reg;
		priv->buf[xfer.len++] = value;
	} else {
		priv->buf[xfer.len++] = CC2520_CMD_MEMORY_WRITE;
		priv->buf[xfer.len++] = reg;
		priv->buf[xfer.len++] = value;
	}
	status = spi_sync(priv->spi, &msg);
	if (msg.status)
		status = msg.status;

	mutex_unlock(&priv->buffer_mutex);

	return status;
}

static int
cc2520_write_ram(struct cc2520_private *priv, u16 reg, u8 len, u8 *data)
{
	int status;
	struct spi_message msg;
	struct spi_transfer xfer_head = {
		.len        = 0,
		.tx_buf        = priv->buf,
		.rx_buf        = priv->buf,
	};

	struct spi_transfer xfer_buf = {
		.len = len,
		.tx_buf = data,
	};

	mutex_lock(&priv->buffer_mutex);
	priv->buf[xfer_head.len++] = (CC2520_CMD_MEMORY_WRITE |
						((reg >> 8) & 0xff));
	priv->buf[xfer_head.len++] = reg & 0xff;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer_head, &msg);
	spi_message_add_tail(&xfer_buf, &msg);

	status = spi_sync(priv->spi, &msg);
	dev_dbg(&priv->spi->dev, "spi status = %d\n", status);
	if (msg.status)
		status = msg.status;

	mutex_unlock(&priv->buffer_mutex);
	return status;
}

static int
cc2520_read_register(struct cc2520_private *priv, u8 reg, u8 *data)
{
	int status;
	struct spi_message msg;
	struct spi_transfer xfer1 = {
		.len = 0,
		.tx_buf = priv->buf,
		.rx_buf = priv->buf,
	};

	struct spi_transfer xfer2 = {
		.len = 1,
		.rx_buf = data,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer1, &msg);
	spi_message_add_tail(&xfer2, &msg);

	mutex_lock(&priv->buffer_mutex);
	priv->buf[xfer1.len++] = CC2520_CMD_MEMORY_READ;
	priv->buf[xfer1.len++] = reg;

	status = spi_sync(priv->spi, &msg);
	dev_dbg(&priv->spi->dev,
		"spi status = %d\n", status);
	if (msg.status)
		status = msg.status;

	mutex_unlock(&priv->buffer_mutex);

	return status;
}

static int
<<<<<<< HEAD
cc2520_write_txfifo(struct cc2520_private *priv, u8 *data, u8 len)
=======
cc2520_write_txfifo(struct cc2520_private *priv, u8 pkt_len, u8 *data, u8 len)
>>>>>>> v4.9.227
{
	int status;

	/* length byte must include FCS even
	 * if it is calculated in the hardware
	 */
<<<<<<< HEAD
	int len_byte = len + 2;
=======
	int len_byte = pkt_len;
>>>>>>> v4.9.227

	struct spi_message msg;

	struct spi_transfer xfer_head = {
		.len = 0,
		.tx_buf = priv->buf,
		.rx_buf = priv->buf,
	};
	struct spi_transfer xfer_len = {
		.len = 1,
		.tx_buf = &len_byte,
	};
	struct spi_transfer xfer_buf = {
		.len = len,
		.tx_buf = data,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer_head, &msg);
	spi_message_add_tail(&xfer_len, &msg);
	spi_message_add_tail(&xfer_buf, &msg);

	mutex_lock(&priv->buffer_mutex);
	priv->buf[xfer_head.len++] = CC2520_CMD_TXBUF;
	dev_vdbg(&priv->spi->dev,
		 "TX_FIFO cmd buf[0] = %02x\n", priv->buf[0]);

	status = spi_sync(priv->spi, &msg);
	dev_vdbg(&priv->spi->dev, "status = %d\n", status);
	if (msg.status)
		status = msg.status;
	dev_vdbg(&priv->spi->dev, "status = %d\n", status);
	dev_vdbg(&priv->spi->dev, "buf[0] = %02x\n", priv->buf[0]);
	mutex_unlock(&priv->buffer_mutex);

	return status;
}

static int
<<<<<<< HEAD
cc2520_read_rxfifo(struct cc2520_private *priv, u8 *data, u8 len, u8 *lqi)
=======
cc2520_read_rxfifo(struct cc2520_private *priv, u8 *data, u8 len)
>>>>>>> v4.9.227
{
	int status;
	struct spi_message msg;

	struct spi_transfer xfer_head = {
		.len = 0,
		.tx_buf = priv->buf,
		.rx_buf = priv->buf,
	};
	struct spi_transfer xfer_buf = {
		.len = len,
		.rx_buf = data,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer_head, &msg);
	spi_message_add_tail(&xfer_buf, &msg);

	mutex_lock(&priv->buffer_mutex);
	priv->buf[xfer_head.len++] = CC2520_CMD_RXBUF;

	dev_vdbg(&priv->spi->dev, "read rxfifo buf[0] = %02x\n", priv->buf[0]);
	dev_vdbg(&priv->spi->dev, "buf[1] = %02x\n", priv->buf[1]);

	status = spi_sync(priv->spi, &msg);
	dev_vdbg(&priv->spi->dev, "status = %d\n", status);
	if (msg.status)
		status = msg.status;
	dev_vdbg(&priv->spi->dev, "status = %d\n", status);
	dev_vdbg(&priv->spi->dev,
		 "return status buf[0] = %02x\n", priv->buf[0]);
	dev_vdbg(&priv->spi->dev, "length buf[1] = %02x\n", priv->buf[1]);

	mutex_unlock(&priv->buffer_mutex);

	return status;
}

<<<<<<< HEAD
static int cc2520_start(struct ieee802154_dev *dev)
{
	return cc2520_cmd_strobe(dev->priv, CC2520_CMD_SRXON);
}

static void cc2520_stop(struct ieee802154_dev *dev)
{
	cc2520_cmd_strobe(dev->priv, CC2520_CMD_SRFOFF);
}

static int
cc2520_tx(struct ieee802154_dev *dev, struct sk_buff *skb)
{
	struct cc2520_private *priv = dev->priv;
	unsigned long flags;
	int rc;
	u8 status = 0;
=======
static int cc2520_start(struct ieee802154_hw *hw)
{
	return cc2520_cmd_strobe(hw->priv, CC2520_CMD_SRXON);
}

static void cc2520_stop(struct ieee802154_hw *hw)
{
	cc2520_cmd_strobe(hw->priv, CC2520_CMD_SRFOFF);
}

static int
cc2520_tx(struct ieee802154_hw *hw, struct sk_buff *skb)
{
	struct cc2520_private *priv = hw->priv;
	unsigned long flags;
	int rc;
	u8 status = 0;
	u8 pkt_len;

	/* In promiscuous mode we disable AUTOCRC so we can get the raw CRC
	 * values on RX. This means we need to manually add the CRC on TX.
	 */
	if (priv->promiscuous) {
		u16 crc = crc_ccitt(0, skb->data, skb->len);

		put_unaligned_le16(crc, skb_put(skb, 2));
		pkt_len = skb->len;
	} else {
		pkt_len = skb->len + 2;
	}
>>>>>>> v4.9.227

	rc = cc2520_cmd_strobe(priv, CC2520_CMD_SFLUSHTX);
	if (rc)
		goto err_tx;

<<<<<<< HEAD
	rc = cc2520_write_txfifo(priv, skb->data, skb->len);
=======
	rc = cc2520_write_txfifo(priv, pkt_len, skb->data, skb->len);
>>>>>>> v4.9.227
	if (rc)
		goto err_tx;

	rc = cc2520_get_status(priv, &status);
	if (rc)
		goto err_tx;

	if (status & CC2520_STATUS_TX_UNDERFLOW) {
		dev_err(&priv->spi->dev, "cc2520 tx underflow exception\n");
		goto err_tx;
	}

	spin_lock_irqsave(&priv->lock, flags);
	BUG_ON(priv->is_tx);
	priv->is_tx = 1;
	spin_unlock_irqrestore(&priv->lock, flags);

	rc = cc2520_cmd_strobe(priv, CC2520_CMD_STXONCCA);
	if (rc)
		goto err;

	rc = wait_for_completion_interruptible(&priv->tx_complete);
	if (rc < 0)
		goto err;

	cc2520_cmd_strobe(priv, CC2520_CMD_SFLUSHTX);
	cc2520_cmd_strobe(priv, CC2520_CMD_SRXON);

	return rc;
err:
	spin_lock_irqsave(&priv->lock, flags);
	priv->is_tx = 0;
	spin_unlock_irqrestore(&priv->lock, flags);
err_tx:
	return rc;
}

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
static int cc2520_rx(struct cc2520_private *priv)
{
	u8 len = 0, lqi = 0, bytes = 1;
	struct sk_buff *skb;

<<<<<<< HEAD
	cc2520_read_rxfifo(priv, &len, bytes, &lqi);

	if (len < 2 || len > IEEE802154_MTU)
		return -EINVAL;

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	if (cc2520_read_rxfifo(priv, skb_put(skb, len), len, &lqi)) {
=======
	/* Read single length byte from the radio. */
	cc2520_read_rxfifo(priv, &len, bytes);

	if (!ieee802154_is_valid_psdu_len(len)) {
		/* Corrupted frame received, clear frame buffer by
		 * reading entire buffer.
		 */
		dev_dbg(&priv->spi->dev, "corrupted frame received\n");
		len = IEEE802154_MTU;
	}

	skb = dev_alloc_skb(len);
	if (!skb)
		return -ENOMEM;

	if (cc2520_read_rxfifo(priv, skb_put(skb, len), len)) {
>>>>>>> v4.9.227
		dev_dbg(&priv->spi->dev, "frame reception failed\n");
		kfree_skb(skb);
		return -EINVAL;
	}

<<<<<<< HEAD
	skb_trim(skb, skb->len - 2);

	ieee802154_rx_irqsafe(priv->dev, skb, lqi);
=======
	/* In promiscuous mode, we configure the radio to include the
	 * CRC (AUTOCRC==0) and we pass on the packet unconditionally. If not
	 * in promiscuous mode, we check the CRC here, but leave the
	 * RSSI/LQI/CRC_OK bytes as they will get removed in the mac layer.
	 */
	if (!priv->promiscuous) {
		bool crc_ok;

		/* Check if the CRC is valid. With AUTOCRC set, the most
		 * significant bit of the last byte returned from the CC2520
		 * is CRC_OK flag. See section 20.3.4 of the datasheet.
		 */
		crc_ok = skb->data[len - 1] & BIT(7);

		/* If we failed CRC drop the packet in the driver layer. */
		if (!crc_ok) {
			dev_dbg(&priv->spi->dev, "CRC check failed\n");
			kfree_skb(skb);
			return -EINVAL;
		}

		/* To calculate LQI, the lower 7 bits of the last byte (the
		 * correlation value provided by the radio) must be scaled to
		 * the range 0-255. According to section 20.6, the correlation
		 * value ranges from 50-110. Ideally this would be calibrated
		 * per hardware design, but we use roughly the datasheet values
		 * to get close enough while avoiding floating point.
		 */
		lqi = skb->data[len - 1] & 0x7f;
		if (lqi < 50)
			lqi = 50;
		else if (lqi > 113)
			lqi = 113;
		lqi = (lqi - 50) * 4;
	}

	ieee802154_rx_irqsafe(priv->hw, skb, lqi);
>>>>>>> v4.9.227

	dev_vdbg(&priv->spi->dev, "RXFIFO: %x %x\n", len, lqi);

	return 0;
}

static int
<<<<<<< HEAD
cc2520_ed(struct ieee802154_dev *dev, u8 *level)
{
	struct cc2520_private *priv = dev->priv;
=======
cc2520_ed(struct ieee802154_hw *hw, u8 *level)
{
	struct cc2520_private *priv = hw->priv;
>>>>>>> v4.9.227
	u8 status = 0xff;
	u8 rssi;
	int ret;

<<<<<<< HEAD
	ret = cc2520_read_register(priv , CC2520_RSSISTAT, &status);
=======
	ret = cc2520_read_register(priv, CC2520_RSSISTAT, &status);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	if (status != RSSI_VALID)
		return -EINVAL;

<<<<<<< HEAD
	ret = cc2520_read_register(priv , CC2520_RSSI, &rssi);
=======
	ret = cc2520_read_register(priv, CC2520_RSSI, &rssi);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* level = RSSI(rssi) - OFFSET [dBm] : offset is 76dBm */
	*level = rssi - RSSI_OFFSET;

	return 0;
}

static int
<<<<<<< HEAD
cc2520_set_channel(struct ieee802154_dev *dev, int page, int channel)
{
	struct cc2520_private *priv = dev->priv;
	int ret;

	might_sleep();
=======
cc2520_set_channel(struct ieee802154_hw *hw, u8 page, u8 channel)
{
	struct cc2520_private *priv = hw->priv;
	int ret;

>>>>>>> v4.9.227
	dev_dbg(&priv->spi->dev, "trying to set channel\n");

	BUG_ON(page != 0);
	BUG_ON(channel < CC2520_MINCHANNEL);
	BUG_ON(channel > CC2520_MAXCHANNEL);

	ret = cc2520_write_register(priv, CC2520_FREQCTRL,
				    11 + 5*(channel - 11));

	return ret;
}

static int
<<<<<<< HEAD
cc2520_filter(struct ieee802154_dev *dev,
	      struct ieee802154_hw_addr_filt *filt, unsigned long changed)
{
	struct cc2520_private *priv = dev->priv;
=======
cc2520_filter(struct ieee802154_hw *hw,
	      struct ieee802154_hw_addr_filt *filt, unsigned long changed)
{
	struct cc2520_private *priv = hw->priv;
	int ret = 0;
>>>>>>> v4.9.227

	if (changed & IEEE802154_AFILT_PANID_CHANGED) {
		u16 panid = le16_to_cpu(filt->pan_id);

		dev_vdbg(&priv->spi->dev,
			 "cc2520_filter called for pan id\n");
<<<<<<< HEAD
		cc2520_write_ram(priv, CC2520RAM_PANID,
				 sizeof(panid), (u8 *)&panid);
=======
		ret = cc2520_write_ram(priv, CC2520RAM_PANID,
				       sizeof(panid), (u8 *)&panid);
>>>>>>> v4.9.227
	}

	if (changed & IEEE802154_AFILT_IEEEADDR_CHANGED) {
		dev_vdbg(&priv->spi->dev,
			 "cc2520_filter called for IEEE addr\n");
<<<<<<< HEAD
		cc2520_write_ram(priv, CC2520RAM_IEEEADDR,
				 sizeof(filt->ieee_addr),
				 (u8 *)&filt->ieee_addr);
=======
		ret = cc2520_write_ram(priv, CC2520RAM_IEEEADDR,
				       sizeof(filt->ieee_addr),
				       (u8 *)&filt->ieee_addr);
>>>>>>> v4.9.227
	}

	if (changed & IEEE802154_AFILT_SADDR_CHANGED) {
		u16 addr = le16_to_cpu(filt->short_addr);

		dev_vdbg(&priv->spi->dev,
			 "cc2520_filter called for saddr\n");
<<<<<<< HEAD
		cc2520_write_ram(priv, CC2520RAM_SHORTADDR,
				 sizeof(addr), (u8 *)&addr);
	}

	if (changed & IEEE802154_AFILT_PANC_CHANGED) {
		dev_vdbg(&priv->spi->dev,
			 "cc2520_filter called for panc change\n");
		if (filt->pan_coord)
			cc2520_write_register(priv, CC2520_FRMFILT0, 0x02);
		else
			cc2520_write_register(priv, CC2520_FRMFILT0, 0x00);
	}

	return 0;
}

static struct ieee802154_ops cc2520_ops = {
	.owner = THIS_MODULE,
	.start = cc2520_start,
	.stop = cc2520_stop,
	.xmit = cc2520_tx,
	.ed = cc2520_ed,
	.set_channel = cc2520_set_channel,
	.set_hw_addr_filt = cc2520_filter,
=======
		ret = cc2520_write_ram(priv, CC2520RAM_SHORTADDR,
				       sizeof(addr), (u8 *)&addr);
	}

	if (changed & IEEE802154_AFILT_PANC_CHANGED) {
		u8 frmfilt0;

		dev_vdbg(&priv->spi->dev,
			 "cc2520_filter called for panc change\n");

		cc2520_read_register(priv, CC2520_FRMFILT0, &frmfilt0);

		if (filt->pan_coord)
			frmfilt0 |= FRMFILT0_PAN_COORDINATOR;
		else
			frmfilt0 &= ~FRMFILT0_PAN_COORDINATOR;

		ret = cc2520_write_register(priv, CC2520_FRMFILT0, frmfilt0);
	}

	return ret;
}

static inline int cc2520_set_tx_power(struct cc2520_private *priv, s32 mbm)
{
	u8 power;

	switch (mbm) {
	case 500:
		power = 0xF7;
		break;
	case 300:
		power = 0xF2;
		break;
	case 200:
		power = 0xAB;
		break;
	case 100:
		power = 0x13;
		break;
	case 0:
		power = 0x32;
		break;
	case -200:
		power = 0x81;
		break;
	case -400:
		power = 0x88;
		break;
	case -700:
		power = 0x2C;
		break;
	case -1800:
		power = 0x03;
		break;
	default:
		return -EINVAL;
	}

	return cc2520_write_register(priv, CC2520_TXPOWER, power);
}

static inline int cc2520_cc2591_set_tx_power(struct cc2520_private *priv,
					     s32 mbm)
{
	u8 power;

	switch (mbm) {
	case 1700:
		power = 0xF9;
		break;
	case 1600:
		power = 0xF0;
		break;
	case 1400:
		power = 0xA0;
		break;
	case 1100:
		power = 0x2C;
		break;
	case -100:
		power = 0x03;
		break;
	case -800:
		power = 0x01;
		break;
	default:
		return -EINVAL;
	}

	return cc2520_write_register(priv, CC2520_TXPOWER, power);
}

#define CC2520_MAX_TX_POWERS 0x8
static const s32 cc2520_powers[CC2520_MAX_TX_POWERS + 1] = {
	500, 300, 200, 100, 0, -200, -400, -700, -1800,
};

#define CC2520_CC2591_MAX_TX_POWERS 0x5
static const s32 cc2520_cc2591_powers[CC2520_CC2591_MAX_TX_POWERS + 1] = {
	1700, 1600, 1400, 1100, -100, -800,
};

static int
cc2520_set_txpower(struct ieee802154_hw *hw, s32 mbm)
{
	struct cc2520_private *priv = hw->priv;

	if (!priv->amplified)
		return cc2520_set_tx_power(priv, mbm);

	return cc2520_cc2591_set_tx_power(priv, mbm);
}

static int
cc2520_set_promiscuous_mode(struct ieee802154_hw *hw, bool on)
{
	struct cc2520_private *priv = hw->priv;
	u8 frmfilt0;

	dev_dbg(&priv->spi->dev, "%s : mode %d\n", __func__, on);

	priv->promiscuous = on;

	cc2520_read_register(priv, CC2520_FRMFILT0, &frmfilt0);

	if (on) {
		/* Disable automatic ACK, automatic CRC, and frame filtering. */
		cc2520_write_register(priv, CC2520_FRMCTRL0, 0);
		frmfilt0 &= ~FRMFILT0_FRAME_FILTER_EN;
	} else {
		cc2520_write_register(priv, CC2520_FRMCTRL0, FRMCTRL0_AUTOACK |
							     FRMCTRL0_AUTOCRC);
		frmfilt0 |= FRMFILT0_FRAME_FILTER_EN;
	}
	return cc2520_write_register(priv, CC2520_FRMFILT0, frmfilt0);
}

static const struct ieee802154_ops cc2520_ops = {
	.owner = THIS_MODULE,
	.start = cc2520_start,
	.stop = cc2520_stop,
	.xmit_sync = cc2520_tx,
	.ed = cc2520_ed,
	.set_channel = cc2520_set_channel,
	.set_hw_addr_filt = cc2520_filter,
	.set_txpower = cc2520_set_txpower,
	.set_promiscuous_mode = cc2520_set_promiscuous_mode,
>>>>>>> v4.9.227
};

static int cc2520_register(struct cc2520_private *priv)
{
	int ret = -ENOMEM;

<<<<<<< HEAD
	priv->dev = ieee802154_alloc_device(sizeof(*priv), &cc2520_ops);
	if (!priv->dev)
		goto err_ret;

	priv->dev->priv = priv;
	priv->dev->parent = &priv->spi->dev;
	priv->dev->extra_tx_headroom = 0;

	/* We do support only 2.4 Ghz */
	priv->dev->phy->channels_supported[0] = 0x7FFF800;
	priv->dev->flags = IEEE802154_HW_OMIT_CKSUM | IEEE802154_HW_AACK;

	dev_vdbg(&priv->spi->dev, "registered cc2520\n");
	ret = ieee802154_register_device(priv->dev);
=======
	priv->hw = ieee802154_alloc_hw(sizeof(*priv), &cc2520_ops);
	if (!priv->hw)
		goto err_ret;

	priv->hw->priv = priv;
	priv->hw->parent = &priv->spi->dev;
	priv->hw->extra_tx_headroom = 0;
	ieee802154_random_extended_addr(&priv->hw->phy->perm_extended_addr);

	/* We do support only 2.4 Ghz */
	priv->hw->phy->supported.channels[0] = 0x7FFF800;
	priv->hw->flags = IEEE802154_HW_TX_OMIT_CKSUM | IEEE802154_HW_AFILT |
			  IEEE802154_HW_PROMISCUOUS;

	priv->hw->phy->flags = WPAN_PHY_FLAG_TXPOWER;

	if (!priv->amplified) {
		priv->hw->phy->supported.tx_powers = cc2520_powers;
		priv->hw->phy->supported.tx_powers_size = ARRAY_SIZE(cc2520_powers);
		priv->hw->phy->transmit_power = priv->hw->phy->supported.tx_powers[4];
	} else {
		priv->hw->phy->supported.tx_powers = cc2520_cc2591_powers;
		priv->hw->phy->supported.tx_powers_size = ARRAY_SIZE(cc2520_cc2591_powers);
		priv->hw->phy->transmit_power = priv->hw->phy->supported.tx_powers[0];
	}

	priv->hw->phy->current_channel = 11;

	dev_vdbg(&priv->spi->dev, "registered cc2520\n");
	ret = ieee802154_register_hw(priv->hw);
>>>>>>> v4.9.227
	if (ret)
		goto err_free_device;

	return 0;

err_free_device:
<<<<<<< HEAD
	ieee802154_free_device(priv->dev);
=======
	ieee802154_free_hw(priv->hw);
>>>>>>> v4.9.227
err_ret:
	return ret;
}

static void cc2520_fifop_irqwork(struct work_struct *work)
{
	struct cc2520_private *priv
		= container_of(work, struct cc2520_private, fifop_irqwork);

	dev_dbg(&priv->spi->dev, "fifop interrupt received\n");

	if (gpio_get_value(priv->fifo_pin))
		cc2520_rx(priv);
	else
		dev_dbg(&priv->spi->dev, "rxfifo overflow\n");

	cc2520_cmd_strobe(priv, CC2520_CMD_SFLUSHRX);
	cc2520_cmd_strobe(priv, CC2520_CMD_SFLUSHRX);
}

static irqreturn_t cc2520_fifop_isr(int irq, void *data)
{
	struct cc2520_private *priv = data;

	schedule_work(&priv->fifop_irqwork);

	return IRQ_HANDLED;
}

static irqreturn_t cc2520_sfd_isr(int irq, void *data)
{
	struct cc2520_private *priv = data;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->is_tx) {
		priv->is_tx = 0;
		spin_unlock_irqrestore(&priv->lock, flags);
		dev_dbg(&priv->spi->dev, "SFD for TX\n");
		complete(&priv->tx_complete);
	} else {
		spin_unlock_irqrestore(&priv->lock, flags);
		dev_dbg(&priv->spi->dev, "SFD for RX\n");
	}

	return IRQ_HANDLED;
}

<<<<<<< HEAD
=======
static int cc2520_get_platform_data(struct spi_device *spi,
				    struct cc2520_platform_data *pdata)
{
	struct device_node *np = spi->dev.of_node;
	struct cc2520_private *priv = spi_get_drvdata(spi);

	if (!np) {
		struct cc2520_platform_data *spi_pdata = spi->dev.platform_data;
		if (!spi_pdata)
			return -ENOENT;
		*pdata = *spi_pdata;
		priv->fifo_pin = pdata->fifo;
		return 0;
	}

	pdata->fifo = of_get_named_gpio(np, "fifo-gpio", 0);
	priv->fifo_pin = pdata->fifo;

	pdata->fifop = of_get_named_gpio(np, "fifop-gpio", 0);

	pdata->sfd = of_get_named_gpio(np, "sfd-gpio", 0);
	pdata->cca = of_get_named_gpio(np, "cca-gpio", 0);
	pdata->vreg = of_get_named_gpio(np, "vreg-gpio", 0);
	pdata->reset = of_get_named_gpio(np, "reset-gpio", 0);

	/* CC2591 front end for CC2520 */
	if (of_property_read_bool(np, "amplified"))
		priv->amplified = true;

	return 0;
}

>>>>>>> v4.9.227
static int cc2520_hw_init(struct cc2520_private *priv)
{
	u8 status = 0, state = 0xff;
	int ret;
	int timeout = 100;
<<<<<<< HEAD
=======
	struct cc2520_platform_data pdata;

	ret = cc2520_get_platform_data(priv->spi, &pdata);
	if (ret)
		goto err_ret;
>>>>>>> v4.9.227

	ret = cc2520_read_register(priv, CC2520_FSMSTAT1, &state);
	if (ret)
		goto err_ret;

	if (state != STATE_IDLE)
		return -EINVAL;

	do {
		ret = cc2520_get_status(priv, &status);
		if (ret)
			goto err_ret;

		if (timeout-- <= 0) {
			dev_err(&priv->spi->dev, "oscillator start failed!\n");
			return ret;
		}
		udelay(1);
	} while (!(status & CC2520_STATUS_XOSC32M_STABLE));

	dev_vdbg(&priv->spi->dev, "oscillator brought up\n");

<<<<<<< HEAD
	/* Registers default value: section 28.1 in Datasheet */
	ret = cc2520_write_register(priv, CC2520_TXPOWER, 0xF7);
	if (ret)
		goto err_ret;

=======
	/* If the CC2520 is connected to a CC2591 amplifier, we must both
	 * configure GPIOs on the CC2520 to correctly configure the CC2591
	 * and change a couple settings of the CC2520 to work with the
	 * amplifier. See section 8 page 17 of TI application note AN065.
	 * http://www.ti.com/lit/an/swra229a/swra229a.pdf
	 */
	if (priv->amplified) {
		ret = cc2520_write_register(priv, CC2520_AGCCTRL1, 0x16);
		if (ret)
			goto err_ret;

		ret = cc2520_write_register(priv, CC2520_GPIOCTRL0, 0x46);
		if (ret)
			goto err_ret;

		ret = cc2520_write_register(priv, CC2520_GPIOCTRL5, 0x47);
		if (ret)
			goto err_ret;

		ret = cc2520_write_register(priv, CC2520_GPIOPOLARITY, 0x1e);
		if (ret)
			goto err_ret;

		ret = cc2520_write_register(priv, CC2520_TXCTRL, 0xc1);
		if (ret)
			goto err_ret;
	} else {
		ret = cc2520_write_register(priv, CC2520_AGCCTRL1, 0x11);
		if (ret)
			goto err_ret;
	}

	/* Registers default value: section 28.1 in Datasheet */

	/* Set the CCA threshold to -50 dBm. This seems to have been copied
	 * from the TinyOS CC2520 driver and is much higher than the -84 dBm
	 * threshold suggested in the datasheet.
	 */
>>>>>>> v4.9.227
	ret = cc2520_write_register(priv, CC2520_CCACTRL0, 0x1A);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_MDMCTRL0, 0x85);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_MDMCTRL1, 0x14);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_RXCTRL, 0x3f);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_FSCTRL, 0x5a);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_FSCAL1, 0x2b);
	if (ret)
		goto err_ret;

<<<<<<< HEAD
	ret = cc2520_write_register(priv, CC2520_AGCCTRL1, 0x11);
	if (ret)
		goto err_ret;

=======
>>>>>>> v4.9.227
	ret = cc2520_write_register(priv, CC2520_ADCTEST0, 0x10);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_ADCTEST1, 0x0e);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_ADCTEST2, 0x03);
	if (ret)
		goto err_ret;

<<<<<<< HEAD
	ret = cc2520_write_register(priv, CC2520_FRMCTRL0, 0x60);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_FRMCTRL1, 0x03);
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_FRMFILT0, 0x00);
=======
	/* Configure registers correctly for this driver. */
	ret = cc2520_write_register(priv, CC2520_FRMCTRL1,
				    FRMCTRL1_SET_RXENMASK_ON_TX |
				    FRMCTRL1_IGNORE_TX_UNDERF);
>>>>>>> v4.9.227
	if (ret)
		goto err_ret;

	ret = cc2520_write_register(priv, CC2520_FIFOPCTRL, 127);
	if (ret)
		goto err_ret;

	return 0;

err_ret:
	return ret;
}

<<<<<<< HEAD
static struct cc2520_platform_data *
cc2520_get_platform_data(struct spi_device *spi)
{
	struct cc2520_platform_data *pdata;
	struct device_node *np = spi->dev.of_node;
	struct cc2520_private *priv = spi_get_drvdata(spi);

	if (!np)
		return spi->dev.platform_data;

	pdata = devm_kzalloc(&spi->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		goto done;

	pdata->fifo = of_get_named_gpio(np, "fifo-gpio", 0);
	priv->fifo_pin = pdata->fifo;

	pdata->fifop = of_get_named_gpio(np, "fifop-gpio", 0);

	pdata->sfd = of_get_named_gpio(np, "sfd-gpio", 0);
	pdata->cca = of_get_named_gpio(np, "cca-gpio", 0);
	pdata->vreg = of_get_named_gpio(np, "vreg-gpio", 0);
	pdata->reset = of_get_named_gpio(np, "reset-gpio", 0);

	spi->dev.platform_data = pdata;

done:
	return pdata;
}

static int cc2520_probe(struct spi_device *spi)
{
	struct cc2520_private *priv;
	struct pinctrl *pinctrl;
	struct cc2520_platform_data *pdata;
	int ret;

	priv = devm_kzalloc(&spi->dev,
			    sizeof(struct cc2520_private), GFP_KERNEL);
	if (!priv) {
		ret = -ENOMEM;
		goto err_ret;
	}

	spi_set_drvdata(spi, priv);

	pinctrl = devm_pinctrl_get_select_default(&spi->dev);
	if (IS_ERR(pinctrl))
		dev_warn(&spi->dev,
			 "pinctrl pins are not configured");

	pdata = cc2520_get_platform_data(spi);
	if (!pdata) {
=======
static int cc2520_probe(struct spi_device *spi)
{
	struct cc2520_private *priv;
	struct cc2520_platform_data pdata;
	int ret;

	priv = devm_kzalloc(&spi->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	spi_set_drvdata(spi, priv);

	ret = cc2520_get_platform_data(spi, &pdata);
	if (ret < 0) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "no platform data\n");
		return -EINVAL;
	}

	priv->spi = spi;

	priv->buf = devm_kzalloc(&spi->dev,
				 SPI_COMMAND_BUFFER, GFP_KERNEL);
<<<<<<< HEAD
	if (!priv->buf) {
		ret = -ENOMEM;
		goto err_ret;
	}
=======
	if (!priv->buf)
		return -ENOMEM;
>>>>>>> v4.9.227

	mutex_init(&priv->buffer_mutex);
	INIT_WORK(&priv->fifop_irqwork, cc2520_fifop_irqwork);
	spin_lock_init(&priv->lock);
	init_completion(&priv->tx_complete);

<<<<<<< HEAD
	/* Request all the gpio's */
	if (!gpio_is_valid(pdata->fifo)) {
=======
	/* Assumption that CC2591 is not connected */
	priv->amplified = false;

	/* Request all the gpio's */
	if (!gpio_is_valid(pdata.fifo)) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "fifo gpio is not valid\n");
		ret = -EINVAL;
		goto err_hw_init;
	}

<<<<<<< HEAD
	ret = devm_gpio_request_one(&spi->dev, pdata->fifo,
=======
	ret = devm_gpio_request_one(&spi->dev, pdata.fifo,
>>>>>>> v4.9.227
				    GPIOF_IN, "fifo");
	if (ret)
		goto err_hw_init;

<<<<<<< HEAD
	if (!gpio_is_valid(pdata->cca)) {
=======
	if (!gpio_is_valid(pdata.cca)) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "cca gpio is not valid\n");
		ret = -EINVAL;
		goto err_hw_init;
	}

<<<<<<< HEAD
	ret = devm_gpio_request_one(&spi->dev, pdata->cca,
=======
	ret = devm_gpio_request_one(&spi->dev, pdata.cca,
>>>>>>> v4.9.227
				    GPIOF_IN, "cca");
	if (ret)
		goto err_hw_init;

<<<<<<< HEAD
	if (!gpio_is_valid(pdata->fifop)) {
=======
	if (!gpio_is_valid(pdata.fifop)) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "fifop gpio is not valid\n");
		ret = -EINVAL;
		goto err_hw_init;
	}

<<<<<<< HEAD
	ret = devm_gpio_request_one(&spi->dev, pdata->fifop,
=======
	ret = devm_gpio_request_one(&spi->dev, pdata.fifop,
>>>>>>> v4.9.227
				    GPIOF_IN, "fifop");
	if (ret)
		goto err_hw_init;

<<<<<<< HEAD
	if (!gpio_is_valid(pdata->sfd)) {
=======
	if (!gpio_is_valid(pdata.sfd)) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "sfd gpio is not valid\n");
		ret = -EINVAL;
		goto err_hw_init;
	}

<<<<<<< HEAD
	ret = devm_gpio_request_one(&spi->dev, pdata->sfd,
=======
	ret = devm_gpio_request_one(&spi->dev, pdata.sfd,
>>>>>>> v4.9.227
				    GPIOF_IN, "sfd");
	if (ret)
		goto err_hw_init;

<<<<<<< HEAD
	if (!gpio_is_valid(pdata->reset)) {
=======
	if (!gpio_is_valid(pdata.reset)) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "reset gpio is not valid\n");
		ret = -EINVAL;
		goto err_hw_init;
	}

<<<<<<< HEAD
	ret = devm_gpio_request_one(&spi->dev, pdata->reset,
=======
	ret = devm_gpio_request_one(&spi->dev, pdata.reset,
>>>>>>> v4.9.227
				    GPIOF_OUT_INIT_LOW, "reset");
	if (ret)
		goto err_hw_init;

<<<<<<< HEAD
	if (!gpio_is_valid(pdata->vreg)) {
=======
	if (!gpio_is_valid(pdata.vreg)) {
>>>>>>> v4.9.227
		dev_err(&spi->dev, "vreg gpio is not valid\n");
		ret = -EINVAL;
		goto err_hw_init;
	}

<<<<<<< HEAD
	ret = devm_gpio_request_one(&spi->dev, pdata->vreg,
=======
	ret = devm_gpio_request_one(&spi->dev, pdata.vreg,
>>>>>>> v4.9.227
				    GPIOF_OUT_INIT_LOW, "vreg");
	if (ret)
		goto err_hw_init;

<<<<<<< HEAD

	gpio_set_value(pdata->vreg, HIGH);
	usleep_range(100, 150);

	gpio_set_value(pdata->reset, HIGH);
=======
	gpio_set_value(pdata.vreg, HIGH);
	usleep_range(100, 150);

	gpio_set_value(pdata.reset, HIGH);
>>>>>>> v4.9.227
	usleep_range(200, 250);

	ret = cc2520_hw_init(priv);
	if (ret)
		goto err_hw_init;

	/* Set up fifop interrupt */
	ret = devm_request_irq(&spi->dev,
<<<<<<< HEAD
			       gpio_to_irq(pdata->fifop),
=======
			       gpio_to_irq(pdata.fifop),
>>>>>>> v4.9.227
			       cc2520_fifop_isr,
			       IRQF_TRIGGER_RISING,
			       dev_name(&spi->dev),
			       priv);
	if (ret) {
		dev_err(&spi->dev, "could not get fifop irq\n");
		goto err_hw_init;
	}

	/* Set up sfd interrupt */
	ret = devm_request_irq(&spi->dev,
<<<<<<< HEAD
			       gpio_to_irq(pdata->sfd),
=======
			       gpio_to_irq(pdata.sfd),
>>>>>>> v4.9.227
			       cc2520_sfd_isr,
			       IRQF_TRIGGER_FALLING,
			       dev_name(&spi->dev),
			       priv);
	if (ret) {
		dev_err(&spi->dev, "could not get sfd irq\n");
		goto err_hw_init;
	}

	ret = cc2520_register(priv);
	if (ret)
		goto err_hw_init;

	return 0;

err_hw_init:
	mutex_destroy(&priv->buffer_mutex);
	flush_work(&priv->fifop_irqwork);
<<<<<<< HEAD

err_ret:
=======
>>>>>>> v4.9.227
	return ret;
}

static int cc2520_remove(struct spi_device *spi)
{
	struct cc2520_private *priv = spi_get_drvdata(spi);

	mutex_destroy(&priv->buffer_mutex);
	flush_work(&priv->fifop_irqwork);

<<<<<<< HEAD
	ieee802154_unregister_device(priv->dev);
	ieee802154_free_device(priv->dev);
=======
	ieee802154_unregister_hw(priv->hw);
	ieee802154_free_hw(priv->hw);
>>>>>>> v4.9.227

	return 0;
}

static const struct spi_device_id cc2520_ids[] = {
	{"cc2520", },
	{},
};
MODULE_DEVICE_TABLE(spi, cc2520_ids);

static const struct of_device_id cc2520_of_ids[] = {
	{.compatible = "ti,cc2520", },
	{},
};
MODULE_DEVICE_TABLE(of, cc2520_of_ids);

/* SPI driver structure */
static struct spi_driver cc2520_driver = {
	.driver = {
		.name = "cc2520",
<<<<<<< HEAD
		.bus = &spi_bus_type,
		.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = of_match_ptr(cc2520_of_ids),
	},
	.id_table = cc2520_ids,
	.probe = cc2520_probe,
	.remove = cc2520_remove,
};
module_spi_driver(cc2520_driver);

MODULE_AUTHOR("Varka Bhadram <varkab@cdac.in>");
MODULE_DESCRIPTION("CC2520 Transceiver Driver");
MODULE_LICENSE("GPL v2");
