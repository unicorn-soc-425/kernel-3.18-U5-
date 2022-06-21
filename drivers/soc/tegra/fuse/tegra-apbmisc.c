/*
 * Copyright (c) 2014, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/io.h>

#include <soc/tegra/fuse.h>
<<<<<<< HEAD

#include "fuse.h"

#define APBMISC_BASE	0x70000800
#define APBMISC_SIZE	0x64
#define FUSE_SKU_INFO	0x10

static void __iomem *apbmisc_base;
static void __iomem *strapping_base;
=======
#include <soc/tegra/common.h>

#include "fuse.h"

#define FUSE_SKU_INFO	0x10

#define PMC_STRAPPING_OPT_A_RAM_CODE_SHIFT	4
#define PMC_STRAPPING_OPT_A_RAM_CODE_MASK_LONG	\
	(0xf << PMC_STRAPPING_OPT_A_RAM_CODE_SHIFT)
#define PMC_STRAPPING_OPT_A_RAM_CODE_MASK_SHORT	\
	(0x3 << PMC_STRAPPING_OPT_A_RAM_CODE_SHIFT)

static void __iomem *apbmisc_base;
static void __iomem *strapping_base;
static bool long_ram_code;
>>>>>>> v4.9.227

u32 tegra_read_chipid(void)
{
	return readl_relaxed(apbmisc_base + 4);
}

u8 tegra_get_chip_id(void)
{
	if (!apbmisc_base) {
		WARN(1, "Tegra Chip ID not yet available\n");
		return 0;
	}

	return (tegra_read_chipid() >> 8) & 0xff;
}

u32 tegra_read_straps(void)
{
	if (strapping_base)
		return readl_relaxed(strapping_base);
	else
		return 0;
}

<<<<<<< HEAD
=======
u32 tegra_read_ram_code(void)
{
	u32 straps = tegra_read_straps();

	if (long_ram_code)
		straps &= PMC_STRAPPING_OPT_A_RAM_CODE_MASK_LONG;
	else
		straps &= PMC_STRAPPING_OPT_A_RAM_CODE_MASK_SHORT;

	return straps >> PMC_STRAPPING_OPT_A_RAM_CODE_SHIFT;
}

>>>>>>> v4.9.227
static const struct of_device_id apbmisc_match[] __initconst = {
	{ .compatible = "nvidia,tegra20-apbmisc", },
	{},
};

void __init tegra_init_revision(void)
{
	u32 id, chip_id, minor_rev;
	int rev;

	id = tegra_read_chipid();
	chip_id = (id >> 8) & 0xff;
	minor_rev = (id >> 16) & 0xf;

	switch (minor_rev) {
	case 1:
		rev = TEGRA_REVISION_A01;
		break;
	case 2:
		rev = TEGRA_REVISION_A02;
		break;
	case 3:
<<<<<<< HEAD
		if (chip_id == TEGRA20 && (tegra20_spare_fuse_early(18) ||
					   tegra20_spare_fuse_early(19)))
=======
		if (chip_id == TEGRA20 && (tegra_fuse_read_spare(18) ||
					   tegra_fuse_read_spare(19)))
>>>>>>> v4.9.227
			rev = TEGRA_REVISION_A03p;
		else
			rev = TEGRA_REVISION_A03;
		break;
	case 4:
		rev = TEGRA_REVISION_A04;
		break;
	default:
		rev = TEGRA_REVISION_UNKNOWN;
	}

	tegra_sku_info.revision = rev;

<<<<<<< HEAD
	if (chip_id == TEGRA20)
		tegra_sku_info.sku_id = tegra20_fuse_early(FUSE_SKU_INFO);
	else
		tegra_sku_info.sku_id = tegra30_fuse_readl(FUSE_SKU_INFO);
=======
	tegra_sku_info.sku_id = tegra_fuse_read_early(FUSE_SKU_INFO);
>>>>>>> v4.9.227
}

void __init tegra_init_apbmisc(void)
{
<<<<<<< HEAD
	struct device_node *np;

	np = of_find_matching_node(NULL, apbmisc_match);
	apbmisc_base = of_iomap(np, 0);
	if (!apbmisc_base) {
		pr_warn("ioremap tegra apbmisc failed. using %08x instead\n",
			APBMISC_BASE);
		apbmisc_base = ioremap(APBMISC_BASE, APBMISC_SIZE);
	}

	strapping_base = of_iomap(np, 1);
	if (!strapping_base)
		pr_err("ioremap tegra strapping_base failed\n");
=======
	struct resource apbmisc, straps;
	struct device_node *np;

	np = of_find_matching_node(NULL, apbmisc_match);
	if (!np) {
		/*
		 * Fall back to legacy initialization for 32-bit ARM only. All
		 * 64-bit ARM device tree files for Tegra are required to have
		 * an APBMISC node.
		 *
		 * This is for backwards-compatibility with old device trees
		 * that didn't contain an APBMISC node.
		 */
		if (IS_ENABLED(CONFIG_ARM) && soc_is_tegra()) {
			/* APBMISC registers (chip revision, ...) */
			apbmisc.start = 0x70000800;
			apbmisc.end = 0x70000863;
			apbmisc.flags = IORESOURCE_MEM;

			/* strapping options */
			if (of_machine_is_compatible("nvidia,tegra124")) {
				straps.start = 0x7000e864;
				straps.end = 0x7000e867;
			} else {
				straps.start = 0x70000008;
				straps.end = 0x7000000b;
			}

			straps.flags = IORESOURCE_MEM;

			pr_warn("Using APBMISC region %pR\n", &apbmisc);
			pr_warn("Using strapping options registers %pR\n",
				&straps);
		} else {
			/*
			 * At this point we're not running on Tegra, so play
			 * nice with multi-platform kernels.
			 */
			return;
		}
	} else {
		/*
		 * Extract information from the device tree if we've found a
		 * matching node.
		 */
		if (of_address_to_resource(np, 0, &apbmisc) < 0) {
			pr_err("failed to get APBMISC registers\n");
			return;
		}

		if (of_address_to_resource(np, 1, &straps) < 0) {
			pr_err("failed to get strapping options registers\n");
			return;
		}
	}

	apbmisc_base = ioremap_nocache(apbmisc.start, resource_size(&apbmisc));
	if (!apbmisc_base)
		pr_err("failed to map APBMISC registers\n");

	strapping_base = ioremap_nocache(straps.start, resource_size(&straps));
	if (!strapping_base)
		pr_err("failed to map strapping options registers\n");

	long_ram_code = of_property_read_bool(np, "nvidia,long-ram-code");
>>>>>>> v4.9.227
}
