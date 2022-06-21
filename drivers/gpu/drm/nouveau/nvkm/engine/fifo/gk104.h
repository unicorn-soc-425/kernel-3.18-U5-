#ifndef __GK104_FIFO_H__
#define __GK104_FIFO_H__
#define gk104_fifo(p) container_of((p), struct gk104_fifo, base)
#include "priv.h"

<<<<<<< HEAD
#include <subdev/mmu.h>

struct gk104_fifo_engn {
	struct nvkm_memory *runlist[2];
	int cur_runlist;
	wait_queue_head_t wait;
	struct list_head chan;
};

struct gk104_fifo {
	struct nvkm_fifo base;

	struct work_struct fault;
	u64 mask;

	struct gk104_fifo_engn engine[7];
=======
#include <core/enum.h>
#include <subdev/mmu.h>

struct gk104_fifo_chan;
struct gk104_fifo {
	const struct gk104_fifo_func *func;
	struct nvkm_fifo base;

	struct {
		struct work_struct work;
		u32 engm;
		u32 runm;
	} recover;

	int pbdma_nr;

	struct {
		struct nvkm_engine *engine;
		int runl;
		int pbid;
	} engine[16];
	int engine_nr;

	struct {
		struct nvkm_memory *mem[2];
		int next;
		wait_queue_head_t wait;
		struct list_head chan;
		u32 engm;
	} runlist[16];
	int runlist_nr;

>>>>>>> v4.9.227
	struct {
		struct nvkm_memory *mem;
		struct nvkm_vma bar;
	} user;
<<<<<<< HEAD
	int spoon_nr;
};

int gk104_fifo_new_(const struct nvkm_fifo_func *, struct nvkm_device *,
		    int index, int nr, struct nvkm_fifo **);
void *gk104_fifo_dtor(struct nvkm_fifo *);
int gk104_fifo_oneinit(struct nvkm_fifo *);
void gk104_fifo_init(struct nvkm_fifo *);
void gk104_fifo_fini(struct nvkm_fifo *);
void gk104_fifo_intr(struct nvkm_fifo *);
void gk104_fifo_uevent_init(struct nvkm_fifo *);
void gk104_fifo_uevent_fini(struct nvkm_fifo *);
void gk104_fifo_runlist_update(struct gk104_fifo *, u32 engine);
=======
};

struct gk104_fifo_func {
	struct {
		const struct nvkm_enum *engine;
		const struct nvkm_enum *reason;
		const struct nvkm_enum *hubclient;
		const struct nvkm_enum *gpcclient;
	} fault;

	const struct nvkm_fifo_chan_oclass *chan[];
};

int gk104_fifo_new_(const struct gk104_fifo_func *, struct nvkm_device *,
		    int index, int nr, struct nvkm_fifo **);
void gk104_fifo_runlist_insert(struct gk104_fifo *, struct gk104_fifo_chan *);
void gk104_fifo_runlist_remove(struct gk104_fifo *, struct gk104_fifo_chan *);
void gk104_fifo_runlist_commit(struct gk104_fifo *, int runl);
>>>>>>> v4.9.227

static inline u64
gk104_fifo_engine_subdev(int engine)
{
	switch (engine) {
	case 0: return (1ULL << NVKM_ENGINE_GR) |
		       (1ULL << NVKM_ENGINE_SW) |
		       (1ULL << NVKM_ENGINE_CE2);
	case 1: return (1ULL << NVKM_ENGINE_MSPDEC);
	case 2: return (1ULL << NVKM_ENGINE_MSPPP);
	case 3: return (1ULL << NVKM_ENGINE_MSVLD);
	case 4: return (1ULL << NVKM_ENGINE_CE0);
	case 5: return (1ULL << NVKM_ENGINE_CE1);
	case 6: return (1ULL << NVKM_ENGINE_MSENC);
	default:
		WARN_ON(1);
		return 0;
	}
}

<<<<<<< HEAD
static inline int
gk104_fifo_subdev_engine(int subdev)
{
	switch (subdev) {
	case NVKM_ENGINE_GR:
	case NVKM_ENGINE_SW:
	case NVKM_ENGINE_CE2   : return 0;
	case NVKM_ENGINE_MSPDEC: return 1;
	case NVKM_ENGINE_MSPPP : return 2;
	case NVKM_ENGINE_MSVLD : return 3;
	case NVKM_ENGINE_CE0   : return 4;
	case NVKM_ENGINE_CE1   : return 5;
	case NVKM_ENGINE_MSENC : return 6;
	default:
		WARN_ON(1);
		return 0;
	}
}
=======
extern const struct nvkm_enum gk104_fifo_fault_engine[];
extern const struct nvkm_enum gk104_fifo_fault_reason[];
extern const struct nvkm_enum gk104_fifo_fault_hubclient[];
extern const struct nvkm_enum gk104_fifo_fault_gpcclient[];

extern const struct nvkm_enum gm107_fifo_fault_engine[];
>>>>>>> v4.9.227
#endif
