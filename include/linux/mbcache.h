<<<<<<< HEAD
/*
  File: linux/mbcache.h

  (C) 2001 by Andreas Gruenbacher, <a.gruenbacher@computer.org>
*/
struct mb_cache_entry {
	struct list_head		e_lru_list;
	struct mb_cache			*e_cache;
	unsigned short			e_used;
	unsigned short			e_queued;
	atomic_t			e_refcnt;
	struct block_device		*e_bdev;
	sector_t			e_block;
	struct hlist_bl_node		e_block_list;
	struct {
		struct hlist_bl_node	o_list;
		unsigned int		o_key;
	} e_index;
	struct hlist_bl_head		*e_block_hash_p;
	struct hlist_bl_head		*e_index_hash_p;
};

struct mb_cache {
	struct list_head		c_cache_list;
	const char			*c_name;
	atomic_t			c_entry_count;
	int				c_max_entries;
	int				c_bucket_bits;
	struct kmem_cache		*c_entry_cache;
	struct hlist_bl_head		*c_block_hash;
	struct hlist_bl_head		*c_index_hash;
};

/* Functions on caches */

struct mb_cache *mb_cache_create(const char *, int);
void mb_cache_shrink(struct block_device *);
void mb_cache_destroy(struct mb_cache *);

/* Functions on cache entries */

struct mb_cache_entry *mb_cache_entry_alloc(struct mb_cache *, gfp_t);
int mb_cache_entry_insert(struct mb_cache_entry *, struct block_device *,
			  sector_t, unsigned int);
void mb_cache_entry_release(struct mb_cache_entry *);
void mb_cache_entry_free(struct mb_cache_entry *);
struct mb_cache_entry *mb_cache_entry_get(struct mb_cache *,
					  struct block_device *,
					  sector_t);
struct mb_cache_entry *mb_cache_entry_find_first(struct mb_cache *cache,
						 struct block_device *, 
						 unsigned int);
struct mb_cache_entry *mb_cache_entry_find_next(struct mb_cache_entry *,
						struct block_device *, 
						unsigned int);
=======
#ifndef _LINUX_MBCACHE_H
#define _LINUX_MBCACHE_H

#include <linux/hash.h>
#include <linux/list_bl.h>
#include <linux/list.h>
#include <linux/atomic.h>
#include <linux/fs.h>

struct mb_cache;

struct mb_cache_entry {
	/* List of entries in cache - protected by cache->c_list_lock */
	struct list_head	e_list;
	/* Hash table list - protected by hash chain bitlock */
	struct hlist_bl_node	e_hash_list;
	atomic_t		e_refcnt;
	/* Key in hash - stable during lifetime of the entry */
	u32			e_key;
	u32			e_referenced:1;
	u32			e_reusable:1;
	/* Block number of hashed block - stable during lifetime of the entry */
	sector_t		e_block;
};

struct mb_cache *mb_cache_create(int bucket_bits);
void mb_cache_destroy(struct mb_cache *cache);

int mb_cache_entry_create(struct mb_cache *cache, gfp_t mask, u32 key,
			  sector_t block, bool reusable);
void __mb_cache_entry_free(struct mb_cache_entry *entry);
static inline int mb_cache_entry_put(struct mb_cache *cache,
				     struct mb_cache_entry *entry)
{
	if (!atomic_dec_and_test(&entry->e_refcnt))
		return 0;
	__mb_cache_entry_free(entry);
	return 1;
}

void mb_cache_entry_delete_block(struct mb_cache *cache, u32 key,
				  sector_t block);
struct mb_cache_entry *mb_cache_entry_get(struct mb_cache *cache, u32 key,
					  sector_t block);
struct mb_cache_entry *mb_cache_entry_find_first(struct mb_cache *cache,
						 u32 key);
struct mb_cache_entry *mb_cache_entry_find_next(struct mb_cache *cache,
						struct mb_cache_entry *entry);
void mb_cache_entry_touch(struct mb_cache *cache,
			  struct mb_cache_entry *entry);

#endif	/* _LINUX_MBCACHE_H */
>>>>>>> v4.9.227
