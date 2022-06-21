/*
 * Copyright (c) 2012 GCT Semiconductor, Inc. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

<<<<<<< HEAD
#include <linux/slab.h>
=======
#include <linux/kernel.h>
>>>>>>> v4.9.227
#include "gdm_endian.h"

void gdm_set_endian(struct gdm_endian *ed, u8 dev_endian)
{
<<<<<<< HEAD
	u8 a[2] = {0x12, 0x34};
	u8 b[2] = {0, };
	u16 c = 0x1234;

=======
>>>>>>> v4.9.227
	if (dev_endian == ENDIANNESS_BIG)
		ed->dev_ed = ENDIANNESS_BIG;
	else
		ed->dev_ed = ENDIANNESS_LITTLE;
<<<<<<< HEAD

	memcpy(b, &c, 2);

	if (a[0] != b[0])
		ed->host_ed = ENDIANNESS_LITTLE;
	else
		ed->host_ed = ENDIANNESS_BIG;

=======
>>>>>>> v4.9.227
}

u16 gdm_cpu_to_dev16(struct gdm_endian *ed, u16 x)
{
<<<<<<< HEAD
	if (ed->dev_ed == ed->host_ed)
		return x;

	return Endian16_Swap(x);
=======
	if (ed->dev_ed == ENDIANNESS_LITTLE)
		return cpu_to_le16(x);
	else
		return cpu_to_be16(x);
>>>>>>> v4.9.227
}

u16 gdm_dev16_to_cpu(struct gdm_endian *ed, u16 x)
{
<<<<<<< HEAD
	if (ed->dev_ed == ed->host_ed)
		return x;

	return Endian16_Swap(x);
=======
	if (ed->dev_ed == ENDIANNESS_LITTLE)
		return le16_to_cpu(x);
	else
		return be16_to_cpu(x);
>>>>>>> v4.9.227
}

u32 gdm_cpu_to_dev32(struct gdm_endian *ed, u32 x)
{
<<<<<<< HEAD
	if (ed->dev_ed == ed->host_ed)
		return x;

	return Endian32_Swap(x);
=======
	if (ed->dev_ed == ENDIANNESS_LITTLE)
		return cpu_to_le32(x);
	else
		return cpu_to_be32(x);
>>>>>>> v4.9.227
}

u32 gdm_dev32_to_cpu(struct gdm_endian *ed, u32 x)
{
<<<<<<< HEAD
	if (ed->dev_ed == ed->host_ed)
		return x;

	return Endian32_Swap(x);
=======
	if (ed->dev_ed == ENDIANNESS_LITTLE)
		return le32_to_cpu(x);
	else
		return be32_to_cpu(x);
>>>>>>> v4.9.227
}
