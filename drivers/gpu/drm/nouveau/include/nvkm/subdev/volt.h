#ifndef __NVKM_VOLT_H__
#define __NVKM_VOLT_H__
#include <core/subdev.h>

struct nvkm_volt {
	const struct nvkm_volt_func *func;
	struct nvkm_subdev subdev;

	u8 vid_mask;
	u8 vid_nr;
	struct {
		u32 uv;
		u8 vid;
	} vid[256];
<<<<<<< HEAD
=======

	u32 max_uv;
	u32 min_uv;
>>>>>>> v4.9.227
};

int nvkm_volt_get(struct nvkm_volt *);
int nvkm_volt_set_id(struct nvkm_volt *, u8 id, int condition);

int nv40_volt_new(struct nvkm_device *, int, struct nvkm_volt **);
int gk104_volt_new(struct nvkm_device *, int, struct nvkm_volt **);
int gk20a_volt_new(struct nvkm_device *, int, struct nvkm_volt **);
<<<<<<< HEAD
=======
int gm20b_volt_new(struct nvkm_device *, int, struct nvkm_volt **);
>>>>>>> v4.9.227
#endif
