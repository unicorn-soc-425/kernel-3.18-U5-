/*
 * Functions related to mapping data to requests
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
<<<<<<< HEAD
#include <scsi/sg.h>		/* for struct sg_iovec */

#include "blk.h"

int blk_rq_append_bio(struct request_queue *q, struct request *rq,
		      struct bio *bio)
{
	if (!rq->bio)
		blk_rq_bio_prep(q, rq, bio);
	else if (!ll_back_merge_fn(q, rq, bio))
		return -EINVAL;
	else {
		rq->biotail->bi_next = bio;
		rq->biotail = bio;

		rq->__data_len += bio->bi_iter.bi_size;
	}
	return 0;
}
=======
#include <linux/uio.h>

#include "blk.h"

/*
 * Append a bio to a passthrough request.  Only works can be merged into
 * the request based on the driver constraints.
 */
int blk_rq_append_bio(struct request *rq, struct bio *bio)
{
	if (!rq->bio) {
		blk_rq_bio_prep(rq->q, rq, bio);
	} else {
		if (!ll_back_merge_fn(rq->q, rq, bio))
			return -EINVAL;

		rq->biotail->bi_next = bio;
		rq->biotail = bio;
		rq->__data_len += bio->bi_iter.bi_size;
	}

	return 0;
}
EXPORT_SYMBOL(blk_rq_append_bio);
>>>>>>> v4.9.227

static int __blk_rq_unmap_user(struct bio *bio)
{
	int ret = 0;

	if (bio) {
		if (bio_flagged(bio, BIO_USER_MAPPED))
			bio_unmap_user(bio);
		else
			ret = bio_uncopy_user(bio);
	}

	return ret;
}

<<<<<<< HEAD
static int __blk_rq_map_user(struct request_queue *q, struct request *rq,
			     struct rq_map_data *map_data, void __user *ubuf,
			     unsigned int len, gfp_t gfp_mask)
{
	unsigned long uaddr;
	struct bio *bio, *orig_bio;
	int reading, ret;

	reading = rq_data_dir(rq) == READ;

	/*
	 * if alignment requirement is satisfied, map in user pages for
	 * direct dma. else, set up kernel bounce buffers
	 */
	uaddr = (unsigned long) ubuf;
	if (blk_rq_aligned(q, uaddr, len) && !map_data)
		bio = bio_map_user(q, NULL, uaddr, len, reading, gfp_mask);
	else
		bio = bio_copy_user(q, map_data, uaddr, len, reading, gfp_mask);
=======
static int __blk_rq_map_user_iov(struct request *rq,
		struct rq_map_data *map_data, struct iov_iter *iter,
		gfp_t gfp_mask, bool copy)
{
	struct request_queue *q = rq->q;
	struct bio *bio, *orig_bio;
	int ret;

	if (copy)
		bio = bio_copy_user_iov(q, map_data, iter, gfp_mask);
	else
		bio = bio_map_user_iov(q, iter, gfp_mask);
>>>>>>> v4.9.227

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	if (map_data && map_data->null_mapped)
<<<<<<< HEAD
		bio->bi_flags |= (1 << BIO_NULL_MAPPED);
=======
		bio_set_flag(bio, BIO_NULL_MAPPED);

	iov_iter_advance(iter, bio->bi_iter.bi_size);
	if (map_data)
		map_data->offset += bio->bi_iter.bi_size;
>>>>>>> v4.9.227

	orig_bio = bio;
	blk_queue_bounce(q, &bio);

	/*
	 * We link the bounce buffer in and could have to traverse it
	 * later so we have to get a ref to prevent it from being freed
	 */
	bio_get(bio);

<<<<<<< HEAD
	ret = blk_rq_append_bio(q, rq, bio);
	if (!ret)
		return bio->bi_iter.bi_size;

	/* if it was boucned we must call the end io function */
	bio_endio(bio, 0);
	__blk_rq_unmap_user(orig_bio);
	bio_put(bio);
	return ret;
}

/**
 * blk_rq_map_user - map user data to a request, for REQ_TYPE_BLOCK_PC usage
 * @q:		request queue where request should be inserted
 * @rq:		request structure to fill
 * @map_data:   pointer to the rq_map_data holding pages (if necessary)
 * @ubuf:	the user buffer
 * @len:	length of user data
 * @gfp_mask:	memory allocation flags
 *
 * Description:
 *    Data will be mapped directly for zero copy I/O, if possible. Otherwise
 *    a kernel bounce buffer is used.
 *
 *    A matching blk_rq_unmap_user() must be issued at the end of I/O, while
 *    still in process context.
 *
 *    Note: The mapped bio may need to be bounced through blk_queue_bounce()
 *    before being submitted to the device, as pages mapped may be out of
 *    reach. It's the callers responsibility to make sure this happens. The
 *    original bio must be passed back in to blk_rq_unmap_user() for proper
 *    unmapping.
 */
int blk_rq_map_user(struct request_queue *q, struct request *rq,
		    struct rq_map_data *map_data, void __user *ubuf,
		    unsigned long len, gfp_t gfp_mask)
{
	unsigned long bytes_read = 0;
	struct bio *bio = NULL;
	int ret;

	if (len > (queue_max_hw_sectors(q) << 9))
		return -EINVAL;
	if (!len)
		return -EINVAL;

	if (!ubuf && (!map_data || !map_data->null_mapped))
		return -EINVAL;

	while (bytes_read != len) {
		unsigned long map_len, end, start;

		map_len = min_t(unsigned long, len - bytes_read, BIO_MAX_SIZE);
		end = ((unsigned long)ubuf + map_len + PAGE_SIZE - 1)
								>> PAGE_SHIFT;
		start = (unsigned long)ubuf >> PAGE_SHIFT;

		/*
		 * A bad offset could cause us to require BIO_MAX_PAGES + 1
		 * pages. If this happens we just lower the requested
		 * mapping len by a page so that we can fit
		 */
		if (end - start > BIO_MAX_PAGES)
			map_len -= PAGE_SIZE;

		ret = __blk_rq_map_user(q, rq, map_data, ubuf, map_len,
					gfp_mask);
		if (ret < 0)
			goto unmap_rq;
		if (!bio)
			bio = rq->bio;
		bytes_read += ret;
		ubuf += ret;

		if (map_data)
			map_data->offset += ret;
	}

	if (!bio_flagged(bio, BIO_USER_MAPPED))
		rq->cmd_flags |= REQ_COPY_USER;

	return 0;
unmap_rq:
	blk_rq_unmap_user(bio);
	rq->bio = NULL;
	return ret;
}
EXPORT_SYMBOL(blk_rq_map_user);
=======
	ret = blk_rq_append_bio(rq, bio);
	if (ret) {
		bio_endio(bio);
		__blk_rq_unmap_user(orig_bio);
		bio_put(bio);
		return ret;
	}

	return 0;
}
>>>>>>> v4.9.227

/**
 * blk_rq_map_user_iov - map user data to a request, for REQ_TYPE_BLOCK_PC usage
 * @q:		request queue where request should be inserted
 * @rq:		request to map data to
 * @map_data:   pointer to the rq_map_data holding pages (if necessary)
<<<<<<< HEAD
 * @iov:	pointer to the iovec
 * @iov_count:	number of elements in the iovec
 * @len:	I/O byte count
=======
 * @iter:	iovec iterator
>>>>>>> v4.9.227
 * @gfp_mask:	memory allocation flags
 *
 * Description:
 *    Data will be mapped directly for zero copy I/O, if possible. Otherwise
 *    a kernel bounce buffer is used.
 *
 *    A matching blk_rq_unmap_user() must be issued at the end of I/O, while
 *    still in process context.
 *
 *    Note: The mapped bio may need to be bounced through blk_queue_bounce()
 *    before being submitted to the device, as pages mapped may be out of
 *    reach. It's the callers responsibility to make sure this happens. The
 *    original bio must be passed back in to blk_rq_unmap_user() for proper
 *    unmapping.
 */
int blk_rq_map_user_iov(struct request_queue *q, struct request *rq,
<<<<<<< HEAD
			struct rq_map_data *map_data, const struct sg_iovec *iov,
			int iov_count, unsigned int len, gfp_t gfp_mask)
{
	struct bio *bio;
	int i, read = rq_data_dir(rq) == READ;
	int unaligned = 0;

	if (!iov || iov_count <= 0)
		return -EINVAL;

	for (i = 0; i < iov_count; i++) {
		unsigned long uaddr = (unsigned long)iov[i].iov_base;

		if (!iov[i].iov_len)
			return -EINVAL;

		/*
		 * Keep going so we check length of all segments
		 */
		if (uaddr & queue_dma_alignment(q))
			unaligned = 1;
	}

	if (unaligned || (q->dma_pad_mask & len) || map_data)
		bio = bio_copy_user_iov(q, map_data, iov, iov_count, read,
					gfp_mask);
	else
		bio = bio_map_user_iov(q, NULL, iov, iov_count, read, gfp_mask);

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	if (bio->bi_iter.bi_size != len) {
		/*
		 * Grab an extra reference to this bio, as bio_unmap_user()
		 * expects to be able to drop it twice as it happens on the
		 * normal IO completion path
		 */
		bio_get(bio);
		bio_endio(bio, 0);
		__blk_rq_unmap_user(bio);
		return -EINVAL;
	}

	if (!bio_flagged(bio, BIO_USER_MAPPED))
		rq->cmd_flags |= REQ_COPY_USER;

	blk_queue_bounce(q, &bio);
	bio_get(bio);
	blk_rq_bio_prep(q, rq, bio);
	return 0;
}
EXPORT_SYMBOL(blk_rq_map_user_iov);

=======
			struct rq_map_data *map_data,
			const struct iov_iter *iter, gfp_t gfp_mask)
{
	bool copy = false;
	unsigned long align = q->dma_pad_mask | queue_dma_alignment(q);
	struct bio *bio = NULL;
	struct iov_iter i;
	int ret = -EINVAL;

	if (!iter_is_iovec(iter))
		goto fail;

	if (map_data)
		copy = true;
	else if (iov_iter_alignment(iter) & align)
		copy = true;
	else if (queue_virt_boundary(q))
		copy = queue_virt_boundary(q) & iov_iter_gap_alignment(iter);

	i = *iter;
	do {
		ret =__blk_rq_map_user_iov(rq, map_data, &i, gfp_mask, copy);
		if (ret)
			goto unmap_rq;
		if (!bio)
			bio = rq->bio;
	} while (iov_iter_count(&i));

	if (!bio_flagged(bio, BIO_USER_MAPPED))
		rq->cmd_flags |= REQ_COPY_USER;
	return 0;

unmap_rq:
	blk_rq_unmap_user(bio);
fail:
	rq->bio = NULL;
	return ret;
}
EXPORT_SYMBOL(blk_rq_map_user_iov);

int blk_rq_map_user(struct request_queue *q, struct request *rq,
		    struct rq_map_data *map_data, void __user *ubuf,
		    unsigned long len, gfp_t gfp_mask)
{
	struct iovec iov;
	struct iov_iter i;
	int ret = import_single_range(rq_data_dir(rq), ubuf, len, &iov, &i);

	if (unlikely(ret < 0))
		return ret;

	return blk_rq_map_user_iov(q, rq, map_data, &i, gfp_mask);
}
EXPORT_SYMBOL(blk_rq_map_user);

>>>>>>> v4.9.227
/**
 * blk_rq_unmap_user - unmap a request with user data
 * @bio:	       start of bio list
 *
 * Description:
 *    Unmap a rq previously mapped by blk_rq_map_user(). The caller must
 *    supply the original rq->bio from the blk_rq_map_user() return, since
 *    the I/O completion may have changed rq->bio.
 */
int blk_rq_unmap_user(struct bio *bio)
{
	struct bio *mapped_bio;
	int ret = 0, ret2;

	while (bio) {
		mapped_bio = bio;
		if (unlikely(bio_flagged(bio, BIO_BOUNCED)))
			mapped_bio = bio->bi_private;

		ret2 = __blk_rq_unmap_user(mapped_bio);
		if (ret2 && !ret)
			ret = ret2;

		mapped_bio = bio;
		bio = bio->bi_next;
		bio_put(mapped_bio);
	}

	return ret;
}
EXPORT_SYMBOL(blk_rq_unmap_user);

/**
 * blk_rq_map_kern - map kernel data to a request, for REQ_TYPE_BLOCK_PC usage
 * @q:		request queue where request should be inserted
 * @rq:		request to fill
 * @kbuf:	the kernel buffer
 * @len:	length of user data
 * @gfp_mask:	memory allocation flags
 *
 * Description:
 *    Data will be mapped directly if possible. Otherwise a bounce
 *    buffer is used. Can be called multiple times to append multiple
 *    buffers.
 */
int blk_rq_map_kern(struct request_queue *q, struct request *rq, void *kbuf,
		    unsigned int len, gfp_t gfp_mask)
{
	int reading = rq_data_dir(rq) == READ;
	unsigned long addr = (unsigned long) kbuf;
	int do_copy = 0;
	struct bio *bio;
	int ret;

	if (len > (queue_max_hw_sectors(q) << 9))
		return -EINVAL;
	if (!len || !kbuf)
		return -EINVAL;

	do_copy = !blk_rq_aligned(q, addr, len) || object_is_on_stack(kbuf);
	if (do_copy)
		bio = bio_copy_kern(q, kbuf, len, gfp_mask, reading);
	else
		bio = bio_map_kern(q, kbuf, len, gfp_mask);

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	if (!reading)
<<<<<<< HEAD
		bio->bi_rw |= REQ_WRITE;
=======
		bio_set_op_attrs(bio, REQ_OP_WRITE, 0);
>>>>>>> v4.9.227

	if (do_copy)
		rq->cmd_flags |= REQ_COPY_USER;

<<<<<<< HEAD
	ret = blk_rq_append_bio(q, rq, bio);
=======
	ret = blk_rq_append_bio(rq, bio);
>>>>>>> v4.9.227
	if (unlikely(ret)) {
		/* request is too big */
		bio_put(bio);
		return ret;
	}

	blk_queue_bounce(q, &rq->bio);
	return 0;
}
EXPORT_SYMBOL(blk_rq_map_kern);
