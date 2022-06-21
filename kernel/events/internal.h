#ifndef _KERNEL_EVENTS_INTERNAL_H
#define _KERNEL_EVENTS_INTERNAL_H

#include <linux/hardirq.h>
#include <linux/uaccess.h>

/* Buffer handling */

#define RING_BUFFER_WRITABLE		0x01

struct ring_buffer {
	atomic_t			refcount;
	struct rcu_head			rcu_head;
#ifdef CONFIG_PERF_USE_VMALLOC
	struct work_struct		work;
	int				page_order;	/* allocation order  */
#endif
	int				nr_pages;	/* nr of data pages  */
	int				overwrite;	/* can overwrite itself */
<<<<<<< HEAD
=======
	int				paused;		/* can write into ring buffer */
>>>>>>> v4.9.227

	atomic_t			poll;		/* POLL_ for wakeups */

	local_t				head;		/* write position    */
	local_t				nest;		/* nested writers    */
	local_t				events;		/* event limit       */
	local_t				wakeup;		/* wakeup stamp      */
	local_t				lost;		/* nr records lost   */

	long				watermark;	/* wakeup watermark  */
<<<<<<< HEAD
=======
	long				aux_watermark;
>>>>>>> v4.9.227
	/* poll crap */
	spinlock_t			event_lock;
	struct list_head		event_list;

	atomic_t			mmap_count;
	unsigned long			mmap_locked;
	struct user_struct		*mmap_user;

<<<<<<< HEAD
=======
	/* AUX area */
	local_t				aux_head;
	local_t				aux_nest;
	local_t				aux_wakeup;
	unsigned long			aux_pgoff;
	int				aux_nr_pages;
	int				aux_overwrite;
	atomic_t			aux_mmap_count;
	unsigned long			aux_mmap_locked;
	void				(*free_aux)(void *);
	atomic_t			aux_refcount;
	void				**aux_pages;
	void				*aux_priv;

>>>>>>> v4.9.227
	struct perf_event_mmap_page	*user_page;
	void				*data_pages[0];
};

extern void rb_free(struct ring_buffer *rb);
<<<<<<< HEAD
extern struct ring_buffer *
rb_alloc(int nr_pages, long watermark, int cpu, int flags);
extern void perf_event_wakeup(struct perf_event *event);

extern void
perf_event_header__init_id(struct perf_event_header *header,
			   struct perf_sample_data *data,
			   struct perf_event *event);
extern void
perf_event__output_id_sample(struct perf_event *event,
			     struct perf_output_handle *handle,
			     struct perf_sample_data *sample);
=======

static inline void rb_free_rcu(struct rcu_head *rcu_head)
{
	struct ring_buffer *rb;

	rb = container_of(rcu_head, struct ring_buffer, rcu_head);
	rb_free(rb);
}

static inline void rb_toggle_paused(struct ring_buffer *rb, bool pause)
{
	if (!pause && rb->nr_pages)
		rb->paused = 0;
	else
		rb->paused = 1;
}

extern struct ring_buffer *
rb_alloc(int nr_pages, long watermark, int cpu, int flags);
extern void perf_event_wakeup(struct perf_event *event);
extern int rb_alloc_aux(struct ring_buffer *rb, struct perf_event *event,
			pgoff_t pgoff, int nr_pages, long watermark, int flags);
extern void rb_free_aux(struct ring_buffer *rb);
extern struct ring_buffer *ring_buffer_get(struct perf_event *event);
extern void ring_buffer_put(struct ring_buffer *rb);

static inline bool rb_has_aux(struct ring_buffer *rb)
{
	return !!rb->aux_nr_pages;
}

void perf_event_aux_event(struct perf_event *event, unsigned long head,
			  unsigned long size, u64 flags);
>>>>>>> v4.9.227

extern struct page *
perf_mmap_to_page(struct ring_buffer *rb, unsigned long pgoff);

#ifdef CONFIG_PERF_USE_VMALLOC
/*
 * Back perf_mmap() with vmalloc memory.
 *
 * Required for architectures that have d-cache aliasing issues.
 */

static inline int page_order(struct ring_buffer *rb)
{
	return rb->page_order;
}

#else

static inline int page_order(struct ring_buffer *rb)
{
	return 0;
}
#endif

static inline unsigned long perf_data_size(struct ring_buffer *rb)
{
	return rb->nr_pages << (PAGE_SHIFT + page_order(rb));
}

<<<<<<< HEAD
#define DEFINE_OUTPUT_COPY(func_name, memcpy_func)			\
static inline unsigned long						\
func_name(struct perf_output_handle *handle,				\
	  const void *buf, unsigned long len)				\
=======
static inline unsigned long perf_aux_size(struct ring_buffer *rb)
{
	return rb->aux_nr_pages << PAGE_SHIFT;
}

#define __DEFINE_OUTPUT_COPY_BODY(advance_buf, memcpy_func, ...)	\
>>>>>>> v4.9.227
{									\
	unsigned long size, written;					\
									\
	do {								\
		size    = min(handle->size, len);			\
<<<<<<< HEAD
		written = memcpy_func(handle->addr, buf, size);		\
=======
		written = memcpy_func(__VA_ARGS__);			\
>>>>>>> v4.9.227
		written = size - written;				\
									\
		len -= written;						\
		handle->addr += written;				\
<<<<<<< HEAD
		buf += written;						\
=======
		if (advance_buf)					\
			buf += written;					\
>>>>>>> v4.9.227
		handle->size -= written;				\
		if (!handle->size) {					\
			struct ring_buffer *rb = handle->rb;		\
									\
			handle->page++;					\
			handle->page &= rb->nr_pages - 1;		\
			handle->addr = rb->data_pages[handle->page];	\
			handle->size = PAGE_SIZE << page_order(rb);	\
		}							\
	} while (len && written == size);				\
									\
	return len;							\
}

<<<<<<< HEAD
=======
#define DEFINE_OUTPUT_COPY(func_name, memcpy_func)			\
static inline unsigned long						\
func_name(struct perf_output_handle *handle,				\
	  const void *buf, unsigned long len)				\
__DEFINE_OUTPUT_COPY_BODY(true, memcpy_func, handle->addr, buf, size)

static inline unsigned long
__output_custom(struct perf_output_handle *handle, perf_copy_f copy_func,
		const void *buf, unsigned long len)
{
	unsigned long orig_len = len;
	__DEFINE_OUTPUT_COPY_BODY(false, copy_func, handle->addr, buf,
				  orig_len - len, size)
}

>>>>>>> v4.9.227
static inline unsigned long
memcpy_common(void *dst, const void *src, unsigned long n)
{
	memcpy(dst, src, n);
	return 0;
}

DEFINE_OUTPUT_COPY(__output_copy, memcpy_common)

static inline unsigned long
memcpy_skip(void *dst, const void *src, unsigned long n)
{
	return 0;
}

DEFINE_OUTPUT_COPY(__output_skip, memcpy_skip)

#ifndef arch_perf_out_copy_user
#define arch_perf_out_copy_user arch_perf_out_copy_user

static inline unsigned long
arch_perf_out_copy_user(void *dst, const void *src, unsigned long n)
{
	unsigned long ret;

	pagefault_disable();
	ret = __copy_from_user_inatomic(dst, src, n);
	pagefault_enable();

	return ret;
}
#endif

DEFINE_OUTPUT_COPY(__output_copy_user, arch_perf_out_copy_user)

/* Callchain handling */
extern struct perf_callchain_entry *
perf_callchain(struct perf_event *event, struct pt_regs *regs);
<<<<<<< HEAD
extern int get_callchain_buffers(void);
extern void put_callchain_buffers(void);
=======
>>>>>>> v4.9.227

static inline int get_recursion_context(int *recursion)
{
	int rctx;

	if (in_nmi())
		rctx = 3;
	else if (in_irq())
		rctx = 2;
	else if (in_softirq())
		rctx = 1;
	else
		rctx = 0;

	if (recursion[rctx])
		return -1;

	recursion[rctx]++;
	barrier();

	return rctx;
}

static inline void put_recursion_context(int *recursion, int rctx)
{
	barrier();
	recursion[rctx]--;
}

#ifdef CONFIG_HAVE_PERF_USER_STACK_DUMP
static inline bool arch_perf_have_user_stack_dump(void)
{
	return true;
}

#define perf_user_stack_pointer(regs) user_stack_pointer(regs)
#else
static inline bool arch_perf_have_user_stack_dump(void)
{
	return false;
}

#define perf_user_stack_pointer(regs) 0
#endif /* CONFIG_HAVE_PERF_USER_STACK_DUMP */

#endif /* _KERNEL_EVENTS_INTERNAL_H */
