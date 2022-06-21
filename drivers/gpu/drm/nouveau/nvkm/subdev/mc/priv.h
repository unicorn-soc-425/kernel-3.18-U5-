#ifndef __NVKM_MC_PRIV_H__
#define __NVKM_MC_PRIV_H__
#define nvkm_mc(p) container_of((p), struct nvkm_mc, subdev)
#include <subdev/mc.h>

<<<<<<< HEAD
int nvkm_mc_new_(const struct nvkm_mc_func *, struct nvkm_device *,
		 int index, struct nvkm_mc **);

struct nvkm_mc_intr {
	u32 stat;
	u32 unit;
=======
void nvkm_mc_ctor(const struct nvkm_mc_func *, struct nvkm_device *,
		  int index, struct nvkm_mc *);
int nvkm_mc_new_(const struct nvkm_mc_func *, struct nvkm_device *,
		 int index, struct nvkm_mc **);

struct nvkm_mc_map {
	u32 stat;
	u32 unit;
	bool noauto;
>>>>>>> v4.9.227
};

struct nvkm_mc_func {
	void (*init)(struct nvkm_mc *);
<<<<<<< HEAD
	const struct nvkm_mc_intr *intr;
=======
	const struct nvkm_mc_map *intr;
>>>>>>> v4.9.227
	/* disable reporting of interrupts to host */
	void (*intr_unarm)(struct nvkm_mc *);
	/* enable reporting of interrupts to host */
	void (*intr_rearm)(struct nvkm_mc *);
<<<<<<< HEAD
	/* retrieve pending interrupt mask (NV_PMC_INTR) */
	u32 (*intr_mask)(struct nvkm_mc *);
=======
	/* (un)mask delivery of specific interrupts */
	void (*intr_mask)(struct nvkm_mc *, u32 mask, u32 stat);
	/* retrieve pending interrupt mask (NV_PMC_INTR) */
	u32 (*intr_stat)(struct nvkm_mc *);
	const struct nvkm_mc_map *reset;
>>>>>>> v4.9.227
	void (*unk260)(struct nvkm_mc *, u32);
};

void nv04_mc_init(struct nvkm_mc *);
<<<<<<< HEAD
extern const struct nvkm_mc_intr nv04_mc_intr[];
void nv04_mc_intr_unarm(struct nvkm_mc *);
void nv04_mc_intr_rearm(struct nvkm_mc *);
u32 nv04_mc_intr_mask(struct nvkm_mc *);
=======
void nv04_mc_intr_unarm(struct nvkm_mc *);
void nv04_mc_intr_rearm(struct nvkm_mc *);
u32 nv04_mc_intr_stat(struct nvkm_mc *);
extern const struct nvkm_mc_map nv04_mc_reset[];

extern const struct nvkm_mc_map nv17_mc_intr[];
extern const struct nvkm_mc_map nv17_mc_reset[];
>>>>>>> v4.9.227

void nv44_mc_init(struct nvkm_mc *);

void nv50_mc_init(struct nvkm_mc *);
<<<<<<< HEAD
extern const struct nvkm_mc_intr nv50_mc_intr[];

extern const struct nvkm_mc_intr gf100_mc_intr[];
void gf100_mc_intr_unarm(struct nvkm_mc *);
void gf100_mc_intr_rearm(struct nvkm_mc *);
u32 gf100_mc_intr_mask(struct nvkm_mc *);
void gf100_mc_unk260(struct nvkm_mc *, u32);
=======

void gf100_mc_intr_unarm(struct nvkm_mc *);
void gf100_mc_intr_rearm(struct nvkm_mc *);
void gf100_mc_intr_mask(struct nvkm_mc *, u32, u32);
u32 gf100_mc_intr_stat(struct nvkm_mc *);
void gf100_mc_unk260(struct nvkm_mc *, u32);

extern const struct nvkm_mc_map gk104_mc_intr[];
extern const struct nvkm_mc_map gk104_mc_reset[];
>>>>>>> v4.9.227
#endif
