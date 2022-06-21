/*
 * Renesas R-Car Gen1 SRU/SSI support
 *
 * Copyright (C) 2013 Renesas Solutions Corp.
 * Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
<<<<<<< HEAD
#include "rsnd.h"

struct rsnd_gen {
	void __iomem *base[RSND_BASE_MAX];

	struct rsnd_gen_ops *ops;

	struct regmap *regmap[RSND_BASE_MAX];
	struct regmap_field *regs[RSND_REG_MAX];
};

#define rsnd_priv_to_gen(p)	((struct rsnd_gen *)(p)->gen)
=======

/*
 * #define DEBUG
 *
 * you can also add below in
 * ${LINUX}/drivers/base/regmap/regmap.c
 * for regmap debug
 *
 * #define LOG_DEVICE "xxxx.rcar_sound"
 */

#include "rsnd.h"

struct rsnd_gen {
	struct rsnd_gen_ops *ops;

	/* RSND_BASE_MAX base */
	void __iomem *base[RSND_BASE_MAX];
	phys_addr_t res[RSND_BASE_MAX];
	struct regmap *regmap[RSND_BASE_MAX];

	/* RSND_REG_MAX base */
	struct regmap_field *regs[RSND_REG_MAX];
	const char *reg_name[RSND_REG_MAX];
};

#define rsnd_priv_to_gen(p)	((struct rsnd_gen *)(p)->gen)
#define rsnd_reg_name(gen, id)	((gen)->reg_name[id])
>>>>>>> v4.9.227

struct rsnd_regmap_field_conf {
	int idx;
	unsigned int reg_offset;
	unsigned int id_offset;
<<<<<<< HEAD
};

#define RSND_REG_SET(id, offset, _id_offset)	\
=======
	const char *reg_name;
};

#define RSND_REG_SET(id, offset, _id_offset, n)	\
>>>>>>> v4.9.227
{						\
	.idx = id,				\
	.reg_offset = offset,			\
	.id_offset = _id_offset,		\
<<<<<<< HEAD
}
/* single address mapping */
#define RSND_GEN_S_REG(id, offset)	\
	RSND_REG_SET(RSND_REG_##id, offset, 0)

/* multi address mapping */
#define RSND_GEN_M_REG(id, offset, _id_offset)	\
	RSND_REG_SET(RSND_REG_##id, offset, _id_offset)
=======
	.reg_name = n,				\
}
/* single address mapping */
#define RSND_GEN_S_REG(id, offset)	\
	RSND_REG_SET(RSND_REG_##id, offset, 0, #id)

/* multi address mapping */
#define RSND_GEN_M_REG(id, offset, _id_offset)	\
	RSND_REG_SET(RSND_REG_##id, offset, _id_offset, #id)
>>>>>>> v4.9.227

/*
 *		basic function
 */
static int rsnd_is_accessible_reg(struct rsnd_priv *priv,
				  struct rsnd_gen *gen, enum rsnd_reg reg)
{
	if (!gen->regs[reg]) {
		struct device *dev = rsnd_priv_to_dev(priv);

		dev_err(dev, "unsupported register access %x\n", reg);
		return 0;
	}

	return 1;
}

u32 rsnd_read(struct rsnd_priv *priv,
	      struct rsnd_mod *mod, enum rsnd_reg reg)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_gen *gen = rsnd_priv_to_gen(priv);
	u32 val;

	if (!rsnd_is_accessible_reg(priv, gen, reg))
		return 0;

	regmap_fields_read(gen->regs[reg], rsnd_mod_id(mod), &val);

<<<<<<< HEAD
	dev_dbg(dev, "r %s - 0x%04d : %08x\n", rsnd_mod_name(mod), reg, val);
=======
	dev_dbg(dev, "r %s[%d] - %-18s (%4d) : %08x\n",
		rsnd_mod_name(mod), rsnd_mod_id(mod),
		rsnd_reg_name(gen, reg), reg, val);
>>>>>>> v4.9.227

	return val;
}

void rsnd_write(struct rsnd_priv *priv,
		struct rsnd_mod *mod,
		enum rsnd_reg reg, u32 data)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_gen *gen = rsnd_priv_to_gen(priv);

	if (!rsnd_is_accessible_reg(priv, gen, reg))
		return;

<<<<<<< HEAD
	regmap_fields_write(gen->regs[reg], rsnd_mod_id(mod), data);

	dev_dbg(dev, "w %s - 0x%04d : %08x\n", rsnd_mod_name(mod), reg, data);
=======
	regmap_fields_force_write(gen->regs[reg], rsnd_mod_id(mod), data);

	dev_dbg(dev, "w %s[%d] - %-18s (%4d) : %08x\n",
		rsnd_mod_name(mod), rsnd_mod_id(mod),
		rsnd_reg_name(gen, reg), reg, data);
>>>>>>> v4.9.227
}

void rsnd_bset(struct rsnd_priv *priv, struct rsnd_mod *mod,
	       enum rsnd_reg reg, u32 mask, u32 data)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_gen *gen = rsnd_priv_to_gen(priv);

	if (!rsnd_is_accessible_reg(priv, gen, reg))
		return;

<<<<<<< HEAD
	regmap_fields_update_bits(gen->regs[reg], rsnd_mod_id(mod),
				  mask, data);

	dev_dbg(dev, "b %s - 0x%04d : %08x/%08x\n",
		rsnd_mod_name(mod), reg, data, mask);
}

#define rsnd_gen_regmap_init(priv, id_size, reg_id, conf)		\
	_rsnd_gen_regmap_init(priv, id_size, reg_id, conf, ARRAY_SIZE(conf))
static int _rsnd_gen_regmap_init(struct rsnd_priv *priv,
				 int id_size,
				 int reg_id,
				 struct rsnd_regmap_field_conf *conf,
=======
	regmap_fields_force_update_bits(gen->regs[reg],
					rsnd_mod_id(mod), mask, data);

	dev_dbg(dev, "b %s[%d] - %-18s (%4d) : %08x/%08x\n",
		rsnd_mod_name(mod), rsnd_mod_id(mod),
		rsnd_reg_name(gen, reg), reg, data, mask);

}

phys_addr_t rsnd_gen_get_phy_addr(struct rsnd_priv *priv, int reg_id)
{
	struct rsnd_gen *gen = rsnd_priv_to_gen(priv);

	return	gen->res[reg_id];
}

#define rsnd_gen_regmap_init(priv, id_size, reg_id, name, conf)		\
	_rsnd_gen_regmap_init(priv, id_size, reg_id, name, conf, ARRAY_SIZE(conf))
static int _rsnd_gen_regmap_init(struct rsnd_priv *priv,
				 int id_size,
				 int reg_id,
				 const char *name,
				 const struct rsnd_regmap_field_conf *conf,
>>>>>>> v4.9.227
				 int conf_size)
{
	struct platform_device *pdev = rsnd_priv_to_pdev(priv);
	struct rsnd_gen *gen = rsnd_priv_to_gen(priv);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct resource *res;
	struct regmap_config regc;
	struct regmap_field *regs;
	struct regmap *regmap;
	struct reg_field regf;
	void __iomem *base;
	int i;

	memset(&regc, 0, sizeof(regc));
	regc.reg_bits = 32;
	regc.val_bits = 32;
	regc.reg_stride = 4;
<<<<<<< HEAD

	res = platform_get_resource(pdev, IORESOURCE_MEM, reg_id);
=======
	regc.name = name;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, name);
	if (!res)
		res = platform_get_resource(pdev, IORESOURCE_MEM, reg_id);
>>>>>>> v4.9.227
	if (!res)
		return -ENODEV;

	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);

	regmap = devm_regmap_init_mmio(dev, base, &regc);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

<<<<<<< HEAD
	gen->base[reg_id] = base;
	gen->regmap[reg_id] = regmap;
=======
	/* RSND_BASE_MAX base */
	gen->base[reg_id] = base;
	gen->regmap[reg_id] = regmap;
	gen->res[reg_id] = res->start;
>>>>>>> v4.9.227

	for (i = 0; i < conf_size; i++) {

		regf.reg	= conf[i].reg_offset;
		regf.id_offset	= conf[i].id_offset;
		regf.lsb	= 0;
		regf.msb	= 31;
		regf.id_size	= id_size;

		regs = devm_regmap_field_alloc(dev, regmap, regf);
		if (IS_ERR(regs))
			return PTR_ERR(regs);

<<<<<<< HEAD
		gen->regs[conf[i].idx] = regs;
=======
		/* RSND_REG_MAX base */
		gen->regs[conf[i].idx] = regs;
		gen->reg_name[conf[i].idx] = conf[i].reg_name;
>>>>>>> v4.9.227
	}

	return 0;
}

/*
<<<<<<< HEAD
 *	DMA read/write register offset
 *
 *	RSND_xxx_I_N	for Audio DMAC input
 *	RSND_xxx_O_N	for Audio DMAC output
 *	RSND_xxx_I_P	for Audio DMAC peri peri input
 *	RSND_xxx_O_P	for Audio DMAC peri peri output
 *
 *	ex) R-Car H2 case
 *	      mod        / DMAC in    / DMAC out   / DMAC PP in / DMAC pp out
 *	SSI : 0xec541000 / 0xec241008 / 0xec24100c
 *	SSIU: 0xec541000 / 0xec100000 / 0xec100000 / 0xec400000 / 0xec400000
 *	SCU : 0xec500000 / 0xec000000 / 0xec004000 / 0xec300000 / 0xec304000
 *	CMD : 0xec500000 /            / 0xec008000                0xec308000
 */
#define RDMA_SSI_I_N(addr, i)	(addr ##_reg - 0x00300000 + (0x40 * i) + 0x8)
#define RDMA_SSI_O_N(addr, i)	(addr ##_reg - 0x00300000 + (0x40 * i) + 0xc)

#define RDMA_SSIU_I_N(addr, i)	(addr ##_reg - 0x00441000 + (0x1000 * i))
#define RDMA_SSIU_O_N(addr, i)	(addr ##_reg - 0x00441000 + (0x1000 * i))

#define RDMA_SSIU_I_P(addr, i)	(addr ##_reg - 0x00141000 + (0x1000 * i))
#define RDMA_SSIU_O_P(addr, i)	(addr ##_reg - 0x00141000 + (0x1000 * i))

#define RDMA_SRC_I_N(addr, i)	(addr ##_reg - 0x00500000 + (0x400 * i))
#define RDMA_SRC_O_N(addr, i)	(addr ##_reg - 0x004fc000 + (0x400 * i))

#define RDMA_SRC_I_P(addr, i)	(addr ##_reg - 0x00200000 + (0x400 * i))
#define RDMA_SRC_O_P(addr, i)	(addr ##_reg - 0x001fc000 + (0x400 * i))

#define RDMA_CMD_O_N(addr, i)	(addr ##_reg - 0x004f8000 + (0x400 * i))
#define RDMA_CMD_O_P(addr, i)	(addr ##_reg - 0x001f8000 + (0x400 * i))

static dma_addr_t
rsnd_gen2_dma_addr(struct rsnd_priv *priv,
		   struct rsnd_mod *mod,
		   int is_play, int is_from)
{
	struct platform_device *pdev = rsnd_priv_to_pdev(priv);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	dma_addr_t ssi_reg = platform_get_resource(pdev,
				IORESOURCE_MEM, RSND_GEN2_SSI)->start;
	dma_addr_t src_reg = platform_get_resource(pdev,
				IORESOURCE_MEM, RSND_GEN2_SCU)->start;
	int is_ssi = !!(rsnd_io_to_mod_ssi(io) == mod);
	int use_src = !!rsnd_io_to_mod_src(io);
	int use_dvc = !!rsnd_io_to_mod_dvc(io);
	int id = rsnd_mod_id(mod);
	struct dma_addr {
		dma_addr_t out_addr;
		dma_addr_t in_addr;
	} dma_addrs[3][2][3] = {
		/* SRC */
		{{{ 0,				0 },
		/* Capture */
		  { RDMA_SRC_O_N(src, id),	RDMA_SRC_I_P(src, id) },
		  { RDMA_CMD_O_N(src, id),	RDMA_SRC_I_P(src, id) } },
		 /* Playback */
		 {{ 0,				0, },
		  { RDMA_SRC_O_P(src, id),	RDMA_SRC_I_N(src, id) },
		  { RDMA_CMD_O_P(src, id),	RDMA_SRC_I_N(src, id) } }
		},
		/* SSI */
		/* Capture */
		{{{ RDMA_SSI_O_N(ssi, id),	0 },
		  { RDMA_SSIU_O_P(ssi, id),	0 },
		  { RDMA_SSIU_O_P(ssi, id),	0 } },
		 /* Playback */
		 {{ 0,				RDMA_SSI_I_N(ssi, id) },
		  { 0,				RDMA_SSIU_I_P(ssi, id) },
		  { 0,				RDMA_SSIU_I_P(ssi, id) } }
		},
		/* SSIU */
		/* Capture */
		{{{ RDMA_SSIU_O_N(ssi, id),	0 },
		  { RDMA_SSIU_O_P(ssi, id),	0 },
		  { RDMA_SSIU_O_P(ssi, id),	0 } },
		 /* Playback */
		 {{ 0,				RDMA_SSIU_I_N(ssi, id) },
		  { 0,				RDMA_SSIU_I_P(ssi, id) },
		  { 0,				RDMA_SSIU_I_P(ssi, id) } } },
	};

	/* it shouldn't happen */
	if (use_dvc && !use_src)
		dev_err(dev, "DVC is selected without SRC\n");

	/* use SSIU or SSI ? */
	if (is_ssi && (0 == strcmp(rsnd_mod_dma_name(mod), "ssiu")))
		is_ssi++;

	return (is_from) ?
		dma_addrs[is_ssi][is_play][use_src + use_dvc].out_addr :
		dma_addrs[is_ssi][is_play][use_src + use_dvc].in_addr;
}

dma_addr_t rsnd_gen_dma_addr(struct rsnd_priv *priv,
			     struct rsnd_mod *mod,
			     int is_play, int is_from)
{
	/*
	 * gen1 uses default DMA addr
	 */
	if (rsnd_is_gen1(priv))
		return 0;

	if (!mod)
		return 0;

	return rsnd_gen2_dma_addr(priv, mod, is_play, is_from);
}

/*
 *		Gen2
 */
static int rsnd_gen2_probe(struct platform_device *pdev,
			   struct rsnd_priv *priv)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_regmap_field_conf conf_ssiu[] = {
		RSND_GEN_S_REG(SSI_MODE0,	0x800),
		RSND_GEN_S_REG(SSI_MODE1,	0x804),
		/* FIXME: it needs SSI_MODE2/3 in the future */
		RSND_GEN_M_REG(SSI_BUSIF_MODE,	0x0,	0x80),
		RSND_GEN_M_REG(SSI_BUSIF_ADINR,	0x4,	0x80),
		RSND_GEN_M_REG(BUSIF_DALIGN,	0x8,	0x80),
		RSND_GEN_M_REG(SSI_CTRL,	0x10,	0x80),
		RSND_GEN_M_REG(INT_ENABLE,	0x18,	0x80),
	};
	struct rsnd_regmap_field_conf conf_scu[] = {
		RSND_GEN_M_REG(SRC_BUSIF_MODE,	0x0,	0x20),
		RSND_GEN_M_REG(SRC_ROUTE_MODE0,	0xc,	0x20),
		RSND_GEN_M_REG(SRC_CTRL,	0x10,	0x20),
		RSND_GEN_M_REG(CMD_ROUTE_SLCT,	0x18c,	0x20),
		RSND_GEN_M_REG(CMD_CTRL,	0x190,	0x20),
=======
 *		Gen2
 */
static int rsnd_gen2_probe(struct rsnd_priv *priv)
{
	static const struct rsnd_regmap_field_conf conf_ssiu[] = {
		RSND_GEN_S_REG(SSI_MODE0,	0x800),
		RSND_GEN_S_REG(SSI_MODE1,	0x804),
		RSND_GEN_S_REG(SSI_MODE2,	0x808),
		RSND_GEN_S_REG(SSI_CONTROL,	0x810),

		/* FIXME: it needs SSI_MODE2/3 in the future */
		RSND_GEN_M_REG(SSI_BUSIF_MODE,	0x0,	0x80),
		RSND_GEN_M_REG(SSI_BUSIF_ADINR,	0x4,	0x80),
		RSND_GEN_M_REG(SSI_BUSIF_DALIGN,0x8,	0x80),
		RSND_GEN_M_REG(SSI_MODE,	0xc,	0x80),
		RSND_GEN_M_REG(SSI_CTRL,	0x10,	0x80),
		RSND_GEN_M_REG(SSI_INT_ENABLE,	0x18,	0x80),
	};

	static const struct rsnd_regmap_field_conf conf_scu[] = {
		RSND_GEN_M_REG(SRC_I_BUSIF_MODE,0x0,	0x20),
		RSND_GEN_M_REG(SRC_O_BUSIF_MODE,0x4,	0x20),
		RSND_GEN_M_REG(SRC_BUSIF_DALIGN,0x8,	0x20),
		RSND_GEN_M_REG(SRC_ROUTE_MODE0,	0xc,	0x20),
		RSND_GEN_M_REG(SRC_CTRL,	0x10,	0x20),
		RSND_GEN_M_REG(SRC_INT_ENABLE0,	0x18,	0x20),
		RSND_GEN_M_REG(CMD_BUSIF_DALIGN,0x188,	0x20),
		RSND_GEN_M_REG(CMD_ROUTE_SLCT,	0x18c,	0x20),
		RSND_GEN_M_REG(CMD_CTRL,	0x190,	0x20),
		RSND_GEN_S_REG(SCU_SYS_STATUS0,	0x1c8),
		RSND_GEN_S_REG(SCU_SYS_INT_EN0,	0x1cc),
		RSND_GEN_S_REG(SCU_SYS_STATUS1,	0x1d0),
		RSND_GEN_S_REG(SCU_SYS_INT_EN1,	0x1d4),
>>>>>>> v4.9.227
		RSND_GEN_M_REG(SRC_SWRSR,	0x200,	0x40),
		RSND_GEN_M_REG(SRC_SRCIR,	0x204,	0x40),
		RSND_GEN_M_REG(SRC_ADINR,	0x214,	0x40),
		RSND_GEN_M_REG(SRC_IFSCR,	0x21c,	0x40),
		RSND_GEN_M_REG(SRC_IFSVR,	0x220,	0x40),
		RSND_GEN_M_REG(SRC_SRCCR,	0x224,	0x40),
		RSND_GEN_M_REG(SRC_BSDSR,	0x22c,	0x40),
		RSND_GEN_M_REG(SRC_BSISR,	0x238,	0x40),
<<<<<<< HEAD
=======
		RSND_GEN_M_REG(CTU_SWRSR,	0x500,	0x100),
		RSND_GEN_M_REG(CTU_CTUIR,	0x504,	0x100),
		RSND_GEN_M_REG(CTU_ADINR,	0x508,	0x100),
		RSND_GEN_M_REG(CTU_CPMDR,	0x510,	0x100),
		RSND_GEN_M_REG(CTU_SCMDR,	0x514,	0x100),
		RSND_GEN_M_REG(CTU_SV00R,	0x518,	0x100),
		RSND_GEN_M_REG(CTU_SV01R,	0x51c,	0x100),
		RSND_GEN_M_REG(CTU_SV02R,	0x520,	0x100),
		RSND_GEN_M_REG(CTU_SV03R,	0x524,	0x100),
		RSND_GEN_M_REG(CTU_SV04R,	0x528,	0x100),
		RSND_GEN_M_REG(CTU_SV05R,	0x52c,	0x100),
		RSND_GEN_M_REG(CTU_SV06R,	0x530,	0x100),
		RSND_GEN_M_REG(CTU_SV07R,	0x534,	0x100),
		RSND_GEN_M_REG(CTU_SV10R,	0x538,	0x100),
		RSND_GEN_M_REG(CTU_SV11R,	0x53c,	0x100),
		RSND_GEN_M_REG(CTU_SV12R,	0x540,	0x100),
		RSND_GEN_M_REG(CTU_SV13R,	0x544,	0x100),
		RSND_GEN_M_REG(CTU_SV14R,	0x548,	0x100),
		RSND_GEN_M_REG(CTU_SV15R,	0x54c,	0x100),
		RSND_GEN_M_REG(CTU_SV16R,	0x550,	0x100),
		RSND_GEN_M_REG(CTU_SV17R,	0x554,	0x100),
		RSND_GEN_M_REG(CTU_SV20R,	0x558,	0x100),
		RSND_GEN_M_REG(CTU_SV21R,	0x55c,	0x100),
		RSND_GEN_M_REG(CTU_SV22R,	0x560,	0x100),
		RSND_GEN_M_REG(CTU_SV23R,	0x564,	0x100),
		RSND_GEN_M_REG(CTU_SV24R,	0x568,	0x100),
		RSND_GEN_M_REG(CTU_SV25R,	0x56c,	0x100),
		RSND_GEN_M_REG(CTU_SV26R,	0x570,	0x100),
		RSND_GEN_M_REG(CTU_SV27R,	0x574,	0x100),
		RSND_GEN_M_REG(CTU_SV30R,	0x578,	0x100),
		RSND_GEN_M_REG(CTU_SV31R,	0x57c,	0x100),
		RSND_GEN_M_REG(CTU_SV32R,	0x580,	0x100),
		RSND_GEN_M_REG(CTU_SV33R,	0x584,	0x100),
		RSND_GEN_M_REG(CTU_SV34R,	0x588,	0x100),
		RSND_GEN_M_REG(CTU_SV35R,	0x58c,	0x100),
		RSND_GEN_M_REG(CTU_SV36R,	0x590,	0x100),
		RSND_GEN_M_REG(CTU_SV37R,	0x594,	0x100),
		RSND_GEN_M_REG(MIX_SWRSR,	0xd00,	0x40),
		RSND_GEN_M_REG(MIX_MIXIR,	0xd04,	0x40),
		RSND_GEN_M_REG(MIX_ADINR,	0xd08,	0x40),
		RSND_GEN_M_REG(MIX_MIXMR,	0xd10,	0x40),
		RSND_GEN_M_REG(MIX_MVPDR,	0xd14,	0x40),
		RSND_GEN_M_REG(MIX_MDBAR,	0xd18,	0x40),
		RSND_GEN_M_REG(MIX_MDBBR,	0xd1c,	0x40),
		RSND_GEN_M_REG(MIX_MDBCR,	0xd20,	0x40),
		RSND_GEN_M_REG(MIX_MDBDR,	0xd24,	0x40),
		RSND_GEN_M_REG(MIX_MDBER,	0xd28,	0x40),
>>>>>>> v4.9.227
		RSND_GEN_M_REG(DVC_SWRSR,	0xe00,	0x100),
		RSND_GEN_M_REG(DVC_DVUIR,	0xe04,	0x100),
		RSND_GEN_M_REG(DVC_ADINR,	0xe08,	0x100),
		RSND_GEN_M_REG(DVC_DVUCR,	0xe10,	0x100),
		RSND_GEN_M_REG(DVC_ZCMCR,	0xe14,	0x100),
<<<<<<< HEAD
		RSND_GEN_M_REG(DVC_VOL0R,	0xe28,	0x100),
		RSND_GEN_M_REG(DVC_VOL1R,	0xe2c,	0x100),
		RSND_GEN_M_REG(DVC_DVUER,	0xe48,	0x100),
	};
	struct rsnd_regmap_field_conf conf_adg[] = {
=======
		RSND_GEN_M_REG(DVC_VRCTR,	0xe18,	0x100),
		RSND_GEN_M_REG(DVC_VRPDR,	0xe1c,	0x100),
		RSND_GEN_M_REG(DVC_VRDBR,	0xe20,	0x100),
		RSND_GEN_M_REG(DVC_VOL0R,	0xe28,	0x100),
		RSND_GEN_M_REG(DVC_VOL1R,	0xe2c,	0x100),
		RSND_GEN_M_REG(DVC_VOL2R,	0xe30,	0x100),
		RSND_GEN_M_REG(DVC_VOL3R,	0xe34,	0x100),
		RSND_GEN_M_REG(DVC_VOL4R,	0xe38,	0x100),
		RSND_GEN_M_REG(DVC_VOL5R,	0xe3c,	0x100),
		RSND_GEN_M_REG(DVC_VOL6R,	0xe40,	0x100),
		RSND_GEN_M_REG(DVC_VOL7R,	0xe44,	0x100),
		RSND_GEN_M_REG(DVC_DVUER,	0xe48,	0x100),
	};
	static const struct rsnd_regmap_field_conf conf_adg[] = {
>>>>>>> v4.9.227
		RSND_GEN_S_REG(BRRA,		0x00),
		RSND_GEN_S_REG(BRRB,		0x04),
		RSND_GEN_S_REG(SSICKR,		0x08),
		RSND_GEN_S_REG(AUDIO_CLK_SEL0,	0x0c),
		RSND_GEN_S_REG(AUDIO_CLK_SEL1,	0x10),
		RSND_GEN_S_REG(AUDIO_CLK_SEL2,	0x14),
		RSND_GEN_S_REG(DIV_EN,		0x30),
		RSND_GEN_S_REG(SRCIN_TIMSEL0,	0x34),
		RSND_GEN_S_REG(SRCIN_TIMSEL1,	0x38),
		RSND_GEN_S_REG(SRCIN_TIMSEL2,	0x3c),
		RSND_GEN_S_REG(SRCIN_TIMSEL3,	0x40),
		RSND_GEN_S_REG(SRCIN_TIMSEL4,	0x44),
		RSND_GEN_S_REG(SRCOUT_TIMSEL0,	0x48),
		RSND_GEN_S_REG(SRCOUT_TIMSEL1,	0x4c),
		RSND_GEN_S_REG(SRCOUT_TIMSEL2,	0x50),
		RSND_GEN_S_REG(SRCOUT_TIMSEL3,	0x54),
		RSND_GEN_S_REG(SRCOUT_TIMSEL4,	0x58),
		RSND_GEN_S_REG(CMDOUT_TIMSEL,	0x5c),
	};
<<<<<<< HEAD
	struct rsnd_regmap_field_conf conf_ssi[] = {
=======
	static const struct rsnd_regmap_field_conf conf_ssi[] = {
>>>>>>> v4.9.227
		RSND_GEN_M_REG(SSICR,		0x00,	0x40),
		RSND_GEN_M_REG(SSISR,		0x04,	0x40),
		RSND_GEN_M_REG(SSITDR,		0x08,	0x40),
		RSND_GEN_M_REG(SSIRDR,		0x0c,	0x40),
		RSND_GEN_M_REG(SSIWSR,		0x20,	0x40),
	};
	int ret_ssiu;
	int ret_scu;
	int ret_adg;
	int ret_ssi;

<<<<<<< HEAD
	ret_ssiu = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_SSIU, conf_ssiu);
	ret_scu  = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_SCU,  conf_scu);
	ret_adg  = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_ADG,  conf_adg);
	ret_ssi  = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_SSI,  conf_ssi);
=======
	ret_ssiu = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_SSIU, "ssiu", conf_ssiu);
	ret_scu  = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_SCU,  "scu",  conf_scu);
	ret_adg  = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_ADG,  "adg",  conf_adg);
	ret_ssi  = rsnd_gen_regmap_init(priv, 10, RSND_GEN2_SSI,  "ssi",  conf_ssi);
>>>>>>> v4.9.227
	if (ret_ssiu < 0 ||
	    ret_scu  < 0 ||
	    ret_adg  < 0 ||
	    ret_ssi  < 0)
		return ret_ssiu | ret_scu | ret_adg | ret_ssi;

<<<<<<< HEAD
	dev_dbg(dev, "Gen2 is probed\n");

=======
>>>>>>> v4.9.227
	return 0;
}

/*
 *		Gen1
 */

<<<<<<< HEAD
static int rsnd_gen1_probe(struct platform_device *pdev,
			   struct rsnd_priv *priv)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_regmap_field_conf conf_sru[] = {
		RSND_GEN_S_REG(SRC_ROUTE_SEL,	0x00),
		RSND_GEN_S_REG(SRC_TMG_SEL0,	0x08),
		RSND_GEN_S_REG(SRC_TMG_SEL1,	0x0c),
		RSND_GEN_S_REG(SRC_TMG_SEL2,	0x10),
		RSND_GEN_S_REG(SRC_ROUTE_CTRL,	0xc0),
		RSND_GEN_S_REG(SSI_MODE0,	0xD0),
		RSND_GEN_S_REG(SSI_MODE1,	0xD4),
		RSND_GEN_M_REG(SRC_BUSIF_MODE,	0x20,	0x4),
		RSND_GEN_M_REG(SRC_ROUTE_MODE0,	0x50,	0x8),
		RSND_GEN_M_REG(SRC_SWRSR,	0x200,	0x40),
		RSND_GEN_M_REG(SRC_SRCIR,	0x204,	0x40),
		RSND_GEN_M_REG(SRC_ADINR,	0x214,	0x40),
		RSND_GEN_M_REG(SRC_IFSCR,	0x21c,	0x40),
		RSND_GEN_M_REG(SRC_IFSVR,	0x220,	0x40),
		RSND_GEN_M_REG(SRC_SRCCR,	0x224,	0x40),
		RSND_GEN_M_REG(SRC_MNFSR,	0x228,	0x40),
	};
	struct rsnd_regmap_field_conf conf_adg[] = {
=======
static int rsnd_gen1_probe(struct rsnd_priv *priv)
{
	static const struct rsnd_regmap_field_conf conf_adg[] = {
>>>>>>> v4.9.227
		RSND_GEN_S_REG(BRRA,		0x00),
		RSND_GEN_S_REG(BRRB,		0x04),
		RSND_GEN_S_REG(SSICKR,		0x08),
		RSND_GEN_S_REG(AUDIO_CLK_SEL0,	0x0c),
		RSND_GEN_S_REG(AUDIO_CLK_SEL1,	0x10),
<<<<<<< HEAD
		RSND_GEN_S_REG(AUDIO_CLK_SEL3,	0x18),
		RSND_GEN_S_REG(AUDIO_CLK_SEL4,	0x1c),
		RSND_GEN_S_REG(AUDIO_CLK_SEL5,	0x20),
	};
	struct rsnd_regmap_field_conf conf_ssi[] = {
=======
	};
	static const struct rsnd_regmap_field_conf conf_ssi[] = {
>>>>>>> v4.9.227
		RSND_GEN_M_REG(SSICR,		0x00,	0x40),
		RSND_GEN_M_REG(SSISR,		0x04,	0x40),
		RSND_GEN_M_REG(SSITDR,		0x08,	0x40),
		RSND_GEN_M_REG(SSIRDR,		0x0c,	0x40),
		RSND_GEN_M_REG(SSIWSR,		0x20,	0x40),
	};
<<<<<<< HEAD
	int ret_sru;
	int ret_adg;
	int ret_ssi;

	ret_sru  = rsnd_gen_regmap_init(priv, 9, RSND_GEN1_SRU,  conf_sru);
	ret_adg  = rsnd_gen_regmap_init(priv, 9, RSND_GEN1_ADG,  conf_adg);
	ret_ssi  = rsnd_gen_regmap_init(priv, 9, RSND_GEN1_SSI,  conf_ssi);
	if (ret_sru  < 0 ||
	    ret_adg  < 0 ||
	    ret_ssi  < 0)
		return ret_sru | ret_adg | ret_ssi;

	dev_dbg(dev, "Gen1 is probed\n");
=======
	int ret_adg;
	int ret_ssi;

	ret_adg  = rsnd_gen_regmap_init(priv, 9, RSND_GEN1_ADG, "adg", conf_adg);
	ret_ssi  = rsnd_gen_regmap_init(priv, 9, RSND_GEN1_SSI, "ssi", conf_ssi);
	if (ret_adg  < 0 ||
	    ret_ssi  < 0)
		return ret_adg | ret_ssi;
>>>>>>> v4.9.227

	return 0;
}

/*
 *		Gen
 */
<<<<<<< HEAD
static void rsnd_of_parse_gen(struct platform_device *pdev,
			      const struct rsnd_of_data *of_data,
			      struct rsnd_priv *priv)
{
	struct rcar_snd_info *info = priv->info;

	if (!of_data)
		return;

	info->flags = of_data->flags;
}

int rsnd_gen_probe(struct platform_device *pdev,
		   const struct rsnd_of_data *of_data,
		   struct rsnd_priv *priv)
=======
int rsnd_gen_probe(struct rsnd_priv *priv)
>>>>>>> v4.9.227
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_gen *gen;
	int ret;

<<<<<<< HEAD
	rsnd_of_parse_gen(pdev, of_data, priv);

=======
>>>>>>> v4.9.227
	gen = devm_kzalloc(dev, sizeof(*gen), GFP_KERNEL);
	if (!gen) {
		dev_err(dev, "GEN allocate failed\n");
		return -ENOMEM;
	}

	priv->gen = gen;

	ret = -ENODEV;
	if (rsnd_is_gen1(priv))
<<<<<<< HEAD
		ret = rsnd_gen1_probe(pdev, priv);
	else if (rsnd_is_gen2(priv))
		ret = rsnd_gen2_probe(pdev, priv);
=======
		ret = rsnd_gen1_probe(priv);
	else if (rsnd_is_gen2(priv))
		ret = rsnd_gen2_probe(priv);
>>>>>>> v4.9.227

	if (ret < 0)
		dev_err(dev, "unknown generation R-Car sound device\n");

	return ret;
}
