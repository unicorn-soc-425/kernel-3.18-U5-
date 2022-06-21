#ifndef _LINUX_FRONTSWAP_H
#define _LINUX_FRONTSWAP_H

#include <linux/swap.h>
#include <linux/mm.h>
#include <linux/bitops.h>
<<<<<<< HEAD

struct frontswap_ops {
	void (*init)(unsigned);
	int (*store)(unsigned, pgoff_t, struct page *);
	int (*load)(unsigned, pgoff_t, struct page *);
	void (*invalidate_page)(unsigned, pgoff_t);
	void (*invalidate_area)(unsigned);
};

extern bool frontswap_enabled;
extern struct frontswap_ops *
	frontswap_register_ops(struct frontswap_ops *ops);
=======
#include <linux/jump_label.h>

struct frontswap_ops {
	void (*init)(unsigned); /* this swap type was just swapon'ed */
	int (*store)(unsigned, pgoff_t, struct page *); /* store a page */
	int (*load)(unsigned, pgoff_t, struct page *); /* load a page */
	void (*invalidate_page)(unsigned, pgoff_t); /* page no longer needed */
	void (*invalidate_area)(unsigned); /* swap type just swapoff'ed */
	struct frontswap_ops *next; /* private pointer to next ops */
};

extern void frontswap_register_ops(struct frontswap_ops *ops);
>>>>>>> v4.9.227
extern void frontswap_shrink(unsigned long);
extern unsigned long frontswap_curr_pages(void);
extern void frontswap_writethrough(bool);
#define FRONTSWAP_HAS_EXCLUSIVE_GETS
extern void frontswap_tmem_exclusive_gets(bool);

extern bool __frontswap_test(struct swap_info_struct *, pgoff_t);
extern void __frontswap_init(unsigned type, unsigned long *map);
extern int __frontswap_store(struct page *page);
extern int __frontswap_load(struct page *page);
extern void __frontswap_invalidate_page(unsigned, pgoff_t);
extern void __frontswap_invalidate_area(unsigned);

#ifdef CONFIG_FRONTSWAP
<<<<<<< HEAD
#define frontswap_enabled (1)
=======
extern struct static_key_false frontswap_enabled_key;

static inline bool frontswap_enabled(void)
{
	return static_branch_unlikely(&frontswap_enabled_key);
}
>>>>>>> v4.9.227

static inline bool frontswap_test(struct swap_info_struct *sis, pgoff_t offset)
{
	return __frontswap_test(sis, offset);
}

static inline void frontswap_map_set(struct swap_info_struct *p,
				     unsigned long *map)
{
	p->frontswap_map = map;
}

static inline unsigned long *frontswap_map_get(struct swap_info_struct *p)
{
	return p->frontswap_map;
}
#else
/* all inline routines become no-ops and all externs are ignored */

<<<<<<< HEAD
#define frontswap_enabled (0)
=======
static inline bool frontswap_enabled(void)
{
	return false;
}
>>>>>>> v4.9.227

static inline bool frontswap_test(struct swap_info_struct *sis, pgoff_t offset)
{
	return false;
}

static inline void frontswap_map_set(struct swap_info_struct *p,
				     unsigned long *map)
{
}

static inline unsigned long *frontswap_map_get(struct swap_info_struct *p)
{
	return NULL;
}
#endif

static inline int frontswap_store(struct page *page)
{
<<<<<<< HEAD
	int ret = -1;

	if (frontswap_enabled)
		ret = __frontswap_store(page);
	return ret;
=======
	if (frontswap_enabled())
		return __frontswap_store(page);

	return -1;
>>>>>>> v4.9.227
}

static inline int frontswap_load(struct page *page)
{
<<<<<<< HEAD
	int ret = -1;

	if (frontswap_enabled)
		ret = __frontswap_load(page);
	return ret;
=======
	if (frontswap_enabled())
		return __frontswap_load(page);

	return -1;
>>>>>>> v4.9.227
}

static inline void frontswap_invalidate_page(unsigned type, pgoff_t offset)
{
<<<<<<< HEAD
	if (frontswap_enabled)
=======
	if (frontswap_enabled())
>>>>>>> v4.9.227
		__frontswap_invalidate_page(type, offset);
}

static inline void frontswap_invalidate_area(unsigned type)
{
<<<<<<< HEAD
	if (frontswap_enabled)
=======
	if (frontswap_enabled())
>>>>>>> v4.9.227
		__frontswap_invalidate_area(type);
}

static inline void frontswap_init(unsigned type, unsigned long *map)
{
<<<<<<< HEAD
	if (frontswap_enabled)
		__frontswap_init(type, map);
=======
#ifdef CONFIG_FRONTSWAP
	__frontswap_init(type, map);
#endif
>>>>>>> v4.9.227
}

#endif /* _LINUX_FRONTSWAP_H */
