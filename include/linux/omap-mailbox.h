/*
 * omap-mailbox: interprocessor communication module for OMAP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef OMAP_MAILBOX_H
#define OMAP_MAILBOX_H

typedef u32 mbox_msg_t;
<<<<<<< HEAD
struct omap_mbox;
=======
>>>>>>> v4.9.227

typedef int __bitwise omap_mbox_irq_t;
#define IRQ_TX ((__force omap_mbox_irq_t) 1)
#define IRQ_RX ((__force omap_mbox_irq_t) 2)

<<<<<<< HEAD
int omap_mbox_msg_send(struct omap_mbox *, mbox_msg_t msg);

struct omap_mbox *omap_mbox_get(const char *, struct notifier_block *nb);
void omap_mbox_put(struct omap_mbox *mbox, struct notifier_block *nb);

void omap_mbox_save_ctx(struct omap_mbox *mbox);
void omap_mbox_restore_ctx(struct omap_mbox *mbox);
void omap_mbox_enable_irq(struct omap_mbox *mbox, omap_mbox_irq_t irq);
void omap_mbox_disable_irq(struct omap_mbox *mbox, omap_mbox_irq_t irq);
=======
struct mbox_chan;
struct mbox_client;

struct mbox_chan *omap_mbox_request_channel(struct mbox_client *cl,
					    const char *chan_name);

void omap_mbox_enable_irq(struct mbox_chan *chan, omap_mbox_irq_t irq);
void omap_mbox_disable_irq(struct mbox_chan *chan, omap_mbox_irq_t irq);
>>>>>>> v4.9.227

#endif /* OMAP_MAILBOX_H */
