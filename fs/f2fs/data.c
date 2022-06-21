/*
 * fs/f2fs/data.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/fs.h>
#include <linux/f2fs_fs.h>
#include <linux/buffer_head.h>
#include <linux/mpage.h>
<<<<<<< HEAD
#include <linux/aio.h>
=======
>>>>>>> v4.9.227
#include <linux/writeback.h>
#include <linux/backing-dev.h>
#include <linux/pagevec.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/prefetch.h>
#include <linux/uio.h>
<<<<<<< HEAD
=======
#include <linux/mm.h>
#include <linux/memcontrol.h>
>>>>>>> v4.9.227
#include <linux/cleancache.h>

#include "f2fs.h"
#include "node.h"
#include "segment.h"
#include "trace.h"
#include <trace/events/f2fs.h>
<<<<<<< HEAD
#include <trace/events/android_fs.h>

#define NUM_PREALLOC_POST_READ_CTXS	128

static struct kmem_cache *bio_post_read_ctx_cache;
static mempool_t *bio_post_read_ctx_pool;

static bool __is_cp_guaranteed(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode;
	struct f2fs_sb_info *sbi;

	if (!mapping)
		return false;

	inode = mapping->host;
	sbi = F2FS_I_SB(inode);

	if (inode->i_ino == F2FS_META_INO(sbi) ||
			inode->i_ino ==  F2FS_NODE_INO(sbi) ||
			S_ISDIR(inode->i_mode) ||
			(S_ISREG(inode->i_mode) &&
			is_inode_flag_set(inode, FI_ATOMIC_FILE)) ||
			is_cold_data(page))
		return true;
	return false;
}

/* postprocessing steps for read bios */
enum bio_post_read_step {
	STEP_INITIAL = 0,
	STEP_DECRYPT,
};

struct bio_post_read_ctx {
	struct bio *bio;
	struct work_struct work;
	unsigned int cur_step;
	unsigned int enabled_steps;
};

static void __read_end_io(struct bio *bio, int err)
{
	struct page *page;
	struct bio_vec *bv;
	int i;

	bio_for_each_segment_all(bv, bio, i) {
		page = bv->bv_page;

		/* PG_error was set if any post_read step failed */
		if (err || PageError(page)) {
			ClearPageUptodate(page);
			SetPageError(page);
		} else {
			SetPageUptodate(page);
		}
		unlock_page(page);
	}
	if (bio->bi_private)
		mempool_free(bio->bi_private, bio_post_read_ctx_pool);
	bio_put(bio);
}

static void bio_post_read_processing(struct bio_post_read_ctx *ctx);

static void decrypt_work(struct work_struct *work)
{
	struct bio_post_read_ctx *ctx =
		container_of(work, struct bio_post_read_ctx, work);

	fscrypt_decrypt_bio(ctx->bio);

	bio_post_read_processing(ctx);
}

static void bio_post_read_processing(struct bio_post_read_ctx *ctx)
{
	switch (++ctx->cur_step) {
	case STEP_DECRYPT:
		if (ctx->enabled_steps & (1 << STEP_DECRYPT)) {
			INIT_WORK(&ctx->work, decrypt_work);
			fscrypt_enqueue_decrypt_work(&ctx->work);
			return;
		}
		ctx->cur_step++;
		/* fall-through */
	default:
		__read_end_io(ctx->bio, 0);
	}
}

static bool f2fs_bio_post_read_required(struct bio *bio, int err)
{
	return bio->bi_private && !err;
}

static void f2fs_read_end_io(struct bio *bio, int err)
{
	}
	if (bio->bi_private)
		mempool_free(bio->bi_private, bio_post_read_ctx_pool);
	bio_put(bio);
}

static void bio_post_read_processing(struct bio_post_read_ctx *ctx);

static void decrypt_work(struct work_struct *work)
{
	struct bio_post_read_ctx *ctx =
		container_of(work, struct bio_post_read_ctx, work);

	fscrypt_decrypt_bio(ctx->bio);

	bio_post_read_processing(ctx);
}

static void bio_post_read_processing(struct bio_post_read_ctx *ctx)
{
	switch (++ctx->cur_step) {
	case STEP_DECRYPT:
		if (ctx->enabled_steps & (1 << STEP_DECRYPT)) {
			INIT_WORK(&ctx->work, decrypt_work);
			fscrypt_enqueue_decrypt_work(&ctx->work);
		ctx->cur_step++;
		/* fall-through */
	default:
		__read_end_io(ctx->bio, 0);
	}
}

static bool f2fs_bio_post_read_required(struct bio *bio, int err)
{
	return bio->bi_private && !err;
}

static void f2fs_read_end_io(struct bio *bio, int err)
{
	if (time_to_inject(F2FS_P_SB(bio->bi_io_vec->bv_page), FAULT_IO)) {
		f2fs_show_injection_info(FAULT_IO);
		err = -EIO;
	}

	if (f2fs_bio_post_read_required(bio, err)) {
		struct bio_post_read_ctx *ctx = bio->bi_private;
		ctx->cur_step = STEP_INITIAL;
		bio_post_read_processing(ctx);
		return;

	__read_end_io(bio, err);
}

static void f2fs_write_end_io(struct bio *bio, int err)
=======

static void f2fs_read_end_io(struct bio *bio)
{
	struct bio_vec *bvec;
	int i;

#ifdef CONFIG_F2FS_FAULT_INJECTION
	if (time_to_inject(F2FS_P_SB(bio->bi_io_vec->bv_page), FAULT_IO))
		bio->bi_error = -EIO;
#endif

	if (f2fs_bio_encrypted(bio)) {
		if (bio->bi_error) {
			fscrypt_release_ctx(bio->bi_private);
		} else {
			fscrypt_decrypt_bio_pages(bio->bi_private, bio);
			return;
		}
	}

	bio_for_each_segment_all(bvec, bio, i) {
		struct page *page = bvec->bv_page;

		if (!bio->bi_error) {
			if (!PageUptodate(page))
				SetPageUptodate(page);
		} else {
			ClearPageUptodate(page);
			SetPageError(page);
		}
		unlock_page(page);
	}
	bio_put(bio);
}

static void f2fs_write_end_io(struct bio *bio)
>>>>>>> v4.9.227
{
	struct f2fs_sb_info *sbi = bio->bi_private;
	struct bio_vec *bvec;
	int i;

	bio_for_each_segment_all(bvec, bio, i) {
		struct page *page = bvec->bv_page;
<<<<<<< HEAD
		enum count_type type = WB_DATA_TYPE(page);

		if (IS_DUMMY_WRITTEN_PAGE(page)) {
			set_page_private(page, (unsigned long)NULL);
			ClearPagePrivate(page);
			unlock_page(page);
			mempool_free(page, sbi->write_io_dummy);

			if (unlikely(err))
				f2fs_stop_checkpoint(sbi, true);
			continue;
		}

		fscrypt_pullback_bio_page(&page, true);

		if (unlikely(err)) {
			set_bit(AS_EIO, &page->mapping->flags);
			f2fs_bug_on(sbi, page->mapping == NODE_MAPPING(sbi) ||
					 page->mapping == META_MAPPING(sbi));
			if (type == F2FS_WB_CP_DATA)
				f2fs_stop_checkpoint(sbi, true);
		}

		f2fs_bug_on(sbi, page->mapping == NODE_MAPPING(sbi) &&
					page->index != nid_of_node(page));

		dec_page_count(sbi, type);
		if (f2fs_in_warm_node_list(sbi, page))
			f2fs_del_fsync_node_entry(sbi, page);
		clear_cold_data(page);
		end_page_writeback(page);
	}
	if (!get_pages(sbi, F2FS_WB_CP_DATA) &&
=======

		fscrypt_pullback_bio_page(&page, true);

		if (unlikely(bio->bi_error)) {
			mapping_set_error(page->mapping, -EIO);
			f2fs_stop_checkpoint(sbi, true);
		}
		end_page_writeback(page);
	}
	if (atomic_dec_and_test(&sbi->nr_wb_bios) &&
>>>>>>> v4.9.227
				wq_has_sleeper(&sbi->cp_wait))
		wake_up(&sbi->cp_wait);

	bio_put(bio);
}

/*
<<<<<<< HEAD
 * Return true, if pre_bio's bdev is same as its target device.
 */
struct block_device *f2fs_target_device(struct f2fs_sb_info *sbi,
				block_t blk_addr, struct bio *bio)
{
	struct block_device *bdev = sbi->sb->s_bdev;
	int i;

	for (i = 0; i < sbi->s_ndevs; i++) {
		if (FDEV(i).start_blk <= blk_addr &&
					FDEV(i).end_blk >= blk_addr) {
			blk_addr -= FDEV(i).start_blk;
			bdev = FDEV(i).bdev;
			break;
		}
	}
	if (bio) {
		bio->bi_bdev = bdev;
		bio->bi_iter.bi_sector = SECTOR_FROM_BLOCK(blk_addr);
	}
	return bdev;
}

int f2fs_target_device_index(struct f2fs_sb_info *sbi, block_t blkaddr)
{
	int i;

	for (i = 0; i < sbi->s_ndevs; i++)
		if (FDEV(i).start_blk <= blkaddr && FDEV(i).end_blk >= blkaddr)
			return i;
	return 0;
}

static bool __same_bdev(struct f2fs_sb_info *sbi,
				block_t blk_addr, struct bio *bio)
{
	return f2fs_target_device(sbi, blk_addr, NULL) == bio->bi_bdev;
}

/*
 * Low-level block read/write IO operations.
 */
static struct bio *__bio_alloc(struct f2fs_sb_info *sbi, block_t blk_addr,
				struct writeback_control *wbc,
				int npages, bool is_read,
				enum page_type type, enum temp_type temp)
{
	struct bio *bio;

	bio = f2fs_bio_alloc(sbi, npages, true);

	f2fs_target_device(sbi, blk_addr, bio);
	if (is_read) {
		bio->bi_end_io = f2fs_read_end_io;
		bio->bi_private = NULL;
	} else {
		bio->bi_end_io = f2fs_write_end_io;
		bio->bi_private = sbi;
		/* bio->bi_write_hint = f2fs_io_type_to_rw_hint(sbi, type, temp); */
	}
	if (wbc)
		wbc_init_bio(wbc, bio);
=======
 * Low-level block read/write IO operations.
 */
static struct bio *__bio_alloc(struct f2fs_sb_info *sbi, block_t blk_addr,
				int npages, bool is_read)
{
	struct bio *bio;

	bio = f2fs_bio_alloc(npages);

	bio->bi_bdev = sbi->sb->s_bdev;
	bio->bi_iter.bi_sector = SECTOR_FROM_BLOCK(blk_addr);
	bio->bi_end_io = is_read ? f2fs_read_end_io : f2fs_write_end_io;
	bio->bi_private = is_read ? NULL : sbi;
>>>>>>> v4.9.227

	return bio;
}

static inline void __submit_bio(struct f2fs_sb_info *sbi,
				struct bio *bio, enum page_type type)
{
	if (!is_read_io(bio_op(bio))) {
<<<<<<< HEAD
		unsigned int start;

		if (type != DATA && type != NODE)
			goto submit_io;

		if (test_opt(sbi, LFS) && current->plug)
			blk_finish_plug(current->plug);

		start = bio->bi_iter.bi_size >> F2FS_BLKSIZE_BITS;
		start %= F2FS_IO_SIZE(sbi);

		if (start == 0)
			goto submit_io;

		/* fill dummy pages */
		for (; start < F2FS_IO_SIZE(sbi); start++) {
			struct page *page =
				mempool_alloc(sbi->write_io_dummy,
					GFP_NOIO | __GFP_ZERO | __GFP_NOFAIL);
			f2fs_bug_on(sbi, !page);

			SetPagePrivate(page);
			set_page_private(page, (unsigned long)DUMMY_WRITTEN_PAGE);
			lock_page(page);
			if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE)
				f2fs_bug_on(sbi, 1);
		}
		/*
		 * In the NODE case, we lose next block address chain. So, we
		 * need to do checkpoint in f2fs_sync_file.
		 */
		if (type == NODE)
			set_sbi_flag(sbi, SBI_NEED_CP);
	}
submit_io:
	if (is_read_io(bio_op(bio)))
		trace_f2fs_submit_read_bio(sbi->sb, type, bio);
	else
		trace_f2fs_submit_write_bio(sbi->sb, type, bio);
	submit_bio(bio_op(bio), bio);
=======
		atomic_inc(&sbi->nr_wb_bios);
		if (f2fs_sb_mounted_hmsmr(sbi->sb) &&
			current->plug && (type == DATA || type == NODE))
			blk_finish_plug(current->plug);
	}
	submit_bio(bio);
>>>>>>> v4.9.227
}

static void __submit_merged_bio(struct f2fs_bio_info *io)
{
	struct f2fs_io_info *fio = &io->fio;

	if (!io->bio)
		return;

<<<<<<< HEAD
	bio_set_op_attrs(io->bio, fio->op, fio->op_flags);

	if (is_read_io(fio->op))
		trace_f2fs_prepare_read_bio(io->sbi->sb, fio->type, io->bio);
	else
		trace_f2fs_prepare_write_bio(io->sbi->sb, fio->type, io->bio);
=======
	if (is_read_io(fio->op))
		trace_f2fs_submit_read_bio(io->sbi->sb, fio, io->bio);
	else
		trace_f2fs_submit_write_bio(io->sbi->sb, fio, io->bio);

	bio_set_op_attrs(io->bio, fio->op, fio->op_flags);
>>>>>>> v4.9.227

	__submit_bio(io->sbi, io->bio, fio->type);
	io->bio = NULL;
}

<<<<<<< HEAD
static bool __has_merged_page(struct f2fs_bio_info *io,
				struct inode *inode, nid_t ino, pgoff_t idx)
=======
static bool __has_merged_page(struct f2fs_bio_info *io, struct inode *inode,
						struct page *page, nid_t ino)
>>>>>>> v4.9.227
{
	struct bio_vec *bvec;
	struct page *target;
	int i;

	if (!io->bio)
		return false;

<<<<<<< HEAD
	if (!inode && !ino)
=======
	if (!inode && !page && !ino)
>>>>>>> v4.9.227
		return true;

	bio_for_each_segment_all(bvec, io->bio, i) {

		if (bvec->bv_page->mapping)
			target = bvec->bv_page;
		else
			target = fscrypt_control_page(bvec->bv_page);

<<<<<<< HEAD
		if (idx != target->index)
			continue;

		if (inode && inode == target->mapping->host)
			return true;
=======
		if (inode && inode == target->mapping->host)
			return true;
		if (page && page == target)
			return true;
>>>>>>> v4.9.227
		if (ino && ino == ino_of_node(target))
			return true;
	}

	return false;
}

static bool has_merged_page(struct f2fs_sb_info *sbi, struct inode *inode,
<<<<<<< HEAD
				nid_t ino, pgoff_t idx, enum page_type type)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	enum temp_type temp;
	struct f2fs_bio_info *io;
	bool ret = false;

	for (temp = HOT; temp < NR_TEMP_TYPE; temp++) {
		io = sbi->write_io[btype] + temp;

		down_read(&io->io_rwsem);
		ret = __has_merged_page(io, inode, ino, idx);
		up_read(&io->io_rwsem);

		/* TODO: use HOT temp only for meta pages now. */
		if (ret || btype == META)
			break;
	}
	return ret;
}

static void __f2fs_submit_merged_write(struct f2fs_sb_info *sbi,
				enum page_type type, enum temp_type temp)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	struct f2fs_bio_info *io = sbi->write_io[btype] + temp;

	down_write(&io->io_rwsem);

=======
						struct page *page, nid_t ino,
						enum page_type type)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	struct f2fs_bio_info *io = &sbi->write_io[btype];
	bool ret;

	down_read(&io->io_rwsem);
	ret = __has_merged_page(io, inode, page, ino);
	up_read(&io->io_rwsem);
	return ret;
}

static void __f2fs_submit_merged_bio(struct f2fs_sb_info *sbi,
				struct inode *inode, struct page *page,
				nid_t ino, enum page_type type, int rw)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	struct f2fs_bio_info *io;

	io = is_read_io(rw) ? &sbi->read_io : &sbi->write_io[btype];

	down_write(&io->io_rwsem);

	if (!__has_merged_page(io, inode, page, ino))
		goto out;

>>>>>>> v4.9.227
	/* change META to META_FLUSH in the checkpoint procedure */
	if (type >= META_FLUSH) {
		io->fio.type = META_FLUSH;
		io->fio.op = REQ_OP_WRITE;
<<<<<<< HEAD
		io->fio.op_flags = REQ_META | REQ_PRIO;
		if (!test_opt(sbi, NOBARRIER))
			io->fio.op_flags |= WRITE_FLUSH | REQ_FUA;
	}
	__submit_merged_bio(io);
	up_write(&io->io_rwsem);
}

static void __submit_merged_write_cond(struct f2fs_sb_info *sbi,
				struct inode *inode, nid_t ino, pgoff_t idx,
				enum page_type type, bool force)
{
	enum temp_type temp;

	if (!force && !has_merged_page(sbi, inode, ino, idx, type))
		return;

	for (temp = HOT; temp < NR_TEMP_TYPE; temp++) {

		__f2fs_submit_merged_write(sbi, type, temp);

		/* TODO: use HOT temp only for meta pages now. */
		if (type >= META)
			break;
	}
}

void f2fs_submit_merged_write(struct f2fs_sb_info *sbi, enum page_type type)
{
	__submit_merged_write_cond(sbi, NULL, 0, 0, type, true);
}

void f2fs_submit_merged_write_cond(struct f2fs_sb_info *sbi,
				struct inode *inode, nid_t ino, pgoff_t idx,
				enum page_type type)
{
	__submit_merged_write_cond(sbi, inode, ino, idx, type, false);
}

void f2fs_flush_merged_writes(struct f2fs_sb_info *sbi)
{
	f2fs_submit_merged_write(sbi, DATA);
	f2fs_submit_merged_write(sbi, NODE);
	f2fs_submit_merged_write(sbi, META);
=======
		if (test_opt(sbi, NOBARRIER))
			io->fio.op_flags = WRITE_FLUSH | REQ_META | REQ_PRIO;
		else
			io->fio.op_flags = WRITE_FLUSH_FUA | REQ_META |
								REQ_PRIO;
	}
	__submit_merged_bio(io);
out:
	up_write(&io->io_rwsem);
}

void f2fs_submit_merged_bio(struct f2fs_sb_info *sbi, enum page_type type,
									int rw)
{
	__f2fs_submit_merged_bio(sbi, NULL, NULL, 0, type, rw);
}

void f2fs_submit_merged_bio_cond(struct f2fs_sb_info *sbi,
				struct inode *inode, struct page *page,
				nid_t ino, enum page_type type, int rw)
{
	if (has_merged_page(sbi, inode, page, ino, type))
		__f2fs_submit_merged_bio(sbi, inode, page, ino, type, rw);
}

void f2fs_flush_merged_bios(struct f2fs_sb_info *sbi)
{
	f2fs_submit_merged_bio(sbi, DATA, WRITE);
	f2fs_submit_merged_bio(sbi, NODE, WRITE);
	f2fs_submit_merged_bio(sbi, META, WRITE);
>>>>>>> v4.9.227
}

/*
 * Fill the locked page with data located in the block address.
<<<<<<< HEAD
 * A caller needs to unlock the page on failure.
=======
 * Return unlocked page.
>>>>>>> v4.9.227
 */
int f2fs_submit_page_bio(struct f2fs_io_info *fio)
{
	struct bio *bio;
	struct page *page = fio->encrypted_page ?
			fio->encrypted_page : fio->page;

	if (!f2fs_is_valid_blkaddr(fio->sbi, fio->new_blkaddr,
			__is_meta_io(fio) ? META_GENERIC : DATA_GENERIC))
		return -EFAULT;

	trace_f2fs_submit_page_bio(page, fio);
	f2fs_trace_ios(fio, 0);

	/* Allocate a new bio */
<<<<<<< HEAD
	bio = __bio_alloc(fio->sbi, fio->new_blkaddr, fio->io_wbc,
				1, is_read_io(fio->op), fio->type, fio->temp);
=======
	bio = __bio_alloc(fio->sbi, fio->new_blkaddr, 1, is_read_io(fio->op));
>>>>>>> v4.9.227

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		bio_put(bio);
		return -EFAULT;
	}
	bio_set_op_attrs(bio, fio->op, fio->op_flags);

	__submit_bio(fio->sbi, bio, fio->type);
<<<<<<< HEAD

	if (!is_read_io(fio->op))
		inc_page_count(fio->sbi, WB_DATA_TYPE(fio->page));
	return 0;
}

void f2fs_submit_page_write(struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = fio->sbi;
	enum page_type btype = PAGE_TYPE_OF_BIO(fio->type);
	struct f2fs_bio_info *io = sbi->write_io[btype] + fio->temp;
	struct page *bio_page;

	f2fs_bug_on(sbi, is_read_io(fio->op));

	down_write(&io->io_rwsem);
next:
	if (fio->in_list) {
		spin_lock(&io->io_lock);
		if (list_empty(&io->io_list)) {
			spin_unlock(&io->io_lock);
			goto out;
		}
		fio = list_first_entry(&io->io_list,
						struct f2fs_io_info, list);
		list_del(&fio->list);
		spin_unlock(&io->io_lock);
	}
=======
	return 0;
}

void f2fs_submit_page_mbio(struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = fio->sbi;
	enum page_type btype = PAGE_TYPE_OF_BIO(fio->type);
	struct f2fs_bio_info *io;
	bool is_read = is_read_io(fio->op);
	struct page *bio_page;

	io = is_read ? &sbi->read_io : &sbi->write_io[btype];
>>>>>>> v4.9.227

	if (__is_valid_data_blkaddr(fio->old_blkaddr))
		verify_block_addr(fio, fio->old_blkaddr);
	verify_block_addr(fio, fio->new_blkaddr);

<<<<<<< HEAD
	bio_page = fio->encrypted_page ? fio->encrypted_page : fio->page;

	/* set submitted = true as a return value */
	fio->submitted = true;

	inc_page_count(sbi, WB_DATA_TYPE(bio_page));

	if (io->bio && (io->last_block_in_bio != fio->new_blkaddr - 1 ||
	    (io->fio.op != fio->op || io->fio.op_flags != fio->op_flags) ||
			!__same_bdev(sbi, fio->new_blkaddr, io->bio)))
		__submit_merged_bio(io);
alloc_new:
	if (io->bio == NULL) {
		if ((fio->type == DATA || fio->type == NODE) &&
				fio->new_blkaddr & F2FS_IO_SIZE_MASK(sbi)) {
			dec_page_count(sbi, WB_DATA_TYPE(bio_page));
			fio->retry = true;
			goto skip;
		}
		io->bio = __bio_alloc(sbi, fio->new_blkaddr, fio->io_wbc,
						BIO_MAX_PAGES, false,
						fio->type, fio->temp);
		io->fio = *fio;
	}

	if (bio_add_page(io->bio, bio_page, PAGE_SIZE, 0) < PAGE_SIZE) {
=======
	down_write(&io->io_rwsem);

	if (io->bio && (io->last_block_in_bio != fio->new_blkaddr - 1 ||
	    (io->fio.op != fio->op || io->fio.op_flags != fio->op_flags)))
		__submit_merged_bio(io);
alloc_new:
	if (io->bio == NULL) {
		int bio_blocks = MAX_BIO_BLOCKS(sbi);

		io->bio = __bio_alloc(sbi, fio->new_blkaddr,
						bio_blocks, is_read);
		io->fio = *fio;
	}

	bio_page = fio->encrypted_page ? fio->encrypted_page : fio->page;

	if (bio_add_page(io->bio, bio_page, PAGE_SIZE, 0) <
							PAGE_SIZE) {
>>>>>>> v4.9.227
		__submit_merged_bio(io);
		goto alloc_new;
	}

<<<<<<< HEAD
	if (fio->io_wbc)
		wbc_account_io(fio->io_wbc, bio_page, PAGE_SIZE);

	io->last_block_in_bio = fio->new_blkaddr;
	f2fs_trace_ios(fio, 0);

	trace_f2fs_submit_page_write(fio->page, fio);
skip:
	if (fio->in_list)
		goto next;
out:
	up_write(&io->io_rwsem);
}

static struct bio *f2fs_grab_read_bio(struct inode *inode, block_t blkaddr,
					unsigned nr_pages, unsigned op_flag)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct bio *bio;
	struct bio_post_read_ctx *ctx;
	unsigned int post_read_steps = 0;

	if (!f2fs_is_valid_blkaddr(sbi, blkaddr, DATA_GENERIC))
		return ERR_PTR(-EFAULT);

	bio = f2fs_bio_alloc(sbi, min_t(int, nr_pages, BIO_MAX_PAGES), false);
	if (!bio)
		return ERR_PTR(-ENOMEM);
	f2fs_target_device(sbi, blkaddr, bio);
	bio->bi_end_io = f2fs_read_end_io;
	bio_set_op_attrs(bio, REQ_OP_READ, op_flag);

	if (f2fs_encrypted_file(inode))
		post_read_steps |= 1 << STEP_DECRYPT;
	if (post_read_steps) {
		ctx = mempool_alloc(bio_post_read_ctx_pool, GFP_NOFS);
		if (!ctx) {
			bio_put(bio);
			return ERR_PTR(-ENOMEM);
		}
		ctx->bio = bio;
		ctx->enabled_steps = post_read_steps;
		bio->bi_private = ctx;

		/* wait the page to be moved by cleaning */
		f2fs_wait_on_block_writeback(sbi, blkaddr);
	}

	return bio;
}

/* This can handle encryption stuffs */
static int f2fs_submit_page_read(struct inode *inode, struct page *page,
							block_t blkaddr)
{
	struct bio *bio = f2fs_grab_read_bio(inode, blkaddr, 1, 0);

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		bio_put(bio);
		return -EFAULT;
	}
	__submit_bio(F2FS_I_SB(inode), bio, DATA);
	return 0;
=======
	io->last_block_in_bio = fio->new_blkaddr;
	f2fs_trace_ios(fio, 0);

	up_write(&io->io_rwsem);
	trace_f2fs_submit_page_mbio(fio->page, fio);
>>>>>>> v4.9.227
}

static void __set_data_blkaddr(struct dnode_of_data *dn)
{
	struct f2fs_node *rn = F2FS_NODE(dn->node_page);
	__le32 *addr_array;
<<<<<<< HEAD
	int base = 0;

	if (IS_INODE(dn->node_page) && f2fs_has_extra_attr(dn->inode))
		base = get_extra_isize(dn->inode);

	/* Get physical address of data block */
	addr_array = blkaddr_in_node(rn);
	addr_array[base + dn->ofs_in_node] = cpu_to_le32(dn->data_blkaddr);
=======

	/* Get physical address of data block */
	addr_array = blkaddr_in_node(rn);
	addr_array[dn->ofs_in_node] = cpu_to_le32(dn->data_blkaddr);
>>>>>>> v4.9.227
}

/*
 * Lock ordering for the change of data block address:
 * ->data_page
 *  ->node_page
 *    update block addresses in the node page
 */
<<<<<<< HEAD
void f2fs_set_data_blkaddr(struct dnode_of_data *dn)
=======
void set_data_blkaddr(struct dnode_of_data *dn)
>>>>>>> v4.9.227
{
	f2fs_wait_on_page_writeback(dn->node_page, NODE, true);
	__set_data_blkaddr(dn);
	if (set_page_dirty(dn->node_page))
		dn->node_changed = true;
}

void f2fs_update_data_blkaddr(struct dnode_of_data *dn, block_t blkaddr)
{
	dn->data_blkaddr = blkaddr;
<<<<<<< HEAD
	f2fs_set_data_blkaddr(dn);
=======
	set_data_blkaddr(dn);
>>>>>>> v4.9.227
	f2fs_update_extent_cache(dn);
}

/* dn->ofs_in_node will be returned with up-to-date last block pointer */
<<<<<<< HEAD
int f2fs_reserve_new_blocks(struct dnode_of_data *dn, blkcnt_t count)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
	int err;
=======
int reserve_new_blocks(struct dnode_of_data *dn, blkcnt_t count)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
>>>>>>> v4.9.227

	if (!count)
		return 0;

	if (unlikely(is_inode_flag_set(dn->inode, FI_NO_ALLOC)))
		return -EPERM;
<<<<<<< HEAD
	if (unlikely((err = inc_valid_block_count(sbi, dn->inode, &count))))
		return err;
=======
	if (unlikely(!inc_valid_block_count(sbi, dn->inode, &count)))
		return -ENOSPC;
>>>>>>> v4.9.227

	trace_f2fs_reserve_new_blocks(dn->inode, dn->nid,
						dn->ofs_in_node, count);

	f2fs_wait_on_page_writeback(dn->node_page, NODE, true);

	for (; count > 0; dn->ofs_in_node++) {
<<<<<<< HEAD
		block_t blkaddr = datablock_addr(dn->inode,
					dn->node_page, dn->ofs_in_node);
=======
		block_t blkaddr =
			datablock_addr(dn->node_page, dn->ofs_in_node);
>>>>>>> v4.9.227
		if (blkaddr == NULL_ADDR) {
			dn->data_blkaddr = NEW_ADDR;
			__set_data_blkaddr(dn);
			count--;
		}
	}

	if (set_page_dirty(dn->node_page))
		dn->node_changed = true;
	return 0;
}

/* Should keep dn->ofs_in_node unchanged */
<<<<<<< HEAD
int f2fs_reserve_new_block(struct dnode_of_data *dn)
=======
int reserve_new_block(struct dnode_of_data *dn)
>>>>>>> v4.9.227
{
	unsigned int ofs_in_node = dn->ofs_in_node;
	int ret;

<<<<<<< HEAD
	ret = f2fs_reserve_new_blocks(dn, 1);
=======
	ret = reserve_new_blocks(dn, 1);
>>>>>>> v4.9.227
	dn->ofs_in_node = ofs_in_node;
	return ret;
}

int f2fs_reserve_block(struct dnode_of_data *dn, pgoff_t index)
{
	bool need_put = dn->inode_page ? false : true;
	int err;

<<<<<<< HEAD
	err = f2fs_get_dnode_of_data(dn, index, ALLOC_NODE);
=======
	err = get_dnode_of_data(dn, index, ALLOC_NODE);
>>>>>>> v4.9.227
	if (err)
		return err;

	if (dn->data_blkaddr == NULL_ADDR)
<<<<<<< HEAD
		err = f2fs_reserve_new_block(dn);
=======
		err = reserve_new_block(dn);
>>>>>>> v4.9.227
	if (err || need_put)
		f2fs_put_dnode(dn);
	return err;
}

int f2fs_get_block(struct dnode_of_data *dn, pgoff_t index)
{
<<<<<<< HEAD
	struct extent_info ei  = {0,0,0};
=======
	struct extent_info ei;
>>>>>>> v4.9.227
	struct inode *inode = dn->inode;

	if (f2fs_lookup_extent_cache(inode, index, &ei)) {
		dn->data_blkaddr = ei.blk + index - ei.fofs;
		return 0;
	}

	return f2fs_reserve_block(dn, index);
}

<<<<<<< HEAD
struct page *f2fs_get_read_data_page(struct inode *inode, pgoff_t index,
=======
struct page *get_read_data_page(struct inode *inode, pgoff_t index,
>>>>>>> v4.9.227
						int op_flags, bool for_write)
{
	struct address_space *mapping = inode->i_mapping;
	struct dnode_of_data dn;
	struct page *page;
<<<<<<< HEAD
	struct extent_info ei = {0,0,0};
	int err;
=======
	struct extent_info ei;
	int err;
	struct f2fs_io_info fio = {
		.sbi = F2FS_I_SB(inode),
		.type = DATA,
		.op = REQ_OP_READ,
		.op_flags = op_flags,
		.encrypted_page = NULL,
	};

	if (f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode))
		return read_mapping_page(mapping, index, NULL);
>>>>>>> v4.9.227

	page = f2fs_grab_cache_page(mapping, index, for_write);
	if (!page)
		return ERR_PTR(-ENOMEM);

	if (f2fs_lookup_extent_cache(inode, index, &ei)) {
		dn.data_blkaddr = ei.blk + index - ei.fofs;
		goto got_it;
	}

	set_new_dnode(&dn, inode, NULL, NULL, 0);
<<<<<<< HEAD
	err = f2fs_get_dnode_of_data(&dn, index, LOOKUP_NODE);
=======
	err = get_dnode_of_data(&dn, index, LOOKUP_NODE);
>>>>>>> v4.9.227
	if (err)
		goto put_err;
	f2fs_put_dnode(&dn);

	if (unlikely(dn.data_blkaddr == NULL_ADDR)) {
		err = -ENOENT;
		goto put_err;
	}
got_it:
	if (PageUptodate(page)) {
		unlock_page(page);
		return page;
	}

	/*
	 * A new dentry page is allocated but not able to be written, since its
	 * new inode page couldn't be allocated due to -ENOSPC.
	 * In such the case, its blkaddr can be remained as NEW_ADDR.
<<<<<<< HEAD
	 * see, f2fs_add_link -> f2fs_get_new_data_page ->
	 * f2fs_init_inode_metadata.
=======
	 * see, f2fs_add_link -> get_new_data_page -> init_inode_metadata.
>>>>>>> v4.9.227
	 */
	if (dn.data_blkaddr == NEW_ADDR) {
		zero_user_segment(page, 0, PAGE_SIZE);
		if (!PageUptodate(page))
			SetPageUptodate(page);
		unlock_page(page);
		return page;
	}

<<<<<<< HEAD
	err = f2fs_submit_page_read(inode, page, dn.data_blkaddr);
=======
	fio.new_blkaddr = fio.old_blkaddr = dn.data_blkaddr;
	fio.page = page;
	err = f2fs_submit_page_bio(&fio);
>>>>>>> v4.9.227
	if (err)
		goto put_err;
	return page;

put_err:
	f2fs_put_page(page, 1);
	return ERR_PTR(err);
}

<<<<<<< HEAD
struct page *f2fs_find_data_page(struct inode *inode, pgoff_t index)
=======
struct page *find_data_page(struct inode *inode, pgoff_t index)
>>>>>>> v4.9.227
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;

	page = find_get_page(mapping, index);
	if (page && PageUptodate(page))
		return page;
	f2fs_put_page(page, 0);

<<<<<<< HEAD
	page = f2fs_get_read_data_page(inode, index, 0, false);
=======
	page = get_read_data_page(inode, index, READ_SYNC, false);
>>>>>>> v4.9.227
	if (IS_ERR(page))
		return page;

	if (PageUptodate(page))
		return page;

	wait_on_page_locked(page);
	if (unlikely(!PageUptodate(page))) {
		f2fs_put_page(page, 0);
		return ERR_PTR(-EIO);
	}
	return page;
}

/*
 * If it tries to access a hole, return an error.
 * Because, the callers, functions in dir.c and GC, should be able to know
 * whether this page exists or not.
 */
<<<<<<< HEAD
struct page *f2fs_get_lock_data_page(struct inode *inode, pgoff_t index,
=======
struct page *get_lock_data_page(struct inode *inode, pgoff_t index,
>>>>>>> v4.9.227
							bool for_write)
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;
repeat:
<<<<<<< HEAD
	page = f2fs_get_read_data_page(inode, index, 0, for_write);
=======
	page = get_read_data_page(inode, index, READ_SYNC, for_write);
>>>>>>> v4.9.227
	if (IS_ERR(page))
		return page;

	/* wait for read completion */
	lock_page(page);
	if (unlikely(page->mapping != mapping)) {
		f2fs_put_page(page, 1);
		goto repeat;
	}
	if (unlikely(!PageUptodate(page))) {
		f2fs_put_page(page, 1);
		return ERR_PTR(-EIO);
	}
	return page;
}

/*
 * Caller ensures that this data page is never allocated.
 * A new zero-filled data page is allocated in the page cache.
 *
 * Also, caller should grab and release a rwsem by calling f2fs_lock_op() and
 * f2fs_unlock_op().
 * Note that, ipage is set only by make_empty_dir, and if any error occur,
 * ipage should be released by this function.
 */
<<<<<<< HEAD
struct page *f2fs_get_new_data_page(struct inode *inode,
=======
struct page *get_new_data_page(struct inode *inode,
>>>>>>> v4.9.227
		struct page *ipage, pgoff_t index, bool new_i_size)
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;
	struct dnode_of_data dn;
	int err;

	page = f2fs_grab_cache_page(mapping, index, true);
	if (!page) {
		/*
		 * before exiting, we should make sure ipage will be released
		 * if any error occur.
		 */
		f2fs_put_page(ipage, 1);
		return ERR_PTR(-ENOMEM);
	}

	set_new_dnode(&dn, inode, ipage, NULL, 0);
	err = f2fs_reserve_block(&dn, index);
	if (err) {
		f2fs_put_page(page, 1);
		return ERR_PTR(err);
	}
	if (!ipage)
		f2fs_put_dnode(&dn);

	if (PageUptodate(page))
		goto got_it;

	if (dn.data_blkaddr == NEW_ADDR) {
		zero_user_segment(page, 0, PAGE_SIZE);
		if (!PageUptodate(page))
			SetPageUptodate(page);
	} else {
		f2fs_put_page(page, 1);

		/* if ipage exists, blkaddr should be NEW_ADDR */
		f2fs_bug_on(F2FS_I_SB(inode), ipage);
<<<<<<< HEAD
		page = f2fs_get_lock_data_page(inode, index, true);
=======
		page = get_lock_data_page(inode, index, true);
>>>>>>> v4.9.227
		if (IS_ERR(page))
			return page;
	}
got_it:
	if (new_i_size && i_size_read(inode) <
				((loff_t)(index + 1) << PAGE_SHIFT))
		f2fs_i_size_write(inode, ((loff_t)(index + 1) << PAGE_SHIFT));
	return page;
}

<<<<<<< HEAD
static int __allocate_data_block(struct dnode_of_data *dn, int seg_type)
=======
static int __allocate_data_block(struct dnode_of_data *dn)
>>>>>>> v4.9.227
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
	struct f2fs_summary sum;
	struct node_info ni;
<<<<<<< HEAD
	block_t old_blkaddr;
	pgoff_t fofs;
	blkcnt_t count = 1;
	int err;
=======
	int seg = CURSEG_WARM_DATA;
	pgoff_t fofs;
	blkcnt_t count = 1;
>>>>>>> v4.9.227

	if (unlikely(is_inode_flag_set(dn->inode, FI_NO_ALLOC)))
		return -EPERM;

<<<<<<< HEAD
	err = f2fs_get_node_info(sbi, dn->nid, &ni);
	if (err)
		return err;

	dn->data_blkaddr = datablock_addr(dn->inode,
				dn->node_page, dn->ofs_in_node);
	if (dn->data_blkaddr == NEW_ADDR)
		goto alloc;

	if (unlikely((err = inc_valid_block_count(sbi, dn->inode, &count))))
		return err;

alloc:
	set_summary(&sum, dn->nid, dn->ofs_in_node, ni.version);
	old_blkaddr = dn->data_blkaddr;
	f2fs_allocate_data_block(sbi, NULL, old_blkaddr, &dn->data_blkaddr,
					&sum, seg_type, NULL, false);
	if (GET_SEGNO(sbi, old_blkaddr) != NULL_SEGNO)
		invalidate_mapping_pages(META_MAPPING(sbi),
					old_blkaddr, old_blkaddr);
	f2fs_set_data_blkaddr(dn);

	/* update i_size */
	fofs = f2fs_start_bidx_of_node(ofs_of_node(dn->node_page), dn->inode) +
=======
	dn->data_blkaddr = datablock_addr(dn->node_page, dn->ofs_in_node);
	if (dn->data_blkaddr == NEW_ADDR)
		goto alloc;

	if (unlikely(!inc_valid_block_count(sbi, dn->inode, &count)))
		return -ENOSPC;

alloc:
	get_node_info(sbi, dn->nid, &ni);
	set_summary(&sum, dn->nid, dn->ofs_in_node, ni.version);

	if (dn->ofs_in_node == 0 && dn->inode_page == dn->node_page)
		seg = CURSEG_DIRECT_IO;

	allocate_data_block(sbi, NULL, dn->data_blkaddr, &dn->data_blkaddr,
								&sum, seg);
	set_data_blkaddr(dn);

	/* update i_size */
	fofs = start_bidx_of_node(ofs_of_node(dn->node_page), dn->inode) +
>>>>>>> v4.9.227
							dn->ofs_in_node;
	if (i_size_read(dn->inode) < ((loff_t)(fofs + 1) << PAGE_SHIFT))
		f2fs_i_size_write(dn->inode,
				((loff_t)(fofs + 1) << PAGE_SHIFT));
	return 0;
}

<<<<<<< HEAD
int f2fs_preallocate_blocks(struct inode *inode, loff_t pos,
				size_t count, bool direct_io)
{
	struct f2fs_map_blocks map;
	int flag;
	int err = 0;

	/* convert inline data for Direct I/O*/
	if (direct_io) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
	}

	if (is_inode_flag_set(inode, FI_NO_PREALLOC))
		return 0;

	map.m_lblk = F2FS_BLK_ALIGN(pos);
	map.m_len = F2FS_BYTES_TO_BLK(pos + count);
=======
ssize_t f2fs_preallocate_blocks(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct f2fs_map_blocks map;
	ssize_t ret = 0;

	map.m_lblk = F2FS_BLK_ALIGN(iocb->ki_pos);
	map.m_len = F2FS_BYTES_TO_BLK(iocb->ki_pos + iov_iter_count(from));
>>>>>>> v4.9.227
	if (map.m_len > map.m_lblk)
		map.m_len -= map.m_lblk;
	else
		map.m_len = 0;

	map.m_next_pgofs = NULL;
<<<<<<< HEAD
	map.m_next_extent = NULL;
	map.m_seg_type = NO_CHECK_TYPE;

	if (direct_io) {
		/* map.m_seg_type = f2fs_rw_hint_to_seg_type(iocb->ki_hint); */
		map.m_seg_type = f2fs_rw_hint_to_seg_type(WRITE_LIFE_NOT_SET);
		flag = f2fs_force_buffered_io(inode, WRITE) ?
					F2FS_GET_BLOCK_PRE_AIO :
					F2FS_GET_BLOCK_PRE_DIO;
		goto map_blocks;
	}
	if (pos + count > MAX_INLINE_DATA(inode)) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
	}
	if (f2fs_has_inline_data(inode))
		return err;

	flag = F2FS_GET_BLOCK_PRE_AIO;

map_blocks:
	err = f2fs_map_blocks(inode, &map, 1, flag);
	if (map.m_len > 0 && err == -ENOSPC) {
		if (!direct_io)
			set_inode_flag(inode, FI_NO_PREALLOC);
		err = 0;
	}
	return err;
}

static inline void __do_map_lock(struct f2fs_sb_info *sbi, int flag, bool lock)
{
	if (flag == F2FS_GET_BLOCK_PRE_AIO) {
		if (lock)
			down_read(&sbi->node_change);
		else
			up_read(&sbi->node_change);
	} else {
		if (lock)
			f2fs_lock_op(sbi);
		else
			f2fs_unlock_op(sbi);
	}
=======

	if (iocb->ki_flags & IOCB_DIRECT) {
		ret = f2fs_convert_inline_inode(inode);
		if (ret)
			return ret;
		return f2fs_map_blocks(inode, &map, 1, F2FS_GET_BLOCK_PRE_DIO);
	}
	if (iocb->ki_pos + iov_iter_count(from) > MAX_INLINE_DATA) {
		ret = f2fs_convert_inline_inode(inode);
		if (ret)
			return ret;
	}
	if (!f2fs_has_inline_data(inode))
		return f2fs_map_blocks(inode, &map, 1, F2FS_GET_BLOCK_PRE_AIO);
	return ret;
>>>>>>> v4.9.227
}

/*
 * f2fs_map_blocks() now supported readahead/bmap/rw direct_IO with
 * f2fs_map_blocks structure.
 * If original data blocks are allocated, then give them to blockdev.
 * Otherwise,
 *     a. preallocate requested block addresses
 *     b. do not use extent cache for better performance
 *     c. give the block addresses to blockdev
 */
int f2fs_map_blocks(struct inode *inode, struct f2fs_map_blocks *map,
						int create, int flag)
{
	unsigned int maxblocks = map->m_len;
	struct dnode_of_data dn;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int mode = create ? ALLOC_NODE : LOOKUP_NODE;
	pgoff_t pgofs, end_offset, end;
	int err = 0, ofs = 1;
	unsigned int ofs_in_node, last_ofs_in_node;
	blkcnt_t prealloc;
<<<<<<< HEAD
	struct extent_info ei = {0,0,0};
	block_t blkaddr;
	unsigned int start_pgofs;
=======
	struct extent_info ei;
	bool allocated = false;
	block_t blkaddr;
>>>>>>> v4.9.227

	if (!maxblocks)
		return 0;

	map->m_len = 0;
	map->m_flags = 0;

	/* it only supports block size == page size */
	pgofs =	(pgoff_t)map->m_lblk;
	end = pgofs + maxblocks;

	if (!create && f2fs_lookup_extent_cache(inode, pgofs, &ei)) {
		map->m_pblk = ei.blk + pgofs - ei.fofs;
		map->m_len = min((pgoff_t)maxblocks, ei.fofs + ei.len - pgofs);
		map->m_flags = F2FS_MAP_MAPPED;
<<<<<<< HEAD
		if (map->m_next_extent)
			*map->m_next_extent = pgofs + map->m_len;
=======
>>>>>>> v4.9.227
		goto out;
	}

next_dnode:
	if (create)
<<<<<<< HEAD
		__do_map_lock(sbi, flag, true);

	/* When reading holes, we need its node page */
	set_new_dnode(&dn, inode, NULL, NULL, 0);
	err = f2fs_get_dnode_of_data(&dn, pgofs, mode);
=======
		f2fs_lock_op(sbi);

	/* When reading holes, we need its node page */
	set_new_dnode(&dn, inode, NULL, NULL, 0);
	err = get_dnode_of_data(&dn, pgofs, mode);
>>>>>>> v4.9.227
	if (err) {
		if (flag == F2FS_GET_BLOCK_BMAP)
			map->m_pblk = 0;
		if (err == -ENOENT) {
			err = 0;
			if (map->m_next_pgofs)
				*map->m_next_pgofs =
<<<<<<< HEAD
					f2fs_get_next_page_offset(&dn, pgofs);
			if (map->m_next_extent)
				*map->m_next_extent =
					f2fs_get_next_page_offset(&dn, pgofs);
=======
					get_next_page_offset(&dn, pgofs);
>>>>>>> v4.9.227
		}
		goto unlock_out;
	}

<<<<<<< HEAD
	start_pgofs = pgofs;
=======
>>>>>>> v4.9.227
	prealloc = 0;
	last_ofs_in_node = ofs_in_node = dn.ofs_in_node;
	end_offset = ADDRS_PER_PAGE(dn.node_page, inode);

next_block:
<<<<<<< HEAD
	blkaddr = datablock_addr(dn.inode, dn.node_page, dn.ofs_in_node);
=======
	blkaddr = datablock_addr(dn.node_page, dn.ofs_in_node);
>>>>>>> v4.9.227

	if (__is_valid_data_blkaddr(blkaddr) &&
		!f2fs_is_valid_blkaddr(sbi, blkaddr, DATA_GENERIC)) {
		err = -EFAULT;
		goto sync_out;
	}

	if (!is_valid_data_blkaddr(sbi, blkaddr)) {
		if (create) {
			if (unlikely(f2fs_cp_error(sbi))) {
				err = -EIO;
				goto sync_out;
			}
			if (flag == F2FS_GET_BLOCK_PRE_AIO) {
				if (blkaddr == NULL_ADDR) {
					prealloc++;
					last_ofs_in_node = dn.ofs_in_node;
				}
			} else {
<<<<<<< HEAD
				err = __allocate_data_block(&dn,
							map->m_seg_type);
				if (!err)
					set_inode_flag(inode, FI_APPEND_WRITE);
			}
			if (err)
				goto sync_out;
			map->m_flags |= F2FS_MAP_NEW;
=======
				err = __allocate_data_block(&dn);
				if (!err) {
					set_inode_flag(inode, FI_APPEND_WRITE);
					allocated = true;
				}
			}
			if (err)
				goto sync_out;
			map->m_flags = F2FS_MAP_NEW;
>>>>>>> v4.9.227
			blkaddr = dn.data_blkaddr;
		} else {
			if (flag == F2FS_GET_BLOCK_BMAP) {
				map->m_pblk = 0;
				goto sync_out;
			}
<<<<<<< HEAD
			if (flag == F2FS_GET_BLOCK_PRECACHE)
				goto sync_out;
=======
>>>>>>> v4.9.227
			if (flag == F2FS_GET_BLOCK_FIEMAP &&
						blkaddr == NULL_ADDR) {
				if (map->m_next_pgofs)
					*map->m_next_pgofs = pgofs + 1;
<<<<<<< HEAD
				goto sync_out;
			}
			if (flag != F2FS_GET_BLOCK_FIEMAP) {
				/* for defragment case */
				if (map->m_next_pgofs)
					*map->m_next_pgofs = pgofs + 1;
				goto sync_out;
			}
=======
			}
			if (flag != F2FS_GET_BLOCK_FIEMAP ||
						blkaddr != NEW_ADDR)
				goto sync_out;
>>>>>>> v4.9.227
		}
	}

	if (flag == F2FS_GET_BLOCK_PRE_AIO)
		goto skip;

	if (map->m_len == 0) {
		/* preallocated unwritten block should be mapped for fiemap. */
		if (blkaddr == NEW_ADDR)
			map->m_flags |= F2FS_MAP_UNWRITTEN;
		map->m_flags |= F2FS_MAP_MAPPED;

		map->m_pblk = blkaddr;
		map->m_len = 1;
	} else if ((map->m_pblk != NEW_ADDR &&
			blkaddr == (map->m_pblk + ofs)) ||
			(map->m_pblk == NEW_ADDR && blkaddr == NEW_ADDR) ||
			flag == F2FS_GET_BLOCK_PRE_DIO) {
		ofs++;
		map->m_len++;
	} else {
		goto sync_out;
	}

skip:
	dn.ofs_in_node++;
	pgofs++;

	/* preallocate blocks in batch for one dnode page */
	if (flag == F2FS_GET_BLOCK_PRE_AIO &&
			(pgofs == end || dn.ofs_in_node == end_offset)) {

		dn.ofs_in_node = ofs_in_node;
<<<<<<< HEAD
		err = f2fs_reserve_new_blocks(&dn, prealloc);
		if (err)
			goto sync_out;
=======
		err = reserve_new_blocks(&dn, prealloc);
		if (err)
			goto sync_out;
		allocated = dn.node_changed;
>>>>>>> v4.9.227

		map->m_len += dn.ofs_in_node - ofs_in_node;
		if (prealloc && dn.ofs_in_node != last_ofs_in_node + 1) {
			err = -ENOSPC;
			goto sync_out;
		}
		dn.ofs_in_node = end_offset;
	}

	if (pgofs >= end)
		goto sync_out;
	else if (dn.ofs_in_node < end_offset)
		goto next_block;

<<<<<<< HEAD
	if (flag == F2FS_GET_BLOCK_PRECACHE) {
		if (map->m_flags & F2FS_MAP_MAPPED) {
			unsigned int ofs = start_pgofs - map->m_lblk;

			f2fs_update_extent_cache_range(&dn,
				start_pgofs, map->m_pblk + ofs,
				map->m_len - ofs);
		}
	}

	f2fs_put_dnode(&dn);

	if (create) {
		__do_map_lock(sbi, flag, false);
		f2fs_balance_fs(sbi, dn.node_changed);
	}
	goto next_dnode;

sync_out:
	if (flag == F2FS_GET_BLOCK_PRECACHE) {
		if (map->m_flags & F2FS_MAP_MAPPED) {
			unsigned int ofs = start_pgofs - map->m_lblk;

			f2fs_update_extent_cache_range(&dn,
				start_pgofs, map->m_pblk + ofs,
				map->m_len - ofs);
		}
		if (map->m_next_extent)
			*map->m_next_extent = pgofs + 1;
	}
	f2fs_put_dnode(&dn);
unlock_out:
	if (create) {
		__do_map_lock(sbi, flag, false);
		f2fs_balance_fs(sbi, dn.node_changed);
=======
	f2fs_put_dnode(&dn);

	if (create) {
		f2fs_unlock_op(sbi);
		f2fs_balance_fs(sbi, allocated);
	}
	allocated = false;
	goto next_dnode;

sync_out:
	f2fs_put_dnode(&dn);
unlock_out:
	if (create) {
		f2fs_unlock_op(sbi);
		f2fs_balance_fs(sbi, allocated);
>>>>>>> v4.9.227
	}
out:
	trace_f2fs_map_blocks(inode, map, err);
	return err;
}

static int __get_data_block(struct inode *inode, sector_t iblock,
			struct buffer_head *bh, int create, int flag,
<<<<<<< HEAD
			pgoff_t *next_pgofs, int seg_type)
{
	struct f2fs_map_blocks map;
	int err;
=======
			pgoff_t *next_pgofs)
{
	struct f2fs_map_blocks map;
	int ret;
>>>>>>> v4.9.227

	map.m_lblk = iblock;
	map.m_len = bh->b_size >> inode->i_blkbits;
	map.m_next_pgofs = next_pgofs;
<<<<<<< HEAD
	map.m_next_extent = NULL;
	map.m_seg_type = seg_type;

	err = f2fs_map_blocks(inode, &map, create, flag);
	if (!err) {
=======

	ret = f2fs_map_blocks(inode, &map, create, flag);
	if (!ret) {
>>>>>>> v4.9.227
		map_bh(bh, inode->i_sb, map.m_pblk);
		bh->b_state = (bh->b_state & ~F2FS_MAP_FLAGS) | map.m_flags;
		bh->b_size = (u64)map.m_len << inode->i_blkbits;
	}
<<<<<<< HEAD
	return err;
=======
	return ret;
>>>>>>> v4.9.227
}

static int get_data_block(struct inode *inode, sector_t iblock,
			struct buffer_head *bh_result, int create, int flag,
			pgoff_t *next_pgofs)
{
	return __get_data_block(inode, iblock, bh_result, create,
<<<<<<< HEAD
							flag, next_pgofs,
							NO_CHECK_TYPE);
=======
							flag, next_pgofs);
>>>>>>> v4.9.227
}

static int get_data_block_dio(struct inode *inode, sector_t iblock,
			struct buffer_head *bh_result, int create)
{
	return __get_data_block(inode, iblock, bh_result, create,
<<<<<<< HEAD
						F2FS_GET_BLOCK_DEFAULT, NULL,
						f2fs_rw_hint_to_seg_type(
							WRITE_LIFE_NOT_SET));
						/* inode->i_write_hint)); */
=======
						F2FS_GET_BLOCK_DIO, NULL);
>>>>>>> v4.9.227
}

static int get_data_block_bmap(struct inode *inode, sector_t iblock,
			struct buffer_head *bh_result, int create)
{
	/* Block number less than F2FS MAX BLOCKS */
	if (unlikely(iblock >= F2FS_I_SB(inode)->max_file_blocks))
		return -EFBIG;

	return __get_data_block(inode, iblock, bh_result, create,
<<<<<<< HEAD
						F2FS_GET_BLOCK_BMAP, NULL,
						NO_CHECK_TYPE);
=======
						F2FS_GET_BLOCK_BMAP, NULL);
>>>>>>> v4.9.227
}

static inline sector_t logical_to_blk(struct inode *inode, loff_t offset)
{
	return (offset >> inode->i_blkbits);
}

static inline loff_t blk_to_logical(struct inode *inode, sector_t blk)
{
	return (blk << inode->i_blkbits);
}

<<<<<<< HEAD
static int f2fs_xattr_fiemap(struct inode *inode,
				struct fiemap_extent_info *fieinfo)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct page *page;
	struct node_info ni;
	__u64 phys = 0, len;
	__u32 flags;
	nid_t xnid = F2FS_I(inode)->i_xattr_nid;
	int err = 0;

	if (f2fs_has_inline_xattr(inode)) {
		int offset;

		page = f2fs_grab_cache_page(NODE_MAPPING(sbi),
						inode->i_ino, false);
		if (!page)
			return -ENOMEM;

		err = f2fs_get_node_info(sbi, inode->i_ino, &ni);
		if (err) {
			f2fs_put_page(page, 1);
			return err;
		}

		phys = (__u64)blk_to_logical(inode, ni.blk_addr);
		offset = offsetof(struct f2fs_inode, i_addr) +
					sizeof(__le32) * (DEF_ADDRS_PER_INODE -
					get_inline_xattr_addrs(inode));

		phys += offset;
		len = inline_xattr_size(inode);

		f2fs_put_page(page, 1);

		flags = FIEMAP_EXTENT_DATA_INLINE | FIEMAP_EXTENT_NOT_ALIGNED;

		if (!xnid)
			flags |= FIEMAP_EXTENT_LAST;

		err = fiemap_fill_next_extent(fieinfo, 0, phys, len, flags);
		if (err || err == 1)
			return err;
	}

	if (xnid) {
		page = f2fs_grab_cache_page(NODE_MAPPING(sbi), xnid, false);
		if (!page)
			return -ENOMEM;

		err = f2fs_get_node_info(sbi, xnid, &ni);
		if (err) {
			f2fs_put_page(page, 1);
			return err;
		}

		phys = (__u64)blk_to_logical(inode, ni.blk_addr);
		len = inode->i_sb->s_blocksize;

		f2fs_put_page(page, 1);

		flags = FIEMAP_EXTENT_LAST;
	}

	if (phys)
		err = fiemap_fill_next_extent(fieinfo, 0, phys, len, flags);

	return (err < 0 ? err : 0);
}

=======
>>>>>>> v4.9.227
int f2fs_fiemap(struct inode *inode, struct fiemap_extent_info *fieinfo,
		u64 start, u64 len)
{
	struct buffer_head map_bh;
	sector_t start_blk, last_blk;
	pgoff_t next_pgofs;
<<<<<<< HEAD
=======
	loff_t isize;
>>>>>>> v4.9.227
	u64 logical = 0, phys = 0, size = 0;
	u32 flags = 0;
	int ret = 0;

<<<<<<< HEAD
	if (fieinfo->fi_flags & FIEMAP_FLAG_CACHE) {
		ret = f2fs_precache_extents(inode);
		if (ret)
			return ret;
	}

	ret = fiemap_check_flags(fieinfo, FIEMAP_FLAG_SYNC | FIEMAP_FLAG_XATTR);
	if (ret)
		return ret;

	inode_lock(inode);

	if (fieinfo->fi_flags & FIEMAP_FLAG_XATTR) {
		ret = f2fs_xattr_fiemap(inode, fieinfo);
		goto out;
	}

	if (f2fs_has_inline_data(inode)) {
		ret = f2fs_inline_data_fiemap(inode, fieinfo, start, len);
		if (ret != -EAGAIN)
			goto out;
	}

=======
	ret = fiemap_check_flags(fieinfo, FIEMAP_FLAG_SYNC);
	if (ret)
		return ret;

	if (f2fs_has_inline_data(inode)) {
		ret = f2fs_inline_data_fiemap(inode, fieinfo, start, len);
		if (ret != -EAGAIN)
			return ret;
	}

	inode_lock(inode);

	isize = i_size_read(inode);
	if (start >= isize)
		goto out;

	if (start + len > isize)
		len = isize - start;

>>>>>>> v4.9.227
	if (logical_to_blk(inode, len) == 0)
		len = blk_to_logical(inode, 1);

	start_blk = logical_to_blk(inode, start);
	last_blk = logical_to_blk(inode, start + len - 1);

next:
	memset(&map_bh, 0, sizeof(struct buffer_head));
	map_bh.b_size = len;

	ret = get_data_block(inode, start_blk, &map_bh, 0,
					F2FS_GET_BLOCK_FIEMAP, &next_pgofs);
	if (ret)
		goto out;

	/* HOLE */
	if (!buffer_mapped(&map_bh)) {
		start_blk = next_pgofs;
<<<<<<< HEAD

		if (blk_to_logical(inode, start_blk) < blk_to_logical(inode,
					F2FS_I_SB(inode)->max_file_blocks))
			goto prep_next;

=======
		/* Go through holes util pass the EOF */
		if (blk_to_logical(inode, start_blk) < isize)
			goto prep_next;
		/* Found a hole beyond isize means no more extents.
		 * Note that the premise is that filesystems don't
		 * punch holes beyond isize and keep size unchanged.
		 */
>>>>>>> v4.9.227
		flags |= FIEMAP_EXTENT_LAST;
	}

	if (size) {
		if (f2fs_encrypted_inode(inode))
			flags |= FIEMAP_EXTENT_DATA_ENCRYPTED;

		ret = fiemap_fill_next_extent(fieinfo, logical,
				phys, size, flags);
	}

	if (start_blk > last_blk || ret)
		goto out;

	logical = blk_to_logical(inode, start_blk);
	phys = blk_to_logical(inode, map_bh.b_blocknr);
	size = map_bh.b_size;
	flags = 0;
	if (buffer_unwritten(&map_bh))
		flags = FIEMAP_EXTENT_UNWRITTEN;

	start_blk += logical_to_blk(inode, size);

prep_next:
	cond_resched();
	if (fatal_signal_pending(current))
		ret = -EINTR;
	else
		goto next;
out:
	if (ret == 1)
		ret = 0;

	inode_unlock(inode);
	return ret;
}

<<<<<<< HEAD
/*
 * This function was originally taken from fs/mpage.c, and customized for f2fs.
 * Major change was from block_size == page_size in f2fs by default.
 *
 * Note that the aops->readpages() function is ONLY used for read-ahead. If
 * this function ever deviates from doing just read-ahead, it should either
 * use ->readpage() or do the necessary surgery to decouple ->readpages()
 * from read-ahead.
 */
static int f2fs_mpage_readpages(struct address_space *mapping,
			struct list_head *pages, struct page *page,
			unsigned nr_pages, bool is_readahead)
{
	struct bio *bio = NULL;
=======
static struct bio *f2fs_grab_bio(struct inode *inode, block_t blkaddr,
				 unsigned nr_pages)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct fscrypt_ctx *ctx = NULL;
	struct block_device *bdev = sbi->sb->s_bdev;
	struct bio *bio;

	if (!f2fs_is_valid_blkaddr(sbi, blkaddr, DATA_GENERIC))
		return ERR_PTR(-EFAULT);

	if (f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode)) {
		ctx = fscrypt_get_ctx(inode, GFP_NOFS);
		if (IS_ERR(ctx))
			return ERR_CAST(ctx);

		/* wait the page to be moved by cleaning */
		f2fs_wait_on_encrypted_page_writeback(sbi, blkaddr);
	}

	bio = bio_alloc(GFP_KERNEL, min_t(int, nr_pages, BIO_MAX_PAGES));
	if (!bio) {
		if (ctx)
			fscrypt_release_ctx(ctx);
		return ERR_PTR(-ENOMEM);
	}
	bio->bi_bdev = bdev;
	bio->bi_iter.bi_sector = SECTOR_FROM_BLOCK(blkaddr);
	bio->bi_end_io = f2fs_read_end_io;
	bio->bi_private = ctx;

	return bio;
}

/*
 * This function was originally taken from fs/mpage.c, and customized for f2fs.
 * Major change was from block_size == page_size in f2fs by default.
 */
static int f2fs_mpage_readpages(struct address_space *mapping,
			struct list_head *pages, struct page *page,
			unsigned nr_pages)
{
	struct bio *bio = NULL;
	unsigned page_idx;
>>>>>>> v4.9.227
	sector_t last_block_in_bio = 0;
	struct inode *inode = mapping->host;
	const unsigned blkbits = inode->i_blkbits;
	const unsigned blocksize = 1 << blkbits;
	sector_t block_in_file;
	sector_t last_block;
	sector_t last_block_in_file;
	sector_t block_nr;
	struct f2fs_map_blocks map;

	map.m_pblk = 0;
	map.m_lblk = 0;
	map.m_len = 0;
	map.m_flags = 0;
	map.m_next_pgofs = NULL;
<<<<<<< HEAD
	map.m_next_extent = NULL;
	map.m_seg_type = NO_CHECK_TYPE;

	for (; nr_pages; nr_pages--) {
		if (pages) {
			page = list_last_entry(pages, struct page, lru);

			prefetchw(&page->flags);
			list_del(&page->lru);
			if (add_to_page_cache_lru(page, mapping,
						  page->index, GFP_KERNEL))
=======

	for (page_idx = 0; nr_pages; page_idx++, nr_pages--) {

		prefetchw(&page->flags);
		if (pages) {
			page = list_entry(pages->prev, struct page, lru);
			list_del(&page->lru);
			if (add_to_page_cache_lru(page, mapping,
						  page->index,
						  readahead_gfp_mask(mapping)))
>>>>>>> v4.9.227
				goto next_page;
		}

		block_in_file = (sector_t)page->index;
		last_block = block_in_file + nr_pages;
		last_block_in_file = (i_size_read(inode) + blocksize - 1) >>
								blkbits;
		if (last_block > last_block_in_file)
			last_block = last_block_in_file;

		/*
		 * Map blocks using the previous result first.
		 */
		if ((map.m_flags & F2FS_MAP_MAPPED) &&
				block_in_file > map.m_lblk &&
				block_in_file < (map.m_lblk + map.m_len))
			goto got_it;

		/*
		 * Then do more f2fs_map_blocks() calls until we are
		 * done with this page.
		 */
		map.m_flags = 0;

		if (block_in_file < last_block) {
			map.m_lblk = block_in_file;
			map.m_len = last_block - block_in_file;

			if (f2fs_map_blocks(inode, &map, 0,
<<<<<<< HEAD
						F2FS_GET_BLOCK_DEFAULT))
=======
						F2FS_GET_BLOCK_READ))
>>>>>>> v4.9.227
				goto set_error_page;
		}
got_it:
		if ((map.m_flags & F2FS_MAP_MAPPED)) {
			block_nr = map.m_pblk + block_in_file - map.m_lblk;
			SetPageMappedToDisk(page);

			if (!PageUptodate(page) && !cleancache_get_page(page)) {
				SetPageUptodate(page);
				goto confused;
			}

			if (!f2fs_is_valid_blkaddr(F2FS_I_SB(inode), block_nr,
								DATA_GENERIC))
				goto set_error_page;
		} else {
			zero_user_segment(page, 0, PAGE_SIZE);
			if (!PageUptodate(page))
				SetPageUptodate(page);
			unlock_page(page);
			goto next_page;
		}

		/*
		 * This page will go to BIO.  Do we need to send this
		 * BIO off first?
		 */
<<<<<<< HEAD
		if (bio && (last_block_in_bio != block_nr - 1 ||
			!__same_bdev(F2FS_I_SB(inode), block_nr, bio))) {
=======
		if (bio && (last_block_in_bio != block_nr - 1)) {
>>>>>>> v4.9.227
submit_and_realloc:
			__submit_bio(F2FS_I_SB(inode), bio, DATA);
			bio = NULL;
		}
		if (bio == NULL) {
<<<<<<< HEAD
			bio = f2fs_grab_read_bio(inode, block_nr, nr_pages,
					is_readahead ? REQ_RAHEAD : 0);
=======
			bio = f2fs_grab_bio(inode, block_nr, nr_pages);
>>>>>>> v4.9.227
			if (IS_ERR(bio)) {
				bio = NULL;
				goto set_error_page;
			}
<<<<<<< HEAD
=======
			bio_set_op_attrs(bio, REQ_OP_READ, 0);
>>>>>>> v4.9.227
		}

		if (bio_add_page(bio, page, blocksize, 0) < blocksize)
			goto submit_and_realloc;

		last_block_in_bio = block_nr;
		goto next_page;
set_error_page:
		SetPageError(page);
		zero_user_segment(page, 0, PAGE_SIZE);
		unlock_page(page);
		goto next_page;
confused:
		if (bio) {
			__submit_bio(F2FS_I_SB(inode), bio, DATA);
			bio = NULL;
		}
		unlock_page(page);
next_page:
		if (pages)
			put_page(page);
	}
	BUG_ON(pages && !list_empty(pages));
	if (bio)
		__submit_bio(F2FS_I_SB(inode), bio, DATA);
	return 0;
}

static int f2fs_read_data_page(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	int ret = -EAGAIN;

	trace_f2fs_readpage(page, DATA);

	/* If the file has inline data, try to read it directly */
	if (f2fs_has_inline_data(inode))
		ret = f2fs_read_inline_data(inode, page);
	if (ret == -EAGAIN)
<<<<<<< HEAD
		ret = f2fs_mpage_readpages(page->mapping, NULL, page, 1, false);
=======
		ret = f2fs_mpage_readpages(page->mapping, NULL, page, 1);
>>>>>>> v4.9.227
	return ret;
}

static int f2fs_read_data_pages(struct file *file,
			struct address_space *mapping,
			struct list_head *pages, unsigned nr_pages)
{
<<<<<<< HEAD
	struct inode *inode = mapping->host;
	struct page *page = list_last_entry(pages, struct page, lru);
=======
	struct inode *inode = file->f_mapping->host;
	struct page *page = list_entry(pages->prev, struct page, lru);
>>>>>>> v4.9.227

	trace_f2fs_readpages(inode, page, nr_pages);

	/* If the file has inline data, skip readpages */
	if (f2fs_has_inline_data(inode))
		return 0;

<<<<<<< HEAD
	return f2fs_mpage_readpages(mapping, pages, NULL, nr_pages, true);
}

static int encrypt_one_page(struct f2fs_io_info *fio)
{
	struct inode *inode = fio->page->mapping->host;
	struct page *mpage;
	gfp_t gfp_flags = GFP_NOFS;

	if (!f2fs_encrypted_file(inode))
		return 0;

	/* wait for GCed page writeback via META_MAPPING */
	f2fs_wait_on_block_writeback(fio->sbi, fio->old_blkaddr);

retry_encrypt:
	fio->encrypted_page = fscrypt_encrypt_page(inode, fio->page,
			PAGE_SIZE, 0, fio->page->index, gfp_flags);
	if (IS_ERR(fio->encrypted_page)) {
		/* flush pending IOs and wait for a while in the ENOMEM case */
		if (PTR_ERR(fio->encrypted_page) == -ENOMEM) {
			f2fs_flush_merged_writes(fio->sbi);
			congestion_wait(BLK_RW_ASYNC, HZ/50);
			gfp_flags |= __GFP_NOFAIL;
			goto retry_encrypt;
		}
		return PTR_ERR(fio->encrypted_page);
	}

	mpage = find_lock_page(META_MAPPING(fio->sbi), fio->old_blkaddr);
	if (mpage) {
		if (PageUptodate(mpage))
			memcpy(page_address(mpage),
				page_address(fio->encrypted_page), PAGE_SIZE);
		f2fs_put_page(mpage, 1);
	}
	return 0;
}

static inline bool check_inplace_update_policy(struct inode *inode,
				struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	unsigned int policy = SM_I(sbi)->ipu_policy;

	if (policy & (0x1 << F2FS_IPU_FORCE))
		return true;
	if (policy & (0x1 << F2FS_IPU_SSR) && f2fs_need_SSR(sbi))
		return true;
	if (policy & (0x1 << F2FS_IPU_UTIL) &&
			utilization(sbi) > SM_I(sbi)->min_ipu_util)
		return true;
	if (policy & (0x1 << F2FS_IPU_SSR_UTIL) && f2fs_need_SSR(sbi) &&
			utilization(sbi) > SM_I(sbi)->min_ipu_util)
		return true;

	/*
	 * IPU for rewrite async pages
	 */
	if (policy & (0x1 << F2FS_IPU_ASYNC) &&
			fio && fio->op == REQ_OP_WRITE &&
			!(fio->op_flags & REQ_SYNC) &&
			!f2fs_encrypted_inode(inode))
		return true;

	/* this is only set during fdatasync */
	if (policy & (0x1 << F2FS_IPU_FSYNC) &&
			is_inode_flag_set(inode, FI_NEED_IPU))
		return true;

	return false;
}

bool f2fs_should_update_inplace(struct inode *inode, struct f2fs_io_info *fio)
{
	if (f2fs_is_pinned_file(inode))
		return true;

	/* if this is cold file, we should overwrite to avoid fragmentation */
	if (file_is_cold(inode))
		return true;

	return check_inplace_update_policy(inode, fio);
}

bool f2fs_should_update_outplace(struct inode *inode, struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (test_opt(sbi, LFS))
		return true;
	if (S_ISDIR(inode->i_mode))
		return true;
	if (f2fs_is_atomic_file(inode))
		return true;
	if (fio) {
		if (is_cold_data(fio->page))
			return true;
		if (IS_ATOMIC_WRITTEN_PAGE(fio->page))
			return true;
	}
	return false;
}

static inline bool need_inplace_update(struct f2fs_io_info *fio)
{
	struct inode *inode = fio->page->mapping->host;

	if (f2fs_should_update_outplace(inode, fio))
		return false;

	return f2fs_should_update_inplace(inode, fio);
}

int f2fs_do_write_data_page(struct f2fs_io_info *fio)
=======
	return f2fs_mpage_readpages(mapping, pages, NULL, nr_pages);
}

int do_write_data_page(struct f2fs_io_info *fio)
>>>>>>> v4.9.227
{
	struct page *page = fio->page;
	struct inode *inode = page->mapping->host;
	struct dnode_of_data dn;
<<<<<<< HEAD
	struct extent_info ei = {0,0,0};
	struct node_info ni;
	bool ipu_force = false;
	int err = 0;

	set_new_dnode(&dn, inode, NULL, NULL, 0);
	if (need_inplace_update(fio) &&
			f2fs_lookup_extent_cache(inode, page->index, &ei)) {
		fio->old_blkaddr = ei.blk + page->index - ei.fofs;

		if (!f2fs_is_valid_blkaddr(fio->sbi, fio->old_blkaddr,
							DATA_GENERIC))
			return -EFAULT;

		ipu_force = true;
		fio->need_lock = LOCK_DONE;
		goto got_it;
	}

	/* Deadlock due to between page->lock and f2fs_lock_op */
	if (fio->need_lock == LOCK_REQ && !f2fs_trylock_op(fio->sbi))
		return -EAGAIN;

	err = f2fs_get_dnode_of_data(&dn, page->index, LOOKUP_NODE);
	if (err)
		goto out;
=======
	int err = 0;

	set_new_dnode(&dn, inode, NULL, NULL, 0);
	err = get_dnode_of_data(&dn, page->index, LOOKUP_NODE);
	if (err)
		return err;
>>>>>>> v4.9.227

	fio->old_blkaddr = dn.data_blkaddr;

	/* This page is already truncated */
	if (fio->old_blkaddr == NULL_ADDR) {
		ClearPageUptodate(page);
<<<<<<< HEAD
		goto out_writepage;
	}
got_it:
=======
		clear_cold_data(page);
		goto out_writepage;
	}

	if (f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode)) {
		gfp_t gfp_flags = GFP_NOFS;

		/* wait for GCed encrypted page writeback */
		f2fs_wait_on_encrypted_page_writeback(F2FS_I_SB(inode),
							fio->old_blkaddr);
retry_encrypt:
		fio->encrypted_page = fscrypt_encrypt_page(inode, fio->page,
								gfp_flags);
		if (IS_ERR(fio->encrypted_page)) {
			err = PTR_ERR(fio->encrypted_page);
			if (err == -ENOMEM) {
				/* flush pending ios and wait for a while */
				f2fs_flush_merged_bios(F2FS_I_SB(inode));
				congestion_wait(BLK_RW_ASYNC, HZ/50);
				gfp_flags |= __GFP_NOFAIL;
				err = 0;
				goto retry_encrypt;
			}
			goto out_writepage;
		}
	}

	set_page_writeback(page);

>>>>>>> v4.9.227
	if (__is_valid_data_blkaddr(fio->old_blkaddr) &&
		!f2fs_is_valid_blkaddr(fio->sbi, fio->old_blkaddr,
							DATA_GENERIC)) {
		err = -EFAULT;
		goto out_writepage;
	}
	/*
	 * If current allocation needs SSR,
	 * it had better in-place writes for updated data.
	 */
<<<<<<< HEAD
	if (ipu_force || (is_valid_data_blkaddr(fio->sbi, fio->old_blkaddr) &&
					need_inplace_update(fio))) {
		err = encrypt_one_page(fio);
		if (err)
			goto out_writepage;

		set_page_writeback(page);
		ClearPageError(page);
		f2fs_put_dnode(&dn);
		if (fio->need_lock == LOCK_REQ)
			f2fs_unlock_op(fio->sbi);
		err = f2fs_inplace_write_data(fio);
		trace_f2fs_do_write_data_page(fio->page, IPU);
		set_inode_flag(inode, FI_UPDATE_WRITE);
		return err;
	}

	if (fio->need_lock == LOCK_RETRY) {
		if (!f2fs_trylock_op(fio->sbi)) {
			err = -EAGAIN;
			goto out_writepage;
		}
		fio->need_lock = LOCK_REQ;
	}

	err = f2fs_get_node_info(fio->sbi, dn.nid, &ni);
	if (err)
		goto out_writepage;

	fio->version = ni.version;

	err = encrypt_one_page(fio);
	if (err)
		goto out_writepage;

	set_page_writeback(page);
	ClearPageError(page);

	/* LFS mode write path */
	f2fs_outplace_write_data(&dn, fio);
	trace_f2fs_do_write_data_page(page, OPU);
	set_inode_flag(inode, FI_APPEND_WRITE);
	if (page->index == 0)
		set_inode_flag(inode, FI_FIRST_BLOCK_WRITTEN);
out_writepage:
	f2fs_put_dnode(&dn);
out:
	if (fio->need_lock == LOCK_REQ)
		f2fs_unlock_op(fio->sbi);
	return err;
}

static int __write_data_page(struct page *page, bool *submitted,
				struct writeback_control *wbc,
				enum iostat_type io_type)
=======
	if (unlikely(is_valid_data_blkaddr(fio->sbi, fio->old_blkaddr) &&
			!is_cold_data(page) &&
			!IS_ATOMIC_WRITTEN_PAGE(page) &&
			need_inplace_update(inode))) {
		rewrite_data_page(fio);
		set_inode_flag(inode, FI_UPDATE_WRITE);
		trace_f2fs_do_write_data_page(page, IPU);
	} else {
		write_data_page(&dn, fio);
		trace_f2fs_do_write_data_page(page, OPU);
		set_inode_flag(inode, FI_APPEND_WRITE);
		if (page->index == 0)
			set_inode_flag(inode, FI_FIRST_BLOCK_WRITTEN);
	}
out_writepage:
	f2fs_put_dnode(&dn);
	return err;
}

static int f2fs_write_data_page(struct page *page,
					struct writeback_control *wbc)
>>>>>>> v4.9.227
{
	struct inode *inode = page->mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	loff_t i_size = i_size_read(inode);
	const pgoff_t end_index = ((unsigned long long) i_size)
							>> PAGE_SHIFT;
<<<<<<< HEAD
	loff_t psize = (page->index + 1) << PAGE_SHIFT;
=======
	loff_t psize = (loff_t)(page->index + 1) << PAGE_SHIFT;
>>>>>>> v4.9.227
	unsigned offset = 0;
	bool need_balance_fs = false;
	int err = 0;
	struct f2fs_io_info fio = {
		.sbi = sbi,
<<<<<<< HEAD
		.ino = inode->i_ino,
		.type = DATA,
		.op = REQ_OP_WRITE,
		.op_flags = wbc_to_write_flags(wbc),
		.old_blkaddr = NULL_ADDR,
		.page = page,
		.encrypted_page = NULL,
		.submitted = false,
		.need_lock = LOCK_RETRY,
		.io_type = io_type,
		.io_wbc = wbc,
=======
		.type = DATA,
		.op = REQ_OP_WRITE,
		.op_flags = (wbc->sync_mode == WB_SYNC_ALL) ? WRITE_SYNC : 0,
		.page = page,
		.encrypted_page = NULL,
>>>>>>> v4.9.227
	};

	trace_f2fs_writepage(page, DATA);

<<<<<<< HEAD
	/* we should bypass data pages to proceed the kworkder jobs */
	if (unlikely(f2fs_cp_error(sbi))) {
		mapping_set_error(page->mapping, -EIO);
		/*
		 * don't drop any dirty dentry pages for keeping lastest
		 * directory structure.
		 */
		if (S_ISDIR(inode->i_mode))
			goto redirty_out;
		goto out;
	}

	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto redirty_out;

=======
>>>>>>> v4.9.227
	if (page->index < end_index)
		goto write;

	/*
	 * If the offset is out-of-range of file size,
	 * this page does not have to be written to disk.
	 */
	offset = i_size & (PAGE_SIZE - 1);
	if ((page->index >= end_index + 1) || !offset)
		goto out;

	zero_user_segment(page, offset, PAGE_SIZE);
write:
<<<<<<< HEAD
=======
	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto redirty_out;
>>>>>>> v4.9.227
	if (f2fs_is_drop_cache(inode))
		goto out;
	/* we should not write 0'th page having journal header */
	if (f2fs_is_volatile_file(inode) && (!page->index ||
			(!wbc->for_reclaim &&
<<<<<<< HEAD
			f2fs_available_free_memory(sbi, BASE_CHECK))))
		goto redirty_out;

	/* Dentry blocks are controlled by checkpoint */
	if (S_ISDIR(inode->i_mode)) {
		fio.need_lock = LOCK_DONE;
		err = f2fs_do_write_data_page(&fio);
=======
			available_free_memory(sbi, BASE_CHECK))))
		goto redirty_out;

	/* we should bypass data pages to proceed the kworkder jobs */
	if (unlikely(f2fs_cp_error(sbi))) {
		mapping_set_error(page->mapping, -EIO);
		goto out;
	}

	/* Dentry blocks are controlled by checkpoint */
	if (S_ISDIR(inode->i_mode)) {
		err = do_write_data_page(&fio);
>>>>>>> v4.9.227
		goto done;
	}

	if (!wbc->for_reclaim)
		need_balance_fs = true;
	else if (has_not_enough_free_secs(sbi, 0, 0))
		goto redirty_out;
<<<<<<< HEAD
	else
		set_inode_flag(inode, FI_HOT_DATA);

	err = -EAGAIN;
	if (f2fs_has_inline_data(inode)) {
		err = f2fs_write_inline_data(inode, page);
		if (!err)
			goto out;
	}

	if (err == -EAGAIN) {
		err = f2fs_do_write_data_page(&fio);
		if (err == -EAGAIN) {
			fio.need_lock = LOCK_REQ;
			err = f2fs_do_write_data_page(&fio);
		}
	}

	if (err) {
		file_set_keep_isize(inode);
	} else {
		down_write(&F2FS_I(inode)->i_sem);
		if (F2FS_I(inode)->last_disk_size < psize)
			F2FS_I(inode)->last_disk_size = psize;
		up_write(&F2FS_I(inode)->i_sem);
	}

=======

	err = -EAGAIN;
	f2fs_lock_op(sbi);
	if (f2fs_has_inline_data(inode))
		err = f2fs_write_inline_data(inode, page);
	if (err == -EAGAIN)
		err = do_write_data_page(&fio);
	if (F2FS_I(inode)->last_disk_size < psize)
		F2FS_I(inode)->last_disk_size = psize;
	f2fs_unlock_op(sbi);
>>>>>>> v4.9.227
done:
	if (err && err != -ENOENT)
		goto redirty_out;

<<<<<<< HEAD
out:
	inode_dec_dirty_pages(inode);
	if (err)
		ClearPageUptodate(page);

	if (wbc->for_reclaim) {
		f2fs_submit_merged_write_cond(sbi, inode, 0, page->index, DATA);
		clear_inode_flag(inode, FI_HOT_DATA);
		f2fs_remove_dirty_inode(inode);
		submitted = NULL;
	}

	unlock_page(page);
	if (!S_ISDIR(inode->i_mode))
		f2fs_balance_fs(sbi, need_balance_fs);

	if (unlikely(f2fs_cp_error(sbi))) {
		f2fs_submit_merged_write(sbi, DATA);
		submitted = NULL;
	}

	if (submitted)
		*submitted = fio.submitted;
=======
	clear_cold_data(page);
out:
	inode_dec_dirty_pages(inode);
	if (err) {
		ClearPageUptodate(page);
		clear_cold_data(page);
	}

	if (wbc->for_reclaim) {
		f2fs_submit_merged_bio_cond(sbi, NULL, page, 0, DATA, WRITE);
		remove_dirty_inode(inode);
	}

	unlock_page(page);
	f2fs_balance_fs(sbi, need_balance_fs);

	if (unlikely(f2fs_cp_error(sbi)))
		f2fs_submit_merged_bio(sbi, DATA, WRITE);
>>>>>>> v4.9.227

	return 0;

redirty_out:
	redirty_page_for_writepage(wbc, page);
<<<<<<< HEAD
	/*
	 * pageout() in MM traslates EAGAIN, so calls handle_write_error()
	 * -> mapping_set_error() -> set_bit(AS_EIO, ...).
	 * file_write_and_wait_range() will see EIO error, which is critical
	 * to return value of fsync() followed by atomic_write failure to user.
	 */
	if (!err || wbc->for_reclaim)
		return AOP_WRITEPAGE_ACTIVATE;
=======
>>>>>>> v4.9.227
	unlock_page(page);
	return err;
}

<<<<<<< HEAD
static int f2fs_write_data_page(struct page *page,
					struct writeback_control *wbc)
{
	return __write_data_page(page, NULL, wbc, FS_DATA_IO);
}

=======
>>>>>>> v4.9.227
/*
 * This function was copied from write_cche_pages from mm/page-writeback.c.
 * The major change is making write step of cold data page separately from
 * warm/hot data page.
 */
static int f2fs_write_cache_pages(struct address_space *mapping,
<<<<<<< HEAD
					struct writeback_control *wbc,
					enum iostat_type io_type)
=======
					struct writeback_control *wbc)
>>>>>>> v4.9.227
{
	int ret = 0;
	int done = 0;
	struct pagevec pvec;
<<<<<<< HEAD
	struct f2fs_sb_info *sbi = F2FS_M_SB(mapping);
=======
>>>>>>> v4.9.227
	int nr_pages;
	pgoff_t uninitialized_var(writeback_index);
	pgoff_t index;
	pgoff_t end;		/* Inclusive */
	pgoff_t done_index;
<<<<<<< HEAD
	pgoff_t last_idx = ULONG_MAX;
	int cycled;
	int range_whole = 0;
	int tag;

	pagevec_init(&pvec, 0);

	if (get_dirty_pages(mapping->host) <=
				SM_I(F2FS_M_SB(mapping))->min_hot_blocks)
		set_inode_flag(mapping->host, FI_HOT_DATA);
	else
		clear_inode_flag(mapping->host, FI_HOT_DATA);

=======
	int cycled;
	int range_whole = 0;
	int tag;
	int nwritten = 0;

	pagevec_init(&pvec, 0);

>>>>>>> v4.9.227
	if (wbc->range_cyclic) {
		writeback_index = mapping->writeback_index; /* prev offset */
		index = writeback_index;
		if (index == 0)
			cycled = 1;
		else
			cycled = 0;
		end = -1;
	} else {
		index = wbc->range_start >> PAGE_SHIFT;
		end = wbc->range_end >> PAGE_SHIFT;
		if (wbc->range_start == 0 && wbc->range_end == LLONG_MAX)
			range_whole = 1;
		cycled = 1; /* ignore range_cyclic tests */
	}
	if (wbc->sync_mode == WB_SYNC_ALL || wbc->tagged_writepages)
		tag = PAGECACHE_TAG_TOWRITE;
	else
		tag = PAGECACHE_TAG_DIRTY;
retry:
	if (wbc->sync_mode == WB_SYNC_ALL || wbc->tagged_writepages)
		tag_pages_for_writeback(mapping, index, end);
	done_index = index;
	while (!done && (index <= end)) {
		int i;

<<<<<<< HEAD
		nr_pages = pagevec_lookup_range_tag(&pvec, mapping, &index, end,
				tag);
=======
		nr_pages = pagevec_lookup_tag(&pvec, mapping, &index, tag,
			      min(end - index, (pgoff_t)PAGEVEC_SIZE - 1) + 1);
>>>>>>> v4.9.227
		if (nr_pages == 0)
			break;

		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];
<<<<<<< HEAD
			bool submitted = false;

			/* give a priority to WB_SYNC threads */
			if (atomic_read(&sbi->wb_sync_req[DATA]) &&
					wbc->sync_mode == WB_SYNC_NONE) {
=======

			if (page->index > end) {
>>>>>>> v4.9.227
				done = 1;
				break;
			}

			done_index = page->index;
<<<<<<< HEAD
retry_write:
=======

>>>>>>> v4.9.227
			lock_page(page);

			if (unlikely(page->mapping != mapping)) {
continue_unlock:
				unlock_page(page);
				continue;
			}

			if (!PageDirty(page)) {
				/* someone wrote it for us */
				goto continue_unlock;
			}

			if (PageWriteback(page)) {
				if (wbc->sync_mode != WB_SYNC_NONE)
					f2fs_wait_on_page_writeback(page,
								DATA, true);
				else
					goto continue_unlock;
			}

			BUG_ON(PageWriteback(page));
			if (!clear_page_dirty_for_io(page))
				goto continue_unlock;

<<<<<<< HEAD
			ret = __write_data_page(page, &submitted, wbc, io_type);
			if (unlikely(ret)) {
				/*
				 * keep nr_to_write, since vfs uses this to
				 * get # of written pages.
				 */
				if (ret == AOP_WRITEPAGE_ACTIVATE) {
					unlock_page(page);
					ret = 0;
					continue;
				} else if (ret == -EAGAIN) {
					ret = 0;
					if (wbc->sync_mode == WB_SYNC_ALL) {
						cond_resched();
						congestion_wait(BLK_RW_ASYNC,
									HZ/50);
						goto retry_write;
					}
					continue;
				}
				done_index = page->index + 1;
				done = 1;
				break;
			} else if (submitted) {
				last_idx = page->index;
			}

			if (--wbc->nr_to_write <= 0 &&
					wbc->sync_mode == WB_SYNC_NONE) {
=======
			ret = mapping->a_ops->writepage(page, wbc);
			if (unlikely(ret)) {
				done_index = page->index + 1;
				done = 1;
				break;
			} else {
				nwritten++;
			}

			if (--wbc->nr_to_write <= 0 &&
			    wbc->sync_mode == WB_SYNC_NONE) {
>>>>>>> v4.9.227
				done = 1;
				break;
			}
		}
		pagevec_release(&pvec);
		cond_resched();
	}

	if (!cycled && !done) {
		cycled = 1;
		index = 0;
		end = writeback_index - 1;
		goto retry;
	}
	if (wbc->range_cyclic || (range_whole && wbc->nr_to_write > 0))
		mapping->writeback_index = done_index;

<<<<<<< HEAD
	if (last_idx != ULONG_MAX)
		f2fs_submit_merged_write_cond(F2FS_M_SB(mapping), mapping->host,
						0, last_idx, DATA);
=======
	if (nwritten)
		f2fs_submit_merged_bio_cond(F2FS_M_SB(mapping), mapping->host,
							NULL, 0, DATA, WRITE);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static inline bool __should_serialize_io(struct inode *inode,
					struct writeback_control *wbc)
{
	if (!S_ISREG(inode->i_mode))
		return false;
	if (wbc->sync_mode != WB_SYNC_ALL)
		return true;
	if (get_dirty_pages(inode) >= SM_I(F2FS_I_SB(inode))->min_seq_blocks)
		return true;
	return false;
}

static int __f2fs_write_data_pages(struct address_space *mapping,
						struct writeback_control *wbc,
						enum iostat_type io_type)
=======
static int f2fs_write_data_pages(struct address_space *mapping,
			    struct writeback_control *wbc)
>>>>>>> v4.9.227
{
	struct inode *inode = mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct blk_plug plug;
	int ret;
<<<<<<< HEAD
	bool locked = false;
=======
>>>>>>> v4.9.227

	/* deal with chardevs and other special file */
	if (!mapping->a_ops->writepage)
		return 0;

	/* skip writing if there is no dirty page in this inode */
	if (!get_dirty_pages(inode) && wbc->sync_mode == WB_SYNC_NONE)
		return 0;

<<<<<<< HEAD
	/* during POR, we don't need to trigger writepage at all. */
	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto skip_write;

	if (S_ISDIR(inode->i_mode) && wbc->sync_mode == WB_SYNC_NONE &&
			get_dirty_pages(inode) < nr_pages_to_skip(sbi, DATA) &&
			f2fs_available_free_memory(sbi, DIRTY_DENTS))
=======
	if (S_ISDIR(inode->i_mode) && wbc->sync_mode == WB_SYNC_NONE &&
			get_dirty_pages(inode) < nr_pages_to_skip(sbi, DATA) &&
			available_free_memory(sbi, DIRTY_DENTS))
>>>>>>> v4.9.227
		goto skip_write;

	/* skip writing during file defragment */
	if (is_inode_flag_set(inode, FI_DO_DEFRAG))
		goto skip_write;

<<<<<<< HEAD
	trace_f2fs_writepages(mapping->host, wbc, DATA);

	/* to avoid spliting IOs due to mixed WB_SYNC_ALL and WB_SYNC_NONE */
	if (wbc->sync_mode == WB_SYNC_ALL)
		atomic_inc(&sbi->wb_sync_req[DATA]);
	else if (atomic_read(&sbi->wb_sync_req[DATA]))
		goto skip_write;

	if (__should_serialize_io(inode, wbc)) {
		mutex_lock(&sbi->writepages);
		locked = true;
	}

	blk_start_plug(&plug);
	ret = f2fs_write_cache_pages(mapping, wbc, io_type);
	blk_finish_plug(&plug);

	if (locked)
		mutex_unlock(&sbi->writepages);

	if (wbc->sync_mode == WB_SYNC_ALL)
		atomic_dec(&sbi->wb_sync_req[DATA]);
=======
	/* during POR, we don't need to trigger writepage at all. */
	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto skip_write;

	trace_f2fs_writepages(mapping->host, wbc, DATA);

	blk_start_plug(&plug);
	ret = f2fs_write_cache_pages(mapping, wbc);
	blk_finish_plug(&plug);
>>>>>>> v4.9.227
	/*
	 * if some pages were truncated, we cannot guarantee its mapping->host
	 * to detect pending bios.
	 */

<<<<<<< HEAD
	f2fs_remove_dirty_inode(inode);
=======
	remove_dirty_inode(inode);
>>>>>>> v4.9.227
	return ret;

skip_write:
	wbc->pages_skipped += get_dirty_pages(inode);
	trace_f2fs_writepages(mapping->host, wbc, DATA);
	return 0;
}

<<<<<<< HEAD
static int f2fs_write_data_pages(struct address_space *mapping,
			    struct writeback_control *wbc)
{
	struct inode *inode = mapping->host;

	return __f2fs_write_data_pages(mapping, wbc,
			F2FS_I(inode)->cp_task == current ?
			FS_CP_DATA_IO : FS_DATA_IO);
}

=======
>>>>>>> v4.9.227
static void f2fs_write_failed(struct address_space *mapping, loff_t to)
{
	struct inode *inode = mapping->host;
	loff_t i_size = i_size_read(inode);

	if (to > i_size) {
<<<<<<< HEAD
		down_write(&F2FS_I(inode)->i_gc_rwsem[WRITE]);
		down_write(&F2FS_I(inode)->i_mmap_sem);

		truncate_pagecache(inode, i_size);
		f2fs_truncate_blocks(inode, i_size, true);

		up_write(&F2FS_I(inode)->i_mmap_sem);
		up_write(&F2FS_I(inode)->i_gc_rwsem[WRITE]);
=======
		truncate_pagecache(inode, i_size);
		truncate_blocks(inode, i_size, true);
>>>>>>> v4.9.227
	}
}

static int prepare_write_begin(struct f2fs_sb_info *sbi,
			struct page *page, loff_t pos, unsigned len,
			block_t *blk_addr, bool *node_changed)
{
	struct inode *inode = page->mapping->host;
	pgoff_t index = page->index;
	struct dnode_of_data dn;
	struct page *ipage;
	bool locked = false;
<<<<<<< HEAD
	struct extent_info ei = {0,0,0};
=======
	struct extent_info ei;
>>>>>>> v4.9.227
	int err = 0;

	/*
	 * we already allocated all the blocks, so we don't need to get
	 * the block addresses when there is no need to fill the page.
	 */
<<<<<<< HEAD
	if (!f2fs_has_inline_data(inode) && len == PAGE_SIZE &&
			!is_inode_flag_set(inode, FI_NO_PREALLOC))
=======
	if (!f2fs_has_inline_data(inode) && len == PAGE_SIZE)
>>>>>>> v4.9.227
		return 0;

	if (f2fs_has_inline_data(inode) ||
			(pos & PAGE_MASK) >= i_size_read(inode)) {
<<<<<<< HEAD
		__do_map_lock(sbi, F2FS_GET_BLOCK_PRE_AIO, true);
=======
		f2fs_lock_op(sbi);
>>>>>>> v4.9.227
		locked = true;
	}
restart:
	/* check inline_data */
<<<<<<< HEAD
	ipage = f2fs_get_node_page(sbi, inode->i_ino);
=======
	ipage = get_node_page(sbi, inode->i_ino);
>>>>>>> v4.9.227
	if (IS_ERR(ipage)) {
		err = PTR_ERR(ipage);
		goto unlock_out;
	}

	set_new_dnode(&dn, inode, ipage, ipage, 0);

	if (f2fs_has_inline_data(inode)) {
<<<<<<< HEAD
		if (pos + len <= MAX_INLINE_DATA(inode)) {
			f2fs_do_read_inline_data(page, ipage);
=======
		if (pos + len <= MAX_INLINE_DATA) {
			read_inline_data(page, ipage);
>>>>>>> v4.9.227
			set_inode_flag(inode, FI_DATA_EXIST);
			if (inode->i_nlink)
				set_inline_node(ipage);
		} else {
			err = f2fs_convert_inline_page(&dn, page);
			if (err)
				goto out;
			if (dn.data_blkaddr == NULL_ADDR)
				err = f2fs_get_block(&dn, index);
		}
	} else if (locked) {
		err = f2fs_get_block(&dn, index);
	} else {
		if (f2fs_lookup_extent_cache(inode, index, &ei)) {
			dn.data_blkaddr = ei.blk + index - ei.fofs;
		} else {
			/* hole case */
<<<<<<< HEAD
			err = f2fs_get_dnode_of_data(&dn, index, LOOKUP_NODE);
			if (err || dn.data_blkaddr == NULL_ADDR) {
				f2fs_put_dnode(&dn);
				__do_map_lock(sbi, F2FS_GET_BLOCK_PRE_AIO,
								true);
=======
			err = get_dnode_of_data(&dn, index, LOOKUP_NODE);
			if (err || dn.data_blkaddr == NULL_ADDR) {
				f2fs_put_dnode(&dn);
				f2fs_lock_op(sbi);
>>>>>>> v4.9.227
				locked = true;
				goto restart;
			}
		}
	}

	/* convert_inline_page can make node_changed */
	*blk_addr = dn.data_blkaddr;
	*node_changed = dn.node_changed;
out:
	f2fs_put_dnode(&dn);
unlock_out:
	if (locked)
<<<<<<< HEAD
		__do_map_lock(sbi, F2FS_GET_BLOCK_PRE_AIO, false);
=======
		f2fs_unlock_op(sbi);
>>>>>>> v4.9.227
	return err;
}

static int f2fs_write_begin(struct file *file, struct address_space *mapping,
		loff_t pos, unsigned len, unsigned flags,
		struct page **pagep, void **fsdata)
{
	struct inode *inode = mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct page *page = NULL;
	pgoff_t index = ((unsigned long long) pos) >> PAGE_SHIFT;
<<<<<<< HEAD
	bool need_balance = false, drop_atomic = false;
	block_t blkaddr = NULL_ADDR;
	int err = 0;

	if (trace_android_fs_datawrite_start_enabled()) {
		char *path, pathbuf[MAX_TRACE_PATHBUF_LEN];

		path = android_fstrace_get_pathname(pathbuf,
						    MAX_TRACE_PATHBUF_LEN,
						    inode);
		trace_android_fs_datawrite_start(inode, pos, len,
						 current->pid, path,
						 current->comm);
	}
	trace_f2fs_write_begin(inode, pos, len, flags);

	if ((f2fs_is_atomic_file(inode) &&
			!f2fs_available_free_memory(sbi, INMEM_PAGES)) ||
			is_inode_flag_set(inode, FI_ATOMIC_REVOKE_REQUEST)) {
		err = -ENOMEM;
		drop_atomic = true;
		goto fail;
	}

=======
	bool need_balance = false;
	block_t blkaddr = NULL_ADDR;
	int err = 0;

	trace_f2fs_write_begin(inode, pos, len, flags);

>>>>>>> v4.9.227
	/*
	 * We should check this at this moment to avoid deadlock on inode page
	 * and #0 page. The locking rule for inline_data conversion should be:
	 * lock_page(page #0) -> lock_page(inode_page)
	 */
	if (index != 0) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			goto fail;
	}
repeat:
	/*
	 * Do not use grab_cache_page_write_begin() to avoid deadlock due to
	 * wait_for_stable_page. Will wait that below with our IO control.
	 */
<<<<<<< HEAD
	page = f2fs_pagecache_get_page(mapping, index,
=======
	page = pagecache_get_page(mapping, index,
>>>>>>> v4.9.227
				FGP_LOCK | FGP_WRITE | FGP_CREAT, GFP_NOFS);
	if (!page) {
		err = -ENOMEM;
		goto fail;
	}

	*pagep = page;

	err = prepare_write_begin(sbi, page, pos, len,
					&blkaddr, &need_balance);
	if (err)
		goto fail;

	if (need_balance && has_not_enough_free_secs(sbi, 0, 0)) {
		unlock_page(page);
		f2fs_balance_fs(sbi, true);
		lock_page(page);
		if (page->mapping != mapping) {
			/* The page got truncated from under us */
			f2fs_put_page(page, 1);
			goto repeat;
		}
	}

	f2fs_wait_on_page_writeback(page, DATA, false);

<<<<<<< HEAD
	/* wait for GCed page writeback via META_MAPPING */
	if (f2fs_post_read_required(inode))
		f2fs_wait_on_block_writeback(sbi, blkaddr);
=======
	/* wait for GCed encrypted page writeback */
	if (f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode))
		f2fs_wait_on_encrypted_page_writeback(sbi, blkaddr);
>>>>>>> v4.9.227

	if (len == PAGE_SIZE || PageUptodate(page))
		return 0;

<<<<<<< HEAD
	if (!(pos & (PAGE_SIZE - 1)) && (pos + len) >= i_size_read(inode)) {
		zero_user_segment(page, len, PAGE_SIZE);
		return 0;
	}

=======
>>>>>>> v4.9.227
	if (blkaddr == NEW_ADDR) {
		zero_user_segment(page, 0, PAGE_SIZE);
		SetPageUptodate(page);
	} else {
<<<<<<< HEAD
		err = f2fs_submit_page_read(inode, page, blkaddr);
		if (err)
			goto fail;
=======
		struct bio *bio;

		bio = f2fs_grab_bio(inode, blkaddr, 1);
		if (IS_ERR(bio)) {
			err = PTR_ERR(bio);
			goto fail;
		}
		bio_set_op_attrs(bio, REQ_OP_READ, READ_SYNC);
		if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
			bio_put(bio);
			err = -EFAULT;
			goto fail;
		}

		__submit_bio(sbi, bio, DATA);
>>>>>>> v4.9.227

		lock_page(page);
		if (unlikely(page->mapping != mapping)) {
			f2fs_put_page(page, 1);
			goto repeat;
		}
		if (unlikely(!PageUptodate(page))) {
			err = -EIO;
			goto fail;
		}
	}
	return 0;

fail:
	f2fs_put_page(page, 1);
	f2fs_write_failed(mapping, pos + len);
<<<<<<< HEAD
	if (drop_atomic)
		f2fs_drop_inmem_pages_all(sbi, false);
=======
>>>>>>> v4.9.227
	return err;
}

static int f2fs_write_end(struct file *file,
			struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = page->mapping->host;

<<<<<<< HEAD
	trace_android_fs_datawrite_end(inode, pos, len);
=======
>>>>>>> v4.9.227
	trace_f2fs_write_end(inode, pos, len, copied);

	/*
	 * This should be come from len == PAGE_SIZE, and we expect copied
	 * should be PAGE_SIZE. Otherwise, we treat it with zero copied and
	 * let generic_perform_write() try to copy data again through copied=0.
	 */
	if (!PageUptodate(page)) {
<<<<<<< HEAD
		if (unlikely(copied != len))
=======
		if (unlikely(copied != PAGE_SIZE))
>>>>>>> v4.9.227
			copied = 0;
		else
			SetPageUptodate(page);
	}
	if (!copied)
		goto unlock_out;

	set_page_dirty(page);
<<<<<<< HEAD
=======
	clear_cold_data(page);
>>>>>>> v4.9.227

	if (pos + copied > i_size_read(inode))
		f2fs_i_size_write(inode, pos + copied);
unlock_out:
	f2fs_put_page(page, 1);
	f2fs_update_time(F2FS_I_SB(inode), REQ_TIME);
	return copied;
}

static int check_direct_IO(struct inode *inode, struct iov_iter *iter,
			   loff_t offset)
{
<<<<<<< HEAD
	unsigned i_blkbits = READ_ONCE(inode->i_blkbits);
	unsigned blkbits = i_blkbits;
	unsigned blocksize_mask = (1 << blkbits) - 1;
	unsigned long align = offset | iov_iter_alignment(iter);
	struct block_device *bdev = inode->i_sb->s_bdev;

	if (align & blocksize_mask) {
		if (bdev)
			blkbits = blksize_bits(bdev_logical_block_size(bdev));
		blocksize_mask = (1 << blkbits) - 1;
		if (align & blocksize_mask)
			return -EINVAL;
		return 1;
	}
	return 0;
}

static ssize_t f2fs_direct_IO(int rw, struct kiocb *iocb, struct iov_iter *iter,
			      loff_t offset)
{
	struct address_space *mapping = iocb->ki_filp->f_mapping;
	struct inode *inode = mapping->host;
	/* struct f2fs_sb_info *sbi = F2FS_I_SB(inode); */
	size_t count = iov_iter_count(iter);
	int err;
	/* enum rw_hint hint = iocb->ki_hint;
	int whint_mode = F2FS_OPTION(sbi).whint_mode; */

	err = check_direct_IO(inode, iter, offset);
	if (err)
		return err < 0 ? err : 0;

	if (f2fs_force_buffered_io(inode, rw))
		return 0;

	if (trace_android_fs_dataread_start_enabled() && (rw == READ)) {
		char *path, pathbuf[MAX_TRACE_PATHBUF_LEN];

		path = android_fstrace_get_pathname(pathbuf,
						    MAX_TRACE_PATHBUF_LEN,
						    inode);
		trace_android_fs_dataread_start(inode, offset,
						count, current->pid, path,
						current->comm);
	}
	if (trace_android_fs_datawrite_start_enabled() && (rw == WRITE)) {
		char *path, pathbuf[MAX_TRACE_PATHBUF_LEN];

		path = android_fstrace_get_pathname(pathbuf,
						    MAX_TRACE_PATHBUF_LEN,
						    inode);
		trace_android_fs_datawrite_start(inode, offset, count,
						 current->pid, path,
						 current->comm);
	}

	trace_f2fs_direct_IO_enter(inode, offset, count, rw);

	/* if (rw == WRITE && whint_mode == WHINT_MODE_OFF)
		iocb->ki_hint = WRITE_LIFE_NOT_SET; */

	down_read(&F2FS_I(inode)->i_gc_rwsem[rw]);
	err = blockdev_direct_IO(rw, iocb, inode, iter, offset, get_data_block_dio);
	up_read(&F2FS_I(inode)->i_gc_rwsem[rw]);

	if (rw & WRITE) {
		/* if (whint_mode == WHINT_MODE_OFF)
			iocb->ki_hint = hint; */
		if (err > 0) {
			f2fs_update_iostat(F2FS_I_SB(inode), APP_DIRECT_IO,
									err);
			set_inode_flag(inode, FI_UPDATE_WRITE);
		} else if (err < 0) {
			f2fs_write_failed(mapping, offset + count);
		}
=======
	unsigned blocksize_mask = inode->i_sb->s_blocksize - 1;

	if (offset & blocksize_mask)
		return -EINVAL;

	if (iov_iter_alignment(iter) & blocksize_mask)
		return -EINVAL;

	return 0;
}

static ssize_t f2fs_direct_IO(struct kiocb *iocb, struct iov_iter *iter)
{
	struct address_space *mapping = iocb->ki_filp->f_mapping;
	struct inode *inode = mapping->host;
	size_t count = iov_iter_count(iter);
	loff_t offset = iocb->ki_pos;
	int rw = iov_iter_rw(iter);
	int err;

	err = check_direct_IO(inode, iter, offset);
	if (err)
		return err;

	if (f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode))
		return 0;
	if (test_opt(F2FS_I_SB(inode), LFS))
		return 0;

	trace_f2fs_direct_IO_enter(inode, offset, count, rw);

	down_read(&F2FS_I(inode)->dio_rwsem[rw]);
	err = blockdev_direct_IO(iocb, inode, iter, get_data_block_dio);
	up_read(&F2FS_I(inode)->dio_rwsem[rw]);

	if (rw == WRITE) {
		if (err > 0)
			set_inode_flag(inode, FI_UPDATE_WRITE);
		else if (err < 0)
			f2fs_write_failed(mapping, offset + count);
>>>>>>> v4.9.227
	}

	trace_f2fs_direct_IO_exit(inode, offset, count, rw, err);

<<<<<<< HEAD
	if (trace_android_fs_dataread_start_enabled() && (rw == READ))
		trace_android_fs_dataread_end(inode, offset, count);
	if (trace_android_fs_datawrite_start_enabled() && (rw == WRITE))
		trace_android_fs_datawrite_end(inode, offset, count);

=======
>>>>>>> v4.9.227
	return err;
}

void f2fs_invalidate_page(struct page *page, unsigned int offset,
							unsigned int length)
{
	struct inode *inode = page->mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (inode->i_ino >= F2FS_ROOT_INO(sbi) &&
		(offset % PAGE_SIZE || length != PAGE_SIZE))
		return;

	if (PageDirty(page)) {
<<<<<<< HEAD
		if (inode->i_ino == F2FS_META_INO(sbi)) {
			dec_page_count(sbi, F2FS_DIRTY_META);
		} else if (inode->i_ino == F2FS_NODE_INO(sbi)) {
			dec_page_count(sbi, F2FS_DIRTY_NODES);
		} else {
			inode_dec_dirty_pages(inode);
			f2fs_remove_dirty_inode(inode);
		}
	}

	/* This is atomic written page, keep Private */
	if (IS_ATOMIC_WRITTEN_PAGE(page))
		return f2fs_drop_inmem_page(inode, page);
=======
		if (inode->i_ino == F2FS_META_INO(sbi))
			dec_page_count(sbi, F2FS_DIRTY_META);
		else if (inode->i_ino == F2FS_NODE_INO(sbi))
			dec_page_count(sbi, F2FS_DIRTY_NODES);
		else
			inode_dec_dirty_pages(inode);
	}

	clear_cold_data(page);

	/* This is atomic written page, keep Private */
	if (IS_ATOMIC_WRITTEN_PAGE(page))
		return;
>>>>>>> v4.9.227

	set_page_private(page, 0);
	ClearPagePrivate(page);
}

int f2fs_release_page(struct page *page, gfp_t wait)
{
	/* If this is dirty page, keep PagePrivate */
	if (PageDirty(page))
		return 0;

	/* This is atomic written page, keep Private */
	if (IS_ATOMIC_WRITTEN_PAGE(page))
		return 0;

<<<<<<< HEAD
=======
	clear_cold_data(page);
>>>>>>> v4.9.227
	set_page_private(page, 0);
	ClearPagePrivate(page);
	return 1;
}

<<<<<<< HEAD
=======
/*
 * This was copied from __set_page_dirty_buffers which gives higher performance
 * in very high speed storages. (e.g., pmem)
 */
void f2fs_set_page_dirty_nobuffers(struct page *page)
{
	struct address_space *mapping = page->mapping;
	unsigned long flags;

	if (unlikely(!mapping))
		return;

	spin_lock(&mapping->private_lock);
	lock_page_memcg(page);
	SetPageDirty(page);
	spin_unlock(&mapping->private_lock);

	spin_lock_irqsave(&mapping->tree_lock, flags);
	WARN_ON_ONCE(!PageUptodate(page));
	account_page_dirtied(page, mapping);
	radix_tree_tag_set(&mapping->page_tree,
			page_index(page), PAGECACHE_TAG_DIRTY);
	spin_unlock_irqrestore(&mapping->tree_lock, flags);
	unlock_page_memcg(page);

	__mark_inode_dirty(mapping->host, I_DIRTY_PAGES);
	return;
}

>>>>>>> v4.9.227
static int f2fs_set_data_page_dirty(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;

	trace_f2fs_set_page_dirty(page, DATA);

	if (!PageUptodate(page))
		SetPageUptodate(page);

<<<<<<< HEAD
	/* don't remain PG_checked flag which was set during GC */
	if (is_cold_data(page))
		clear_cold_data(page);

	if (f2fs_is_atomic_file(inode) && !f2fs_is_commit_atomic_write(inode)) {
		if (!IS_ATOMIC_WRITTEN_PAGE(page)) {
			f2fs_register_inmem_page(inode, page);
=======
	if (f2fs_is_atomic_file(inode)) {
		if (!IS_ATOMIC_WRITTEN_PAGE(page)) {
			register_inmem_page(inode, page);
>>>>>>> v4.9.227
			return 1;
		}
		/*
		 * Previously, this page has been registered, we just
		 * return here.
		 */
		return 0;
	}

	if (!PageDirty(page)) {
<<<<<<< HEAD
		__set_page_dirty_nobuffers(page);
		f2fs_update_dirty_page(inode, page);
=======
		f2fs_set_page_dirty_nobuffers(page);
		update_dirty_page(inode, page);
>>>>>>> v4.9.227
		return 1;
	}
	return 0;
}

static sector_t f2fs_bmap(struct address_space *mapping, sector_t block)
{
	struct inode *inode = mapping->host;

	if (f2fs_has_inline_data(inode))
		return 0;

	/* make sure allocating whole blocks */
	if (mapping_tagged(mapping, PAGECACHE_TAG_DIRTY))
		filemap_write_and_wait(mapping);

	return generic_block_bmap(mapping, block, get_data_block_bmap);
}

#ifdef CONFIG_MIGRATION
#include <linux/migrate.h>

int f2fs_migrate_page(struct address_space *mapping,
		struct page *newpage, struct page *page, enum migrate_mode mode)
{
	int rc, extra_count;
	struct f2fs_inode_info *fi = F2FS_I(mapping->host);
	bool atomic_written = IS_ATOMIC_WRITTEN_PAGE(page);

	BUG_ON(PageWriteback(page));

	/* migrating an atomic written page is safe with the inmem_lock hold */
<<<<<<< HEAD
	if (atomic_written) {
		if (mode != MIGRATE_SYNC)
			return -EBUSY;
		if (!mutex_trylock(&fi->inmem_lock))
			return -EAGAIN;
	}
=======
	if (atomic_written && !mutex_trylock(&fi->inmem_lock))
		return -EAGAIN;
>>>>>>> v4.9.227

	/*
	 * A reference is expected if PagePrivate set when move mapping,
	 * however F2FS breaks this for maintaining dirty page counts when
	 * truncating pages. So here adjusting the 'extra_count' make it work.
	 */
	extra_count = (atomic_written ? 1 : 0) - page_has_private(page);
	rc = migrate_page_move_mapping(mapping, newpage,
				page, NULL, mode, extra_count);
	if (rc != MIGRATEPAGE_SUCCESS) {
		if (atomic_written)
			mutex_unlock(&fi->inmem_lock);
		return rc;
	}

	if (atomic_written) {
		struct inmem_pages *cur;
		list_for_each_entry(cur, &fi->inmem_pages, list)
			if (cur->page == page) {
				cur->page = newpage;
				break;
			}
		mutex_unlock(&fi->inmem_lock);
		put_page(page);
		get_page(newpage);
	}

	if (PagePrivate(page))
		SetPagePrivate(newpage);
	set_page_private(newpage, page_private(page));

	migrate_page_copy(newpage, page);

	return MIGRATEPAGE_SUCCESS;
}
#endif

const struct address_space_operations f2fs_dblock_aops = {
	.readpage	= f2fs_read_data_page,
	.readpages	= f2fs_read_data_pages,
	.writepage	= f2fs_write_data_page,
	.writepages	= f2fs_write_data_pages,
	.write_begin	= f2fs_write_begin,
	.write_end	= f2fs_write_end,
	.set_page_dirty	= f2fs_set_data_page_dirty,
	.invalidatepage	= f2fs_invalidate_page,
	.releasepage	= f2fs_release_page,
	.direct_IO	= f2fs_direct_IO,
	.bmap		= f2fs_bmap,
#ifdef CONFIG_MIGRATION
	.migratepage    = f2fs_migrate_page,
#endif
};
<<<<<<< HEAD

void f2fs_clear_radix_tree_dirty_tag(struct page *page)
{
	struct address_space *mapping = page_mapping(page);
	unsigned long flags;

	spin_lock_irqsave(&mapping->tree_lock, flags);
	radix_tree_tag_clear(&mapping->page_tree, page_index(page),
					PAGECACHE_TAG_DIRTY);
	spin_unlock_irqrestore(&mapping->tree_lock, flags);
}

int __init f2fs_init_post_read_processing(void)
{
	bio_post_read_ctx_cache = KMEM_CACHE(bio_post_read_ctx, 0);
	if (!bio_post_read_ctx_cache)
		goto fail;
	bio_post_read_ctx_pool =
		mempool_create_slab_pool(NUM_PREALLOC_POST_READ_CTXS,
					 bio_post_read_ctx_cache);
	if (!bio_post_read_ctx_pool)
		goto fail_free_cache;
	return 0;

fail_free_cache:
	kmem_cache_destroy(bio_post_read_ctx_cache);
fail:
	return -ENOMEM;
}

void __exit f2fs_destroy_post_read_processing(void)
{
	mempool_destroy(bio_post_read_ctx_pool);
	kmem_cache_destroy(bio_post_read_ctx_cache);
}
=======
>>>>>>> v4.9.227
