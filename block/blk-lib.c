/*
 * Functions related to generic helpers functions
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/scatterlist.h>

#include "blk.h"

<<<<<<< HEAD
struct bio_batch {
	atomic_t		done;
	unsigned long		flags;
	struct completion	*wait;
};

static void bio_batch_end_io(struct bio *bio, int err)
{
	struct bio_batch *bb = bio->bi_private;

	if (err && (err != -EOPNOTSUPP))
		clear_bit(BIO_UPTODATE, &bb->flags);
	if (atomic_dec_and_test(&bb->done))
		complete(bb->wait);
	bio_put(bio);
}

static struct bio *next_bio(struct bio *bio, unsigned int nr_pages,
		int type, gfp_t gfp)
=======
static struct bio *next_bio(struct bio *bio, unsigned int nr_pages,
		gfp_t gfp)
>>>>>>> v4.9.227
{
	struct bio *new = bio_alloc(gfp, nr_pages);

	if (bio) {
		bio_chain(bio, new);
<<<<<<< HEAD
		submit_bio(type, bio);
=======
		submit_bio(bio);
>>>>>>> v4.9.227
	}

	return new;
}

int __blkdev_issue_discard(struct block_device *bdev, sector_t sector,
<<<<<<< HEAD
		sector_t nr_sects, gfp_t gfp_mask, unsigned long flags,
=======
		sector_t nr_sects, gfp_t gfp_mask, int flags,
>>>>>>> v4.9.227
		struct bio **biop)
{
	struct request_queue *q = bdev_get_queue(bdev);
	struct bio *bio = *biop;
<<<<<<< HEAD
	int type = REQ_WRITE | REQ_DISCARD | REQ_PRIO;
	unsigned int max_discard_sectors, granularity;
=======
	unsigned int granularity;
	enum req_op op;
>>>>>>> v4.9.227
	int alignment;
	sector_t bs_mask;

	if (!q)
		return -ENXIO;

<<<<<<< HEAD
	if (!blk_queue_discard(q))
		return -EOPNOTSUPP;
=======
	if (flags & BLKDEV_DISCARD_SECURE) {
		if (flags & BLKDEV_DISCARD_ZERO)
			return -EOPNOTSUPP;
		if (!blk_queue_secure_erase(q))
			return -EOPNOTSUPP;
		op = REQ_OP_SECURE_ERASE;
	} else {
		if (!blk_queue_discard(q))
			return -EOPNOTSUPP;
		if ((flags & BLKDEV_DISCARD_ZERO) &&
		    !q->limits.discard_zeroes_data)
			return -EOPNOTSUPP;
		op = REQ_OP_DISCARD;
	}
>>>>>>> v4.9.227

	bs_mask = (bdev_logical_block_size(bdev) >> 9) - 1;
	if ((sector | nr_sects) & bs_mask)
		return -EINVAL;

<<<<<<< HEAD
	/* Zero-sector (unknown) and one-sector granularities are the same. */
	granularity = max(q->limits.discard_granularity >> 9, 1U);
	alignment = (bdev_discard_alignment(bdev) >> 9) % granularity;

	/*
	 * Ensure that max_discard_sectors is of the proper
	 * granularity, so that requests stay aligned after a split.
	 */
	max_discard_sectors = min(q->limits.max_discard_sectors, UINT_MAX >> 9);
	max_discard_sectors -= max_discard_sectors % granularity;
	if (unlikely(!max_discard_sectors)) {
		/* Avoid infinite loop below. Being cautious never hurts. */
		return -EOPNOTSUPP;
	}

	if (flags & BLKDEV_DISCARD_SECURE) {
		if (!blk_queue_secdiscard(q))
			return -EOPNOTSUPP;
		type |= REQ_SECURE;
	}

	if (flags & BLKDEV_DISCARD_SYNC)
		type |= REQ_SYNC;
=======
	/* Zero-sector (unknown) and one-sector granularities are the same.  */
	granularity = max(q->limits.discard_granularity >> 9, 1U);
	alignment = (bdev_discard_alignment(bdev) >> 9) % granularity;

>>>>>>> v4.9.227
	while (nr_sects) {
		unsigned int req_sects;
		sector_t end_sect, tmp;

<<<<<<< HEAD
		req_sects = min_t(sector_t, nr_sects, max_discard_sectors);

		/**
=======
		/*
		 * Issue in chunks of the user defined max discard setting,
		 * ensuring that bi_size doesn't overflow
		 */
		req_sects = min_t(sector_t, nr_sects,
					q->limits.max_discard_sectors);
		if (!req_sects)
			goto fail;
		if (req_sects > UINT_MAX >> 9)
			req_sects = UINT_MAX >> 9;

		/*
>>>>>>> v4.9.227
		 * If splitting a request, and the next starting sector would be
		 * misaligned, stop the discard at the previous aligned sector.
		 */
		end_sect = sector + req_sects;
		tmp = end_sect;
		if (req_sects < nr_sects &&
		    sector_div(tmp, granularity) != alignment) {
			end_sect = end_sect - alignment;
			sector_div(end_sect, granularity);
			end_sect = end_sect * granularity + alignment;
			req_sects = end_sect - sector;
		}

<<<<<<< HEAD
		bio = next_bio(bio, 1, type, gfp_mask);
		bio->bi_iter.bi_sector = sector;
		bio->bi_bdev = bdev;
		bio->bi_rw = type;
=======
		bio = next_bio(bio, 1, gfp_mask);
		bio->bi_iter.bi_sector = sector;
		bio->bi_bdev = bdev;
		bio_set_op_attrs(bio, op, 0);
>>>>>>> v4.9.227

		bio->bi_iter.bi_size = req_sects << 9;
		nr_sects -= req_sects;
		sector = end_sect;

		/*
		 * We can loop for a long time in here, if someone does
		 * full device discards (like mkfs). Be nice and allow
		 * us to schedule out to avoid softlocking if preempt
		 * is disabled.
		 */
		cond_resched();
	}

	*biop = bio;
	return 0;
<<<<<<< HEAD
=======

fail:
	if (bio) {
		submit_bio_wait(bio);
		bio_put(bio);
	}
	*biop = NULL;
	return -EOPNOTSUPP;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(__blkdev_issue_discard);

/**
 * blkdev_issue_discard - queue a discard
 * @bdev:	blockdev to issue discard for
 * @sector:	start sector
 * @nr_sects:	number of sectors to discard
 * @gfp_mask:	memory allocation flags (for bio_alloc)
 * @flags:	BLKDEV_IFL_* flags to control behaviour
 *
 * Description:
 *    Issue a discard request for the sectors in question.
 */
int blkdev_issue_discard(struct block_device *bdev, sector_t sector,
		sector_t nr_sects, gfp_t gfp_mask, unsigned long flags)
{
	struct bio *bio = NULL;
	struct blk_plug plug;
<<<<<<< HEAD
	int type = REQ_WRITE | REQ_DISCARD | REQ_PRIO;
=======
>>>>>>> v4.9.227
	int ret;

	blk_start_plug(&plug);
	ret = __blkdev_issue_discard(bdev, sector, nr_sects, gfp_mask, flags,
<<<<<<< HEAD
				     &bio);
	if (!ret && bio) {
		ret = submit_bio_wait(type, bio);
		if (ret == -EOPNOTSUPP)
=======
			&bio);
	if (!ret && bio) {
		ret = submit_bio_wait(bio);
		if (ret == -EOPNOTSUPP && !(flags & BLKDEV_DISCARD_ZERO))
>>>>>>> v4.9.227
			ret = 0;
		bio_put(bio);
	}
	blk_finish_plug(&plug);

	return ret;
}
EXPORT_SYMBOL(blkdev_issue_discard);

/**
 * blkdev_issue_write_same - queue a write same operation
 * @bdev:	target blockdev
 * @sector:	start sector
 * @nr_sects:	number of sectors to write
 * @gfp_mask:	memory allocation flags (for bio_alloc)
 * @page:	page containing data to write
 *
 * Description:
 *    Issue a write same request for the sectors in question.
 */
int blkdev_issue_write_same(struct block_device *bdev, sector_t sector,
			    sector_t nr_sects, gfp_t gfp_mask,
			    struct page *page)
{
<<<<<<< HEAD
	DECLARE_COMPLETION_ONSTACK(wait);
	struct request_queue *q = bdev_get_queue(bdev);
	unsigned int max_write_same_sectors;
	struct bio_batch bb;
	struct bio *bio;
	int ret = 0;
=======
	struct request_queue *q = bdev_get_queue(bdev);
	unsigned int max_write_same_sectors;
	struct bio *bio = NULL;
	int ret = 0;
	sector_t bs_mask;
>>>>>>> v4.9.227

	if (!q)
		return -ENXIO;

<<<<<<< HEAD
	max_write_same_sectors = q->limits.max_write_same_sectors;

	if (max_write_same_sectors == 0)
		return -EOPNOTSUPP;

	atomic_set(&bb.done, 1);
	bb.flags = 1 << BIO_UPTODATE;
	bb.wait = &wait;

	while (nr_sects) {
		bio = bio_alloc(gfp_mask, 1);
		if (!bio) {
			ret = -ENOMEM;
			break;
		}

		bio->bi_iter.bi_sector = sector;
		bio->bi_end_io = bio_batch_end_io;
		bio->bi_bdev = bdev;
		bio->bi_private = &bb;
=======
	bs_mask = (bdev_logical_block_size(bdev) >> 9) - 1;
	if ((sector | nr_sects) & bs_mask)
		return -EINVAL;

	/* Ensure that max_write_same_sectors doesn't overflow bi_size */
	max_write_same_sectors = UINT_MAX >> 9;

	while (nr_sects) {
		bio = next_bio(bio, 1, gfp_mask);
		bio->bi_iter.bi_sector = sector;
		bio->bi_bdev = bdev;
>>>>>>> v4.9.227
		bio->bi_vcnt = 1;
		bio->bi_io_vec->bv_page = page;
		bio->bi_io_vec->bv_offset = 0;
		bio->bi_io_vec->bv_len = bdev_logical_block_size(bdev);
<<<<<<< HEAD
=======
		bio_set_op_attrs(bio, REQ_OP_WRITE_SAME, 0);
>>>>>>> v4.9.227

		if (nr_sects > max_write_same_sectors) {
			bio->bi_iter.bi_size = max_write_same_sectors << 9;
			nr_sects -= max_write_same_sectors;
			sector += max_write_same_sectors;
		} else {
			bio->bi_iter.bi_size = nr_sects << 9;
			nr_sects = 0;
		}
<<<<<<< HEAD

		atomic_inc(&bb.done);
		submit_bio(REQ_WRITE | REQ_WRITE_SAME, bio);
	}

	/* Wait for bios in-flight */
	if (!atomic_dec_and_test(&bb.done))
		wait_for_completion_io(&wait);

	if (!test_bit(BIO_UPTODATE, &bb.flags))
		ret = -ENOTSUPP;

=======
	}

	if (bio) {
		ret = submit_bio_wait(bio);
		bio_put(bio);
	}
>>>>>>> v4.9.227
	return ret;
}
EXPORT_SYMBOL(blkdev_issue_write_same);

/**
 * blkdev_issue_zeroout - generate number of zero filed write bios
 * @bdev:	blockdev to issue
 * @sector:	start sector
 * @nr_sects:	number of sectors to write
 * @gfp_mask:	memory allocation flags (for bio_alloc)
 *
 * Description:
 *  Generate and issue number of bios with zerofiled pages.
 */

static int __blkdev_issue_zeroout(struct block_device *bdev, sector_t sector,
				  sector_t nr_sects, gfp_t gfp_mask)
{
	int ret;
<<<<<<< HEAD
	struct bio *bio;
	struct bio_batch bb;
	unsigned int sz;
	DECLARE_COMPLETION_ONSTACK(wait);

	atomic_set(&bb.done, 1);
	bb.flags = 1 << BIO_UPTODATE;
	bb.wait = &wait;

	ret = 0;
	while (nr_sects != 0) {
		bio = bio_alloc(gfp_mask,
				min(nr_sects, (sector_t)BIO_MAX_PAGES));
		if (!bio) {
			ret = -ENOMEM;
			break;
		}

		bio->bi_iter.bi_sector = sector;
		bio->bi_bdev   = bdev;
		bio->bi_end_io = bio_batch_end_io;
		bio->bi_private = &bb;
=======
	struct bio *bio = NULL;
	unsigned int sz;
	sector_t bs_mask;

	bs_mask = (bdev_logical_block_size(bdev) >> 9) - 1;
	if ((sector | nr_sects) & bs_mask)
		return -EINVAL;

	while (nr_sects != 0) {
		bio = next_bio(bio, min(nr_sects, (sector_t)BIO_MAX_PAGES),
				gfp_mask);
		bio->bi_iter.bi_sector = sector;
		bio->bi_bdev   = bdev;
		bio_set_op_attrs(bio, REQ_OP_WRITE, 0);
>>>>>>> v4.9.227

		while (nr_sects != 0) {
			sz = min((sector_t) PAGE_SIZE >> 9 , nr_sects);
			ret = bio_add_page(bio, ZERO_PAGE(0), sz << 9, 0);
			nr_sects -= ret >> 9;
			sector += ret >> 9;
			if (ret < (sz << 9))
				break;
		}
<<<<<<< HEAD
		ret = 0;
		atomic_inc(&bb.done);
		submit_bio(WRITE, bio);
	}

	/* Wait for bios in-flight */
	if (!atomic_dec_and_test(&bb.done))
		wait_for_completion_io(&wait);

	if (!test_bit(BIO_UPTODATE, &bb.flags))
		/* One of bios in the batch was completed with error.*/
		ret = -EIO;

	return ret;
=======
	}

	if (bio) {
		ret = submit_bio_wait(bio);
		bio_put(bio);
		return ret;
	}
	return 0;
>>>>>>> v4.9.227
}

/**
 * blkdev_issue_zeroout - zero-fill a block range
 * @bdev:	blockdev to write
 * @sector:	start sector
 * @nr_sects:	number of sectors to write
 * @gfp_mask:	memory allocation flags (for bio_alloc)
<<<<<<< HEAD
 *
 * Description:
 *  Generate and issue number of bios with zerofiled pages.
 */

int blkdev_issue_zeroout(struct block_device *bdev, sector_t sector,
			 sector_t nr_sects, gfp_t gfp_mask)
{
	if (bdev_write_same(bdev)) {
		unsigned char bdn[BDEVNAME_SIZE];

		if (!blkdev_issue_write_same(bdev, sector, nr_sects, gfp_mask,
					     ZERO_PAGE(0)))
			return 0;

		bdevname(bdev, bdn);
		pr_err("%s: WRITE SAME failed. Manually zeroing.\n", bdn);
	}

=======
 * @discard:	whether to discard the block range
 *
 * Description:
 *  Zero-fill a block range.  If the discard flag is set and the block
 *  device guarantees that subsequent READ operations to the block range
 *  in question will return zeroes, the blocks will be discarded. Should
 *  the discard request fail, if the discard flag is not set, or if
 *  discard_zeroes_data is not supported, this function will resort to
 *  zeroing the blocks manually, thus provisioning (allocating,
 *  anchoring) them. If the block device supports the WRITE SAME command
 *  blkdev_issue_zeroout() will use it to optimize the process of
 *  clearing the block range. Otherwise the zeroing will be performed
 *  using regular WRITE calls.
 */

int blkdev_issue_zeroout(struct block_device *bdev, sector_t sector,
			 sector_t nr_sects, gfp_t gfp_mask, bool discard)
{
	if (discard) {
		if (!blkdev_issue_discard(bdev, sector, nr_sects, gfp_mask,
				BLKDEV_DISCARD_ZERO))
			return 0;
	}

	if (bdev_write_same(bdev) &&
	    blkdev_issue_write_same(bdev, sector, nr_sects, gfp_mask,
				    ZERO_PAGE(0)) == 0)
		return 0;

>>>>>>> v4.9.227
	return __blkdev_issue_zeroout(bdev, sector, nr_sects, gfp_mask);
}
EXPORT_SYMBOL(blkdev_issue_zeroout);
