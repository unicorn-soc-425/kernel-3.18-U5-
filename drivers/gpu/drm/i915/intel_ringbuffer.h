#ifndef _INTEL_RINGBUFFER_H_
#define _INTEL_RINGBUFFER_H_

#include <linux/hashtable.h>
#include "i915_gem_batch_pool.h"
<<<<<<< HEAD
=======
#include "i915_gem_request.h"
>>>>>>> v4.9.227

#define I915_CMD_HASH_ORDER 9

/* Early gen2 devices have a cacheline of just 32 bytes, using 64 is overkill,
 * but keeps the logic simple. Indeed, the whole purpose of this macro is just
 * to give some inclination as to some of the magic values used in the various
 * workarounds!
 */
#define CACHELINE_BYTES 64
#define CACHELINE_DWORDS (CACHELINE_BYTES / sizeof(uint32_t))

/*
 * Gen2 BSpec "1. Programming Environment" / 1.4.4.6 "Ring Buffer Use"
 * Gen3 BSpec "vol1c Memory Interface Functions" / 2.3.4.5 "Ring Buffer Use"
 * Gen4+ BSpec "vol1c Memory Interface and Command Stream" / 5.3.4.5 "Ring Buffer Use"
 *
 * "If the Ring Buffer Head Pointer and the Tail Pointer are on the same
 * cacheline, the Head Pointer must not be greater than the Tail
 * Pointer."
 */
#define I915_RING_FREE_SPACE 64

<<<<<<< HEAD
struct  intel_hw_status_page {
	u32		*page_addr;
	unsigned int	gfx_addr;
	struct		drm_i915_gem_object *obj;
};

#define I915_READ_TAIL(ring) I915_READ(RING_TAIL((ring)->mmio_base))
#define I915_WRITE_TAIL(ring, val) I915_WRITE(RING_TAIL((ring)->mmio_base), val)

#define I915_READ_START(ring) I915_READ(RING_START((ring)->mmio_base))
#define I915_WRITE_START(ring, val) I915_WRITE(RING_START((ring)->mmio_base), val)

#define I915_READ_HEAD(ring)  I915_READ(RING_HEAD((ring)->mmio_base))
#define I915_WRITE_HEAD(ring, val) I915_WRITE(RING_HEAD((ring)->mmio_base), val)

#define I915_READ_CTL(ring) I915_READ(RING_CTL((ring)->mmio_base))
#define I915_WRITE_CTL(ring, val) I915_WRITE(RING_CTL((ring)->mmio_base), val)

#define I915_READ_IMR(ring) I915_READ(RING_IMR((ring)->mmio_base))
#define I915_WRITE_IMR(ring, val) I915_WRITE(RING_IMR((ring)->mmio_base), val)

#define I915_READ_MODE(ring) I915_READ(RING_MI_MODE((ring)->mmio_base))
#define I915_WRITE_MODE(ring, val) I915_WRITE(RING_MI_MODE((ring)->mmio_base), val)
=======
struct intel_hw_status_page {
	struct i915_vma *vma;
	u32 *page_addr;
	u32 ggtt_offset;
};

#define I915_READ_TAIL(engine) I915_READ(RING_TAIL((engine)->mmio_base))
#define I915_WRITE_TAIL(engine, val) I915_WRITE(RING_TAIL((engine)->mmio_base), val)

#define I915_READ_START(engine) I915_READ(RING_START((engine)->mmio_base))
#define I915_WRITE_START(engine, val) I915_WRITE(RING_START((engine)->mmio_base), val)

#define I915_READ_HEAD(engine)  I915_READ(RING_HEAD((engine)->mmio_base))
#define I915_WRITE_HEAD(engine, val) I915_WRITE(RING_HEAD((engine)->mmio_base), val)

#define I915_READ_CTL(engine) I915_READ(RING_CTL((engine)->mmio_base))
#define I915_WRITE_CTL(engine, val) I915_WRITE(RING_CTL((engine)->mmio_base), val)

#define I915_READ_IMR(engine) I915_READ(RING_IMR((engine)->mmio_base))
#define I915_WRITE_IMR(engine, val) I915_WRITE(RING_IMR((engine)->mmio_base), val)

#define I915_READ_MODE(engine) I915_READ(RING_MI_MODE((engine)->mmio_base))
#define I915_WRITE_MODE(engine, val) I915_WRITE(RING_MI_MODE((engine)->mmio_base), val)
>>>>>>> v4.9.227

/* seqno size is actually only a uint32, but since we plan to use MI_FLUSH_DW to
 * do the writes, and that must have qw aligned offsets, simply pretend it's 8b.
 */
<<<<<<< HEAD
#define i915_semaphore_seqno_size sizeof(uint64_t)
#define GEN8_SIGNAL_OFFSET(__ring, to)			     \
	(i915_gem_obj_ggtt_offset(dev_priv->semaphore_obj) + \
	((__ring)->id * I915_NUM_RINGS * i915_semaphore_seqno_size) +	\
	(i915_semaphore_seqno_size * (to)))

#define GEN8_WAIT_OFFSET(__ring, from)			     \
	(i915_gem_obj_ggtt_offset(dev_priv->semaphore_obj) + \
	((from) * I915_NUM_RINGS * i915_semaphore_seqno_size) + \
	(i915_semaphore_seqno_size * (__ring)->id))

#define GEN8_RING_SEMAPHORE_INIT do { \
	if (!dev_priv->semaphore_obj) { \
		break; \
	} \
	ring->semaphore.signal_ggtt[RCS] = GEN8_SIGNAL_OFFSET(ring, RCS); \
	ring->semaphore.signal_ggtt[VCS] = GEN8_SIGNAL_OFFSET(ring, VCS); \
	ring->semaphore.signal_ggtt[BCS] = GEN8_SIGNAL_OFFSET(ring, BCS); \
	ring->semaphore.signal_ggtt[VECS] = GEN8_SIGNAL_OFFSET(ring, VECS); \
	ring->semaphore.signal_ggtt[VCS2] = GEN8_SIGNAL_OFFSET(ring, VCS2); \
	ring->semaphore.signal_ggtt[ring->id] = MI_SEMAPHORE_SYNC_INVALID; \
	} while(0)

enum intel_ring_hangcheck_action {
	HANGCHECK_IDLE = 0,
	HANGCHECK_WAIT,
	HANGCHECK_ACTIVE,
	HANGCHECK_ACTIVE_LOOP,
=======
#define gen8_semaphore_seqno_size sizeof(uint64_t)
#define GEN8_SEMAPHORE_OFFSET(__from, __to)			     \
	(((__from) * I915_NUM_ENGINES  + (__to)) * gen8_semaphore_seqno_size)
#define GEN8_SIGNAL_OFFSET(__ring, to)			     \
	(dev_priv->semaphore->node.start + \
	 GEN8_SEMAPHORE_OFFSET((__ring)->id, (to)))
#define GEN8_WAIT_OFFSET(__ring, from)			     \
	(dev_priv->semaphore->node.start + \
	 GEN8_SEMAPHORE_OFFSET(from, (__ring)->id))

enum intel_engine_hangcheck_action {
	HANGCHECK_IDLE = 0,
	HANGCHECK_WAIT,
	HANGCHECK_ACTIVE,
>>>>>>> v4.9.227
	HANGCHECK_KICK,
	HANGCHECK_HUNG,
};

#define HANGCHECK_SCORE_RING_HUNG 31

<<<<<<< HEAD
struct intel_ring_hangcheck {
	u64 acthd;
	u64 max_acthd;
	u32 seqno;
	int score;
	enum intel_ring_hangcheck_action action;
	int deadlock;
};

struct intel_ringbuffer {
	struct drm_i915_gem_object *obj;
	void __iomem *virtual_start;

	struct intel_engine_cs *ring;
=======
struct intel_engine_hangcheck {
	u64 acthd;
	u32 seqno;
	int score;
	enum intel_engine_hangcheck_action action;
	int deadlock;
	u32 instdone[I915_NUM_INSTDONE_REG];
};

struct intel_ring {
	struct i915_vma *vma;
	void *vaddr;

	struct intel_engine_cs *engine;

	struct list_head request_list;
>>>>>>> v4.9.227

	u32 head;
	u32 tail;
	int space;
	int size;
	int effective_size;
<<<<<<< HEAD
	int reserved_size;
	int reserved_tail;
	bool reserved_in_use;
=======
>>>>>>> v4.9.227

	/** We track the position of the requests in the ring buffer, and
	 * when each is retired we increment last_retired_head as the GPU
	 * must have finished processing the request and so we know we
	 * can advance the ringbuffer up to that position.
	 *
	 * last_retired_head is set to -1 after the value is consumed so
	 * we can detect new retirements.
	 */
	u32 last_retired_head;
};

<<<<<<< HEAD
struct	intel_context;
struct drm_i915_reg_descriptor;
=======
struct i915_gem_context;
struct drm_i915_reg_table;
>>>>>>> v4.9.227

/*
 * we use a single page to load ctx workarounds so all of these
 * values are referred in terms of dwords
 *
 * struct i915_wa_ctx_bb:
 *  offset: specifies batch starting position, also helpful in case
 *    if we want to have multiple batches at different offsets based on
 *    some criteria. It is not a requirement at the moment but provides
 *    an option for future use.
 *  size: size of the batch in DWORDS
 */
<<<<<<< HEAD
struct  i915_ctx_workarounds {
=======
struct i915_ctx_workarounds {
>>>>>>> v4.9.227
	struct i915_wa_ctx_bb {
		u32 offset;
		u32 size;
	} indirect_ctx, per_ctx;
<<<<<<< HEAD
	struct drm_i915_gem_object *obj;
};

struct  intel_engine_cs {
	const char	*name;
	enum intel_ring_id {
		RCS = 0x0,
		VCS,
		BCS,
		VECS,
		VCS2
	} id;
#define I915_NUM_RINGS 5
#define LAST_USER_RING (VECS + 1)
	u32		mmio_base;
	struct		drm_device *dev;
	struct intel_ringbuffer *buffer;
=======
	struct i915_vma *vma;
};

struct drm_i915_gem_request;

struct intel_engine_cs {
	struct drm_i915_private *i915;
	const char	*name;
	enum intel_engine_id {
		RCS = 0,
		BCS,
		VCS,
		VCS2,	/* Keep instances of the same type engine together. */
		VECS
	} id;
#define I915_NUM_ENGINES 5
#define _VCS(n) (VCS + (n))
	unsigned int exec_id;
	enum intel_engine_hw_id {
		RCS_HW = 0,
		VCS_HW,
		BCS_HW,
		VECS_HW,
		VCS2_HW
	} hw_id;
	enum intel_engine_hw_id guc_id; /* XXX same as hw_id? */
	u64 fence_context;
	u32		mmio_base;
	unsigned int irq_shift;
	struct intel_ring *buffer;

	/* Rather than have every client wait upon all user interrupts,
	 * with the herd waking after every interrupt and each doing the
	 * heavyweight seqno dance, we delegate the task (of being the
	 * bottom-half of the user interrupt) to the first client. After
	 * every interrupt, we wake up one client, who does the heavyweight
	 * coherent seqno read and either goes back to sleep (if incomplete),
	 * or wakes up all the completed clients in parallel, before then
	 * transferring the bottom-half status to the next client in the queue.
	 *
	 * Compared to walking the entire list of waiters in a single dedicated
	 * bottom-half, we reduce the latency of the first waiter by avoiding
	 * a context switch, but incur additional coherent seqno reads when
	 * following the chain of request breadcrumbs. Since it is most likely
	 * that we have a single client waiting on each seqno, then reducing
	 * the overhead of waking that client is much preferred.
	 */
	struct intel_breadcrumbs {
		struct task_struct __rcu *irq_seqno_bh; /* bh for interrupts */
		bool irq_posted;

		spinlock_t lock; /* protects the lists of requests */
		struct rb_root waiters; /* sorted by retirement, priority */
		struct rb_root signals; /* sorted by retirement */
		struct intel_wait *first_wait; /* oldest waiter by retirement */
		struct task_struct *signaler; /* used for fence signalling */
		struct drm_i915_gem_request *first_signal;
		struct timer_list fake_irq; /* used after a missed interrupt */
		struct timer_list hangcheck; /* detect missed interrupts */

		unsigned long timeout;

		bool irq_enabled : 1;
		bool rpm_wakelock : 1;
	} breadcrumbs;
>>>>>>> v4.9.227

	/*
	 * A pool of objects to use as shadow copies of client batch buffers
	 * when the command parser is enabled. Prevents the client from
	 * modifying the batch contents after software parsing.
	 */
	struct i915_gem_batch_pool batch_pool;

	struct intel_hw_status_page status_page;
	struct i915_ctx_workarounds wa_ctx;
<<<<<<< HEAD

	unsigned irq_refcount; /* protected by dev_priv->irq_lock */
	u32		irq_enable_mask;	/* bitmask to enable ring interrupt */
	struct drm_i915_gem_request *trace_irq_req;
	bool __must_check (*irq_get)(struct intel_engine_cs *ring);
	void		(*irq_put)(struct intel_engine_cs *ring);

	int		(*init_hw)(struct intel_engine_cs *ring);

	int		(*init_context)(struct drm_i915_gem_request *req);

	void		(*write_tail)(struct intel_engine_cs *ring,
				      u32 value);
	int __must_check (*flush)(struct drm_i915_gem_request *req,
				  u32	invalidate_domains,
				  u32	flush_domains);
	int		(*add_request)(struct drm_i915_gem_request *req);
=======
	struct i915_vma *scratch;

	u32             irq_keep_mask; /* always keep these interrupts */
	u32		irq_enable_mask; /* bitmask to enable ring interrupt */
	void		(*irq_enable)(struct intel_engine_cs *engine);
	void		(*irq_disable)(struct intel_engine_cs *engine);

	int		(*init_hw)(struct intel_engine_cs *engine);
	void		(*reset_hw)(struct intel_engine_cs *engine,
				    struct drm_i915_gem_request *req);

	int		(*init_context)(struct drm_i915_gem_request *req);

	int		(*emit_flush)(struct drm_i915_gem_request *request,
				      u32 mode);
#define EMIT_INVALIDATE	BIT(0)
#define EMIT_FLUSH	BIT(1)
#define EMIT_BARRIER	(EMIT_INVALIDATE | EMIT_FLUSH)
	int		(*emit_bb_start)(struct drm_i915_gem_request *req,
					 u64 offset, u32 length,
					 unsigned int dispatch_flags);
#define I915_DISPATCH_SECURE BIT(0)
#define I915_DISPATCH_PINNED BIT(1)
#define I915_DISPATCH_RS     BIT(2)
	int		(*emit_request)(struct drm_i915_gem_request *req);

	/* Pass the request to the hardware queue (e.g. directly into
	 * the legacy ringbuffer or to the end of an execlist).
	 *
	 * This is called from an atomic context with irqs disabled; must
	 * be irq safe.
	 */
	void		(*submit_request)(struct drm_i915_gem_request *req);

>>>>>>> v4.9.227
	/* Some chipsets are not quite as coherent as advertised and need
	 * an expensive kick to force a true read of the up-to-date seqno.
	 * However, the up-to-date seqno is not always required and the last
	 * seen value is good enough. Note that the seqno will always be
	 * monotonic, even if not coherent.
	 */
<<<<<<< HEAD
	u32		(*get_seqno)(struct intel_engine_cs *ring,
				     bool lazy_coherency);
	void		(*set_seqno)(struct intel_engine_cs *ring,
				     u32 seqno);
	int		(*dispatch_execbuffer)(struct drm_i915_gem_request *req,
					       u64 offset, u32 length,
					       unsigned dispatch_flags);
#define I915_DISPATCH_SECURE 0x1
#define I915_DISPATCH_PINNED 0x2
#define I915_DISPATCH_RS     0x4
	void		(*cleanup)(struct intel_engine_cs *ring);
=======
	void		(*irq_seqno_barrier)(struct intel_engine_cs *engine);
	void		(*cleanup)(struct intel_engine_cs *engine);
>>>>>>> v4.9.227

	/* GEN8 signal/wait table - never trust comments!
	 *	  signal to	signal to    signal to   signal to      signal to
	 *	    RCS		   VCS          BCS        VECS		 VCS2
	 *      --------------------------------------------------------------------
	 *  RCS | NOP (0x00) | VCS (0x08) | BCS (0x10) | VECS (0x18) | VCS2 (0x20) |
	 *	|-------------------------------------------------------------------
	 *  VCS | RCS (0x28) | NOP (0x30) | BCS (0x38) | VECS (0x40) | VCS2 (0x48) |
	 *	|-------------------------------------------------------------------
	 *  BCS | RCS (0x50) | VCS (0x58) | NOP (0x60) | VECS (0x68) | VCS2 (0x70) |
	 *	|-------------------------------------------------------------------
	 * VECS | RCS (0x78) | VCS (0x80) | BCS (0x88) |  NOP (0x90) | VCS2 (0x98) |
	 *	|-------------------------------------------------------------------
	 * VCS2 | RCS (0xa0) | VCS (0xa8) | BCS (0xb0) | VECS (0xb8) | NOP  (0xc0) |
	 *	|-------------------------------------------------------------------
	 *
	 * Generalization:
	 *  f(x, y) := (x->id * NUM_RINGS * seqno_size) + (seqno_size * y->id)
	 *  ie. transpose of g(x, y)
	 *
	 *	 sync from	sync from    sync from    sync from	sync from
	 *	    RCS		   VCS          BCS        VECS		 VCS2
	 *      --------------------------------------------------------------------
	 *  RCS | NOP (0x00) | VCS (0x28) | BCS (0x50) | VECS (0x78) | VCS2 (0xa0) |
	 *	|-------------------------------------------------------------------
	 *  VCS | RCS (0x08) | NOP (0x30) | BCS (0x58) | VECS (0x80) | VCS2 (0xa8) |
	 *	|-------------------------------------------------------------------
	 *  BCS | RCS (0x10) | VCS (0x38) | NOP (0x60) | VECS (0x88) | VCS2 (0xb0) |
	 *	|-------------------------------------------------------------------
	 * VECS | RCS (0x18) | VCS (0x40) | BCS (0x68) |  NOP (0x90) | VCS2 (0xb8) |
	 *	|-------------------------------------------------------------------
	 * VCS2 | RCS (0x20) | VCS (0x48) | BCS (0x70) | VECS (0x98) |  NOP (0xc0) |
	 *	|-------------------------------------------------------------------
	 *
	 * Generalization:
	 *  g(x, y) := (y->id * NUM_RINGS * seqno_size) + (seqno_size * x->id)
	 *  ie. transpose of f(x, y)
	 */
	struct {
<<<<<<< HEAD
		u32	sync_seqno[I915_NUM_RINGS-1];

		union {
			struct {
				/* our mbox written by others */
				u32		wait[I915_NUM_RINGS];
				/* mboxes this ring signals to */
				u32		signal[I915_NUM_RINGS];
			} mbox;
			u64		signal_ggtt[I915_NUM_RINGS];
		};

		/* AKA wait() */
		int	(*sync_to)(struct drm_i915_gem_request *to_req,
				   struct intel_engine_cs *from,
				   u32 seqno);
		int	(*signal)(struct drm_i915_gem_request *signaller_req,
				  /* num_dwords needed by caller */
				  unsigned int num_dwords);
	} semaphore;

	/* Execlists */
	spinlock_t execlist_lock;
	struct list_head execlist_queue;
	struct list_head execlist_retired_req_list;
	u8 next_context_status_buffer;
	u32             irq_keep_mask; /* bitmask for interrupts that should not be masked */
	int		(*emit_request)(struct drm_i915_gem_request *request);
	int		(*emit_flush)(struct drm_i915_gem_request *request,
				      u32 invalidate_domains,
				      u32 flush_domains);
	int		(*emit_bb_start)(struct drm_i915_gem_request *req,
					 u64 offset, unsigned dispatch_flags);

	/**
	 * List of objects currently involved in rendering from the
	 * ringbuffer.
	 *
	 * Includes buffers having the contents of their GPU caches
	 * flushed, not necessarily primitives.  last_read_req
	 * represents when the rendering involved will be completed.
	 *
	 * A reference is held on the buffer while on this list.
	 */
	struct list_head active_list;
=======
		u32	sync_seqno[I915_NUM_ENGINES-1];

		union {
#define GEN6_SEMAPHORE_LAST	VECS_HW
#define GEN6_NUM_SEMAPHORES	(GEN6_SEMAPHORE_LAST + 1)
#define GEN6_SEMAPHORES_MASK	GENMASK(GEN6_SEMAPHORE_LAST, 0)
			struct {
				/* our mbox written by others */
				u32		wait[GEN6_NUM_SEMAPHORES];
				/* mboxes this ring signals to */
				i915_reg_t	signal[GEN6_NUM_SEMAPHORES];
			} mbox;
			u64		signal_ggtt[I915_NUM_ENGINES];
		};

		/* AKA wait() */
		int	(*sync_to)(struct drm_i915_gem_request *req,
				   struct drm_i915_gem_request *signal);
		int	(*signal)(struct drm_i915_gem_request *req);
	} semaphore;

	/* Execlists */
	struct tasklet_struct irq_tasklet;
	spinlock_t execlist_lock; /* used inside tasklet, use spin_lock_bh */
	struct execlist_port {
		struct drm_i915_gem_request *request;
		unsigned int count;
	} execlist_port[2];
	struct list_head execlist_queue;
	unsigned int fw_domains;
	bool disable_lite_restore_wa;
	bool preempt_wa;
	u32 ctx_desc_template;
>>>>>>> v4.9.227

	/**
	 * List of breadcrumbs associated with GPU requests currently
	 * outstanding.
	 */
	struct list_head request_list;

	/**
	 * Seqno of request most recently submitted to request_list.
	 * Used exclusively by hang checker to avoid grabbing lock while
	 * inspecting request list.
	 */
	u32 last_submitted_seqno;
<<<<<<< HEAD

	bool gpu_caches_dirty;

	wait_queue_head_t irq_queue;

	struct intel_context *default_context;
	struct intel_context *last_context;

	struct intel_ring_hangcheck hangcheck;

	struct {
		struct drm_i915_gem_object *obj;
		u32 gtt_offset;
		volatile u32 *cpu_page;
	} scratch;

	bool needs_cmd_parser;

	/*
	 * Table of commands the command parser needs to know about
	 * for this ring.
=======
	u32 last_pending_seqno;

	/* An RCU guarded pointer to the last request. No reference is
	 * held to the request, users must carefully acquire a reference to
	 * the request using i915_gem_active_get_rcu(), or hold the
	 * struct_mutex.
	 */
	struct i915_gem_active last_request;

	struct i915_gem_context *last_context;

	struct intel_engine_hangcheck hangcheck;

#define I915_ENGINE_USING_CMD_PARSER	BIT(0)
#define I915_ENGINE_REQUIRES_CMD_PARSER	BIT(3)
	unsigned int flags;

	/*
	 * Table of commands the command parser needs to know about
	 * for this engine.
>>>>>>> v4.9.227
	 */
	DECLARE_HASHTABLE(cmd_hash, I915_CMD_HASH_ORDER);

	/*
	 * Table of registers allowed in commands that read/write registers.
	 */
<<<<<<< HEAD
	const struct drm_i915_reg_descriptor *reg_table;
	int reg_count;

	/*
	 * Table of registers allowed in commands that read/write registers, but
	 * only from the DRM master.
	 */
	const struct drm_i915_reg_descriptor *master_reg_table;
	int master_reg_count;
=======
	const struct drm_i915_reg_table *reg_tables;
	int reg_table_count;
>>>>>>> v4.9.227

	/*
	 * Returns the bitmask for the length field of the specified command.
	 * Return 0 for an unrecognized/invalid command.
	 *
<<<<<<< HEAD
	 * If the command parser finds an entry for a command in the ring's
	 * cmd_tables, it gets the command's length based on the table entry.
	 * If not, it calls this function to determine the per-ring length field
	 * encoding for the command (i.e. certain opcode ranges use certain bits
	 * to encode the command length in the header).
=======
	 * If the command parser finds an entry for a command in the engine's
	 * cmd_tables, it gets the command's length based on the table entry.
	 * If not, it calls this function to determine the per-engine length
	 * field encoding for the command (i.e. different opcode ranges use
	 * certain bits to encode the command length in the header).
>>>>>>> v4.9.227
	 */
	u32 (*get_cmd_length_mask)(u32 cmd_header);
};

<<<<<<< HEAD
bool intel_ring_initialized(struct intel_engine_cs *ring);

static inline unsigned
intel_ring_flag(struct intel_engine_cs *ring)
{
	return 1 << ring->id;
}

static inline u32
intel_ring_sync_index(struct intel_engine_cs *ring,
		      struct intel_engine_cs *other)
=======
static inline bool
intel_engine_initialized(const struct intel_engine_cs *engine)
{
	return engine->i915 != NULL;
}

static inline bool
intel_engine_using_cmd_parser(const struct intel_engine_cs *engine)
{
	return engine->flags & I915_ENGINE_USING_CMD_PARSER;
}

static inline bool
intel_engine_requires_cmd_parser(const struct intel_engine_cs *engine)
{
	return engine->flags & I915_ENGINE_REQUIRES_CMD_PARSER;
}

static inline unsigned int
intel_engine_flag(const struct intel_engine_cs *engine)
{
	return 1 << engine->id;
}

static inline u32
intel_engine_sync_index(struct intel_engine_cs *engine,
			struct intel_engine_cs *other)
>>>>>>> v4.9.227
{
	int idx;

	/*
	 * rcs -> 0 = vcs, 1 = bcs, 2 = vecs, 3 = vcs2;
	 * vcs -> 0 = bcs, 1 = vecs, 2 = vcs2, 3 = rcs;
	 * bcs -> 0 = vecs, 1 = vcs2. 2 = rcs, 3 = vcs;
	 * vecs -> 0 = vcs2, 1 = rcs, 2 = vcs, 3 = bcs;
	 * vcs2 -> 0 = rcs, 1 = vcs, 2 = bcs, 3 = vecs;
	 */

<<<<<<< HEAD
	idx = (other - ring) - 1;
	if (idx < 0)
		idx += I915_NUM_RINGS;
=======
	idx = (other - engine) - 1;
	if (idx < 0)
		idx += I915_NUM_ENGINES;
>>>>>>> v4.9.227

	return idx;
}

static inline void
<<<<<<< HEAD
intel_flush_status_page(struct intel_engine_cs *ring, int reg)
{
	drm_clflush_virt_range(&ring->status_page.page_addr[reg],
			       sizeof(uint32_t));
}

static inline u32
intel_read_status_page(struct intel_engine_cs *ring,
		       int reg)
{
	/* Ensure that the compiler doesn't optimize away the load. */
	barrier();
	return ring->status_page.page_addr[reg];
}

static inline void
intel_write_status_page(struct intel_engine_cs *ring,
			int reg, u32 value)
{
	ring->status_page.page_addr[reg] = value;
}

/**
=======
intel_flush_status_page(struct intel_engine_cs *engine, int reg)
{
	mb();
	clflush(&engine->status_page.page_addr[reg]);
	mb();
}

static inline u32
intel_read_status_page(struct intel_engine_cs *engine, int reg)
{
	/* Ensure that the compiler doesn't optimize away the load. */
	return READ_ONCE(engine->status_page.page_addr[reg]);
}

static inline void
intel_write_status_page(struct intel_engine_cs *engine,
			int reg, u32 value)
{
	engine->status_page.page_addr[reg] = value;
}

/*
>>>>>>> v4.9.227
 * Reads a dword out of the status page, which is written to from the command
 * queue by automatic updates, MI_REPORT_HEAD, MI_STORE_DATA_INDEX, or
 * MI_STORE_DATA_IMM.
 *
 * The following dwords have a reserved meaning:
 * 0x00: ISR copy, updated when an ISR bit not set in the HWSTAM changes.
 * 0x04: ring 0 head pointer
 * 0x05: ring 1 head pointer (915-class)
 * 0x06: ring 2 head pointer (915-class)
 * 0x10-0x1b: Context status DWords (GM45)
 * 0x1f: Last written status offset. (GM45)
 * 0x20-0x2f: Reserved (Gen6+)
 *
 * The area from dword 0x30 to 0x3ff is available for driver usage.
 */
#define I915_GEM_HWS_INDEX		0x30
<<<<<<< HEAD
#define I915_GEM_HWS_SCRATCH_INDEX	0x40
#define I915_GEM_HWS_SCRATCH_ADDR (I915_GEM_HWS_SCRATCH_INDEX << MI_STORE_DWORD_INDEX_SHIFT)

struct intel_ringbuffer *
intel_engine_create_ringbuffer(struct intel_engine_cs *engine, int size);
int intel_pin_and_map_ringbuffer_obj(struct drm_device *dev,
				     struct intel_ringbuffer *ringbuf);
void intel_unpin_ringbuffer_obj(struct intel_ringbuffer *ringbuf);
void intel_ringbuffer_free(struct intel_ringbuffer *ring);

void intel_stop_ring_buffer(struct intel_engine_cs *ring);
void intel_cleanup_ring_buffer(struct intel_engine_cs *ring);
=======
#define I915_GEM_HWS_INDEX_ADDR (I915_GEM_HWS_INDEX << MI_STORE_DWORD_INDEX_SHIFT)
#define I915_GEM_HWS_SCRATCH_INDEX	0x40
#define I915_GEM_HWS_SCRATCH_ADDR (I915_GEM_HWS_SCRATCH_INDEX << MI_STORE_DWORD_INDEX_SHIFT)

struct intel_ring *
intel_engine_create_ring(struct intel_engine_cs *engine, int size);
int intel_ring_pin(struct intel_ring *ring);
void intel_ring_unpin(struct intel_ring *ring);
void intel_ring_free(struct intel_ring *ring);

void intel_engine_stop(struct intel_engine_cs *engine);
void intel_engine_cleanup(struct intel_engine_cs *engine);

void intel_legacy_submission_resume(struct drm_i915_private *dev_priv);
>>>>>>> v4.9.227

int intel_ring_alloc_request_extras(struct drm_i915_gem_request *request);

int __must_check intel_ring_begin(struct drm_i915_gem_request *req, int n);
int __must_check intel_ring_cacheline_align(struct drm_i915_gem_request *req);
<<<<<<< HEAD
static inline void intel_ring_emit(struct intel_engine_cs *ring,
				   u32 data)
{
	struct intel_ringbuffer *ringbuf = ring->buffer;
	iowrite32(data, ringbuf->virtual_start + ringbuf->tail);
	ringbuf->tail += 4;
}
static inline void intel_ring_advance(struct intel_engine_cs *ring)
{
	struct intel_ringbuffer *ringbuf = ring->buffer;
	ringbuf->tail &= ringbuf->size - 1;
}
int __intel_ring_space(int head, int tail, int size);
void intel_ring_update_space(struct intel_ringbuffer *ringbuf);
int intel_ring_space(struct intel_ringbuffer *ringbuf);
bool intel_ring_stopped(struct intel_engine_cs *ring);

int __must_check intel_ring_idle(struct intel_engine_cs *ring);
void intel_ring_init_seqno(struct intel_engine_cs *ring, u32 seqno);
int intel_ring_flush_all_caches(struct drm_i915_gem_request *req);
int intel_ring_invalidate_all_caches(struct drm_i915_gem_request *req);

void intel_fini_pipe_control(struct intel_engine_cs *ring);
int intel_init_pipe_control(struct intel_engine_cs *ring);

int intel_init_render_ring_buffer(struct drm_device *dev);
int intel_init_bsd_ring_buffer(struct drm_device *dev);
int intel_init_bsd2_ring_buffer(struct drm_device *dev);
int intel_init_blt_ring_buffer(struct drm_device *dev);
int intel_init_vebox_ring_buffer(struct drm_device *dev);

u64 intel_ring_get_active_head(struct intel_engine_cs *ring);

int init_workarounds_ring(struct intel_engine_cs *ring);

static inline u32 intel_ring_get_tail(struct intel_ringbuffer *ringbuf)
{
	return ringbuf->tail;
}

/*
 * Arbitrary size for largest possible 'add request' sequence. The code paths
 * are complex and variable. Empirical measurement shows that the worst case
 * is ILK at 136 words. Reserving too much is better than reserving too little
 * as that allows for corner cases that might have been missed. So the figure
 * has been rounded up to 160 words.
 */
#define MIN_SPACE_FOR_ADD_REQUEST	160

/*
 * Reserve space in the ring to guarantee that the i915_add_request() call
 * will always have sufficient room to do its stuff. The request creation
 * code calls this automatically.
 */
void intel_ring_reserved_space_reserve(struct intel_ringbuffer *ringbuf, int size);
/* Cancel the reservation, e.g. because the request is being discarded. */
void intel_ring_reserved_space_cancel(struct intel_ringbuffer *ringbuf);
/* Use the reserved space - for use by i915_add_request() only. */
void intel_ring_reserved_space_use(struct intel_ringbuffer *ringbuf);
/* Finish with the reserved space - for use by i915_add_request() only. */
void intel_ring_reserved_space_end(struct intel_ringbuffer *ringbuf);

/* Legacy ringbuffer specific portion of reservation code: */
int intel_ring_reserve_space(struct drm_i915_gem_request *request);
=======

static inline void intel_ring_emit(struct intel_ring *ring, u32 data)
{
	*(uint32_t *)(ring->vaddr + ring->tail) = data;
	ring->tail += 4;
}

static inline void intel_ring_emit_reg(struct intel_ring *ring, i915_reg_t reg)
{
	intel_ring_emit(ring, i915_mmio_reg_offset(reg));
}

static inline void intel_ring_advance(struct intel_ring *ring)
{
	/* Dummy function.
	 *
	 * This serves as a placeholder in the code so that the reader
	 * can compare against the preceding intel_ring_begin() and
	 * check that the number of dwords emitted matches the space
	 * reserved for the command packet (i.e. the value passed to
	 * intel_ring_begin()).
	 */
}

static inline u32 intel_ring_offset(struct intel_ring *ring, u32 value)
{
	/* Don't write ring->size (equivalent to 0) as that hangs some GPUs. */
	return value & (ring->size - 1);
}

int __intel_ring_space(int head, int tail, int size);
void intel_ring_update_space(struct intel_ring *ring);

void intel_engine_init_seqno(struct intel_engine_cs *engine, u32 seqno);

void intel_engine_setup_common(struct intel_engine_cs *engine);
int intel_engine_init_common(struct intel_engine_cs *engine);
int intel_engine_create_scratch(struct intel_engine_cs *engine, int size);
void intel_engine_cleanup_common(struct intel_engine_cs *engine);

static inline int intel_engine_idle(struct intel_engine_cs *engine,
				    unsigned int flags)
{
	/* Wait upon the last request to be completed */
	return i915_gem_active_wait_unlocked(&engine->last_request,
					     flags, NULL, NULL);
}

int intel_init_render_ring_buffer(struct intel_engine_cs *engine);
int intel_init_bsd_ring_buffer(struct intel_engine_cs *engine);
int intel_init_bsd2_ring_buffer(struct intel_engine_cs *engine);
int intel_init_blt_ring_buffer(struct intel_engine_cs *engine);
int intel_init_vebox_ring_buffer(struct intel_engine_cs *engine);

u64 intel_engine_get_active_head(struct intel_engine_cs *engine);
static inline u32 intel_engine_get_seqno(struct intel_engine_cs *engine)
{
	return intel_read_status_page(engine, I915_GEM_HWS_INDEX);
}

int init_workarounds_ring(struct intel_engine_cs *engine);

/*
 * Arbitrary size for largest possible 'add request' sequence. The code paths
 * are complex and variable. Empirical measurement shows that the worst case
 * is BDW at 192 bytes (6 + 6 + 36 dwords), then ILK at 136 bytes. However,
 * we need to allocate double the largest single packet within that emission
 * to account for tail wraparound (so 6 + 6 + 72 dwords for BDW).
 */
#define MIN_SPACE_FOR_ADD_REQUEST 336

static inline u32 intel_hws_seqno_address(struct intel_engine_cs *engine)
{
	return engine->status_page.ggtt_offset + I915_GEM_HWS_INDEX_ADDR;
}

/* intel_breadcrumbs.c -- user interrupt bottom-half for waiters */
int intel_engine_init_breadcrumbs(struct intel_engine_cs *engine);

static inline void intel_wait_init(struct intel_wait *wait, u32 seqno)
{
	wait->tsk = current;
	wait->seqno = seqno;
}

static inline bool intel_wait_complete(const struct intel_wait *wait)
{
	return RB_EMPTY_NODE(&wait->node);
}

bool intel_engine_add_wait(struct intel_engine_cs *engine,
			   struct intel_wait *wait);
void intel_engine_remove_wait(struct intel_engine_cs *engine,
			      struct intel_wait *wait);
void intel_engine_enable_signaling(struct drm_i915_gem_request *request);

static inline bool intel_engine_has_waiter(const struct intel_engine_cs *engine)
{
	return rcu_access_pointer(engine->breadcrumbs.irq_seqno_bh);
}

static inline bool intel_engine_wakeup(const struct intel_engine_cs *engine)
{
	bool wakeup = false;

	/* Note that for this not to dangerously chase a dangling pointer,
	 * we must hold the rcu_read_lock here.
	 *
	 * Also note that tsk is likely to be in !TASK_RUNNING state so an
	 * early test for tsk->state != TASK_RUNNING before wake_up_process()
	 * is unlikely to be beneficial.
	 */
	if (intel_engine_has_waiter(engine)) {
		struct task_struct *tsk;

		rcu_read_lock();
		tsk = rcu_dereference(engine->breadcrumbs.irq_seqno_bh);
		if (tsk)
			wakeup = wake_up_process(tsk);
		rcu_read_unlock();
	}

	return wakeup;
}

void intel_engine_reset_breadcrumbs(struct intel_engine_cs *engine);
void intel_engine_fini_breadcrumbs(struct intel_engine_cs *engine);
unsigned int intel_kick_waiters(struct drm_i915_private *i915);
unsigned int intel_kick_signalers(struct drm_i915_private *i915);

static inline bool intel_engine_is_active(struct intel_engine_cs *engine)
{
	return i915_gem_active_isset(&engine->last_request);
}
>>>>>>> v4.9.227

#endif /* _INTEL_RINGBUFFER_H_ */
