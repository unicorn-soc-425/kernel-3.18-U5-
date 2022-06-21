/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * SGI UV IRQ functions
 *
 * Copyright (C) 2008 Silicon Graphics, Inc. All rights reserved.
 */

<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/export.h>
>>>>>>> v4.9.227
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/irq.h>

<<<<<<< HEAD
=======
#include <asm/irqdomain.h>
>>>>>>> v4.9.227
#include <asm/apic.h>
#include <asm/uv/uv_irq.h>
#include <asm/uv/uv_hub.h>

/* MMR offset and pnode of hub sourcing interrupts for a given irq */
<<<<<<< HEAD
struct uv_irq_2_mmr_pnode{
	struct rb_node		list;
	unsigned long		offset;
	int			pnode;
	int			irq;
};

static DEFINE_SPINLOCK(uv_irq_lock);
static struct rb_root		uv_irq_root;

static int uv_set_irq_affinity(struct irq_data *, const struct cpumask *, bool);
=======
struct uv_irq_2_mmr_pnode {
	unsigned long		offset;
	int			pnode;
};

static void uv_program_mmr(struct irq_cfg *cfg, struct uv_irq_2_mmr_pnode *info)
{
	unsigned long mmr_value;
	struct uv_IO_APIC_route_entry *entry;

	BUILD_BUG_ON(sizeof(struct uv_IO_APIC_route_entry) !=
		     sizeof(unsigned long));

	mmr_value = 0;
	entry = (struct uv_IO_APIC_route_entry *)&mmr_value;
	entry->vector		= cfg->vector;
	entry->delivery_mode	= apic->irq_delivery_mode;
	entry->dest_mode	= apic->irq_dest_mode;
	entry->polarity		= 0;
	entry->trigger		= 0;
	entry->mask		= 0;
	entry->dest		= cfg->dest_apicid;

	uv_write_global_mmr64(info->pnode, info->offset, mmr_value);
}
>>>>>>> v4.9.227

static void uv_noop(struct irq_data *data) { }

static void uv_ack_apic(struct irq_data *data)
{
	ack_APIC_irq();
}

<<<<<<< HEAD
=======
static int
uv_set_irq_affinity(struct irq_data *data, const struct cpumask *mask,
		    bool force)
{
	struct irq_data *parent = data->parent_data;
	struct irq_cfg *cfg = irqd_cfg(data);
	int ret;

	ret = parent->chip->irq_set_affinity(parent, mask, force);
	if (ret >= 0) {
		uv_program_mmr(cfg, data->chip_data);
		send_cleanup_vector(cfg);
	}

	return ret;
}

>>>>>>> v4.9.227
static struct irq_chip uv_irq_chip = {
	.name			= "UV-CORE",
	.irq_mask		= uv_noop,
	.irq_unmask		= uv_noop,
	.irq_eoi		= uv_ack_apic,
	.irq_set_affinity	= uv_set_irq_affinity,
};

<<<<<<< HEAD
/*
 * Add offset and pnode information of the hub sourcing interrupts to the
 * rb tree for a specific irq.
 */
static int uv_set_irq_2_mmr_info(int irq, unsigned long offset, unsigned blade)
{
	struct rb_node **link = &uv_irq_root.rb_node;
	struct rb_node *parent = NULL;
	struct uv_irq_2_mmr_pnode *n;
	struct uv_irq_2_mmr_pnode *e;
	unsigned long irqflags;

	n = kmalloc_node(sizeof(struct uv_irq_2_mmr_pnode), GFP_KERNEL,
				uv_blade_to_memory_nid(blade));
	if (!n)
		return -ENOMEM;

	n->irq = irq;
	n->offset = offset;
	n->pnode = uv_blade_to_pnode(blade);
	spin_lock_irqsave(&uv_irq_lock, irqflags);
	/* Find the right place in the rbtree: */
	while (*link) {
		parent = *link;
		e = rb_entry(parent, struct uv_irq_2_mmr_pnode, list);

		if (unlikely(irq == e->irq)) {
			/* irq entry exists */
			e->pnode = uv_blade_to_pnode(blade);
			e->offset = offset;
			spin_unlock_irqrestore(&uv_irq_lock, irqflags);
			kfree(n);
			return 0;
		}

		if (irq < e->irq)
			link = &(*link)->rb_left;
		else
			link = &(*link)->rb_right;
	}

	/* Insert the node into the rbtree. */
	rb_link_node(&n->list, parent, link);
	rb_insert_color(&n->list, &uv_irq_root);

	spin_unlock_irqrestore(&uv_irq_lock, irqflags);
	return 0;
}

/* Retrieve offset and pnode information from the rb tree for a specific irq */
int uv_irq_2_mmr_info(int irq, unsigned long *offset, int *pnode)
{
	struct uv_irq_2_mmr_pnode *e;
	struct rb_node *n;
	unsigned long irqflags;

	spin_lock_irqsave(&uv_irq_lock, irqflags);
	n = uv_irq_root.rb_node;
	while (n) {
		e = rb_entry(n, struct uv_irq_2_mmr_pnode, list);

		if (e->irq == irq) {
			*offset = e->offset;
			*pnode = e->pnode;
			spin_unlock_irqrestore(&uv_irq_lock, irqflags);
			return 0;
		}

		if (irq < e->irq)
			n = n->rb_left;
		else
			n = n->rb_right;
	}
	spin_unlock_irqrestore(&uv_irq_lock, irqflags);
	return -1;
=======
static int uv_domain_alloc(struct irq_domain *domain, unsigned int virq,
			   unsigned int nr_irqs, void *arg)
{
	struct uv_irq_2_mmr_pnode *chip_data;
	struct irq_alloc_info *info = arg;
	struct irq_data *irq_data = irq_domain_get_irq_data(domain, virq);
	int ret;

	if (nr_irqs > 1 || !info || info->type != X86_IRQ_ALLOC_TYPE_UV)
		return -EINVAL;

	chip_data = kmalloc_node(sizeof(*chip_data), GFP_KERNEL,
				 irq_data_get_node(irq_data));
	if (!chip_data)
		return -ENOMEM;

	ret = irq_domain_alloc_irqs_parent(domain, virq, nr_irqs, arg);
	if (ret >= 0) {
		if (info->uv_limit == UV_AFFINITY_CPU)
			irq_set_status_flags(virq, IRQ_NO_BALANCING);
		else
			irq_set_status_flags(virq, IRQ_MOVE_PCNTXT);

		chip_data->pnode = uv_blade_to_pnode(info->uv_blade);
		chip_data->offset = info->uv_offset;
		irq_domain_set_info(domain, virq, virq, &uv_irq_chip, chip_data,
				    handle_percpu_irq, NULL, info->uv_name);
	} else {
		kfree(chip_data);
	}

	return ret;
}

static void uv_domain_free(struct irq_domain *domain, unsigned int virq,
			   unsigned int nr_irqs)
{
	struct irq_data *irq_data = irq_domain_get_irq_data(domain, virq);

	BUG_ON(nr_irqs != 1);
	kfree(irq_data->chip_data);
	irq_clear_status_flags(virq, IRQ_MOVE_PCNTXT);
	irq_clear_status_flags(virq, IRQ_NO_BALANCING);
	irq_domain_free_irqs_top(domain, virq, nr_irqs);
>>>>>>> v4.9.227
}

/*
 * Re-target the irq to the specified CPU and enable the specified MMR located
 * on the specified blade to allow the sending of MSIs to the specified CPU.
 */
<<<<<<< HEAD
static int
arch_enable_uv_irq(char *irq_name, unsigned int irq, int cpu, int mmr_blade,
		       unsigned long mmr_offset, int limit)
{
	const struct cpumask *eligible_cpu = cpumask_of(cpu);
	struct irq_cfg *cfg = irq_get_chip_data(irq);
	unsigned long mmr_value;
	struct uv_IO_APIC_route_entry *entry;
	int mmr_pnode, err;
	unsigned int dest;

	BUILD_BUG_ON(sizeof(struct uv_IO_APIC_route_entry) !=
			sizeof(unsigned long));

	err = assign_irq_vector(irq, cfg, eligible_cpu);
	if (err != 0)
		return err;

	err = apic->cpu_mask_to_apicid_and(eligible_cpu, eligible_cpu, &dest);
	if (err != 0)
		return err;

	if (limit == UV_AFFINITY_CPU)
		irq_set_status_flags(irq, IRQ_NO_BALANCING);
	else
		irq_set_status_flags(irq, IRQ_MOVE_PCNTXT);

	irq_set_chip_and_handler_name(irq, &uv_irq_chip, handle_percpu_irq,
				      irq_name);

	mmr_value = 0;
	entry = (struct uv_IO_APIC_route_entry *)&mmr_value;
	entry->vector		= cfg->vector;
	entry->delivery_mode	= apic->irq_delivery_mode;
	entry->dest_mode	= apic->irq_dest_mode;
	entry->polarity		= 0;
	entry->trigger		= 0;
	entry->mask		= 0;
	entry->dest		= dest;

	mmr_pnode = uv_blade_to_pnode(mmr_blade);
	uv_write_global_mmr64(mmr_pnode, mmr_offset, mmr_value);

	if (cfg->move_in_progress)
		send_cleanup_vector(cfg);

	return irq;
=======
static void uv_domain_activate(struct irq_domain *domain,
			       struct irq_data *irq_data)
{
	uv_program_mmr(irqd_cfg(irq_data), irq_data->chip_data);
>>>>>>> v4.9.227
}

/*
 * Disable the specified MMR located on the specified blade so that MSIs are
 * longer allowed to be sent.
 */
<<<<<<< HEAD
static void arch_disable_uv_irq(int mmr_pnode, unsigned long mmr_offset)
=======
static void uv_domain_deactivate(struct irq_domain *domain,
				 struct irq_data *irq_data)
>>>>>>> v4.9.227
{
	unsigned long mmr_value;
	struct uv_IO_APIC_route_entry *entry;

<<<<<<< HEAD
	BUILD_BUG_ON(sizeof(struct uv_IO_APIC_route_entry) !=
			sizeof(unsigned long));

	mmr_value = 0;
	entry = (struct uv_IO_APIC_route_entry *)&mmr_value;
	entry->mask = 1;

	uv_write_global_mmr64(mmr_pnode, mmr_offset, mmr_value);
}

static int
uv_set_irq_affinity(struct irq_data *data, const struct cpumask *mask,
		    bool force)
{
	struct irq_cfg *cfg = data->chip_data;
	unsigned int dest;
	unsigned long mmr_value, mmr_offset;
	struct uv_IO_APIC_route_entry *entry;
	int mmr_pnode;

	if (__ioapic_set_affinity(data, mask, &dest))
		return -1;

	mmr_value = 0;
	entry = (struct uv_IO_APIC_route_entry *)&mmr_value;

	entry->vector		= cfg->vector;
	entry->delivery_mode	= apic->irq_delivery_mode;
	entry->dest_mode	= apic->irq_dest_mode;
	entry->polarity		= 0;
	entry->trigger		= 0;
	entry->mask		= 0;
	entry->dest		= dest;

	/* Get previously stored MMR and pnode of hub sourcing interrupts */
	if (uv_irq_2_mmr_info(data->irq, &mmr_offset, &mmr_pnode))
		return -1;

	uv_write_global_mmr64(mmr_pnode, mmr_offset, mmr_value);

	if (cfg->move_in_progress)
		send_cleanup_vector(cfg);

	return IRQ_SET_MASK_OK_NOCOPY;
=======
	mmr_value = 0;
	entry = (struct uv_IO_APIC_route_entry *)&mmr_value;
	entry->mask = 1;
	uv_program_mmr(irqd_cfg(irq_data), irq_data->chip_data);
}

static const struct irq_domain_ops uv_domain_ops = {
	.alloc		= uv_domain_alloc,
	.free		= uv_domain_free,
	.activate	= uv_domain_activate,
	.deactivate	= uv_domain_deactivate,
};

static struct irq_domain *uv_get_irq_domain(void)
{
	static struct irq_domain *uv_domain;
	static DEFINE_MUTEX(uv_lock);

	mutex_lock(&uv_lock);
	if (uv_domain == NULL) {
		uv_domain = irq_domain_add_tree(NULL, &uv_domain_ops, NULL);
		if (uv_domain)
			uv_domain->parent = x86_vector_domain;
	}
	mutex_unlock(&uv_lock);

	return uv_domain;
>>>>>>> v4.9.227
}

/*
 * Set up a mapping of an available irq and vector, and enable the specified
 * MMR that defines the MSI that is to be sent to the specified CPU when an
 * interrupt is raised.
 */
int uv_setup_irq(char *irq_name, int cpu, int mmr_blade,
		 unsigned long mmr_offset, int limit)
{
<<<<<<< HEAD
	int ret, irq = irq_alloc_hwirq(uv_blade_to_memory_nid(mmr_blade));

	if (!irq)
		return -EBUSY;

	ret = arch_enable_uv_irq(irq_name, irq, cpu, mmr_blade, mmr_offset,
		limit);
	if (ret == irq)
		uv_set_irq_2_mmr_info(irq, mmr_offset, mmr_blade);
	else
		irq_free_hwirq(irq);

	return ret;
=======
	struct irq_alloc_info info;
	struct irq_domain *domain = uv_get_irq_domain();

	if (!domain)
		return -ENOMEM;

	init_irq_alloc_info(&info, cpumask_of(cpu));
	info.type = X86_IRQ_ALLOC_TYPE_UV;
	info.uv_limit = limit;
	info.uv_blade = mmr_blade;
	info.uv_offset = mmr_offset;
	info.uv_name = irq_name;

	return irq_domain_alloc_irqs(domain, 1,
				     uv_blade_to_memory_nid(mmr_blade), &info);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(uv_setup_irq);

/*
 * Tear down a mapping of an irq and vector, and disable the specified MMR that
 * defined the MSI that was to be sent to the specified CPU when an interrupt
 * was raised.
 *
 * Set mmr_blade and mmr_offset to what was passed in on uv_setup_irq().
 */
void uv_teardown_irq(unsigned int irq)
{
<<<<<<< HEAD
	struct uv_irq_2_mmr_pnode *e;
	struct rb_node *n;
	unsigned long irqflags;

	spin_lock_irqsave(&uv_irq_lock, irqflags);
	n = uv_irq_root.rb_node;
	while (n) {
		e = rb_entry(n, struct uv_irq_2_mmr_pnode, list);
		if (e->irq == irq) {
			arch_disable_uv_irq(e->pnode, e->offset);
			rb_erase(n, &uv_irq_root);
			kfree(e);
			break;
		}
		if (irq < e->irq)
			n = n->rb_left;
		else
			n = n->rb_right;
	}
	spin_unlock_irqrestore(&uv_irq_lock, irqflags);
	irq_free_hwirq(irq);
=======
	irq_domain_free_irqs(irq, 1);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(uv_teardown_irq);
