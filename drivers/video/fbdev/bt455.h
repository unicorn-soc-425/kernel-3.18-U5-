/*
 *	linux/drivers/video/bt455.h
 *
 *	Copyright 2003  Thiemo Seufer <seufer@csv.ica.uni-stuttgart.de>
<<<<<<< HEAD
=======
 *	Copyright 2016  Maciej W. Rozycki <macro@linux-mips.org>
>>>>>>> v4.9.227
 *
 *	This file is subject to the terms and conditions of the GNU General
 *	Public License. See the file COPYING in the main directory of this
 *	archive for more details.
 */
#include <linux/types.h>

/*
 * Bt455 byte-wide registers, 32-bit aligned.
 */
struct bt455_regs {
	volatile u8 addr_cmap;
	u8 pad0[3];
	volatile u8 addr_cmap_data;
	u8 pad1[3];
	volatile u8 addr_clr;
	u8 pad2[3];
	volatile u8 addr_ovly;
	u8 pad3[3];
};

static inline void bt455_select_reg(struct bt455_regs *regs, int ir)
{
	mb();
	regs->addr_cmap = ir & 0x0f;
}

<<<<<<< HEAD
/*
 * Read/write to a Bt455 color map register.
 */
static inline void bt455_read_cmap_entry(struct bt455_regs *regs, int cr,
					 u8* red, u8* green, u8* blue)
{
	bt455_select_reg(regs, cr);
	mb();
	*red = regs->addr_cmap_data & 0x0f;
	rmb();
	*green = regs->addr_cmap_data & 0x0f;
	rmb();
	*blue = regs->addr_cmap_data & 0x0f;
}

static inline void bt455_write_cmap_entry(struct bt455_regs *regs, int cr,
					  u8 red, u8 green, u8 blue)
{
	bt455_select_reg(regs, cr);
	wmb();
	regs->addr_cmap_data = red & 0x0f;
	wmb();
	regs->addr_cmap_data = green & 0x0f;
	wmb();
	regs->addr_cmap_data = blue & 0x0f;
}

static inline void bt455_write_ovly_entry(struct bt455_regs *regs, int cr,
					  u8 red, u8 green, u8 blue)
{
	bt455_select_reg(regs, cr);
	wmb();
	regs->addr_ovly = red & 0x0f;
	wmb();
	regs->addr_ovly = green & 0x0f;
	wmb();
	regs->addr_ovly = blue & 0x0f;
}

static inline void bt455_set_cursor(struct bt455_regs *regs)
{
	mb();
	regs->addr_ovly = 0x0f;
	wmb();
	regs->addr_ovly = 0x0f;
	wmb();
	regs->addr_ovly = 0x0f;
}

static inline void bt455_erase_cursor(struct bt455_regs *regs)
{
	/* bt455_write_cmap_entry(regs, 8, 0x00, 0x00, 0x00); */
	/* bt455_write_cmap_entry(regs, 9, 0x00, 0x00, 0x00); */
	bt455_write_ovly_entry(regs, 8, 0x03, 0x03, 0x03);
	bt455_write_ovly_entry(regs, 9, 0x07, 0x07, 0x07);

	wmb();
	regs->addr_ovly = 0x09;
	wmb();
	regs->addr_ovly = 0x09;
	wmb();
	regs->addr_ovly = 0x09;
=======
static inline void bt455_reset_reg(struct bt455_regs *regs)
{
	mb();
	regs->addr_clr = 0;
}

/*
 * Read/write to a Bt455 color map register.
 */
static inline void bt455_read_cmap_next(struct bt455_regs *regs, u8 *grey)
{
	mb();
	regs->addr_cmap_data;
	rmb();
	*grey = regs->addr_cmap_data & 0xf;
	rmb();
	regs->addr_cmap_data;
}

static inline void bt455_write_cmap_next(struct bt455_regs *regs, u8 grey)
{
	wmb();
	regs->addr_cmap_data = 0x0;
	wmb();
	regs->addr_cmap_data = grey & 0xf;
	wmb();
	regs->addr_cmap_data = 0x0;
}

static inline void bt455_write_ovly_next(struct bt455_regs *regs, u8 grey)
{
	wmb();
	regs->addr_ovly = 0x0;
	wmb();
	regs->addr_ovly = grey & 0xf;
	wmb();
	regs->addr_ovly = 0x0;
}

static inline void bt455_read_cmap_entry(struct bt455_regs *regs,
					 int cr, u8 *grey)
{
	bt455_select_reg(regs, cr);
	bt455_read_cmap_next(regs, grey);
}

static inline void bt455_write_cmap_entry(struct bt455_regs *regs,
					  int cr, u8 grey)
{
	bt455_select_reg(regs, cr);
	bt455_write_cmap_next(regs, grey);
}

static inline void bt455_write_ovly_entry(struct bt455_regs *regs, u8 grey)
{
	bt455_reset_reg(regs);
	bt455_write_ovly_next(regs, grey);
>>>>>>> v4.9.227
}
