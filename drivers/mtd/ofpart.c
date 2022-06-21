/*
 * Flash partitions described by the OF (or flattened) device tree
 *
 * Copyright © 2006 MontaVista Software Inc.
 * Author: Vitaly Wool <vwool@ru.mvista.com>
 *
 * Revised to handle newer style flash binding by:
 *   Copyright © 2007 David Gibson, IBM Corporation.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/mtd/mtd.h>
#include <linux/slab.h>
#include <linux/mtd/partitions.h>

static bool node_has_compatible(struct device_node *pp)
{
	return of_get_property(pp, "compatible", NULL);
}

static int parse_ofpart_partitions(struct mtd_info *master,
<<<<<<< HEAD
				   struct mtd_partition **pparts,
				   struct mtd_part_parser_data *data)
{
	struct device_node *node;
	const char *partname;
	struct device_node *pp;
	int nr_parts, i;


	if (!data)
		return 0;

	node = data->of_node;
	if (!node)
		return 0;

	/* First count the subnodes */
	nr_parts = 0;
	for_each_child_of_node(node,  pp) {
		if (node_has_compatible(pp))
=======
				   const struct mtd_partition **pparts,
				   struct mtd_part_parser_data *data)
{
	struct mtd_partition *parts;
	struct device_node *mtd_node;
	struct device_node *ofpart_node;
	const char *partname;
	struct device_node *pp;
	int nr_parts, i, ret = 0;
	bool dedicated = true;


	/* Pull of_node from the master device node */
	mtd_node = mtd_get_of_node(master);
	if (!mtd_node)
		return 0;

	ofpart_node = of_get_child_by_name(mtd_node, "partitions");
	if (!ofpart_node) {
		/*
		 * We might get here even when ofpart isn't used at all (e.g.,
		 * when using another parser), so don't be louder than
		 * KERN_DEBUG
		 */
		pr_debug("%s: 'partitions' subnode not found on %s. Trying to parse direct subnodes as partitions.\n",
			 master->name, mtd_node->full_name);
		ofpart_node = mtd_node;
		dedicated = false;
	} else if (!of_device_is_compatible(ofpart_node, "fixed-partitions")) {
		/* The 'partitions' subnode might be used by another parser */
		return 0;
	}

	/* First count the subnodes */
	nr_parts = 0;
	for_each_child_of_node(ofpart_node,  pp) {
		if (!dedicated && node_has_compatible(pp))
>>>>>>> v4.9.227
			continue;

		nr_parts++;
	}

	if (nr_parts == 0)
		return 0;

<<<<<<< HEAD
	*pparts = kzalloc(nr_parts * sizeof(**pparts), GFP_KERNEL);
	if (!*pparts)
		return -ENOMEM;

	i = 0;
	for_each_child_of_node(node,  pp) {
=======
	parts = kzalloc(nr_parts * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	i = 0;
	for_each_child_of_node(ofpart_node,  pp) {
>>>>>>> v4.9.227
		const __be32 *reg;
		int len;
		int a_cells, s_cells;

<<<<<<< HEAD
		if (node_has_compatible(pp))
=======
		if (!dedicated && node_has_compatible(pp))
>>>>>>> v4.9.227
			continue;

		reg = of_get_property(pp, "reg", &len);
		if (!reg) {
<<<<<<< HEAD
			nr_parts--;
			continue;
=======
			if (dedicated) {
				pr_debug("%s: ofpart partition %s (%s) missing reg property.\n",
					 master->name, pp->full_name,
					 mtd_node->full_name);
				goto ofpart_fail;
			} else {
				nr_parts--;
				continue;
			}
>>>>>>> v4.9.227
		}

		a_cells = of_n_addr_cells(pp);
		s_cells = of_n_size_cells(pp);
<<<<<<< HEAD
		(*pparts)[i].offset = of_read_number(reg, a_cells);
		(*pparts)[i].size = of_read_number(reg + a_cells, s_cells);
=======
		if (len / 4 != a_cells + s_cells) {
			pr_debug("%s: ofpart partition %s (%s) error parsing reg property.\n",
				 master->name, pp->full_name,
				 mtd_node->full_name);
			goto ofpart_fail;
		}

		parts[i].offset = of_read_number(reg, a_cells);
		parts[i].size = of_read_number(reg + a_cells, s_cells);
>>>>>>> v4.9.227

		partname = of_get_property(pp, "label", &len);
		if (!partname)
			partname = of_get_property(pp, "name", &len);
<<<<<<< HEAD
		(*pparts)[i].name = (char *)partname;

		if (of_get_property(pp, "read-only", &len))
			(*pparts)[i].mask_flags |= MTD_WRITEABLE;

		if (of_get_property(pp, "lock", &len))
			(*pparts)[i].mask_flags |= MTD_POWERUP_LOCK;
=======
		parts[i].name = partname;

		if (of_get_property(pp, "read-only", &len))
			parts[i].mask_flags |= MTD_WRITEABLE;

		if (of_get_property(pp, "lock", &len))
			parts[i].mask_flags |= MTD_POWERUP_LOCK;
>>>>>>> v4.9.227

		i++;
	}

<<<<<<< HEAD
	if (!i) {
		of_node_put(pp);
		pr_err("No valid partition found on %s\n", node->full_name);
		kfree(*pparts);
		*pparts = NULL;
		return -EINVAL;
	}

	return nr_parts;
}

static struct mtd_part_parser ofpart_parser = {
	.owner = THIS_MODULE,
=======
	if (!nr_parts)
		goto ofpart_none;

	*pparts = parts;
	return nr_parts;

ofpart_fail:
	pr_err("%s: error parsing ofpart partition %s (%s)\n",
	       master->name, pp->full_name, mtd_node->full_name);
	ret = -EINVAL;
ofpart_none:
	of_node_put(pp);
	kfree(parts);
	return ret;
}

static struct mtd_part_parser ofpart_parser = {
>>>>>>> v4.9.227
	.parse_fn = parse_ofpart_partitions,
	.name = "ofpart",
};

static int parse_ofoldpart_partitions(struct mtd_info *master,
<<<<<<< HEAD
				      struct mtd_partition **pparts,
				      struct mtd_part_parser_data *data)
{
=======
				      const struct mtd_partition **pparts,
				      struct mtd_part_parser_data *data)
{
	struct mtd_partition *parts;
>>>>>>> v4.9.227
	struct device_node *dp;
	int i, plen, nr_parts;
	const struct {
		__be32 offset, len;
	} *part;
	const char *names;

<<<<<<< HEAD
	if (!data)
		return 0;

	dp = data->of_node;
=======
	/* Pull of_node from the master device node */
	dp = mtd_get_of_node(master);
>>>>>>> v4.9.227
	if (!dp)
		return 0;

	part = of_get_property(dp, "partitions", &plen);
	if (!part)
		return 0; /* No partitions found */

	pr_warning("Device tree uses obsolete partition map binding: %s\n",
			dp->full_name);

	nr_parts = plen / sizeof(part[0]);

<<<<<<< HEAD
	*pparts = kzalloc(nr_parts * sizeof(*(*pparts)), GFP_KERNEL);
	if (!*pparts)
=======
	parts = kzalloc(nr_parts * sizeof(*parts), GFP_KERNEL);
	if (!parts)
>>>>>>> v4.9.227
		return -ENOMEM;

	names = of_get_property(dp, "partition-names", &plen);

	for (i = 0; i < nr_parts; i++) {
<<<<<<< HEAD
		(*pparts)[i].offset = be32_to_cpu(part->offset);
		(*pparts)[i].size   = be32_to_cpu(part->len) & ~1;
		/* bit 0 set signifies read only partition */
		if (be32_to_cpu(part->len) & 1)
			(*pparts)[i].mask_flags = MTD_WRITEABLE;
=======
		parts[i].offset = be32_to_cpu(part->offset);
		parts[i].size   = be32_to_cpu(part->len) & ~1;
		/* bit 0 set signifies read only partition */
		if (be32_to_cpu(part->len) & 1)
			parts[i].mask_flags = MTD_WRITEABLE;
>>>>>>> v4.9.227

		if (names && (plen > 0)) {
			int len = strlen(names) + 1;

<<<<<<< HEAD
			(*pparts)[i].name = (char *)names;
			plen -= len;
			names += len;
		} else {
			(*pparts)[i].name = "unnamed";
=======
			parts[i].name = names;
			plen -= len;
			names += len;
		} else {
			parts[i].name = "unnamed";
>>>>>>> v4.9.227
		}

		part++;
	}

<<<<<<< HEAD
=======
	*pparts = parts;
>>>>>>> v4.9.227
	return nr_parts;
}

static struct mtd_part_parser ofoldpart_parser = {
<<<<<<< HEAD
	.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
	.parse_fn = parse_ofoldpart_partitions,
	.name = "ofoldpart",
};

static int __init ofpart_parser_init(void)
{
	register_mtd_parser(&ofpart_parser);
	register_mtd_parser(&ofoldpart_parser);
	return 0;
}

static void __exit ofpart_parser_exit(void)
{
	deregister_mtd_parser(&ofpart_parser);
	deregister_mtd_parser(&ofoldpart_parser);
}

module_init(ofpart_parser_init);
module_exit(ofpart_parser_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Parser for MTD partitioning information in device tree");
MODULE_AUTHOR("Vitaly Wool, David Gibson");
/*
 * When MTD core cannot find the requested parser, it tries to load the module
 * with the same name. Since we provide the ofoldpart parser, we should have
 * the corresponding alias.
 */
MODULE_ALIAS("ofoldpart");
