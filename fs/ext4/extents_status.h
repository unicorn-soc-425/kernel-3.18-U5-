/*
 *  fs/ext4/extents_status.h
 *
 * Written by Yongqiang Yang <xiaoqiangnk@gmail.com>
 * Modified by
 *	Allison Henderson <achender@linux.vnet.ibm.com>
 *	Zheng Liu <wenqing.lz@taobao.com>
 *
 */

#ifndef _EXT4_EXTENTS_STATUS_H
#define _EXT4_EXTENTS_STATUS_H

/*
 * Turn on ES_DEBUG__ to get lots of info about extent status operations.
 */
#ifdef ES_DEBUG__
#define es_debug(fmt, ...)	printk(fmt, ##__VA_ARGS__)
#else
#define es_debug(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/*
 * With ES_AGGRESSIVE_TEST defined, the result of es caching will be
 * checked with old map_block's result.
 */
#define ES_AGGRESSIVE_TEST__

/*
 * These flags live in the high bits of extent_status.es_pblk
 */
<<<<<<< HEAD
#define ES_SHIFT	60

#define EXTENT_STATUS_WRITTEN	(1 << 3)
#define EXTENT_STATUS_UNWRITTEN (1 << 2)
#define EXTENT_STATUS_DELAYED	(1 << 1)
#define EXTENT_STATUS_HOLE	(1 << 0)

#define EXTENT_STATUS_FLAGS	(EXTENT_STATUS_WRITTEN | \
				 EXTENT_STATUS_UNWRITTEN | \
				 EXTENT_STATUS_DELAYED | \
				 EXTENT_STATUS_HOLE)

#define ES_WRITTEN		(1ULL << 63)
#define ES_UNWRITTEN		(1ULL << 62)
#define ES_DELAYED		(1ULL << 61)
#define ES_HOLE			(1ULL << 60)

#define ES_MASK			(ES_WRITTEN | ES_UNWRITTEN | \
				 ES_DELAYED | ES_HOLE)
=======
enum {
	ES_WRITTEN_B,
	ES_UNWRITTEN_B,
	ES_DELAYED_B,
	ES_HOLE_B,
	ES_REFERENCED_B,
	ES_FLAGS
};

#define ES_SHIFT (sizeof(ext4_fsblk_t)*8 - ES_FLAGS)
#define ES_MASK (~((ext4_fsblk_t)0) << ES_SHIFT)

#define EXTENT_STATUS_WRITTEN	(1 << ES_WRITTEN_B)
#define EXTENT_STATUS_UNWRITTEN (1 << ES_UNWRITTEN_B)
#define EXTENT_STATUS_DELAYED	(1 << ES_DELAYED_B)
#define EXTENT_STATUS_HOLE	(1 << ES_HOLE_B)
#define EXTENT_STATUS_REFERENCED	(1 << ES_REFERENCED_B)

#define ES_TYPE_MASK	((ext4_fsblk_t)(EXTENT_STATUS_WRITTEN | \
			  EXTENT_STATUS_UNWRITTEN | \
			  EXTENT_STATUS_DELAYED | \
			  EXTENT_STATUS_HOLE) << ES_SHIFT)
>>>>>>> v4.9.227

struct ext4_sb_info;
struct ext4_extent;

struct extent_status {
	struct rb_node rb_node;
	ext4_lblk_t es_lblk;	/* first logical block extent covers */
	ext4_lblk_t es_len;	/* length of extent in block */
	ext4_fsblk_t es_pblk;	/* first physical block */
};

struct ext4_es_tree {
	struct rb_root root;
	struct extent_status *cache_es;	/* recently accessed extent */
};

struct ext4_es_stats {
<<<<<<< HEAD
	unsigned long es_stats_last_sorted;
=======
>>>>>>> v4.9.227
	unsigned long es_stats_shrunk;
	unsigned long es_stats_cache_hits;
	unsigned long es_stats_cache_misses;
	u64 es_stats_scan_time;
	u64 es_stats_max_scan_time;
	struct percpu_counter es_stats_all_cnt;
<<<<<<< HEAD
	struct percpu_counter es_stats_lru_cnt;
=======
	struct percpu_counter es_stats_shk_cnt;
>>>>>>> v4.9.227
};

extern int __init ext4_init_es(void);
extern void ext4_exit_es(void);
extern void ext4_es_init_tree(struct ext4_es_tree *tree);

extern int ext4_es_insert_extent(struct inode *inode, ext4_lblk_t lblk,
				 ext4_lblk_t len, ext4_fsblk_t pblk,
				 unsigned int status);
extern void ext4_es_cache_extent(struct inode *inode, ext4_lblk_t lblk,
				 ext4_lblk_t len, ext4_fsblk_t pblk,
				 unsigned int status);
extern int ext4_es_remove_extent(struct inode *inode, ext4_lblk_t lblk,
				 ext4_lblk_t len);
extern void ext4_es_find_delayed_extent_range(struct inode *inode,
					ext4_lblk_t lblk, ext4_lblk_t end,
					struct extent_status *es);
extern int ext4_es_lookup_extent(struct inode *inode, ext4_lblk_t lblk,
				 struct extent_status *es);

<<<<<<< HEAD
static inline int ext4_es_is_written(struct extent_status *es)
{
	return (es->es_pblk & ES_WRITTEN) != 0;
=======
static inline unsigned int ext4_es_status(struct extent_status *es)
{
	return es->es_pblk >> ES_SHIFT;
}

static inline unsigned int ext4_es_type(struct extent_status *es)
{
	return (es->es_pblk & ES_TYPE_MASK) >> ES_SHIFT;
}

static inline int ext4_es_is_written(struct extent_status *es)
{
	return (ext4_es_type(es) & EXTENT_STATUS_WRITTEN) != 0;
>>>>>>> v4.9.227
}

static inline int ext4_es_is_unwritten(struct extent_status *es)
{
<<<<<<< HEAD
	return (es->es_pblk & ES_UNWRITTEN) != 0;
=======
	return (ext4_es_type(es) & EXTENT_STATUS_UNWRITTEN) != 0;
>>>>>>> v4.9.227
}

static inline int ext4_es_is_delayed(struct extent_status *es)
{
<<<<<<< HEAD
	return (es->es_pblk & ES_DELAYED) != 0;
=======
	return (ext4_es_type(es) & EXTENT_STATUS_DELAYED) != 0;
>>>>>>> v4.9.227
}

static inline int ext4_es_is_hole(struct extent_status *es)
{
<<<<<<< HEAD
	return (es->es_pblk & ES_HOLE) != 0;
}

static inline unsigned int ext4_es_status(struct extent_status *es)
{
	return es->es_pblk >> ES_SHIFT;
=======
	return (ext4_es_type(es) & EXTENT_STATUS_HOLE) != 0;
}

static inline void ext4_es_set_referenced(struct extent_status *es)
{
	es->es_pblk |= ((ext4_fsblk_t)EXTENT_STATUS_REFERENCED) << ES_SHIFT;
}

static inline void ext4_es_clear_referenced(struct extent_status *es)
{
	es->es_pblk &= ~(((ext4_fsblk_t)EXTENT_STATUS_REFERENCED) << ES_SHIFT);
}

static inline int ext4_es_is_referenced(struct extent_status *es)
{
	return (ext4_es_status(es) & EXTENT_STATUS_REFERENCED) != 0;
>>>>>>> v4.9.227
}

static inline ext4_fsblk_t ext4_es_pblock(struct extent_status *es)
{
	return es->es_pblk & ~ES_MASK;
}

static inline void ext4_es_store_pblock(struct extent_status *es,
					ext4_fsblk_t pb)
{
	ext4_fsblk_t block;

	block = (pb & ~ES_MASK) | (es->es_pblk & ES_MASK);
	es->es_pblk = block;
}

static inline void ext4_es_store_status(struct extent_status *es,
					unsigned int status)
{
<<<<<<< HEAD
	es->es_pblk = (((ext4_fsblk_t)
			(status & EXTENT_STATUS_FLAGS) << ES_SHIFT) |
		       (es->es_pblk & ~ES_MASK));
=======
	es->es_pblk = (((ext4_fsblk_t)status << ES_SHIFT) & ES_MASK) |
		      (es->es_pblk & ~ES_MASK);
>>>>>>> v4.9.227
}

static inline void ext4_es_store_pblock_status(struct extent_status *es,
					       ext4_fsblk_t pb,
					       unsigned int status)
{
<<<<<<< HEAD
	es->es_pblk = (((ext4_fsblk_t)
			(status & EXTENT_STATUS_FLAGS) << ES_SHIFT) |
		       (pb & ~ES_MASK));
=======
	es->es_pblk = (((ext4_fsblk_t)status << ES_SHIFT) & ES_MASK) |
		      (pb & ~ES_MASK);
>>>>>>> v4.9.227
}

extern int ext4_es_register_shrinker(struct ext4_sb_info *sbi);
extern void ext4_es_unregister_shrinker(struct ext4_sb_info *sbi);
<<<<<<< HEAD
extern void ext4_es_lru_add(struct inode *inode);
extern void ext4_es_lru_del(struct inode *inode);
=======

extern int ext4_seq_es_shrinker_info_show(struct seq_file *seq, void *v);
>>>>>>> v4.9.227

#endif /* _EXT4_EXTENTS_STATUS_H */
