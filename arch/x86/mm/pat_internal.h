#ifndef __PAT_INTERNAL_H_
#define __PAT_INTERNAL_H_

extern int pat_debug_enable;

#define dprintk(fmt, arg...) \
<<<<<<< HEAD
	do { if (pat_debug_enable) printk(KERN_INFO fmt, ##arg); } while (0)
=======
	do { if (pat_debug_enable) pr_info("x86/PAT: " fmt, ##arg); } while (0)
>>>>>>> v4.9.227

struct memtype {
	u64			start;
	u64			end;
	u64			subtree_max_end;
<<<<<<< HEAD
	unsigned long		type;
	struct rb_node		rb;
};

static inline char *cattr_name(unsigned long flags)
{
	switch (flags & _PAGE_CACHE_MASK) {
	case _PAGE_CACHE_UC:		return "uncached";
	case _PAGE_CACHE_UC_MINUS:	return "uncached-minus";
	case _PAGE_CACHE_WB:		return "write-back";
	case _PAGE_CACHE_WC:		return "write-combining";
	default:			return "broken";
=======
	enum page_cache_mode	type;
	struct rb_node		rb;
};

static inline char *cattr_name(enum page_cache_mode pcm)
{
	switch (pcm) {
	case _PAGE_CACHE_MODE_UC:		return "uncached";
	case _PAGE_CACHE_MODE_UC_MINUS:		return "uncached-minus";
	case _PAGE_CACHE_MODE_WB:		return "write-back";
	case _PAGE_CACHE_MODE_WC:		return "write-combining";
	case _PAGE_CACHE_MODE_WT:		return "write-through";
	case _PAGE_CACHE_MODE_WP:		return "write-protected";
	default:				return "broken";
>>>>>>> v4.9.227
	}
}

#ifdef CONFIG_X86_PAT
extern int rbt_memtype_check_insert(struct memtype *new,
<<<<<<< HEAD
					unsigned long *new_type);
=======
					enum page_cache_mode *new_type);
>>>>>>> v4.9.227
extern struct memtype *rbt_memtype_erase(u64 start, u64 end);
extern struct memtype *rbt_memtype_lookup(u64 addr);
extern int rbt_memtype_copy_nth_element(struct memtype *out, loff_t pos);
#else
static inline int rbt_memtype_check_insert(struct memtype *new,
<<<<<<< HEAD
					unsigned long *new_type)
=======
					enum page_cache_mode *new_type)
>>>>>>> v4.9.227
{ return 0; }
static inline struct memtype *rbt_memtype_erase(u64 start, u64 end)
{ return NULL; }
static inline struct memtype *rbt_memtype_lookup(u64 addr)
{ return NULL; }
static inline int rbt_memtype_copy_nth_element(struct memtype *out, loff_t pos)
{ return 0; }
#endif

#endif /* __PAT_INTERNAL_H_ */
