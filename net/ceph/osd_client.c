
#include <linux/ceph/ceph_debug.h>

#include <linux/module.h>
#include <linux/err.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#ifdef CONFIG_BLOCK
#include <linux/bio.h>
#endif

#include <linux/ceph/libceph.h>
#include <linux/ceph/osd_client.h>
#include <linux/ceph/messenger.h>
#include <linux/ceph/decode.h>
#include <linux/ceph/auth.h>
#include <linux/ceph/pagelist.h>

<<<<<<< HEAD
#define OSD_OP_FRONT_LEN	4096
=======
>>>>>>> v4.9.227
#define OSD_OPREPLY_FRONT_LEN	512

static struct kmem_cache	*ceph_osd_request_cache;

static const struct ceph_connection_operations osd_con_ops;

<<<<<<< HEAD
static void __send_queued(struct ceph_osd_client *osdc);
static int __reset_osd(struct ceph_osd_client *osdc, struct ceph_osd *osd);
static void __register_request(struct ceph_osd_client *osdc,
			       struct ceph_osd_request *req);
static void __unregister_request(struct ceph_osd_client *osdc,
				 struct ceph_osd_request *req);
static void __unregister_linger_request(struct ceph_osd_client *osdc,
					struct ceph_osd_request *req);
static void __enqueue_request(struct ceph_osd_request *req);
static void __send_request(struct ceph_osd_client *osdc,
			   struct ceph_osd_request *req);

=======
>>>>>>> v4.9.227
/*
 * Implement client access to distributed object storage cluster.
 *
 * All data objects are stored within a cluster/cloud of OSDs, or
 * "object storage devices."  (Note that Ceph OSDs have _nothing_ to
 * do with the T10 OSD extensions to SCSI.)  Ceph OSDs are simply
 * remote daemons serving up and coordinating consistent and safe
 * access to storage.
 *
 * Cluster membership and the mapping of data objects onto storage devices
 * are described by the osd map.
 *
 * We keep track of pending OSD requests (read, write), resubmit
 * requests to different OSDs when the cluster topology/data layout
 * change, or retry the affected requests when the communications
 * channel with an OSD is reset.
 */

<<<<<<< HEAD
=======
static void link_request(struct ceph_osd *osd, struct ceph_osd_request *req);
static void unlink_request(struct ceph_osd *osd, struct ceph_osd_request *req);
static void link_linger(struct ceph_osd *osd,
			struct ceph_osd_linger_request *lreq);
static void unlink_linger(struct ceph_osd *osd,
			  struct ceph_osd_linger_request *lreq);

#if 1
static inline bool rwsem_is_wrlocked(struct rw_semaphore *sem)
{
	bool wrlocked = true;

	if (unlikely(down_read_trylock(sem))) {
		wrlocked = false;
		up_read(sem);
	}

	return wrlocked;
}
static inline void verify_osdc_locked(struct ceph_osd_client *osdc)
{
	WARN_ON(!rwsem_is_locked(&osdc->lock));
}
static inline void verify_osdc_wrlocked(struct ceph_osd_client *osdc)
{
	WARN_ON(!rwsem_is_wrlocked(&osdc->lock));
}
static inline void verify_osd_locked(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;

	WARN_ON(!(mutex_is_locked(&osd->lock) &&
		  rwsem_is_locked(&osdc->lock)) &&
		!rwsem_is_wrlocked(&osdc->lock));
}
static inline void verify_lreq_locked(struct ceph_osd_linger_request *lreq)
{
	WARN_ON(!mutex_is_locked(&lreq->lock));
}
#else
static inline void verify_osdc_locked(struct ceph_osd_client *osdc) { }
static inline void verify_osdc_wrlocked(struct ceph_osd_client *osdc) { }
static inline void verify_osd_locked(struct ceph_osd *osd) { }
static inline void verify_lreq_locked(struct ceph_osd_linger_request *lreq) { }
#endif

>>>>>>> v4.9.227
/*
 * calculate the mapping of a file extent onto an object, and fill out the
 * request accordingly.  shorten extent as necessary if it crosses an
 * object boundary.
 *
 * fill osd op in request message.
 */
static int calc_layout(struct ceph_file_layout *layout, u64 off, u64 *plen,
			u64 *objnum, u64 *objoff, u64 *objlen)
{
	u64 orig_len = *plen;
	int r;

	/* object extent? */
	r = ceph_calc_file_object_mapping(layout, off, orig_len, objnum,
					  objoff, objlen);
	if (r < 0)
		return r;
	if (*objlen < orig_len) {
		*plen = *objlen;
		dout(" skipping last %llu, final file extent %llu~%llu\n",
		     orig_len - *plen, off, *plen);
	}

	dout("calc_layout objnum=%llx %llu~%llu\n", *objnum, *objoff, *objlen);

	return 0;
}

static void ceph_osd_data_init(struct ceph_osd_data *osd_data)
{
	memset(osd_data, 0, sizeof (*osd_data));
	osd_data->type = CEPH_OSD_DATA_TYPE_NONE;
}

static void ceph_osd_data_pages_init(struct ceph_osd_data *osd_data,
			struct page **pages, u64 length, u32 alignment,
			bool pages_from_pool, bool own_pages)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_PAGES;
	osd_data->pages = pages;
	osd_data->length = length;
	osd_data->alignment = alignment;
	osd_data->pages_from_pool = pages_from_pool;
	osd_data->own_pages = own_pages;
}

static void ceph_osd_data_pagelist_init(struct ceph_osd_data *osd_data,
			struct ceph_pagelist *pagelist)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_PAGELIST;
	osd_data->pagelist = pagelist;
}

#ifdef CONFIG_BLOCK
static void ceph_osd_data_bio_init(struct ceph_osd_data *osd_data,
			struct bio *bio, size_t bio_length)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_BIO;
	osd_data->bio = bio;
	osd_data->bio_length = bio_length;
}
#endif /* CONFIG_BLOCK */

<<<<<<< HEAD
#define osd_req_op_data(oreq, whch, typ, fld)	\
	({						\
		BUG_ON(whch >= (oreq)->r_num_ops);	\
		&(oreq)->r_ops[whch].typ.fld;		\
	})
=======
#define osd_req_op_data(oreq, whch, typ, fld)				\
({									\
	struct ceph_osd_request *__oreq = (oreq);			\
	unsigned int __whch = (whch);					\
	BUG_ON(__whch >= __oreq->r_num_ops);				\
	&__oreq->r_ops[__whch].typ.fld;					\
})
>>>>>>> v4.9.227

static struct ceph_osd_data *
osd_req_op_raw_data_in(struct ceph_osd_request *osd_req, unsigned int which)
{
	BUG_ON(which >= osd_req->r_num_ops);

	return &osd_req->r_ops[which].raw_data_in;
}

struct ceph_osd_data *
osd_req_op_extent_osd_data(struct ceph_osd_request *osd_req,
			unsigned int which)
{
	return osd_req_op_data(osd_req, which, extent, osd_data);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data);

<<<<<<< HEAD
struct ceph_osd_data *
osd_req_op_cls_response_data(struct ceph_osd_request *osd_req,
			unsigned int which)
{
	return osd_req_op_data(osd_req, which, cls, response_data);
}
EXPORT_SYMBOL(osd_req_op_cls_response_data);	/* ??? */

=======
>>>>>>> v4.9.227
void osd_req_op_raw_data_in_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages,
			u64 length, u32 alignment,
			bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_raw_data_in(osd_req, which);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
}
EXPORT_SYMBOL(osd_req_op_raw_data_in_pages);

void osd_req_op_extent_osd_data_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages,
			u64 length, u32 alignment,
			bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_pages);

void osd_req_op_extent_osd_data_pagelist(struct ceph_osd_request *osd_req,
			unsigned int which, struct ceph_pagelist *pagelist)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_pagelist_init(osd_data, pagelist);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_pagelist);

#ifdef CONFIG_BLOCK
void osd_req_op_extent_osd_data_bio(struct ceph_osd_request *osd_req,
			unsigned int which, struct bio *bio, size_t bio_length)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_bio_init(osd_data, bio, bio_length);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_bio);
#endif /* CONFIG_BLOCK */

static void osd_req_op_cls_request_info_pagelist(
			struct ceph_osd_request *osd_req,
			unsigned int which, struct ceph_pagelist *pagelist)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, request_info);
	ceph_osd_data_pagelist_init(osd_data, pagelist);
}

void osd_req_op_cls_request_data_pagelist(
			struct ceph_osd_request *osd_req,
			unsigned int which, struct ceph_pagelist *pagelist)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, request_data);
	ceph_osd_data_pagelist_init(osd_data, pagelist);
<<<<<<< HEAD
=======
	osd_req->r_ops[which].cls.indata_len += pagelist->length;
	osd_req->r_ops[which].indata_len += pagelist->length;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(osd_req_op_cls_request_data_pagelist);

void osd_req_op_cls_request_data_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages, u64 length,
			u32 alignment, bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, request_data);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
<<<<<<< HEAD
=======
	osd_req->r_ops[which].cls.indata_len += length;
	osd_req->r_ops[which].indata_len += length;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(osd_req_op_cls_request_data_pages);

void osd_req_op_cls_response_data_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages, u64 length,
			u32 alignment, bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, response_data);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
}
EXPORT_SYMBOL(osd_req_op_cls_response_data_pages);

static u64 ceph_osd_data_length(struct ceph_osd_data *osd_data)
{
	switch (osd_data->type) {
	case CEPH_OSD_DATA_TYPE_NONE:
		return 0;
	case CEPH_OSD_DATA_TYPE_PAGES:
		return osd_data->length;
	case CEPH_OSD_DATA_TYPE_PAGELIST:
		return (u64)osd_data->pagelist->length;
#ifdef CONFIG_BLOCK
	case CEPH_OSD_DATA_TYPE_BIO:
		return (u64)osd_data->bio_length;
#endif /* CONFIG_BLOCK */
	default:
		WARN(true, "unrecognized data type %d\n", (int)osd_data->type);
		return 0;
	}
}

static void ceph_osd_data_release(struct ceph_osd_data *osd_data)
{
	if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGES && osd_data->own_pages) {
		int num_pages;

		num_pages = calc_pages_for((u64)osd_data->alignment,
						(u64)osd_data->length);
		ceph_release_page_vector(osd_data->pages, num_pages);
	}
	ceph_osd_data_init(osd_data);
}

static void osd_req_op_data_release(struct ceph_osd_request *osd_req,
			unsigned int which)
{
	struct ceph_osd_req_op *op;

	BUG_ON(which >= osd_req->r_num_ops);
	op = &osd_req->r_ops[which];

	switch (op->op) {
	case CEPH_OSD_OP_READ:
	case CEPH_OSD_OP_WRITE:
<<<<<<< HEAD
=======
	case CEPH_OSD_OP_WRITEFULL:
>>>>>>> v4.9.227
		ceph_osd_data_release(&op->extent.osd_data);
		break;
	case CEPH_OSD_OP_CALL:
		ceph_osd_data_release(&op->cls.request_info);
		ceph_osd_data_release(&op->cls.request_data);
		ceph_osd_data_release(&op->cls.response_data);
		break;
<<<<<<< HEAD
=======
	case CEPH_OSD_OP_SETXATTR:
	case CEPH_OSD_OP_CMPXATTR:
		ceph_osd_data_release(&op->xattr.osd_data);
		break;
	case CEPH_OSD_OP_STAT:
		ceph_osd_data_release(&op->raw_data_in);
		break;
	case CEPH_OSD_OP_NOTIFY_ACK:
		ceph_osd_data_release(&op->notify_ack.request_data);
		break;
	case CEPH_OSD_OP_NOTIFY:
		ceph_osd_data_release(&op->notify.request_data);
		ceph_osd_data_release(&op->notify.response_data);
		break;
	case CEPH_OSD_OP_LIST_WATCHERS:
		ceph_osd_data_release(&op->list_watchers.response_data);
		break;
>>>>>>> v4.9.227
	default:
		break;
	}
}

/*
<<<<<<< HEAD
 * requests
 */
=======
 * Assumes @t is zero-initialized.
 */
static void target_init(struct ceph_osd_request_target *t)
{
	ceph_oid_init(&t->base_oid);
	ceph_oloc_init(&t->base_oloc);
	ceph_oid_init(&t->target_oid);
	ceph_oloc_init(&t->target_oloc);

	ceph_osds_init(&t->acting);
	ceph_osds_init(&t->up);
	t->size = -1;
	t->min_size = -1;

	t->osd = CEPH_HOMELESS_OSD;
}

static void target_copy(struct ceph_osd_request_target *dest,
			const struct ceph_osd_request_target *src)
{
	ceph_oid_copy(&dest->base_oid, &src->base_oid);
	ceph_oloc_copy(&dest->base_oloc, &src->base_oloc);
	ceph_oid_copy(&dest->target_oid, &src->target_oid);
	ceph_oloc_copy(&dest->target_oloc, &src->target_oloc);

	dest->pgid = src->pgid; /* struct */
	dest->pg_num = src->pg_num;
	dest->pg_num_mask = src->pg_num_mask;
	ceph_osds_copy(&dest->acting, &src->acting);
	ceph_osds_copy(&dest->up, &src->up);
	dest->size = src->size;
	dest->min_size = src->min_size;
	dest->sort_bitwise = src->sort_bitwise;

	dest->flags = src->flags;
	dest->paused = src->paused;

	dest->osd = src->osd;
}

static void target_destroy(struct ceph_osd_request_target *t)
{
	ceph_oid_destroy(&t->base_oid);
	ceph_oloc_destroy(&t->base_oloc);
	ceph_oid_destroy(&t->target_oid);
	ceph_oloc_destroy(&t->target_oloc);
}

/*
 * requests
 */
static void request_release_checks(struct ceph_osd_request *req)
{
	WARN_ON(!RB_EMPTY_NODE(&req->r_node));
	WARN_ON(!RB_EMPTY_NODE(&req->r_mc_node));
	WARN_ON(!list_empty(&req->r_unsafe_item));
	WARN_ON(req->r_osd);
}

>>>>>>> v4.9.227
static void ceph_osdc_release_request(struct kref *kref)
{
	struct ceph_osd_request *req = container_of(kref,
					    struct ceph_osd_request, r_kref);
	unsigned int which;

	dout("%s %p (r_request %p r_reply %p)\n", __func__, req,
	     req->r_request, req->r_reply);
<<<<<<< HEAD
	WARN_ON(!RB_EMPTY_NODE(&req->r_node));
	WARN_ON(!list_empty(&req->r_req_lru_item));
	WARN_ON(!list_empty(&req->r_osd_item));
	WARN_ON(!list_empty(&req->r_linger_item));
	WARN_ON(!list_empty(&req->r_linger_osd_item));
	WARN_ON(req->r_osd);

	if (req->r_request)
		ceph_msg_put(req->r_request);
	if (req->r_reply) {
		ceph_msg_revoke_incoming(req->r_reply);
		ceph_msg_put(req->r_reply);
	}
=======
	request_release_checks(req);

	if (req->r_request)
		ceph_msg_put(req->r_request);
	if (req->r_reply)
		ceph_msg_put(req->r_reply);
>>>>>>> v4.9.227

	for (which = 0; which < req->r_num_ops; which++)
		osd_req_op_data_release(req, which);

<<<<<<< HEAD
	ceph_put_snap_context(req->r_snapc);
	if (req->r_mempool)
		mempool_free(req, req->r_osdc->req_mempool);
	else
		kmem_cache_free(ceph_osd_request_cache, req);

=======
	target_destroy(&req->r_t);
	ceph_put_snap_context(req->r_snapc);

	if (req->r_mempool)
		mempool_free(req, req->r_osdc->req_mempool);
	else if (req->r_num_ops <= CEPH_OSD_SLAB_OPS)
		kmem_cache_free(ceph_osd_request_cache, req);
	else
		kfree(req);
>>>>>>> v4.9.227
}

void ceph_osdc_get_request(struct ceph_osd_request *req)
{
	dout("%s %p (was %d)\n", __func__, req,
	     atomic_read(&req->r_kref.refcount));
	kref_get(&req->r_kref);
}
EXPORT_SYMBOL(ceph_osdc_get_request);

void ceph_osdc_put_request(struct ceph_osd_request *req)
{
<<<<<<< HEAD
	dout("%s %p (was %d)\n", __func__, req,
	     atomic_read(&req->r_kref.refcount));
	kref_put(&req->r_kref, ceph_osdc_release_request);
}
EXPORT_SYMBOL(ceph_osdc_put_request);

=======
	if (req) {
		dout("%s %p (was %d)\n", __func__, req,
		     atomic_read(&req->r_kref.refcount));
		kref_put(&req->r_kref, ceph_osdc_release_request);
	}
}
EXPORT_SYMBOL(ceph_osdc_put_request);

static void request_init(struct ceph_osd_request *req)
{
	/* req only, each op is zeroed in _osd_req_op_init() */
	memset(req, 0, sizeof(*req));

	kref_init(&req->r_kref);
	init_completion(&req->r_completion);
	init_completion(&req->r_safe_completion);
	RB_CLEAR_NODE(&req->r_node);
	RB_CLEAR_NODE(&req->r_mc_node);
	INIT_LIST_HEAD(&req->r_unsafe_item);

	target_init(&req->r_t);
}

/*
 * This is ugly, but it allows us to reuse linger registration and ping
 * requests, keeping the structure of the code around send_linger{_ping}()
 * reasonable.  Setting up a min_nr=2 mempool for each linger request
 * and dealing with copying ops (this blasts req only, watch op remains
 * intact) isn't any better.
 */
static void request_reinit(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	bool mempool = req->r_mempool;
	unsigned int num_ops = req->r_num_ops;
	u64 snapid = req->r_snapid;
	struct ceph_snap_context *snapc = req->r_snapc;
	bool linger = req->r_linger;
	struct ceph_msg *request_msg = req->r_request;
	struct ceph_msg *reply_msg = req->r_reply;

	dout("%s req %p\n", __func__, req);
	WARN_ON(atomic_read(&req->r_kref.refcount) != 1);
	request_release_checks(req);

	WARN_ON(atomic_read(&request_msg->kref.refcount) != 1);
	WARN_ON(atomic_read(&reply_msg->kref.refcount) != 1);
	target_destroy(&req->r_t);

	request_init(req);
	req->r_osdc = osdc;
	req->r_mempool = mempool;
	req->r_num_ops = num_ops;
	req->r_snapid = snapid;
	req->r_snapc = snapc;
	req->r_linger = linger;
	req->r_request = request_msg;
	req->r_reply = reply_msg;
}

>>>>>>> v4.9.227
struct ceph_osd_request *ceph_osdc_alloc_request(struct ceph_osd_client *osdc,
					       struct ceph_snap_context *snapc,
					       unsigned int num_ops,
					       bool use_mempool,
					       gfp_t gfp_flags)
{
	struct ceph_osd_request *req;
<<<<<<< HEAD
	struct ceph_msg *msg;
	size_t msg_size;

	BUILD_BUG_ON(CEPH_OSD_MAX_OP > U16_MAX);
	BUG_ON(num_ops > CEPH_OSD_MAX_OP);

	msg_size = 4 + 4 + 8 + 8 + 4+8;
	msg_size += 2 + 4 + 8 + 4 + 4; /* oloc */
	msg_size += 1 + 8 + 4 + 4;     /* pg_t */
	msg_size += 4 + CEPH_MAX_OID_NAME_LEN; /* oid */
	msg_size += 2 + num_ops*sizeof(struct ceph_osd_op);
	msg_size += 8;  /* snapid */
	msg_size += 8;  /* snap_seq */
	msg_size += 8 * (snapc ? snapc->num_snaps : 0);  /* snaps */
	msg_size += 4;

	if (use_mempool) {
		req = mempool_alloc(osdc->req_mempool, gfp_flags);
		memset(req, 0, sizeof(*req));
	} else {
		req = kmem_cache_zalloc(ceph_osd_request_cache, gfp_flags);
	}
	if (req == NULL)
		return NULL;

	req->r_osdc = osdc;
	req->r_mempool = use_mempool;
	req->r_num_ops = num_ops;

	kref_init(&req->r_kref);
	init_completion(&req->r_completion);
	init_completion(&req->r_safe_completion);
	RB_CLEAR_NODE(&req->r_node);
	INIT_LIST_HEAD(&req->r_unsafe_item);
	INIT_LIST_HEAD(&req->r_linger_item);
	INIT_LIST_HEAD(&req->r_linger_osd_item);
	INIT_LIST_HEAD(&req->r_req_lru_item);
	INIT_LIST_HEAD(&req->r_osd_item);

	req->r_base_oloc.pool = -1;
	req->r_target_oloc.pool = -1;

	/* create reply message */
	if (use_mempool)
		msg = ceph_msgpool_get(&osdc->msgpool_op_reply, 0);
	else
		msg = ceph_msg_new(CEPH_MSG_OSD_OPREPLY,
				   OSD_OPREPLY_FRONT_LEN, gfp_flags, true);
	if (!msg) {
		ceph_osdc_put_request(req);
		return NULL;
	}
	req->r_reply = msg;

	/* create request message; allow space for oid */
	if (use_mempool)
		msg = ceph_msgpool_get(&osdc->msgpool_op, 0);
	else
		msg = ceph_msg_new(CEPH_MSG_OSD_OP, msg_size, gfp_flags, true);
	if (!msg) {
		ceph_osdc_put_request(req);
		return NULL;
	}

	memset(msg->front.iov_base, 0, msg->front.iov_len);

	req->r_request = msg;

=======

	if (use_mempool) {
		BUG_ON(num_ops > CEPH_OSD_SLAB_OPS);
		req = mempool_alloc(osdc->req_mempool, gfp_flags);
	} else if (num_ops <= CEPH_OSD_SLAB_OPS) {
		req = kmem_cache_alloc(ceph_osd_request_cache, gfp_flags);
	} else {
		BUG_ON(num_ops > CEPH_OSD_MAX_OPS);
		req = kmalloc(sizeof(*req) + num_ops * sizeof(req->r_ops[0]),
			      gfp_flags);
	}
	if (unlikely(!req))
		return NULL;

	request_init(req);
	req->r_osdc = osdc;
	req->r_mempool = use_mempool;
	req->r_num_ops = num_ops;
	req->r_snapid = CEPH_NOSNAP;
	req->r_snapc = ceph_get_snap_context(snapc);

	dout("%s req %p\n", __func__, req);
>>>>>>> v4.9.227
	return req;
}
EXPORT_SYMBOL(ceph_osdc_alloc_request);

<<<<<<< HEAD
=======
static int ceph_oloc_encoding_size(struct ceph_object_locator *oloc)
{
	return 8 + 4 + 4 + 4 + (oloc->pool_ns ? oloc->pool_ns->len : 0);
}

int ceph_osdc_alloc_messages(struct ceph_osd_request *req, gfp_t gfp)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_msg *msg;
	int msg_size;

	WARN_ON(ceph_oid_empty(&req->r_base_oid));
	WARN_ON(ceph_oloc_empty(&req->r_base_oloc));

	/* create request message */
	msg_size = 4 + 4 + 4; /* client_inc, osdmap_epoch, flags */
	msg_size += 4 + 4 + 4 + 8; /* mtime, reassert_version */
	msg_size += CEPH_ENCODING_START_BLK_LEN +
			ceph_oloc_encoding_size(&req->r_base_oloc); /* oloc */
	msg_size += 1 + 8 + 4 + 4; /* pgid */
	msg_size += 4 + req->r_base_oid.name_len; /* oid */
	msg_size += 2 + req->r_num_ops * sizeof(struct ceph_osd_op);
	msg_size += 8; /* snapid */
	msg_size += 8; /* snap_seq */
	msg_size += 4 + 8 * (req->r_snapc ? req->r_snapc->num_snaps : 0);
	msg_size += 4; /* retry_attempt */

	if (req->r_mempool)
		msg = ceph_msgpool_get(&osdc->msgpool_op, 0);
	else
		msg = ceph_msg_new(CEPH_MSG_OSD_OP, msg_size, gfp, true);
	if (!msg)
		return -ENOMEM;

	memset(msg->front.iov_base, 0, msg->front.iov_len);
	req->r_request = msg;

	/* create reply message */
	msg_size = OSD_OPREPLY_FRONT_LEN;
	msg_size += req->r_base_oid.name_len;
	msg_size += req->r_num_ops * sizeof(struct ceph_osd_op);

	if (req->r_mempool)
		msg = ceph_msgpool_get(&osdc->msgpool_op_reply, 0);
	else
		msg = ceph_msg_new(CEPH_MSG_OSD_OPREPLY, msg_size, gfp, true);
	if (!msg)
		return -ENOMEM;

	req->r_reply = msg;

	return 0;
}
EXPORT_SYMBOL(ceph_osdc_alloc_messages);

>>>>>>> v4.9.227
static bool osd_req_opcode_valid(u16 opcode)
{
	switch (opcode) {
#define GENERATE_CASE(op, opcode, str)	case CEPH_OSD_OP_##op: return true;
__CEPH_FORALL_OSD_OPS(GENERATE_CASE)
#undef GENERATE_CASE
	default:
		return false;
	}
}

/*
 * This is an osd op init function for opcodes that have no data or
 * other information associated with them.  It also serves as a
 * common init routine for all the other init functions, below.
 */
static struct ceph_osd_req_op *
_osd_req_op_init(struct ceph_osd_request *osd_req, unsigned int which,
<<<<<<< HEAD
				u16 opcode)
=======
		 u16 opcode, u32 flags)
>>>>>>> v4.9.227
{
	struct ceph_osd_req_op *op;

	BUG_ON(which >= osd_req->r_num_ops);
	BUG_ON(!osd_req_opcode_valid(opcode));

	op = &osd_req->r_ops[which];
	memset(op, 0, sizeof (*op));
	op->op = opcode;
<<<<<<< HEAD
=======
	op->flags = flags;
>>>>>>> v4.9.227

	return op;
}

void osd_req_op_init(struct ceph_osd_request *osd_req,
<<<<<<< HEAD
				unsigned int which, u16 opcode)
{
	(void)_osd_req_op_init(osd_req, which, opcode);
=======
		     unsigned int which, u16 opcode, u32 flags)
{
	(void)_osd_req_op_init(osd_req, which, opcode, flags);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(osd_req_op_init);

void osd_req_op_extent_init(struct ceph_osd_request *osd_req,
				unsigned int which, u16 opcode,
				u64 offset, u64 length,
				u64 truncate_size, u32 truncate_seq)
{
<<<<<<< HEAD
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which, opcode);
	size_t payload_len = 0;

	BUG_ON(opcode != CEPH_OSD_OP_READ && opcode != CEPH_OSD_OP_WRITE &&
	       opcode != CEPH_OSD_OP_DELETE && opcode != CEPH_OSD_OP_ZERO &&
=======
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which,
						      opcode, 0);
	size_t payload_len = 0;

	BUG_ON(opcode != CEPH_OSD_OP_READ && opcode != CEPH_OSD_OP_WRITE &&
	       opcode != CEPH_OSD_OP_WRITEFULL && opcode != CEPH_OSD_OP_ZERO &&
>>>>>>> v4.9.227
	       opcode != CEPH_OSD_OP_TRUNCATE);

	op->extent.offset = offset;
	op->extent.length = length;
	op->extent.truncate_size = truncate_size;
	op->extent.truncate_seq = truncate_seq;
<<<<<<< HEAD
	if (opcode == CEPH_OSD_OP_WRITE)
		payload_len += length;

	op->payload_len = payload_len;
=======
	if (opcode == CEPH_OSD_OP_WRITE || opcode == CEPH_OSD_OP_WRITEFULL)
		payload_len += length;

	op->indata_len = payload_len;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(osd_req_op_extent_init);

void osd_req_op_extent_update(struct ceph_osd_request *osd_req,
				unsigned int which, u64 length)
{
	struct ceph_osd_req_op *op;
	u64 previous;

	BUG_ON(which >= osd_req->r_num_ops);
	op = &osd_req->r_ops[which];
	previous = op->extent.length;

	if (length == previous)
		return;		/* Nothing to do */
	BUG_ON(length > previous);

	op->extent.length = length;
<<<<<<< HEAD
	op->payload_len -= previous - length;
}
EXPORT_SYMBOL(osd_req_op_extent_update);

void osd_req_op_cls_init(struct ceph_osd_request *osd_req, unsigned int which,
			u16 opcode, const char *class, const char *method)
{
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which, opcode);
=======
	if (op->op == CEPH_OSD_OP_WRITE || op->op == CEPH_OSD_OP_WRITEFULL)
		op->indata_len -= previous - length;
}
EXPORT_SYMBOL(osd_req_op_extent_update);

void osd_req_op_extent_dup_last(struct ceph_osd_request *osd_req,
				unsigned int which, u64 offset_inc)
{
	struct ceph_osd_req_op *op, *prev_op;

	BUG_ON(which + 1 >= osd_req->r_num_ops);

	prev_op = &osd_req->r_ops[which];
	op = _osd_req_op_init(osd_req, which + 1, prev_op->op, prev_op->flags);
	/* dup previous one */
	op->indata_len = prev_op->indata_len;
	op->outdata_len = prev_op->outdata_len;
	op->extent = prev_op->extent;
	/* adjust offset */
	op->extent.offset += offset_inc;
	op->extent.length -= offset_inc;

	if (op->op == CEPH_OSD_OP_WRITE || op->op == CEPH_OSD_OP_WRITEFULL)
		op->indata_len -= offset_inc;
}
EXPORT_SYMBOL(osd_req_op_extent_dup_last);

void osd_req_op_cls_init(struct ceph_osd_request *osd_req, unsigned int which,
			u16 opcode, const char *class, const char *method)
{
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which,
						      opcode, 0);
>>>>>>> v4.9.227
	struct ceph_pagelist *pagelist;
	size_t payload_len = 0;
	size_t size;

	BUG_ON(opcode != CEPH_OSD_OP_CALL);

	pagelist = kmalloc(sizeof (*pagelist), GFP_NOFS);
	BUG_ON(!pagelist);
	ceph_pagelist_init(pagelist);

	op->cls.class_name = class;
	size = strlen(class);
	BUG_ON(size > (size_t) U8_MAX);
	op->cls.class_len = size;
	ceph_pagelist_append(pagelist, class, size);
	payload_len += size;

	op->cls.method_name = method;
	size = strlen(method);
	BUG_ON(size > (size_t) U8_MAX);
	op->cls.method_len = size;
	ceph_pagelist_append(pagelist, method, size);
	payload_len += size;

	osd_req_op_cls_request_info_pagelist(osd_req, which, pagelist);

<<<<<<< HEAD
	op->cls.argc = 0;	/* currently unused */

	op->payload_len = payload_len;
}
EXPORT_SYMBOL(osd_req_op_cls_init);

void osd_req_op_watch_init(struct ceph_osd_request *osd_req,
				unsigned int which, u16 opcode,
				u64 cookie, u64 version, int flag)
{
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which, opcode);

	BUG_ON(opcode != CEPH_OSD_OP_NOTIFY_ACK && opcode != CEPH_OSD_OP_WATCH);

	op->watch.cookie = cookie;
	op->watch.ver = version;
	if (opcode == CEPH_OSD_OP_WATCH && flag)
		op->watch.flag = (u8)1;
}
EXPORT_SYMBOL(osd_req_op_watch_init);
=======
	op->indata_len = payload_len;
}
EXPORT_SYMBOL(osd_req_op_cls_init);

int osd_req_op_xattr_init(struct ceph_osd_request *osd_req, unsigned int which,
			  u16 opcode, const char *name, const void *value,
			  size_t size, u8 cmp_op, u8 cmp_mode)
{
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which,
						      opcode, 0);
	struct ceph_pagelist *pagelist;
	size_t payload_len;

	BUG_ON(opcode != CEPH_OSD_OP_SETXATTR && opcode != CEPH_OSD_OP_CMPXATTR);

	pagelist = kmalloc(sizeof(*pagelist), GFP_NOFS);
	if (!pagelist)
		return -ENOMEM;

	ceph_pagelist_init(pagelist);

	payload_len = strlen(name);
	op->xattr.name_len = payload_len;
	ceph_pagelist_append(pagelist, name, payload_len);

	op->xattr.value_len = size;
	ceph_pagelist_append(pagelist, value, size);
	payload_len += size;

	op->xattr.cmp_op = cmp_op;
	op->xattr.cmp_mode = cmp_mode;

	ceph_osd_data_pagelist_init(&op->xattr.osd_data, pagelist);
	op->indata_len = payload_len;
	return 0;
}
EXPORT_SYMBOL(osd_req_op_xattr_init);

/*
 * @watch_opcode: CEPH_OSD_WATCH_OP_*
 */
static void osd_req_op_watch_init(struct ceph_osd_request *req, int which,
				  u64 cookie, u8 watch_opcode)
{
	struct ceph_osd_req_op *op;

	op = _osd_req_op_init(req, which, CEPH_OSD_OP_WATCH, 0);
	op->watch.cookie = cookie;
	op->watch.op = watch_opcode;
	op->watch.gen = 0;
}
>>>>>>> v4.9.227

void osd_req_op_alloc_hint_init(struct ceph_osd_request *osd_req,
				unsigned int which,
				u64 expected_object_size,
				u64 expected_write_size)
{
	struct ceph_osd_req_op *op = _osd_req_op_init(osd_req, which,
<<<<<<< HEAD
						      CEPH_OSD_OP_SETALLOCHINT);
=======
						      CEPH_OSD_OP_SETALLOCHINT,
						      0);
>>>>>>> v4.9.227

	op->alloc_hint.expected_object_size = expected_object_size;
	op->alloc_hint.expected_write_size = expected_write_size;

	/*
	 * CEPH_OSD_OP_SETALLOCHINT op is advisory and therefore deemed
	 * not worth a feature bit.  Set FAILOK per-op flag to make
	 * sure older osds don't trip over an unsupported opcode.
	 */
	op->flags |= CEPH_OSD_OP_FLAG_FAILOK;
}
EXPORT_SYMBOL(osd_req_op_alloc_hint_init);

static void ceph_osdc_msg_data_add(struct ceph_msg *msg,
				struct ceph_osd_data *osd_data)
{
	u64 length = ceph_osd_data_length(osd_data);

	if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGES) {
		BUG_ON(length > (u64) SIZE_MAX);
		if (length)
			ceph_msg_data_add_pages(msg, osd_data->pages,
					length, osd_data->alignment);
	} else if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGELIST) {
		BUG_ON(!length);
		ceph_msg_data_add_pagelist(msg, osd_data->pagelist);
#ifdef CONFIG_BLOCK
	} else if (osd_data->type == CEPH_OSD_DATA_TYPE_BIO) {
		ceph_msg_data_add_bio(msg, osd_data->bio, length);
#endif
	} else {
		BUG_ON(osd_data->type != CEPH_OSD_DATA_TYPE_NONE);
	}
}

<<<<<<< HEAD
static u64 osd_req_encode_op(struct ceph_osd_request *req,
			      struct ceph_osd_op *dst, unsigned int which)
{
	struct ceph_osd_req_op *src;
	struct ceph_osd_data *osd_data;
	u64 request_data_len = 0;
	u64 data_length;

	BUG_ON(which >= req->r_num_ops);
	src = &req->r_ops[which];
=======
static u32 osd_req_encode_op(struct ceph_osd_op *dst,
			     const struct ceph_osd_req_op *src)
{
>>>>>>> v4.9.227
	if (WARN_ON(!osd_req_opcode_valid(src->op))) {
		pr_err("unrecognized osd opcode %d\n", src->op);

		return 0;
	}

	switch (src->op) {
	case CEPH_OSD_OP_STAT:
<<<<<<< HEAD
		osd_data = &src->raw_data_in;
		ceph_osdc_msg_data_add(req->r_reply, osd_data);
		break;
	case CEPH_OSD_OP_READ:
	case CEPH_OSD_OP_WRITE:
	case CEPH_OSD_OP_ZERO:
	case CEPH_OSD_OP_DELETE:
	case CEPH_OSD_OP_TRUNCATE:
		if (src->op == CEPH_OSD_OP_WRITE)
			request_data_len = src->extent.length;
=======
		break;
	case CEPH_OSD_OP_READ:
	case CEPH_OSD_OP_WRITE:
	case CEPH_OSD_OP_WRITEFULL:
	case CEPH_OSD_OP_ZERO:
	case CEPH_OSD_OP_TRUNCATE:
>>>>>>> v4.9.227
		dst->extent.offset = cpu_to_le64(src->extent.offset);
		dst->extent.length = cpu_to_le64(src->extent.length);
		dst->extent.truncate_size =
			cpu_to_le64(src->extent.truncate_size);
		dst->extent.truncate_seq =
			cpu_to_le32(src->extent.truncate_seq);
<<<<<<< HEAD
		osd_data = &src->extent.osd_data;
		if (src->op == CEPH_OSD_OP_WRITE)
			ceph_osdc_msg_data_add(req->r_request, osd_data);
		else
			ceph_osdc_msg_data_add(req->r_reply, osd_data);
=======
>>>>>>> v4.9.227
		break;
	case CEPH_OSD_OP_CALL:
		dst->cls.class_len = src->cls.class_len;
		dst->cls.method_len = src->cls.method_len;
<<<<<<< HEAD
		osd_data = &src->cls.request_info;
		ceph_osdc_msg_data_add(req->r_request, osd_data);
		BUG_ON(osd_data->type != CEPH_OSD_DATA_TYPE_PAGELIST);
		request_data_len = osd_data->pagelist->length;

		osd_data = &src->cls.request_data;
		data_length = ceph_osd_data_length(osd_data);
		if (data_length) {
			BUG_ON(osd_data->type == CEPH_OSD_DATA_TYPE_NONE);
			dst->cls.indata_len = cpu_to_le32(data_length);
			ceph_osdc_msg_data_add(req->r_request, osd_data);
			src->payload_len += data_length;
			request_data_len += data_length;
		}
		osd_data = &src->cls.response_data;
		ceph_osdc_msg_data_add(req->r_reply, osd_data);
		break;
	case CEPH_OSD_OP_STARTSYNC:
		break;
	case CEPH_OSD_OP_NOTIFY_ACK:
	case CEPH_OSD_OP_WATCH:
		dst->watch.cookie = cpu_to_le64(src->watch.cookie);
		dst->watch.ver = cpu_to_le64(src->watch.ver);
		dst->watch.flag = src->watch.flag;
=======
		dst->cls.indata_len = cpu_to_le32(src->cls.indata_len);
		break;
	case CEPH_OSD_OP_STARTSYNC:
		break;
	case CEPH_OSD_OP_WATCH:
		dst->watch.cookie = cpu_to_le64(src->watch.cookie);
		dst->watch.ver = cpu_to_le64(0);
		dst->watch.op = src->watch.op;
		dst->watch.gen = cpu_to_le32(src->watch.gen);
		break;
	case CEPH_OSD_OP_NOTIFY_ACK:
		break;
	case CEPH_OSD_OP_NOTIFY:
		dst->notify.cookie = cpu_to_le64(src->notify.cookie);
		break;
	case CEPH_OSD_OP_LIST_WATCHERS:
>>>>>>> v4.9.227
		break;
	case CEPH_OSD_OP_SETALLOCHINT:
		dst->alloc_hint.expected_object_size =
		    cpu_to_le64(src->alloc_hint.expected_object_size);
		dst->alloc_hint.expected_write_size =
		    cpu_to_le64(src->alloc_hint.expected_write_size);
		break;
<<<<<<< HEAD
=======
	case CEPH_OSD_OP_SETXATTR:
	case CEPH_OSD_OP_CMPXATTR:
		dst->xattr.name_len = cpu_to_le32(src->xattr.name_len);
		dst->xattr.value_len = cpu_to_le32(src->xattr.value_len);
		dst->xattr.cmp_op = src->xattr.cmp_op;
		dst->xattr.cmp_mode = src->xattr.cmp_mode;
		break;
	case CEPH_OSD_OP_CREATE:
	case CEPH_OSD_OP_DELETE:
		break;
>>>>>>> v4.9.227
	default:
		pr_err("unsupported osd opcode %s\n",
			ceph_osd_op_name(src->op));
		WARN_ON(1);

		return 0;
	}

	dst->op = cpu_to_le16(src->op);
	dst->flags = cpu_to_le32(src->flags);
<<<<<<< HEAD
	dst->payload_len = cpu_to_le32(src->payload_len);

	return request_data_len;
=======
	dst->payload_len = cpu_to_le32(src->indata_len);

	return src->indata_len;
>>>>>>> v4.9.227
}

/*
 * build new request AND message, calculate layout, and adjust file
 * extent as needed.
 *
 * if the file was recently truncated, we include information about its
 * old and new size so that the object can be updated appropriately.  (we
 * avoid synchronously deleting truncated objects because it's slow.)
 *
 * if @do_sync, include a 'startsync' command so that the osd will flush
 * data quickly.
 */
struct ceph_osd_request *ceph_osdc_new_request(struct ceph_osd_client *osdc,
					       struct ceph_file_layout *layout,
					       struct ceph_vino vino,
<<<<<<< HEAD
					       u64 off, u64 *plen, int num_ops,
=======
					       u64 off, u64 *plen,
					       unsigned int which, int num_ops,
>>>>>>> v4.9.227
					       int opcode, int flags,
					       struct ceph_snap_context *snapc,
					       u32 truncate_seq,
					       u64 truncate_size,
					       bool use_mempool)
{
	struct ceph_osd_request *req;
	u64 objnum = 0;
	u64 objoff = 0;
	u64 objlen = 0;
<<<<<<< HEAD
	u32 object_size;
	u64 object_base;
	int r;

	BUG_ON(opcode != CEPH_OSD_OP_READ && opcode != CEPH_OSD_OP_WRITE &&
	       opcode != CEPH_OSD_OP_DELETE && opcode != CEPH_OSD_OP_ZERO &&
	       opcode != CEPH_OSD_OP_TRUNCATE);

	req = ceph_osdc_alloc_request(osdc, snapc, num_ops, use_mempool,
					GFP_NOFS);
	if (!req)
		return ERR_PTR(-ENOMEM);

	req->r_flags = flags;

	/* calculate max write size */
	r = calc_layout(layout, off, plen, &objnum, &objoff, &objlen);
	if (r < 0) {
		ceph_osdc_put_request(req);
		return ERR_PTR(r);
	}

	object_size = le32_to_cpu(layout->fl_object_size);
	object_base = off - objoff;
	if (!(truncate_seq == 1 && truncate_size == -1ULL)) {
		if (truncate_size <= object_base) {
			truncate_size = 0;
		} else {
			truncate_size -= object_base;
			if (truncate_size > object_size)
				truncate_size = object_size;
		}
	}

	osd_req_op_extent_init(req, 0, opcode, objoff, objlen,
				truncate_size, truncate_seq);

	/*
	 * A second op in the ops array means the caller wants to
	 * also issue a include a 'startsync' command so that the
	 * osd will flush data quickly.
	 */
	if (num_ops > 1)
		osd_req_op_init(req, 1, CEPH_OSD_OP_STARTSYNC);

	req->r_base_oloc.pool = ceph_file_layout_pg_pool(*layout);

	snprintf(req->r_base_oid.name, sizeof(req->r_base_oid.name),
		 "%llx.%08llx", vino.ino, objnum);
	req->r_base_oid.name_len = strlen(req->r_base_oid.name);

	return req;
=======
	int r;

	BUG_ON(opcode != CEPH_OSD_OP_READ && opcode != CEPH_OSD_OP_WRITE &&
	       opcode != CEPH_OSD_OP_ZERO && opcode != CEPH_OSD_OP_TRUNCATE &&
	       opcode != CEPH_OSD_OP_CREATE && opcode != CEPH_OSD_OP_DELETE);

	req = ceph_osdc_alloc_request(osdc, snapc, num_ops, use_mempool,
					GFP_NOFS);
	if (!req) {
		r = -ENOMEM;
		goto fail;
	}

	/* calculate max write size */
	r = calc_layout(layout, off, plen, &objnum, &objoff, &objlen);
	if (r)
		goto fail;

	if (opcode == CEPH_OSD_OP_CREATE || opcode == CEPH_OSD_OP_DELETE) {
		osd_req_op_init(req, which, opcode, 0);
	} else {
		u32 object_size = layout->object_size;
		u32 object_base = off - objoff;
		if (!(truncate_seq == 1 && truncate_size == -1ULL)) {
			if (truncate_size <= object_base) {
				truncate_size = 0;
			} else {
				truncate_size -= object_base;
				if (truncate_size > object_size)
					truncate_size = object_size;
			}
		}
		osd_req_op_extent_init(req, which, opcode, objoff, objlen,
				       truncate_size, truncate_seq);
	}

	req->r_flags = flags;
	req->r_base_oloc.pool = layout->pool_id;
	req->r_base_oloc.pool_ns = ceph_try_get_string(layout->pool_ns);
	ceph_oid_printf(&req->r_base_oid, "%llx.%08llx", vino.ino, objnum);

	req->r_snapid = vino.snap;
	if (flags & CEPH_OSD_FLAG_WRITE)
		req->r_data_offset = off;

	r = ceph_osdc_alloc_messages(req, GFP_NOFS);
	if (r)
		goto fail;

	return req;

fail:
	ceph_osdc_put_request(req);
	return ERR_PTR(r);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(ceph_osdc_new_request);

/*
 * We keep osd requests in an rbtree, sorted by ->r_tid.
 */
<<<<<<< HEAD
static void __insert_request(struct ceph_osd_client *osdc,
			     struct ceph_osd_request *new)
{
	struct rb_node **p = &osdc->requests.rb_node;
	struct rb_node *parent = NULL;
	struct ceph_osd_request *req = NULL;

	while (*p) {
		parent = *p;
		req = rb_entry(parent, struct ceph_osd_request, r_node);
		if (new->r_tid < req->r_tid)
			p = &(*p)->rb_left;
		else if (new->r_tid > req->r_tid)
			p = &(*p)->rb_right;
		else
			BUG();
	}

	rb_link_node(&new->r_node, parent, p);
	rb_insert_color(&new->r_node, &osdc->requests);
}

static struct ceph_osd_request *__lookup_request(struct ceph_osd_client *osdc,
						 u64 tid)
{
	struct ceph_osd_request *req;
	struct rb_node *n = osdc->requests.rb_node;

	while (n) {
		req = rb_entry(n, struct ceph_osd_request, r_node);
		if (tid < req->r_tid)
			n = n->rb_left;
		else if (tid > req->r_tid)
			n = n->rb_right;
		else
			return req;
	}
	return NULL;
}

static struct ceph_osd_request *
__lookup_request_ge(struct ceph_osd_client *osdc,
		    u64 tid)
{
	struct ceph_osd_request *req;
	struct rb_node *n = osdc->requests.rb_node;

	while (n) {
		req = rb_entry(n, struct ceph_osd_request, r_node);
		if (tid < req->r_tid) {
			if (!n->rb_left)
				return req;
			n = n->rb_left;
		} else if (tid > req->r_tid) {
			n = n->rb_right;
		} else {
			return req;
		}
	}
	return NULL;
}

static void __kick_linger_request(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd *osd = req->r_osd;

	/*
	 * Linger requests need to be resent with a new tid to avoid
	 * the dup op detection logic on the OSDs.  Achieve this with
	 * a re-register dance instead of open-coding.
	 */
	ceph_osdc_get_request(req);
	if (!list_empty(&req->r_linger_item))
		__unregister_linger_request(osdc, req);
	else
		__unregister_request(osdc, req);
	__register_request(osdc, req);
	ceph_osdc_put_request(req);

	/*
	 * Unless request has been registered as both normal and
	 * lingering, __unregister{,_linger}_request clears r_osd.
	 * However, here we need to preserve r_osd to make sure we
	 * requeue on the same OSD.
	 */
	WARN_ON(req->r_osd || !osd);
	req->r_osd = osd;

	dout("%s requeueing %p tid %llu\n", __func__, req, req->r_tid);
	__enqueue_request(req);
}

/*
 * Resubmit requests pending on the given osd.
 */
static void __kick_osd_requests(struct ceph_osd_client *osdc,
				struct ceph_osd *osd)
{
	struct ceph_osd_request *req, *nreq;
	LIST_HEAD(resend);
	LIST_HEAD(resend_linger);
	int err;

	dout("%s osd%d\n", __func__, osd->o_osd);
	err = __reset_osd(osdc, osd);
	if (err)
		return;

	/*
	 * Build up a list of requests to resend by traversing the
	 * osd's list of requests.  Requests for a given object are
	 * sent in tid order, and that is also the order they're
	 * kept on this list.  Therefore all requests that are in
	 * flight will be found first, followed by all requests that
	 * have not yet been sent.  And to resend requests while
	 * preserving this order we will want to put any sent
	 * requests back on the front of the osd client's unsent
	 * list.
	 *
	 * So we build a separate ordered list of already-sent
	 * requests for the affected osd and splice it onto the
	 * front of the osd client's unsent list.  Once we've seen a
	 * request that has not yet been sent we're done.  Those
	 * requests are already sitting right where they belong.
	 */
	list_for_each_entry(req, &osd->o_requests, r_osd_item) {
		if (!req->r_sent)
			break;

		if (!req->r_linger) {
			dout("%s requeueing %p tid %llu\n", __func__, req,
			     req->r_tid);
			list_move_tail(&req->r_req_lru_item, &resend);
			req->r_flags |= CEPH_OSD_FLAG_RETRY;
		} else {
			list_move_tail(&req->r_req_lru_item, &resend_linger);
		}
	}
	list_splice(&resend, &osdc->req_unsent);

	/*
	 * Both registered and not yet registered linger requests are
	 * enqueued with a new tid on the same OSD.  We add/move them
	 * to req_unsent/o_requests at the end to keep things in tid
	 * order.
	 */
	list_for_each_entry_safe(req, nreq, &osd->o_linger_requests,
				 r_linger_osd_item) {
		WARN_ON(!list_empty(&req->r_req_lru_item));
		__kick_linger_request(req);
	}

	list_for_each_entry_safe(req, nreq, &resend_linger, r_req_lru_item)
		__kick_linger_request(req);
}

/*
 * If the osd connection drops, we need to resubmit all requests.
 */
static void osd_reset(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	struct ceph_osd_client *osdc;

	if (!osd)
		return;
	dout("osd_reset osd%d\n", osd->o_osd);
	osdc = osd->o_osdc;
	down_read(&osdc->map_sem);
	mutex_lock(&osdc->request_mutex);
	__kick_osd_requests(osdc, osd);
	__send_queued(osdc);
	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);
=======
DEFINE_RB_FUNCS(request, struct ceph_osd_request, r_tid, r_node)
DEFINE_RB_FUNCS(request_mc, struct ceph_osd_request, r_tid, r_mc_node)

static bool osd_homeless(struct ceph_osd *osd)
{
	return osd->o_osd == CEPH_HOMELESS_OSD;
}

static bool osd_registered(struct ceph_osd *osd)
{
	verify_osdc_locked(osd->o_osdc);

	return !RB_EMPTY_NODE(&osd->o_node);
}

/*
 * Assumes @osd is zero-initialized.
 */
static void osd_init(struct ceph_osd *osd)
{
	atomic_set(&osd->o_ref, 1);
	RB_CLEAR_NODE(&osd->o_node);
	osd->o_requests = RB_ROOT;
	osd->o_linger_requests = RB_ROOT;
	INIT_LIST_HEAD(&osd->o_osd_lru);
	INIT_LIST_HEAD(&osd->o_keepalive_item);
	osd->o_incarnation = 1;
	mutex_init(&osd->lock);
}

static void osd_cleanup(struct ceph_osd *osd)
{
	WARN_ON(!RB_EMPTY_NODE(&osd->o_node));
	WARN_ON(!RB_EMPTY_ROOT(&osd->o_requests));
	WARN_ON(!RB_EMPTY_ROOT(&osd->o_linger_requests));
	WARN_ON(!list_empty(&osd->o_osd_lru));
	WARN_ON(!list_empty(&osd->o_keepalive_item));

	if (osd->o_auth.authorizer) {
		WARN_ON(osd_homeless(osd));
		ceph_auth_destroy_authorizer(osd->o_auth.authorizer);
	}
>>>>>>> v4.9.227
}

/*
 * Track open sessions with osds.
 */
static struct ceph_osd *create_osd(struct ceph_osd_client *osdc, int onum)
{
	struct ceph_osd *osd;

<<<<<<< HEAD
	osd = kzalloc(sizeof(*osd), GFP_NOFS);
	if (!osd)
		return NULL;

	atomic_set(&osd->o_ref, 1);
	osd->o_osdc = osdc;
	osd->o_osd = onum;
	RB_CLEAR_NODE(&osd->o_node);
	INIT_LIST_HEAD(&osd->o_requests);
	INIT_LIST_HEAD(&osd->o_linger_requests);
	INIT_LIST_HEAD(&osd->o_osd_lru);
	osd->o_incarnation = 1;

	ceph_con_init(&osd->o_con, osd, &osd_con_ops, &osdc->client->msgr);

	INIT_LIST_HEAD(&osd->o_keepalive_item);
=======
	WARN_ON(onum == CEPH_HOMELESS_OSD);

	osd = kzalloc(sizeof(*osd), GFP_NOIO | __GFP_NOFAIL);
	osd_init(osd);
	osd->o_osdc = osdc;
	osd->o_osd = onum;

	ceph_con_init(&osd->o_con, osd, &osd_con_ops, &osdc->client->msgr);

>>>>>>> v4.9.227
	return osd;
}

static struct ceph_osd *get_osd(struct ceph_osd *osd)
{
	if (atomic_inc_not_zero(&osd->o_ref)) {
		dout("get_osd %p %d -> %d\n", osd, atomic_read(&osd->o_ref)-1,
		     atomic_read(&osd->o_ref));
		return osd;
	} else {
		dout("get_osd %p FAIL\n", osd);
		return NULL;
	}
}

static void put_osd(struct ceph_osd *osd)
{
	dout("put_osd %p %d -> %d\n", osd, atomic_read(&osd->o_ref),
	     atomic_read(&osd->o_ref) - 1);
<<<<<<< HEAD
	if (atomic_dec_and_test(&osd->o_ref) && osd->o_auth.authorizer) {
		struct ceph_auth_client *ac = osd->o_osdc->client->monc.auth;

		ceph_auth_destroy_authorizer(ac, osd->o_auth.authorizer);
=======
	if (atomic_dec_and_test(&osd->o_ref)) {
		osd_cleanup(osd);
>>>>>>> v4.9.227
		kfree(osd);
	}
}

<<<<<<< HEAD
/*
 * remove an osd from our map
 */
static void __remove_osd(struct ceph_osd_client *osdc, struct ceph_osd *osd)
{
	dout("%s %p osd%d\n", __func__, osd, osd->o_osd);
	WARN_ON(!list_empty(&osd->o_requests));
	WARN_ON(!list_empty(&osd->o_linger_requests));

	list_del_init(&osd->o_osd_lru);
	rb_erase(&osd->o_node, &osdc->osds);
	RB_CLEAR_NODE(&osd->o_node);
}

static void remove_osd(struct ceph_osd_client *osdc, struct ceph_osd *osd)
{
	dout("%s %p osd%d\n", __func__, osd, osd->o_osd);

	if (!RB_EMPTY_NODE(&osd->o_node)) {
		ceph_con_close(&osd->o_con);
		__remove_osd(osdc, osd);
		put_osd(osd);
	}
}

static void remove_all_osds(struct ceph_osd_client *osdc)
{
	dout("%s %p\n", __func__, osdc);
	mutex_lock(&osdc->request_mutex);
	while (!RB_EMPTY_ROOT(&osdc->osds)) {
		struct ceph_osd *osd = rb_entry(rb_first(&osdc->osds),
						struct ceph_osd, o_node);
		remove_osd(osdc, osd);
	}
	mutex_unlock(&osdc->request_mutex);
}

static void __move_osd_to_lru(struct ceph_osd_client *osdc,
			      struct ceph_osd *osd)
{
	dout("%s %p\n", __func__, osd);
	BUG_ON(!list_empty(&osd->o_osd_lru));

	list_add_tail(&osd->o_osd_lru, &osdc->osd_lru);
	osd->lru_ttl = jiffies + osdc->client->options->osd_idle_ttl * HZ;
}

static void maybe_move_osd_to_lru(struct ceph_osd_client *osdc,
				  struct ceph_osd *osd)
{
	dout("%s %p\n", __func__, osd);

	if (list_empty(&osd->o_requests) &&
	    list_empty(&osd->o_linger_requests))
		__move_osd_to_lru(osdc, osd);
=======
DEFINE_RB_FUNCS(osd, struct ceph_osd, o_osd, o_node)

static void __move_osd_to_lru(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);
	BUG_ON(!list_empty(&osd->o_osd_lru));

	spin_lock(&osdc->osd_lru_lock);
	list_add_tail(&osd->o_osd_lru, &osdc->osd_lru);
	spin_unlock(&osdc->osd_lru_lock);

	osd->lru_ttl = jiffies + osdc->client->options->osd_idle_ttl;
}

static void maybe_move_osd_to_lru(struct ceph_osd *osd)
{
	if (RB_EMPTY_ROOT(&osd->o_requests) &&
	    RB_EMPTY_ROOT(&osd->o_linger_requests))
		__move_osd_to_lru(osd);
>>>>>>> v4.9.227
}

static void __remove_osd_from_lru(struct ceph_osd *osd)
{
<<<<<<< HEAD
	dout("__remove_osd_from_lru %p\n", osd);
	if (!list_empty(&osd->o_osd_lru))
		list_del_init(&osd->o_osd_lru);
}

static void remove_old_osds(struct ceph_osd_client *osdc)
{
	struct ceph_osd *osd, *nosd;

	dout("__remove_old_osds %p\n", osdc);
	mutex_lock(&osdc->request_mutex);
	list_for_each_entry_safe(osd, nosd, &osdc->osd_lru, o_osd_lru) {
		if (time_before(jiffies, osd->lru_ttl))
			break;
		remove_osd(osdc, osd);
	}
	mutex_unlock(&osdc->request_mutex);
=======
	struct ceph_osd_client *osdc = osd->o_osdc;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	spin_lock(&osdc->osd_lru_lock);
	if (!list_empty(&osd->o_osd_lru))
		list_del_init(&osd->o_osd_lru);
	spin_unlock(&osdc->osd_lru_lock);
}

/*
 * Close the connection and assign any leftover requests to the
 * homeless session.
 */
static void close_osd(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct rb_node *n;

	verify_osdc_wrlocked(osdc);
	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	ceph_con_close(&osd->o_con);

	for (n = rb_first(&osd->o_requests); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);

		n = rb_next(n); /* unlink_request() */

		dout(" reassigning req %p tid %llu\n", req, req->r_tid);
		unlink_request(osd, req);
		link_request(&osdc->homeless_osd, req);
	}
	for (n = rb_first(&osd->o_linger_requests); n; ) {
		struct ceph_osd_linger_request *lreq =
		    rb_entry(n, struct ceph_osd_linger_request, node);

		n = rb_next(n); /* unlink_linger() */

		dout(" reassigning lreq %p linger_id %llu\n", lreq,
		     lreq->linger_id);
		unlink_linger(osd, lreq);
		link_linger(&osdc->homeless_osd, lreq);
	}

	__remove_osd_from_lru(osd);
	erase_osd(&osdc->osds, osd);
	put_osd(osd);
>>>>>>> v4.9.227
}

/*
 * reset osd connect
 */
<<<<<<< HEAD
static int __reset_osd(struct ceph_osd_client *osdc, struct ceph_osd *osd)
{
	struct ceph_entity_addr *peer_addr;

	dout("__reset_osd %p osd%d\n", osd, osd->o_osd);
	if (list_empty(&osd->o_requests) &&
	    list_empty(&osd->o_linger_requests)) {
		remove_osd(osdc, osd);
		return -ENODEV;
	}

	peer_addr = &osdc->osdmap->osd_addr[osd->o_osd];
	if (!memcmp(peer_addr, &osd->o_con.peer_addr, sizeof (*peer_addr)) &&
			!ceph_con_opened(&osd->o_con)) {
		struct ceph_osd_request *req;
=======
static int reopen_osd(struct ceph_osd *osd)
{
	struct ceph_entity_addr *peer_addr;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	if (RB_EMPTY_ROOT(&osd->o_requests) &&
	    RB_EMPTY_ROOT(&osd->o_linger_requests)) {
		close_osd(osd);
		return -ENODEV;
	}

	peer_addr = &osd->o_osdc->osdmap->osd_addr[osd->o_osd];
	if (!memcmp(peer_addr, &osd->o_con.peer_addr, sizeof (*peer_addr)) &&
			!ceph_con_opened(&osd->o_con)) {
		struct rb_node *n;
>>>>>>> v4.9.227

		dout("osd addr hasn't changed and connection never opened, "
		     "letting msgr retry\n");
		/* touch each r_stamp for handle_timeout()'s benfit */
<<<<<<< HEAD
		list_for_each_entry(req, &osd->o_requests, r_osd_item)
			req->r_stamp = jiffies;
=======
		for (n = rb_first(&osd->o_requests); n; n = rb_next(n)) {
			struct ceph_osd_request *req =
			    rb_entry(n, struct ceph_osd_request, r_node);
			req->r_stamp = jiffies;
		}
>>>>>>> v4.9.227

		return -EAGAIN;
	}

	ceph_con_close(&osd->o_con);
	ceph_con_open(&osd->o_con, CEPH_ENTITY_TYPE_OSD, osd->o_osd, peer_addr);
	osd->o_incarnation++;

	return 0;
}

<<<<<<< HEAD
static void __insert_osd(struct ceph_osd_client *osdc, struct ceph_osd *new)
{
	struct rb_node **p = &osdc->osds.rb_node;
	struct rb_node *parent = NULL;
	struct ceph_osd *osd = NULL;

	dout("__insert_osd %p osd%d\n", new, new->o_osd);
	while (*p) {
		parent = *p;
		osd = rb_entry(parent, struct ceph_osd, o_node);
		if (new->o_osd < osd->o_osd)
			p = &(*p)->rb_left;
		else if (new->o_osd > osd->o_osd)
			p = &(*p)->rb_right;
		else
			BUG();
	}

	rb_link_node(&new->o_node, parent, p);
	rb_insert_color(&new->o_node, &osdc->osds);
}

static struct ceph_osd *__lookup_osd(struct ceph_osd_client *osdc, int o)
{
	struct ceph_osd *osd;
	struct rb_node *n = osdc->osds.rb_node;

	while (n) {
		osd = rb_entry(n, struct ceph_osd, o_node);
		if (o < osd->o_osd)
			n = n->rb_left;
		else if (o > osd->o_osd)
			n = n->rb_right;
		else
			return osd;
	}
	return NULL;
}

static void __schedule_osd_timeout(struct ceph_osd_client *osdc)
{
	schedule_delayed_work(&osdc->timeout_work,
			osdc->client->options->osd_keepalive_timeout * HZ);
}

static void __cancel_osd_timeout(struct ceph_osd_client *osdc)
{
	cancel_delayed_work(&osdc->timeout_work);
}

/*
 * Register request, assign tid.  If this is the first request, set up
 * the timeout event.
 */
static void __register_request(struct ceph_osd_client *osdc,
			       struct ceph_osd_request *req)
{
	req->r_tid = ++osdc->last_tid;
	req->r_request->hdr.tid = cpu_to_le64(req->r_tid);
	dout("__register_request %p tid %lld\n", req, req->r_tid);
	__insert_request(osdc, req);
	ceph_osdc_get_request(req);
	osdc->num_requests++;
	if (osdc->num_requests == 1) {
		dout(" first request, scheduling timeout\n");
		__schedule_osd_timeout(osdc);
	}
}

/*
 * called under osdc->request_mutex
 */
static void __unregister_request(struct ceph_osd_client *osdc,
				 struct ceph_osd_request *req)
{
	if (RB_EMPTY_NODE(&req->r_node)) {
		dout("__unregister_request %p tid %lld not registered\n",
			req, req->r_tid);
		return;
	}

	dout("__unregister_request %p tid %lld\n", req, req->r_tid);
	rb_erase(&req->r_node, &osdc->requests);
	RB_CLEAR_NODE(&req->r_node);
	osdc->num_requests--;

	if (req->r_osd) {
		/* make sure the original request isn't in flight. */
		ceph_msg_revoke(req->r_request);

		list_del_init(&req->r_osd_item);
		maybe_move_osd_to_lru(osdc, req->r_osd);
		if (list_empty(&req->r_linger_osd_item))
			req->r_osd = NULL;
	}

	list_del_init(&req->r_req_lru_item);
	ceph_osdc_put_request(req);

	if (osdc->num_requests == 0) {
		dout(" no requests, canceling timeout\n");
		__cancel_osd_timeout(osdc);
	}
}

/*
 * Cancel a previously queued request message
 */
static void __cancel_request(struct ceph_osd_request *req)
{
	if (req->r_sent && req->r_osd) {
		ceph_msg_revoke(req->r_request);
		req->r_sent = 0;
	}
}

static void __register_linger_request(struct ceph_osd_client *osdc,
				    struct ceph_osd_request *req)
{
	dout("%s %p tid %llu\n", __func__, req, req->r_tid);
	WARN_ON(!req->r_linger);

	ceph_osdc_get_request(req);
	list_add_tail(&req->r_linger_item, &osdc->req_linger);
	if (req->r_osd)
		list_add_tail(&req->r_linger_osd_item,
			      &req->r_osd->o_linger_requests);
}

static void __unregister_linger_request(struct ceph_osd_client *osdc,
					struct ceph_osd_request *req)
{
	WARN_ON(!req->r_linger);

	if (list_empty(&req->r_linger_item)) {
		dout("%s %p tid %llu not registered\n", __func__, req,
		     req->r_tid);
		return;
	}

	dout("%s %p tid %llu\n", __func__, req, req->r_tid);
	list_del_init(&req->r_linger_item);

	if (req->r_osd) {
		list_del_init(&req->r_linger_osd_item);
		maybe_move_osd_to_lru(osdc, req->r_osd);
		if (list_empty(&req->r_osd_item))
			req->r_osd = NULL;
	}
	ceph_osdc_put_request(req);
}

void ceph_osdc_set_request_linger(struct ceph_osd_client *osdc,
				  struct ceph_osd_request *req)
{
	if (!req->r_linger) {
		dout("set_request_linger %p\n", req);
		req->r_linger = 1;
	}
}
EXPORT_SYMBOL(ceph_osdc_set_request_linger);
=======
static struct ceph_osd *lookup_create_osd(struct ceph_osd_client *osdc, int o,
					  bool wrlocked)
{
	struct ceph_osd *osd;

	if (wrlocked)
		verify_osdc_wrlocked(osdc);
	else
		verify_osdc_locked(osdc);

	if (o != CEPH_HOMELESS_OSD)
		osd = lookup_osd(&osdc->osds, o);
	else
		osd = &osdc->homeless_osd;
	if (!osd) {
		if (!wrlocked)
			return ERR_PTR(-EAGAIN);

		osd = create_osd(osdc, o);
		insert_osd(&osdc->osds, osd);
		ceph_con_open(&osd->o_con, CEPH_ENTITY_TYPE_OSD, osd->o_osd,
			      &osdc->osdmap->osd_addr[osd->o_osd]);
	}

	dout("%s osdc %p osd%d -> osd %p\n", __func__, osdc, o, osd);
	return osd;
}

/*
 * Create request <-> OSD session relation.
 *
 * @req has to be assigned a tid, @osd may be homeless.
 */
static void link_request(struct ceph_osd *osd, struct ceph_osd_request *req)
{
	verify_osd_locked(osd);
	WARN_ON(!req->r_tid || req->r_osd);
	dout("%s osd %p osd%d req %p tid %llu\n", __func__, osd, osd->o_osd,
	     req, req->r_tid);

	if (!osd_homeless(osd))
		__remove_osd_from_lru(osd);
	else
		atomic_inc(&osd->o_osdc->num_homeless);

	get_osd(osd);
	insert_request(&osd->o_requests, req);
	req->r_osd = osd;
}

static void unlink_request(struct ceph_osd *osd, struct ceph_osd_request *req)
{
	verify_osd_locked(osd);
	WARN_ON(req->r_osd != osd);
	dout("%s osd %p osd%d req %p tid %llu\n", __func__, osd, osd->o_osd,
	     req, req->r_tid);

	req->r_osd = NULL;
	erase_request(&osd->o_requests, req);
	put_osd(osd);

	if (!osd_homeless(osd))
		maybe_move_osd_to_lru(osd);
	else
		atomic_dec(&osd->o_osdc->num_homeless);
}

static bool __pool_full(struct ceph_pg_pool_info *pi)
{
	return pi->flags & CEPH_POOL_FLAG_FULL;
}

static bool have_pool_full(struct ceph_osd_client *osdc)
{
	struct rb_node *n;

	for (n = rb_first(&osdc->osdmap->pg_pools); n; n = rb_next(n)) {
		struct ceph_pg_pool_info *pi =
		    rb_entry(n, struct ceph_pg_pool_info, node);

		if (__pool_full(pi))
			return true;
	}

	return false;
}

static bool pool_full(struct ceph_osd_client *osdc, s64 pool_id)
{
	struct ceph_pg_pool_info *pi;

	pi = ceph_pg_pool_by_id(osdc->osdmap, pool_id);
	if (!pi)
		return false;

	return __pool_full(pi);
}
>>>>>>> v4.9.227

/*
 * Returns whether a request should be blocked from being sent
 * based on the current osdmap and osd_client settings.
<<<<<<< HEAD
 *
 * Caller should hold map_sem for read.
 */
static bool __req_should_be_paused(struct ceph_osd_client *osdc,
				   struct ceph_osd_request *req)
{
	bool pauserd = ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_PAUSERD);
	bool pausewr = ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_PAUSEWR) ||
		ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_FULL);
	return (req->r_flags & CEPH_OSD_FLAG_READ && pauserd) ||
		(req->r_flags & CEPH_OSD_FLAG_WRITE && pausewr);
}

/*
 * Calculate mapping of a request to a PG.  Takes tiering into account.
 */
static int __calc_request_pg(struct ceph_osdmap *osdmap,
			     struct ceph_osd_request *req,
			     struct ceph_pg *pg_out)
{
	bool need_check_tiering;

	need_check_tiering = false;
	if (req->r_target_oloc.pool == -1) {
		req->r_target_oloc = req->r_base_oloc; /* struct */
		need_check_tiering = true;
	}
	if (req->r_target_oid.name_len == 0) {
		ceph_oid_copy(&req->r_target_oid, &req->r_base_oid);
=======
 */
static bool target_should_be_paused(struct ceph_osd_client *osdc,
				    const struct ceph_osd_request_target *t,
				    struct ceph_pg_pool_info *pi)
{
	bool pauserd = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD);
	bool pausewr = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR) ||
		       ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		       __pool_full(pi);

	WARN_ON(pi->id != t->base_oloc.pool);
	return (t->flags & CEPH_OSD_FLAG_READ && pauserd) ||
	       (t->flags & CEPH_OSD_FLAG_WRITE && pausewr);
}

enum calc_target_result {
	CALC_TARGET_NO_ACTION = 0,
	CALC_TARGET_NEED_RESEND,
	CALC_TARGET_POOL_DNE,
};

static enum calc_target_result calc_target(struct ceph_osd_client *osdc,
					   struct ceph_osd_request_target *t,
					   u32 *last_force_resend,
					   bool any_change)
{
	struct ceph_pg_pool_info *pi;
	struct ceph_pg pgid, last_pgid;
	struct ceph_osds up, acting;
	bool force_resend = false;
	bool need_check_tiering = false;
	bool need_resend = false;
	bool sort_bitwise = ceph_osdmap_flag(osdc, CEPH_OSDMAP_SORTBITWISE);
	enum calc_target_result ct_res;
	int ret;

	pi = ceph_pg_pool_by_id(osdc->osdmap, t->base_oloc.pool);
	if (!pi) {
		t->osd = CEPH_HOMELESS_OSD;
		ct_res = CALC_TARGET_POOL_DNE;
		goto out;
	}

	if (osdc->osdmap->epoch == pi->last_force_request_resend) {
		if (last_force_resend &&
		    *last_force_resend < pi->last_force_request_resend) {
			*last_force_resend = pi->last_force_request_resend;
			force_resend = true;
		} else if (!last_force_resend) {
			force_resend = true;
		}
	}
	if (ceph_oid_empty(&t->target_oid) || force_resend) {
		ceph_oid_copy(&t->target_oid, &t->base_oid);
		need_check_tiering = true;
	}
	if (ceph_oloc_empty(&t->target_oloc) || force_resend) {
		ceph_oloc_copy(&t->target_oloc, &t->base_oloc);
>>>>>>> v4.9.227
		need_check_tiering = true;
	}

	if (need_check_tiering &&
<<<<<<< HEAD
	    (req->r_flags & CEPH_OSD_FLAG_IGNORE_OVERLAY) == 0) {
		struct ceph_pg_pool_info *pi;

		pi = ceph_pg_pool_by_id(osdmap, req->r_target_oloc.pool);
		if (pi) {
			if ((req->r_flags & CEPH_OSD_FLAG_READ) &&
			    pi->read_tier >= 0)
				req->r_target_oloc.pool = pi->read_tier;
			if ((req->r_flags & CEPH_OSD_FLAG_WRITE) &&
			    pi->write_tier >= 0)
				req->r_target_oloc.pool = pi->write_tier;
		}
		/* !pi is caught in ceph_oloc_oid_to_pg() */
	}

	return ceph_oloc_oid_to_pg(osdmap, &req->r_target_oloc,
				   &req->r_target_oid, pg_out);
}

static void __enqueue_request(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;

	dout("%s %p tid %llu to osd%d\n", __func__, req, req->r_tid,
	     req->r_osd ? req->r_osd->o_osd : -1);

	if (req->r_osd) {
		__remove_osd_from_lru(req->r_osd);
		list_add_tail(&req->r_osd_item, &req->r_osd->o_requests);
		list_move_tail(&req->r_req_lru_item, &osdc->req_unsent);
	} else {
		list_move_tail(&req->r_req_lru_item, &osdc->req_notarget);
	}
}

/*
 * Pick an osd (the first 'up' osd in the pg), allocate the osd struct
 * (as needed), and set the request r_osd appropriately.  If there is
 * no up osd, set r_osd to NULL.  Move the request to the appropriate list
 * (unsent, homeless) or leave on in-flight lru.
 *
 * Return 0 if unchanged, 1 if changed, or negative on error.
 *
 * Caller should hold map_sem for read and request_mutex.
 */
static int __map_request(struct ceph_osd_client *osdc,
			 struct ceph_osd_request *req, int force_resend)
{
	struct ceph_pg pgid;
	int acting[CEPH_PG_MAX_SIZE];
	int num, o;
	int err;
	bool was_paused;

	dout("map_request %p tid %lld\n", req, req->r_tid);

	err = __calc_request_pg(osdc->osdmap, req, &pgid);
	if (err) {
		list_move(&req->r_req_lru_item, &osdc->req_notarget);
		return err;
	}
	req->r_pgid = pgid;

	num = ceph_calc_pg_acting(osdc->osdmap, pgid, acting, &o);
	if (num < 0)
		num = 0;

	was_paused = req->r_paused;
	req->r_paused = __req_should_be_paused(osdc, req);
	if (was_paused && !req->r_paused)
		force_resend = 1;

	if ((!force_resend &&
	     req->r_osd && req->r_osd->o_osd == o &&
	     req->r_sent >= req->r_osd->o_incarnation &&
	     req->r_num_pg_osds == num &&
	     memcmp(req->r_pg_osds, acting, sizeof(acting[0])*num) == 0) ||
	    (req->r_osd == NULL && o == -1) ||
	    req->r_paused)
		return 0;  /* no change */

	dout("map_request tid %llu pgid %lld.%x osd%d (was osd%d)\n",
	     req->r_tid, pgid.pool, pgid.seed, o,
	     req->r_osd ? req->r_osd->o_osd : -1);

	/* record full pg acting set */
	memcpy(req->r_pg_osds, acting, sizeof(acting[0]) * num);
	req->r_num_pg_osds = num;

	if (req->r_osd) {
		__cancel_request(req);
		list_del_init(&req->r_osd_item);
		list_del_init(&req->r_linger_osd_item);
		req->r_osd = NULL;
	}

	req->r_osd = __lookup_osd(osdc, o);
	if (!req->r_osd && o >= 0) {
		err = -ENOMEM;
		req->r_osd = create_osd(osdc, o);
		if (!req->r_osd) {
			list_move(&req->r_req_lru_item, &osdc->req_notarget);
			goto out;
		}

		dout("map_request osd %p is osd%d\n", req->r_osd, o);
		__insert_osd(osdc, req->r_osd);

		ceph_con_open(&req->r_osd->o_con,
			      CEPH_ENTITY_TYPE_OSD, o,
			      &osdc->osdmap->osd_addr[o]);
	}

	__enqueue_request(req);
	err = 1;   /* osd or pg changed */

out:
	return err;
}

/*
 * caller should hold map_sem (for read) and request_mutex
 */
static void __send_request(struct ceph_osd_client *osdc,
			   struct ceph_osd_request *req)
{
	void *p;

	dout("send_request %p tid %llu to osd%d flags %d pg %lld.%x\n",
	     req, req->r_tid, req->r_osd->o_osd, req->r_flags,
	     (unsigned long long)req->r_pgid.pool, req->r_pgid.seed);

	/* fill in message content that changes each time we send it */
	put_unaligned_le32(osdc->osdmap->epoch, req->r_request_osdmap_epoch);
	put_unaligned_le32(req->r_flags, req->r_request_flags);
	put_unaligned_le64(req->r_target_oloc.pool, req->r_request_pool);
	p = req->r_request_pgid;
	ceph_encode_64(&p, req->r_pgid.pool);
	ceph_encode_32(&p, req->r_pgid.seed);
	put_unaligned_le64(1, req->r_request_attempts);  /* FIXME */
	memcpy(req->r_request_reassert_version, &req->r_reassert_version,
	       sizeof(req->r_reassert_version));

	req->r_stamp = jiffies;
	list_move_tail(&req->r_req_lru_item, &osdc->req_lru);

	ceph_msg_get(req->r_request); /* send consumes a ref */

	req->r_sent = req->r_osd->o_incarnation;

	ceph_con_send(&req->r_osd->o_con, req->r_request);
}

/*
 * Send any requests in the queue (req_unsent).
 */
static void __send_queued(struct ceph_osd_client *osdc)
{
	struct ceph_osd_request *req, *tmp;

	dout("__send_queued\n");
	list_for_each_entry_safe(req, tmp, &osdc->req_unsent, r_req_lru_item)
		__send_request(osdc, req);
}

/*
 * Caller should hold map_sem for read and request_mutex.
 */
static int __ceph_osdc_start_request(struct ceph_osd_client *osdc,
				     struct ceph_osd_request *req,
				     bool nofail)
{
	int rc;

	__register_request(osdc, req);
	req->r_sent = 0;
	req->r_got_reply = 0;
	rc = __map_request(osdc, req, 0);
	if (rc < 0) {
		if (nofail) {
			dout("osdc_start_request failed map, "
				" will retry %lld\n", req->r_tid);
			rc = 0;
		} else {
			__unregister_request(osdc, req);
		}
		return rc;
	}

	if (req->r_osd == NULL) {
		dout("send_request %p no up osds in pg\n", req);
		ceph_monc_request_next_osdmap(&osdc->client->monc);
	} else {
		__send_queued(osdc);
	}

	return 0;
}

/*
 * Timeout callback, called every N seconds when 1 or more osd
 * requests has been active for more than N seconds.  When this
 * happens, we ping all OSDs with requests who have timed out to
 * ensure any communications channel reset is detected.  Reset the
 * request timeouts another N seconds in the future as we go.
 * Reschedule the timeout event another N seconds in future (unless
 * there are no open requests).
=======
	    (t->flags & CEPH_OSD_FLAG_IGNORE_OVERLAY) == 0) {
		if (t->flags & CEPH_OSD_FLAG_READ && pi->read_tier >= 0)
			t->target_oloc.pool = pi->read_tier;
		if (t->flags & CEPH_OSD_FLAG_WRITE && pi->write_tier >= 0)
			t->target_oloc.pool = pi->write_tier;
	}

	ret = ceph_object_locator_to_pg(osdc->osdmap, &t->target_oid,
					&t->target_oloc, &pgid);
	if (ret) {
		WARN_ON(ret != -ENOENT);
		t->osd = CEPH_HOMELESS_OSD;
		ct_res = CALC_TARGET_POOL_DNE;
		goto out;
	}
	last_pgid.pool = pgid.pool;
	last_pgid.seed = ceph_stable_mod(pgid.seed, t->pg_num, t->pg_num_mask);

	ceph_pg_to_up_acting_osds(osdc->osdmap, &pgid, &up, &acting);
	if (any_change &&
	    ceph_is_new_interval(&t->acting,
				 &acting,
				 &t->up,
				 &up,
				 t->size,
				 pi->size,
				 t->min_size,
				 pi->min_size,
				 t->pg_num,
				 pi->pg_num,
				 t->sort_bitwise,
				 sort_bitwise,
				 &last_pgid))
		force_resend = true;

	if (t->paused && !target_should_be_paused(osdc, t, pi)) {
		t->paused = false;
		need_resend = true;
	}

	if (ceph_pg_compare(&t->pgid, &pgid) ||
	    ceph_osds_changed(&t->acting, &acting, any_change) ||
	    force_resend) {
		t->pgid = pgid; /* struct */
		ceph_osds_copy(&t->acting, &acting);
		ceph_osds_copy(&t->up, &up);
		t->size = pi->size;
		t->min_size = pi->min_size;
		t->pg_num = pi->pg_num;
		t->pg_num_mask = pi->pg_num_mask;
		t->sort_bitwise = sort_bitwise;

		t->osd = acting.primary;
		need_resend = true;
	}

	ct_res = need_resend ? CALC_TARGET_NEED_RESEND : CALC_TARGET_NO_ACTION;
out:
	dout("%s t %p -> ct_res %d osd %d\n", __func__, t, ct_res, t->osd);
	return ct_res;
}

static void setup_request_data(struct ceph_osd_request *req,
			       struct ceph_msg *msg)
{
	u32 data_len = 0;
	int i;

	if (!list_empty(&msg->data))
		return;

	WARN_ON(msg->data_length);
	for (i = 0; i < req->r_num_ops; i++) {
		struct ceph_osd_req_op *op = &req->r_ops[i];

		switch (op->op) {
		/* request */
		case CEPH_OSD_OP_WRITE:
		case CEPH_OSD_OP_WRITEFULL:
			WARN_ON(op->indata_len != op->extent.length);
			ceph_osdc_msg_data_add(msg, &op->extent.osd_data);
			break;
		case CEPH_OSD_OP_SETXATTR:
		case CEPH_OSD_OP_CMPXATTR:
			WARN_ON(op->indata_len != op->xattr.name_len +
						  op->xattr.value_len);
			ceph_osdc_msg_data_add(msg, &op->xattr.osd_data);
			break;
		case CEPH_OSD_OP_NOTIFY_ACK:
			ceph_osdc_msg_data_add(msg,
					       &op->notify_ack.request_data);
			break;

		/* reply */
		case CEPH_OSD_OP_STAT:
			ceph_osdc_msg_data_add(req->r_reply,
					       &op->raw_data_in);
			break;
		case CEPH_OSD_OP_READ:
			ceph_osdc_msg_data_add(req->r_reply,
					       &op->extent.osd_data);
			break;
		case CEPH_OSD_OP_LIST_WATCHERS:
			ceph_osdc_msg_data_add(req->r_reply,
					       &op->list_watchers.response_data);
			break;

		/* both */
		case CEPH_OSD_OP_CALL:
			WARN_ON(op->indata_len != op->cls.class_len +
						  op->cls.method_len +
						  op->cls.indata_len);
			ceph_osdc_msg_data_add(msg, &op->cls.request_info);
			/* optional, can be NONE */
			ceph_osdc_msg_data_add(msg, &op->cls.request_data);
			/* optional, can be NONE */
			ceph_osdc_msg_data_add(req->r_reply,
					       &op->cls.response_data);
			break;
		case CEPH_OSD_OP_NOTIFY:
			ceph_osdc_msg_data_add(msg,
					       &op->notify.request_data);
			ceph_osdc_msg_data_add(req->r_reply,
					       &op->notify.response_data);
			break;
		}

		data_len += op->indata_len;
	}

	WARN_ON(data_len != msg->data_length);
}

static void encode_request(struct ceph_osd_request *req, struct ceph_msg *msg)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front_alloc_len;
	u32 data_len = 0;
	int i;

	if (req->r_flags & CEPH_OSD_FLAG_WRITE) {
		/* snapshots aren't writeable */
		WARN_ON(req->r_snapid != CEPH_NOSNAP);
	} else {
		WARN_ON(req->r_mtime.tv_sec || req->r_mtime.tv_nsec ||
			req->r_data_offset || req->r_snapc);
	}

	setup_request_data(req, msg);

	ceph_encode_32(&p, 1); /* client_inc, always 1 */
	ceph_encode_32(&p, req->r_osdc->osdmap->epoch);
	ceph_encode_32(&p, req->r_flags);
	ceph_encode_timespec(p, &req->r_mtime);
	p += sizeof(struct ceph_timespec);
	/* aka reassert_version */
	memcpy(p, &req->r_replay_version, sizeof(req->r_replay_version));
	p += sizeof(req->r_replay_version);

	/* oloc */
	ceph_start_encoding(&p, 5, 4,
			    ceph_oloc_encoding_size(&req->r_t.target_oloc));
	ceph_encode_64(&p, req->r_t.target_oloc.pool);
	ceph_encode_32(&p, -1); /* preferred */
	ceph_encode_32(&p, 0); /* key len */
	if (req->r_t.target_oloc.pool_ns)
		ceph_encode_string(&p, end, req->r_t.target_oloc.pool_ns->str,
				   req->r_t.target_oloc.pool_ns->len);
	else
		ceph_encode_32(&p, 0);

	/* pgid */
	ceph_encode_8(&p, 1);
	ceph_encode_64(&p, req->r_t.pgid.pool);
	ceph_encode_32(&p, req->r_t.pgid.seed);
	ceph_encode_32(&p, -1); /* preferred */

	/* oid */
	ceph_encode_32(&p, req->r_t.target_oid.name_len);
	memcpy(p, req->r_t.target_oid.name, req->r_t.target_oid.name_len);
	p += req->r_t.target_oid.name_len;

	/* ops, can imply data */
	ceph_encode_16(&p, req->r_num_ops);
	for (i = 0; i < req->r_num_ops; i++) {
		data_len += osd_req_encode_op(p, &req->r_ops[i]);
		p += sizeof(struct ceph_osd_op);
	}

	ceph_encode_64(&p, req->r_snapid); /* snapid */
	if (req->r_snapc) {
		ceph_encode_64(&p, req->r_snapc->seq);
		ceph_encode_32(&p, req->r_snapc->num_snaps);
		for (i = 0; i < req->r_snapc->num_snaps; i++)
			ceph_encode_64(&p, req->r_snapc->snaps[i]);
	} else {
		ceph_encode_64(&p, 0); /* snap_seq */
		ceph_encode_32(&p, 0); /* snaps len */
	}

	ceph_encode_32(&p, req->r_attempts); /* retry_attempt */

	BUG_ON(p > end);
	msg->front.iov_len = p - msg->front.iov_base;
	msg->hdr.version = cpu_to_le16(4); /* MOSDOp v4 */
	msg->hdr.front_len = cpu_to_le32(msg->front.iov_len);
	msg->hdr.data_len = cpu_to_le32(data_len);
	/*
	 * The header "data_off" is a hint to the receiver allowing it
	 * to align received data into its buffers such that there's no
	 * need to re-copy it before writing it to disk (direct I/O).
	 */
	msg->hdr.data_off = cpu_to_le16(req->r_data_offset);

	dout("%s req %p oid %s oid_len %d front %zu data %u\n", __func__,
	     req, req->r_t.target_oid.name, req->r_t.target_oid.name_len,
	     msg->front.iov_len, data_len);
}

/*
 * @req has to be assigned a tid and registered.
 */
static void send_request(struct ceph_osd_request *req)
{
	struct ceph_osd *osd = req->r_osd;

	verify_osd_locked(osd);
	WARN_ON(osd->o_osd != req->r_t.osd);

	/*
	 * We may have a previously queued request message hanging
	 * around.  Cancel it to avoid corrupting the msgr.
	 */
	if (req->r_sent)
		ceph_msg_revoke(req->r_request);

	req->r_flags |= CEPH_OSD_FLAG_KNOWN_REDIR;
	if (req->r_attempts)
		req->r_flags |= CEPH_OSD_FLAG_RETRY;
	else
		WARN_ON(req->r_flags & CEPH_OSD_FLAG_RETRY);

	encode_request(req, req->r_request);

	dout("%s req %p tid %llu to pg %llu.%x osd%d flags 0x%x attempt %d\n",
	     __func__, req, req->r_tid, req->r_t.pgid.pool, req->r_t.pgid.seed,
	     req->r_t.osd, req->r_flags, req->r_attempts);

	req->r_t.paused = false;
	req->r_stamp = jiffies;
	req->r_attempts++;

	req->r_sent = osd->o_incarnation;
	req->r_request->hdr.tid = cpu_to_le64(req->r_tid);
	ceph_con_send(&osd->o_con, ceph_msg_get(req->r_request));
}

static void maybe_request_map(struct ceph_osd_client *osdc)
{
	bool continuous = false;

	verify_osdc_locked(osdc);
	WARN_ON(!osdc->osdmap->epoch);

	if (ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
	    ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD) ||
	    ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR)) {
		dout("%s osdc %p continuous\n", __func__, osdc);
		continuous = true;
	} else {
		dout("%s osdc %p onetime\n", __func__, osdc);
	}

	if (ceph_monc_want_map(&osdc->client->monc, CEPH_SUB_OSDMAP,
			       osdc->osdmap->epoch + 1, continuous))
		ceph_monc_renew_subs(&osdc->client->monc);
}

static void send_map_check(struct ceph_osd_request *req);

static void __submit_request(struct ceph_osd_request *req, bool wrlocked)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd *osd;
	enum calc_target_result ct_res;
	bool need_send = false;
	bool promoted = false;

	WARN_ON(req->r_tid || req->r_got_reply);
	dout("%s req %p wrlocked %d\n", __func__, req, wrlocked);

again:
	ct_res = calc_target(osdc, &req->r_t, &req->r_last_force_resend, false);
	if (ct_res == CALC_TARGET_POOL_DNE && !wrlocked)
		goto promote;

	osd = lookup_create_osd(osdc, req->r_t.osd, wrlocked);
	if (IS_ERR(osd)) {
		WARN_ON(PTR_ERR(osd) != -EAGAIN || wrlocked);
		goto promote;
	}

	if ((req->r_flags & CEPH_OSD_FLAG_WRITE) &&
	    ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR)) {
		dout("req %p pausewr\n", req);
		req->r_t.paused = true;
		maybe_request_map(osdc);
	} else if ((req->r_flags & CEPH_OSD_FLAG_READ) &&
		   ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD)) {
		dout("req %p pauserd\n", req);
		req->r_t.paused = true;
		maybe_request_map(osdc);
	} else if ((req->r_flags & CEPH_OSD_FLAG_WRITE) &&
		   !(req->r_flags & (CEPH_OSD_FLAG_FULL_TRY |
				     CEPH_OSD_FLAG_FULL_FORCE)) &&
		   (ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		    pool_full(osdc, req->r_t.base_oloc.pool))) {
		dout("req %p full/pool_full\n", req);
		pr_warn_ratelimited("FULL or reached pool quota\n");
		req->r_t.paused = true;
		maybe_request_map(osdc);
	} else if (!osd_homeless(osd)) {
		need_send = true;
	} else {
		maybe_request_map(osdc);
	}

	mutex_lock(&osd->lock);
	/*
	 * Assign the tid atomically with send_request() to protect
	 * multiple writes to the same object from racing with each
	 * other, resulting in out of order ops on the OSDs.
	 */
	req->r_tid = atomic64_inc_return(&osdc->last_tid);
	link_request(osd, req);
	if (need_send)
		send_request(req);
	mutex_unlock(&osd->lock);

	if (ct_res == CALC_TARGET_POOL_DNE)
		send_map_check(req);

	if (promoted)
		downgrade_write(&osdc->lock);
	return;

promote:
	up_read(&osdc->lock);
	down_write(&osdc->lock);
	wrlocked = true;
	promoted = true;
	goto again;
}

static void account_request(struct ceph_osd_request *req)
{
	unsigned int mask = CEPH_OSD_FLAG_ACK | CEPH_OSD_FLAG_ONDISK;

	if (req->r_flags & CEPH_OSD_FLAG_READ) {
		WARN_ON(req->r_flags & mask);
		req->r_flags |= CEPH_OSD_FLAG_ACK;
	} else if (req->r_flags & CEPH_OSD_FLAG_WRITE)
		WARN_ON(!(req->r_flags & mask));
	else
		WARN_ON(1);

	WARN_ON(req->r_unsafe_callback && (req->r_flags & mask) != mask);
	atomic_inc(&req->r_osdc->num_requests);
}

static void submit_request(struct ceph_osd_request *req, bool wrlocked)
{
	ceph_osdc_get_request(req);
	account_request(req);
	__submit_request(req, wrlocked);
}

static void __finish_request(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd *osd = req->r_osd;

	verify_osd_locked(osd);
	dout("%s req %p tid %llu\n", __func__, req, req->r_tid);

	WARN_ON(lookup_request_mc(&osdc->map_checks, req->r_tid));
	unlink_request(osd, req);
	atomic_dec(&osdc->num_requests);

	/*
	 * If an OSD has failed or returned and a request has been sent
	 * twice, it's possible to get a reply and end up here while the
	 * request message is queued for delivery.  We will ignore the
	 * reply, so not a big deal, but better to try and catch it.
	 */
	ceph_msg_revoke(req->r_request);
	ceph_msg_revoke_incoming(req->r_reply);
}

static void finish_request(struct ceph_osd_request *req)
{
	__finish_request(req);
	ceph_osdc_put_request(req);
}

static void __complete_request(struct ceph_osd_request *req)
{
	if (req->r_callback)
		req->r_callback(req);
	else
		complete_all(&req->r_completion);
}

/*
 * Note that this is open-coded in handle_reply(), which has to deal
 * with ack vs commit, dup acks, etc.
 */
static void complete_request(struct ceph_osd_request *req, int err)
{
	dout("%s req %p tid %llu err %d\n", __func__, req, req->r_tid, err);

	req->r_result = err;
	__finish_request(req);
	__complete_request(req);
	complete_all(&req->r_safe_completion);
	ceph_osdc_put_request(req);
}

static void cancel_map_check(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd_request *lookup_req;

	verify_osdc_wrlocked(osdc);

	lookup_req = lookup_request_mc(&osdc->map_checks, req->r_tid);
	if (!lookup_req)
		return;

	WARN_ON(lookup_req != req);
	erase_request_mc(&osdc->map_checks, req);
	ceph_osdc_put_request(req);
}

static void cancel_request(struct ceph_osd_request *req)
{
	dout("%s req %p tid %llu\n", __func__, req, req->r_tid);

	cancel_map_check(req);
	finish_request(req);
}

static void check_pool_dne(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osdmap *map = osdc->osdmap;

	verify_osdc_wrlocked(osdc);
	WARN_ON(!map->epoch);

	if (req->r_attempts) {
		/*
		 * We sent a request earlier, which means that
		 * previously the pool existed, and now it does not
		 * (i.e., it was deleted).
		 */
		req->r_map_dne_bound = map->epoch;
		dout("%s req %p tid %llu pool disappeared\n", __func__, req,
		     req->r_tid);
	} else {
		dout("%s req %p tid %llu map_dne_bound %u have %u\n", __func__,
		     req, req->r_tid, req->r_map_dne_bound, map->epoch);
	}

	if (req->r_map_dne_bound) {
		if (map->epoch >= req->r_map_dne_bound) {
			/* we had a new enough map */
			pr_info_ratelimited("tid %llu pool does not exist\n",
					    req->r_tid);
			complete_request(req, -ENOENT);
		}
	} else {
		send_map_check(req);
	}
}

static void map_check_cb(struct ceph_mon_generic_request *greq)
{
	struct ceph_osd_client *osdc = &greq->monc->client->osdc;
	struct ceph_osd_request *req;
	u64 tid = greq->private_data;

	WARN_ON(greq->result || !greq->u.newest);

	down_write(&osdc->lock);
	req = lookup_request_mc(&osdc->map_checks, tid);
	if (!req) {
		dout("%s tid %llu dne\n", __func__, tid);
		goto out_unlock;
	}

	dout("%s req %p tid %llu map_dne_bound %u newest %llu\n", __func__,
	     req, req->r_tid, req->r_map_dne_bound, greq->u.newest);
	if (!req->r_map_dne_bound)
		req->r_map_dne_bound = greq->u.newest;
	erase_request_mc(&osdc->map_checks, req);
	check_pool_dne(req);

	ceph_osdc_put_request(req);
out_unlock:
	up_write(&osdc->lock);
}

static void send_map_check(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd_request *lookup_req;
	int ret;

	verify_osdc_wrlocked(osdc);

	lookup_req = lookup_request_mc(&osdc->map_checks, req->r_tid);
	if (lookup_req) {
		WARN_ON(lookup_req != req);
		return;
	}

	ceph_osdc_get_request(req);
	insert_request_mc(&osdc->map_checks, req);
	ret = ceph_monc_get_version_async(&osdc->client->monc, "osdmap",
					  map_check_cb, req->r_tid);
	WARN_ON(ret);
}

/*
 * lingering requests, watch/notify v2 infrastructure
 */
static void linger_release(struct kref *kref)
{
	struct ceph_osd_linger_request *lreq =
	    container_of(kref, struct ceph_osd_linger_request, kref);

	dout("%s lreq %p reg_req %p ping_req %p\n", __func__, lreq,
	     lreq->reg_req, lreq->ping_req);
	WARN_ON(!RB_EMPTY_NODE(&lreq->node));
	WARN_ON(!RB_EMPTY_NODE(&lreq->osdc_node));
	WARN_ON(!RB_EMPTY_NODE(&lreq->mc_node));
	WARN_ON(!list_empty(&lreq->scan_item));
	WARN_ON(!list_empty(&lreq->pending_lworks));
	WARN_ON(lreq->osd);

	if (lreq->reg_req)
		ceph_osdc_put_request(lreq->reg_req);
	if (lreq->ping_req)
		ceph_osdc_put_request(lreq->ping_req);
	target_destroy(&lreq->t);
	kfree(lreq);
}

static void linger_put(struct ceph_osd_linger_request *lreq)
{
	if (lreq)
		kref_put(&lreq->kref, linger_release);
}

static struct ceph_osd_linger_request *
linger_get(struct ceph_osd_linger_request *lreq)
{
	kref_get(&lreq->kref);
	return lreq;
}

static struct ceph_osd_linger_request *
linger_alloc(struct ceph_osd_client *osdc)
{
	struct ceph_osd_linger_request *lreq;

	lreq = kzalloc(sizeof(*lreq), GFP_NOIO);
	if (!lreq)
		return NULL;

	kref_init(&lreq->kref);
	mutex_init(&lreq->lock);
	RB_CLEAR_NODE(&lreq->node);
	RB_CLEAR_NODE(&lreq->osdc_node);
	RB_CLEAR_NODE(&lreq->mc_node);
	INIT_LIST_HEAD(&lreq->scan_item);
	INIT_LIST_HEAD(&lreq->pending_lworks);
	init_completion(&lreq->reg_commit_wait);
	init_completion(&lreq->notify_finish_wait);

	lreq->osdc = osdc;
	target_init(&lreq->t);

	dout("%s lreq %p\n", __func__, lreq);
	return lreq;
}

DEFINE_RB_INSDEL_FUNCS(linger, struct ceph_osd_linger_request, linger_id, node)
DEFINE_RB_FUNCS(linger_osdc, struct ceph_osd_linger_request, linger_id, osdc_node)
DEFINE_RB_FUNCS(linger_mc, struct ceph_osd_linger_request, linger_id, mc_node)

/*
 * Create linger request <-> OSD session relation.
 *
 * @lreq has to be registered, @osd may be homeless.
 */
static void link_linger(struct ceph_osd *osd,
			struct ceph_osd_linger_request *lreq)
{
	verify_osd_locked(osd);
	WARN_ON(!lreq->linger_id || lreq->osd);
	dout("%s osd %p osd%d lreq %p linger_id %llu\n", __func__, osd,
	     osd->o_osd, lreq, lreq->linger_id);

	if (!osd_homeless(osd))
		__remove_osd_from_lru(osd);
	else
		atomic_inc(&osd->o_osdc->num_homeless);

	get_osd(osd);
	insert_linger(&osd->o_linger_requests, lreq);
	lreq->osd = osd;
}

static void unlink_linger(struct ceph_osd *osd,
			  struct ceph_osd_linger_request *lreq)
{
	verify_osd_locked(osd);
	WARN_ON(lreq->osd != osd);
	dout("%s osd %p osd%d lreq %p linger_id %llu\n", __func__, osd,
	     osd->o_osd, lreq, lreq->linger_id);

	lreq->osd = NULL;
	erase_linger(&osd->o_linger_requests, lreq);
	put_osd(osd);

	if (!osd_homeless(osd))
		maybe_move_osd_to_lru(osd);
	else
		atomic_dec(&osd->o_osdc->num_homeless);
}

static bool __linger_registered(struct ceph_osd_linger_request *lreq)
{
	verify_osdc_locked(lreq->osdc);

	return !RB_EMPTY_NODE(&lreq->osdc_node);
}

static bool linger_registered(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	bool registered;

	down_read(&osdc->lock);
	registered = __linger_registered(lreq);
	up_read(&osdc->lock);

	return registered;
}

static void linger_register(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;

	verify_osdc_wrlocked(osdc);
	WARN_ON(lreq->linger_id);

	linger_get(lreq);
	lreq->linger_id = ++osdc->last_linger_id;
	insert_linger_osdc(&osdc->linger_requests, lreq);
}

static void linger_unregister(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;

	verify_osdc_wrlocked(osdc);

	erase_linger_osdc(&osdc->linger_requests, lreq);
	linger_put(lreq);
}

static void cancel_linger_request(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	WARN_ON(!req->r_linger);
	cancel_request(req);
	linger_put(lreq);
}

struct linger_work {
	struct work_struct work;
	struct ceph_osd_linger_request *lreq;
	struct list_head pending_item;
	unsigned long queued_stamp;

	union {
		struct {
			u64 notify_id;
			u64 notifier_id;
			void *payload; /* points into @msg front */
			size_t payload_len;

			struct ceph_msg *msg; /* for ceph_msg_put() */
		} notify;
		struct {
			int err;
		} error;
	};
};

static struct linger_work *lwork_alloc(struct ceph_osd_linger_request *lreq,
				       work_func_t workfn)
{
	struct linger_work *lwork;

	lwork = kzalloc(sizeof(*lwork), GFP_NOIO);
	if (!lwork)
		return NULL;

	INIT_WORK(&lwork->work, workfn);
	INIT_LIST_HEAD(&lwork->pending_item);
	lwork->lreq = linger_get(lreq);

	return lwork;
}

static void lwork_free(struct linger_work *lwork)
{
	struct ceph_osd_linger_request *lreq = lwork->lreq;

	mutex_lock(&lreq->lock);
	list_del(&lwork->pending_item);
	mutex_unlock(&lreq->lock);

	linger_put(lreq);
	kfree(lwork);
}

static void lwork_queue(struct linger_work *lwork)
{
	struct ceph_osd_linger_request *lreq = lwork->lreq;
	struct ceph_osd_client *osdc = lreq->osdc;

	verify_lreq_locked(lreq);
	WARN_ON(!list_empty(&lwork->pending_item));

	lwork->queued_stamp = jiffies;
	list_add_tail(&lwork->pending_item, &lreq->pending_lworks);
	queue_work(osdc->notify_wq, &lwork->work);
}

static void do_watch_notify(struct work_struct *w)
{
	struct linger_work *lwork = container_of(w, struct linger_work, work);
	struct ceph_osd_linger_request *lreq = lwork->lreq;

	if (!linger_registered(lreq)) {
		dout("%s lreq %p not registered\n", __func__, lreq);
		goto out;
	}

	WARN_ON(!lreq->is_watch);
	dout("%s lreq %p notify_id %llu notifier_id %llu payload_len %zu\n",
	     __func__, lreq, lwork->notify.notify_id, lwork->notify.notifier_id,
	     lwork->notify.payload_len);
	lreq->wcb(lreq->data, lwork->notify.notify_id, lreq->linger_id,
		  lwork->notify.notifier_id, lwork->notify.payload,
		  lwork->notify.payload_len);

out:
	ceph_msg_put(lwork->notify.msg);
	lwork_free(lwork);
}

static void do_watch_error(struct work_struct *w)
{
	struct linger_work *lwork = container_of(w, struct linger_work, work);
	struct ceph_osd_linger_request *lreq = lwork->lreq;

	if (!linger_registered(lreq)) {
		dout("%s lreq %p not registered\n", __func__, lreq);
		goto out;
	}

	dout("%s lreq %p err %d\n", __func__, lreq, lwork->error.err);
	lreq->errcb(lreq->data, lreq->linger_id, lwork->error.err);

out:
	lwork_free(lwork);
}

static void queue_watch_error(struct ceph_osd_linger_request *lreq)
{
	struct linger_work *lwork;

	lwork = lwork_alloc(lreq, do_watch_error);
	if (!lwork) {
		pr_err("failed to allocate error-lwork\n");
		return;
	}

	lwork->error.err = lreq->last_error;
	lwork_queue(lwork);
}

static void linger_reg_commit_complete(struct ceph_osd_linger_request *lreq,
				       int result)
{
	if (!completion_done(&lreq->reg_commit_wait)) {
		lreq->reg_commit_error = (result <= 0 ? result : 0);
		complete_all(&lreq->reg_commit_wait);
	}
}

static void linger_commit_cb(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	mutex_lock(&lreq->lock);
	dout("%s lreq %p linger_id %llu result %d\n", __func__, lreq,
	     lreq->linger_id, req->r_result);
	WARN_ON(!__linger_registered(lreq));
	linger_reg_commit_complete(lreq, req->r_result);
	lreq->committed = true;

	if (!lreq->is_watch) {
		struct ceph_osd_data *osd_data =
		    osd_req_op_data(req, 0, notify, response_data);
		void *p = page_address(osd_data->pages[0]);

		WARN_ON(req->r_ops[0].op != CEPH_OSD_OP_NOTIFY ||
			osd_data->type != CEPH_OSD_DATA_TYPE_PAGES);

		/* make note of the notify_id */
		if (req->r_ops[0].outdata_len >= sizeof(u64)) {
			lreq->notify_id = ceph_decode_64(&p);
			dout("lreq %p notify_id %llu\n", lreq,
			     lreq->notify_id);
		} else {
			dout("lreq %p no notify_id\n", lreq);
		}
	}

	mutex_unlock(&lreq->lock);
	linger_put(lreq);
}

static int normalize_watch_error(int err)
{
	/*
	 * Translate ENOENT -> ENOTCONN so that a delete->disconnection
	 * notification and a failure to reconnect because we raced with
	 * the delete appear the same to the user.
	 */
	if (err == -ENOENT)
		err = -ENOTCONN;

	return err;
}

static void linger_reconnect_cb(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	mutex_lock(&lreq->lock);
	dout("%s lreq %p linger_id %llu result %d last_error %d\n", __func__,
	     lreq, lreq->linger_id, req->r_result, lreq->last_error);
	if (req->r_result < 0) {
		if (!lreq->last_error) {
			lreq->last_error = normalize_watch_error(req->r_result);
			queue_watch_error(lreq);
		}
	}

	mutex_unlock(&lreq->lock);
	linger_put(lreq);
}

static void send_linger(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_request *req = lreq->reg_req;
	struct ceph_osd_req_op *op = &req->r_ops[0];

	verify_osdc_wrlocked(req->r_osdc);
	dout("%s lreq %p linger_id %llu\n", __func__, lreq, lreq->linger_id);

	if (req->r_osd)
		cancel_linger_request(req);

	request_reinit(req);
	ceph_oid_copy(&req->r_base_oid, &lreq->t.base_oid);
	ceph_oloc_copy(&req->r_base_oloc, &lreq->t.base_oloc);
	req->r_flags = lreq->t.flags;
	req->r_mtime = lreq->mtime;

	mutex_lock(&lreq->lock);
	if (lreq->is_watch && lreq->committed) {
		WARN_ON(op->op != CEPH_OSD_OP_WATCH ||
			op->watch.cookie != lreq->linger_id);
		op->watch.op = CEPH_OSD_WATCH_OP_RECONNECT;
		op->watch.gen = ++lreq->register_gen;
		dout("lreq %p reconnect register_gen %u\n", lreq,
		     op->watch.gen);
		req->r_callback = linger_reconnect_cb;
	} else {
		if (!lreq->is_watch)
			lreq->notify_id = 0;
		else
			WARN_ON(op->watch.op != CEPH_OSD_WATCH_OP_WATCH);
		dout("lreq %p register\n", lreq);
		req->r_callback = linger_commit_cb;
	}
	mutex_unlock(&lreq->lock);

	req->r_priv = linger_get(lreq);
	req->r_linger = true;

	submit_request(req, true);
}

static void linger_ping_cb(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	mutex_lock(&lreq->lock);
	dout("%s lreq %p linger_id %llu result %d ping_sent %lu last_error %d\n",
	     __func__, lreq, lreq->linger_id, req->r_result, lreq->ping_sent,
	     lreq->last_error);
	if (lreq->register_gen == req->r_ops[0].watch.gen) {
		if (!req->r_result) {
			lreq->watch_valid_thru = lreq->ping_sent;
		} else if (!lreq->last_error) {
			lreq->last_error = normalize_watch_error(req->r_result);
			queue_watch_error(lreq);
		}
	} else {
		dout("lreq %p register_gen %u ignoring old pong %u\n", lreq,
		     lreq->register_gen, req->r_ops[0].watch.gen);
	}

	mutex_unlock(&lreq->lock);
	linger_put(lreq);
}

static void send_linger_ping(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd_request *req = lreq->ping_req;
	struct ceph_osd_req_op *op = &req->r_ops[0];

	if (ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD)) {
		dout("%s PAUSERD\n", __func__);
		return;
	}

	lreq->ping_sent = jiffies;
	dout("%s lreq %p linger_id %llu ping_sent %lu register_gen %u\n",
	     __func__, lreq, lreq->linger_id, lreq->ping_sent,
	     lreq->register_gen);

	if (req->r_osd)
		cancel_linger_request(req);

	request_reinit(req);
	target_copy(&req->r_t, &lreq->t);

	WARN_ON(op->op != CEPH_OSD_OP_WATCH ||
		op->watch.cookie != lreq->linger_id ||
		op->watch.op != CEPH_OSD_WATCH_OP_PING);
	op->watch.gen = lreq->register_gen;
	req->r_callback = linger_ping_cb;
	req->r_priv = linger_get(lreq);
	req->r_linger = true;

	ceph_osdc_get_request(req);
	account_request(req);
	req->r_tid = atomic64_inc_return(&osdc->last_tid);
	link_request(lreq->osd, req);
	send_request(req);
}

static void linger_submit(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd *osd;

	calc_target(osdc, &lreq->t, &lreq->last_force_resend, false);
	osd = lookup_create_osd(osdc, lreq->t.osd, true);
	link_linger(osd, lreq);

	send_linger(lreq);
}

static void cancel_linger_map_check(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd_linger_request *lookup_lreq;

	verify_osdc_wrlocked(osdc);

	lookup_lreq = lookup_linger_mc(&osdc->linger_map_checks,
				       lreq->linger_id);
	if (!lookup_lreq)
		return;

	WARN_ON(lookup_lreq != lreq);
	erase_linger_mc(&osdc->linger_map_checks, lreq);
	linger_put(lreq);
}

/*
 * @lreq has to be both registered and linked.
 */
static void __linger_cancel(struct ceph_osd_linger_request *lreq)
{
	if (lreq->is_watch && lreq->ping_req->r_osd)
		cancel_linger_request(lreq->ping_req);
	if (lreq->reg_req->r_osd)
		cancel_linger_request(lreq->reg_req);
	cancel_linger_map_check(lreq);
	unlink_linger(lreq->osd, lreq);
	linger_unregister(lreq);
}

static void linger_cancel(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;

	down_write(&osdc->lock);
	if (__linger_registered(lreq))
		__linger_cancel(lreq);
	up_write(&osdc->lock);
}

static void send_linger_map_check(struct ceph_osd_linger_request *lreq);

static void check_linger_pool_dne(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osdmap *map = osdc->osdmap;

	verify_osdc_wrlocked(osdc);
	WARN_ON(!map->epoch);

	if (lreq->register_gen) {
		lreq->map_dne_bound = map->epoch;
		dout("%s lreq %p linger_id %llu pool disappeared\n", __func__,
		     lreq, lreq->linger_id);
	} else {
		dout("%s lreq %p linger_id %llu map_dne_bound %u have %u\n",
		     __func__, lreq, lreq->linger_id, lreq->map_dne_bound,
		     map->epoch);
	}

	if (lreq->map_dne_bound) {
		if (map->epoch >= lreq->map_dne_bound) {
			/* we had a new enough map */
			pr_info("linger_id %llu pool does not exist\n",
				lreq->linger_id);
			linger_reg_commit_complete(lreq, -ENOENT);
			__linger_cancel(lreq);
		}
	} else {
		send_linger_map_check(lreq);
	}
}

static void linger_map_check_cb(struct ceph_mon_generic_request *greq)
{
	struct ceph_osd_client *osdc = &greq->monc->client->osdc;
	struct ceph_osd_linger_request *lreq;
	u64 linger_id = greq->private_data;

	WARN_ON(greq->result || !greq->u.newest);

	down_write(&osdc->lock);
	lreq = lookup_linger_mc(&osdc->linger_map_checks, linger_id);
	if (!lreq) {
		dout("%s linger_id %llu dne\n", __func__, linger_id);
		goto out_unlock;
	}

	dout("%s lreq %p linger_id %llu map_dne_bound %u newest %llu\n",
	     __func__, lreq, lreq->linger_id, lreq->map_dne_bound,
	     greq->u.newest);
	if (!lreq->map_dne_bound)
		lreq->map_dne_bound = greq->u.newest;
	erase_linger_mc(&osdc->linger_map_checks, lreq);
	check_linger_pool_dne(lreq);

	linger_put(lreq);
out_unlock:
	up_write(&osdc->lock);
}

static void send_linger_map_check(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd_linger_request *lookup_lreq;
	int ret;

	verify_osdc_wrlocked(osdc);

	lookup_lreq = lookup_linger_mc(&osdc->linger_map_checks,
				       lreq->linger_id);
	if (lookup_lreq) {
		WARN_ON(lookup_lreq != lreq);
		return;
	}

	linger_get(lreq);
	insert_linger_mc(&osdc->linger_map_checks, lreq);
	ret = ceph_monc_get_version_async(&osdc->client->monc, "osdmap",
					  linger_map_check_cb, lreq->linger_id);
	WARN_ON(ret);
}

static int linger_reg_commit_wait(struct ceph_osd_linger_request *lreq)
{
	int ret;

	dout("%s lreq %p linger_id %llu\n", __func__, lreq, lreq->linger_id);
	ret = wait_for_completion_interruptible(&lreq->reg_commit_wait);
	return ret ?: lreq->reg_commit_error;
}

static int linger_notify_finish_wait(struct ceph_osd_linger_request *lreq)
{
	int ret;

	dout("%s lreq %p linger_id %llu\n", __func__, lreq, lreq->linger_id);
	ret = wait_for_completion_interruptible(&lreq->notify_finish_wait);
	return ret ?: lreq->notify_finish_error;
}

/*
 * Timeout callback, called every N seconds.  When 1 or more OSD
 * requests has been active for more than N seconds, we send a keepalive
 * (tag + timestamp) to its OSD to ensure any communications channel
 * reset is detected.
>>>>>>> v4.9.227
 */
static void handle_timeout(struct work_struct *work)
{
	struct ceph_osd_client *osdc =
		container_of(work, struct ceph_osd_client, timeout_work.work);
<<<<<<< HEAD
	struct ceph_osd_request *req;
	struct ceph_osd *osd;
	unsigned long keepalive =
		osdc->client->options->osd_keepalive_timeout * HZ;
	struct list_head slow_osds;
	dout("timeout\n");
	down_read(&osdc->map_sem);

	ceph_monc_request_next_osdmap(&osdc->client->monc);

	mutex_lock(&osdc->request_mutex);
=======
	struct ceph_options *opts = osdc->client->options;
	unsigned long cutoff = jiffies - opts->osd_keepalive_timeout;
	LIST_HEAD(slow_osds);
	struct rb_node *n, *p;

	dout("%s osdc %p\n", __func__, osdc);
	down_write(&osdc->lock);
>>>>>>> v4.9.227

	/*
	 * ping osds that are a bit slow.  this ensures that if there
	 * is a break in the TCP connection we will notice, and reopen
	 * a connection with that osd (from the fault callback).
	 */
<<<<<<< HEAD
	INIT_LIST_HEAD(&slow_osds);
	list_for_each_entry(req, &osdc->req_lru, r_req_lru_item) {
		if (time_before(jiffies, req->r_stamp + keepalive))
			break;

		osd = req->r_osd;
		BUG_ON(!osd);
		dout(" tid %llu is slow, will send keepalive on osd%d\n",
		     req->r_tid, osd->o_osd);
		list_move_tail(&osd->o_keepalive_item, &slow_osds);
	}
	while (!list_empty(&slow_osds)) {
		osd = list_entry(slow_osds.next, struct ceph_osd,
				 o_keepalive_item);
=======
	for (n = rb_first(&osdc->osds); n; n = rb_next(n)) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);
		bool found = false;

		for (p = rb_first(&osd->o_requests); p; p = rb_next(p)) {
			struct ceph_osd_request *req =
			    rb_entry(p, struct ceph_osd_request, r_node);

			if (time_before(req->r_stamp, cutoff)) {
				dout(" req %p tid %llu on osd%d is laggy\n",
				     req, req->r_tid, osd->o_osd);
				found = true;
			}
		}
		for (p = rb_first(&osd->o_linger_requests); p; p = rb_next(p)) {
			struct ceph_osd_linger_request *lreq =
			    rb_entry(p, struct ceph_osd_linger_request, node);

			dout(" lreq %p linger_id %llu is served by osd%d\n",
			     lreq, lreq->linger_id, osd->o_osd);
			found = true;

			mutex_lock(&lreq->lock);
			if (lreq->is_watch && lreq->committed && !lreq->last_error)
				send_linger_ping(lreq);
			mutex_unlock(&lreq->lock);
		}

		if (found)
			list_move_tail(&osd->o_keepalive_item, &slow_osds);
	}

	if (atomic_read(&osdc->num_homeless) || !list_empty(&slow_osds))
		maybe_request_map(osdc);

	while (!list_empty(&slow_osds)) {
		struct ceph_osd *osd = list_first_entry(&slow_osds,
							struct ceph_osd,
							o_keepalive_item);
>>>>>>> v4.9.227
		list_del_init(&osd->o_keepalive_item);
		ceph_con_keepalive(&osd->o_con);
	}

<<<<<<< HEAD
	__schedule_osd_timeout(osdc);
	__send_queued(osdc);
	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);
=======
	up_write(&osdc->lock);
	schedule_delayed_work(&osdc->timeout_work,
			      osdc->client->options->osd_keepalive_timeout);
>>>>>>> v4.9.227
}

static void handle_osds_timeout(struct work_struct *work)
{
	struct ceph_osd_client *osdc =
		container_of(work, struct ceph_osd_client,
			     osds_timeout_work.work);
<<<<<<< HEAD
	unsigned long delay =
		osdc->client->options->osd_idle_ttl * HZ >> 2;

	dout("osds timeout\n");
	down_read(&osdc->map_sem);
	remove_old_osds(osdc);
	up_read(&osdc->map_sem);

=======
	unsigned long delay = osdc->client->options->osd_idle_ttl / 4;
	struct ceph_osd *osd, *nosd;

	dout("%s osdc %p\n", __func__, osdc);
	down_write(&osdc->lock);
	list_for_each_entry_safe(osd, nosd, &osdc->osd_lru, o_osd_lru) {
		if (time_before(jiffies, osd->lru_ttl))
			break;

		WARN_ON(!RB_EMPTY_ROOT(&osd->o_requests));
		WARN_ON(!RB_EMPTY_ROOT(&osd->o_linger_requests));
		close_osd(osd);
	}

	up_write(&osdc->lock);
>>>>>>> v4.9.227
	schedule_delayed_work(&osdc->osds_timeout_work,
			      round_jiffies_relative(delay));
}

static int ceph_oloc_decode(void **p, void *end,
			    struct ceph_object_locator *oloc)
{
	u8 struct_v, struct_cv;
	u32 len;
	void *struct_end;
	int ret = 0;

	ceph_decode_need(p, end, 1 + 1 + 4, e_inval);
	struct_v = ceph_decode_8(p);
	struct_cv = ceph_decode_8(p);
	if (struct_v < 3) {
		pr_warn("got v %d < 3 cv %d of ceph_object_locator\n",
			struct_v, struct_cv);
		goto e_inval;
	}
	if (struct_cv > 6) {
		pr_warn("got v %d cv %d > 6 of ceph_object_locator\n",
			struct_v, struct_cv);
		goto e_inval;
	}
	len = ceph_decode_32(p);
	ceph_decode_need(p, end, len, e_inval);
	struct_end = *p + len;

	oloc->pool = ceph_decode_64(p);
	*p += 4; /* skip preferred */

	len = ceph_decode_32(p);
	if (len > 0) {
		pr_warn("ceph_object_locator::key is set\n");
		goto e_inval;
	}

	if (struct_v >= 5) {
<<<<<<< HEAD
		len = ceph_decode_32(p);
		if (len > 0) {
			pr_warn("ceph_object_locator::nspace is set\n");
=======
		bool changed = false;

		len = ceph_decode_32(p);
		if (len > 0) {
			ceph_decode_need(p, end, len, e_inval);
			if (!oloc->pool_ns ||
			    ceph_compare_string(oloc->pool_ns, *p, len))
				changed = true;
			*p += len;
		} else {
			if (oloc->pool_ns)
				changed = true;
		}
		if (changed) {
			/* redirect changes namespace */
			pr_warn("ceph_object_locator::nspace is changed\n");
>>>>>>> v4.9.227
			goto e_inval;
		}
	}

	if (struct_v >= 6) {
		s64 hash = ceph_decode_64(p);
		if (hash != -1) {
			pr_warn("ceph_object_locator::hash is set\n");
			goto e_inval;
		}
	}

	/* skip the rest */
	*p = struct_end;
out:
	return ret;

e_inval:
	ret = -EINVAL;
	goto out;
}

static int ceph_redirect_decode(void **p, void *end,
				struct ceph_request_redirect *redir)
{
	u8 struct_v, struct_cv;
	u32 len;
	void *struct_end;
	int ret;

	ceph_decode_need(p, end, 1 + 1 + 4, e_inval);
	struct_v = ceph_decode_8(p);
	struct_cv = ceph_decode_8(p);
	if (struct_cv > 1) {
		pr_warn("got v %d cv %d > 1 of ceph_request_redirect\n",
			struct_v, struct_cv);
		goto e_inval;
	}
	len = ceph_decode_32(p);
	ceph_decode_need(p, end, len, e_inval);
	struct_end = *p + len;

	ret = ceph_oloc_decode(p, end, &redir->oloc);
	if (ret)
		goto out;

	len = ceph_decode_32(p);
	if (len > 0) {
		pr_warn("ceph_request_redirect::object_name is set\n");
		goto e_inval;
	}

	len = ceph_decode_32(p);
	*p += len; /* skip osd_instructions */

	/* skip the rest */
	*p = struct_end;
out:
	return ret;

e_inval:
	ret = -EINVAL;
	goto out;
}

<<<<<<< HEAD
static void complete_request(struct ceph_osd_request *req)
{
	complete_all(&req->r_safe_completion);  /* fsync waiter */
=======
struct MOSDOpReply {
	struct ceph_pg pgid;
	u64 flags;
	int result;
	u32 epoch;
	int num_ops;
	u32 outdata_len[CEPH_OSD_MAX_OPS];
	s32 rval[CEPH_OSD_MAX_OPS];
	int retry_attempt;
	struct ceph_eversion replay_version;
	u64 user_version;
	struct ceph_request_redirect redirect;
};

static int decode_MOSDOpReply(const struct ceph_msg *msg, struct MOSDOpReply *m)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	u16 version = le16_to_cpu(msg->hdr.version);
	struct ceph_eversion bad_replay_version;
	u8 decode_redir;
	u32 len;
	int ret;
	int i;

	ceph_decode_32_safe(&p, end, len, e_inval);
	ceph_decode_need(&p, end, len, e_inval);
	p += len; /* skip oid */

	ret = ceph_decode_pgid(&p, end, &m->pgid);
	if (ret)
		return ret;

	ceph_decode_64_safe(&p, end, m->flags, e_inval);
	ceph_decode_32_safe(&p, end, m->result, e_inval);
	ceph_decode_need(&p, end, sizeof(bad_replay_version), e_inval);
	memcpy(&bad_replay_version, p, sizeof(bad_replay_version));
	p += sizeof(bad_replay_version);
	ceph_decode_32_safe(&p, end, m->epoch, e_inval);

	ceph_decode_32_safe(&p, end, m->num_ops, e_inval);
	if (m->num_ops > ARRAY_SIZE(m->outdata_len))
		goto e_inval;

	ceph_decode_need(&p, end, m->num_ops * sizeof(struct ceph_osd_op),
			 e_inval);
	for (i = 0; i < m->num_ops; i++) {
		struct ceph_osd_op *op = p;

		m->outdata_len[i] = le32_to_cpu(op->payload_len);
		p += sizeof(*op);
	}

	ceph_decode_32_safe(&p, end, m->retry_attempt, e_inval);
	for (i = 0; i < m->num_ops; i++)
		ceph_decode_32_safe(&p, end, m->rval[i], e_inval);

	if (version >= 5) {
		ceph_decode_need(&p, end, sizeof(m->replay_version), e_inval);
		memcpy(&m->replay_version, p, sizeof(m->replay_version));
		p += sizeof(m->replay_version);
		ceph_decode_64_safe(&p, end, m->user_version, e_inval);
	} else {
		m->replay_version = bad_replay_version; /* struct */
		m->user_version = le64_to_cpu(m->replay_version.version);
	}

	if (version >= 6) {
		if (version >= 7)
			ceph_decode_8_safe(&p, end, decode_redir, e_inval);
		else
			decode_redir = 1;
	} else {
		decode_redir = 0;
	}

	if (decode_redir) {
		ret = ceph_redirect_decode(&p, end, &m->redirect);
		if (ret)
			return ret;
	} else {
		ceph_oloc_init(&m->redirect.oloc);
	}

	return 0;

e_inval:
	return -EINVAL;
}

/*
 * We are done with @req if
 *   - @m is a safe reply, or
 *   - @m is an unsafe reply and we didn't want a safe one
 */
static bool done_request(const struct ceph_osd_request *req,
			 const struct MOSDOpReply *m)
{
	return (m->result < 0 ||
		(m->flags & CEPH_OSD_FLAG_ONDISK) ||
		!(req->r_flags & CEPH_OSD_FLAG_ONDISK));
>>>>>>> v4.9.227
}

/*
 * handle osd op reply.  either call the callback if it is specified,
 * or do the completion to wake up the waiting thread.
<<<<<<< HEAD
 */
static void handle_reply(struct ceph_osd_client *osdc, struct ceph_msg *msg,
			 struct ceph_connection *con)
{
	void *p, *end;
	struct ceph_osd_request *req;
	struct ceph_request_redirect redir;
	u64 tid;
	int object_len;
	unsigned int numops;
	int payload_len, flags;
	s32 result;
	s32 retry_attempt;
	struct ceph_pg pg;
	int err;
	u32 reassert_epoch;
	u64 reassert_version;
	u32 osdmap_epoch;
	int already_completed;
	u32 bytes;
	unsigned int i;

	tid = le64_to_cpu(msg->hdr.tid);
	dout("handle_reply %p tid %llu\n", msg, tid);

	p = msg->front.iov_base;
	end = p + msg->front.iov_len;

	ceph_decode_need(&p, end, 4, bad);
	object_len = ceph_decode_32(&p);
	ceph_decode_need(&p, end, object_len, bad);
	p += object_len;

	err = ceph_decode_pgid(&p, end, &pg);
	if (err)
		goto bad;

	ceph_decode_need(&p, end, 8 + 4 + 4 + 8 + 4, bad);
	flags = ceph_decode_64(&p);
	result = ceph_decode_32(&p);
	reassert_epoch = ceph_decode_32(&p);
	reassert_version = ceph_decode_64(&p);
	osdmap_epoch = ceph_decode_32(&p);

	/* lookup */
	down_read(&osdc->map_sem);
	mutex_lock(&osdc->request_mutex);
	req = __lookup_request(osdc, tid);
	if (req == NULL) {
		dout("handle_reply tid %llu dne\n", tid);
		goto bad_mutex;
	}
	ceph_osdc_get_request(req);

	dout("handle_reply %p tid %llu req %p result %d\n", msg, tid,
	     req, result);

	ceph_decode_need(&p, end, 4, bad_put);
	numops = ceph_decode_32(&p);
	if (numops > CEPH_OSD_MAX_OP)
		goto bad_put;
	if (numops != req->r_num_ops)
		goto bad_put;
	payload_len = 0;
	ceph_decode_need(&p, end, numops * sizeof(struct ceph_osd_op), bad_put);
	for (i = 0; i < numops; i++) {
		struct ceph_osd_op *op = p;
		int len;

		len = le32_to_cpu(op->payload_len);
		req->r_reply_op_len[i] = len;
		dout(" op %d has %d bytes\n", i, len);
		payload_len += len;
		p += sizeof(*op);
	}
	bytes = le32_to_cpu(msg->hdr.data_len);
	if (payload_len != bytes) {
		pr_warn("sum of op payload lens %d != data_len %d\n",
			payload_len, bytes);
		goto bad_put;
	}

	ceph_decode_need(&p, end, 4 + numops * 4, bad_put);
	retry_attempt = ceph_decode_32(&p);
	for (i = 0; i < numops; i++)
		req->r_reply_op_result[i] = ceph_decode_32(&p);

	if (le16_to_cpu(msg->hdr.version) >= 6) {
		p += 8 + 4; /* skip replay_version */
		p += 8; /* skip user_version */

		err = ceph_redirect_decode(&p, end, &redir);
		if (err)
			goto bad_put;
	} else {
		redir.oloc.pool = -1;
	}

	if (redir.oloc.pool != -1) {
		dout("redirect pool %lld\n", redir.oloc.pool);

		__unregister_request(osdc, req);

		req->r_target_oloc = redir.oloc; /* struct */

		/*
		 * Start redirect requests with nofail=true.  If
		 * mapping fails, request will end up on the notarget
		 * list, waiting for the new osdmap (which can take
		 * a while), even though the original request mapped
		 * successfully.  In the future we might want to follow
		 * original request's nofail setting here.
		 */
		err = __ceph_osdc_start_request(osdc, req, true);
		BUG_ON(err);

		goto out_unlock;
	}

	already_completed = req->r_got_reply;
	if (!req->r_got_reply) {
		req->r_result = result;
		dout("handle_reply result %d bytes %d\n", req->r_result,
		     bytes);
		if (req->r_result == 0)
			req->r_result = bytes;

		/* in case this is a write and we need to replay, */
		req->r_reassert_version.epoch = cpu_to_le32(reassert_epoch);
		req->r_reassert_version.version = cpu_to_le64(reassert_version);

		req->r_got_reply = 1;
	} else if ((flags & CEPH_OSD_FLAG_ONDISK) == 0) {
		dout("handle_reply tid %llu dup ack\n", tid);
		goto out_unlock;
	}

	dout("handle_reply tid %llu flags %d\n", tid, flags);

	if (req->r_linger && (flags & CEPH_OSD_FLAG_ONDISK))
		__register_linger_request(osdc, req);

	/* either this is a read, or we got the safe response */
	if (result < 0 ||
	    (flags & CEPH_OSD_FLAG_ONDISK) ||
	    ((flags & CEPH_OSD_FLAG_WRITE) == 0))
		__unregister_request(osdc, req);

	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);

	if (!already_completed) {
		if (req->r_unsafe_callback &&
		    result >= 0 && !(flags & CEPH_OSD_FLAG_ONDISK))
			req->r_unsafe_callback(req, true);
		if (req->r_callback)
			req->r_callback(req, msg);
		else
			complete_all(&req->r_completion);
	}

	if (flags & CEPH_OSD_FLAG_ONDISK) {
		if (req->r_unsafe_callback && already_completed)
			req->r_unsafe_callback(req, false);
		complete_request(req);
	}

out:
	dout("req=%p req->r_linger=%d\n", req, req->r_linger);
	ceph_osdc_put_request(req);
	return;
out_unlock:
	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);
	goto out;

bad_put:
	req->r_result = -EIO;
	__unregister_request(osdc, req);
	if (req->r_callback)
		req->r_callback(req, msg);
	else
		complete_all(&req->r_completion);
	complete_request(req);
	ceph_osdc_put_request(req);
bad_mutex:
	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);
bad:
	pr_err("corrupt osd_op_reply got %d %d\n",
	       (int)msg->front.iov_len, le32_to_cpu(msg->hdr.front_len));
	ceph_msg_dump(msg);
}

static void reset_changed_osds(struct ceph_osd_client *osdc)
{
	struct rb_node *p, *n;

	dout("%s %p\n", __func__, osdc);
	for (p = rb_first(&osdc->osds); p; p = n) {
		struct ceph_osd *osd = rb_entry(p, struct ceph_osd, o_node);

		n = rb_next(p);
		if (!ceph_osd_is_up(osdc->osdmap, osd->o_osd) ||
		    memcmp(&osd->o_con.peer_addr,
			   ceph_osd_addr(osdc->osdmap,
					 osd->o_osd),
			   sizeof(struct ceph_entity_addr)) != 0)
			__reset_osd(osdc, osd);
	}
}

/*
 * Requeue requests whose mapping to an OSD has changed.  If requests map to
 * no osd, request a new map.
 *
 * Caller should hold map_sem for read.
 */
static void kick_requests(struct ceph_osd_client *osdc, bool force_resend,
			  bool force_resend_writes)
{
	struct ceph_osd_request *req, *nreq;
	struct rb_node *p;
	int needmap = 0;
	int err;
	bool force_resend_req;

	dout("kick_requests %s %s\n", force_resend ? " (force resend)" : "",
		force_resend_writes ? " (force resend writes)" : "");
	mutex_lock(&osdc->request_mutex);
	for (p = rb_first(&osdc->requests); p; ) {
		req = rb_entry(p, struct ceph_osd_request, r_node);
		p = rb_next(p);

		/*
		 * For linger requests that have not yet been
		 * registered, move them to the linger list; they'll
		 * be sent to the osd in the loop below.  Unregister
		 * the request before re-registering it as a linger
		 * request to ensure the __map_request() below
		 * will decide it needs to be sent.
		 */
		if (req->r_linger && list_empty(&req->r_linger_item)) {
			dout("%p tid %llu restart on osd%d\n",
			     req, req->r_tid,
			     req->r_osd ? req->r_osd->o_osd : -1);
			ceph_osdc_get_request(req);
			__unregister_request(osdc, req);
			__register_linger_request(osdc, req);
			ceph_osdc_put_request(req);
			continue;
		}

		force_resend_req = force_resend ||
			(force_resend_writes &&
				req->r_flags & CEPH_OSD_FLAG_WRITE);
		err = __map_request(osdc, req, force_resend_req);
		if (err < 0)
			continue;  /* error */
		if (req->r_osd == NULL) {
			dout("%p tid %llu maps to no osd\n", req, req->r_tid);
			needmap++;  /* request a newer map */
		} else if (err > 0) {
			if (!req->r_linger) {
				dout("%p tid %llu requeued on osd%d\n", req,
				     req->r_tid,
				     req->r_osd ? req->r_osd->o_osd : -1);
				req->r_flags |= CEPH_OSD_FLAG_RETRY;
			}
		}
	}

	list_for_each_entry_safe(req, nreq, &osdc->req_linger,
				 r_linger_item) {
		dout("linger req=%p req->r_osd=%p\n", req, req->r_osd);

		err = __map_request(osdc, req,
				    force_resend || force_resend_writes);
		dout("__map_request returned %d\n", err);
		if (err < 0)
			continue;  /* hrm! */
		if (req->r_osd == NULL || err > 0) {
			if (req->r_osd == NULL) {
				dout("lingering %p tid %llu maps to no osd\n",
				     req, req->r_tid);
				/*
				 * A homeless lingering request makes
				 * no sense, as it's job is to keep
				 * a particular OSD connection open.
				 * Request a newer map and kick the
				 * request, knowing that it won't be
				 * resent until we actually get a map
				 * that can tell us where to send it.
				 */
				needmap++;
			}

			dout("kicking lingering %p tid %llu osd%d\n", req,
			     req->r_tid, req->r_osd ? req->r_osd->o_osd : -1);
			__register_request(osdc, req);
			__unregister_linger_request(osdc, req);
		}
	}
	reset_changed_osds(osdc);
	mutex_unlock(&osdc->request_mutex);

	if (needmap) {
		dout("%d requests for down osds, need new map\n", needmap);
		ceph_monc_request_next_osdmap(&osdc->client->monc);
	}
}

=======
 *
 * ->r_unsafe_callback is set?	yes			no
 *
 * first reply is OK (needed	r_cb/r_completion,	r_cb/r_completion,
 * any or needed/got safe)	r_safe_completion	r_safe_completion
 *
 * first reply is unsafe	r_unsafe_cb(true)	(nothing)
 *
 * when we get the safe reply	r_unsafe_cb(false),	r_cb/r_completion,
 *				r_safe_completion	r_safe_completion
 */
static void handle_reply(struct ceph_osd *osd, struct ceph_msg *msg)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct ceph_osd_request *req;
	struct MOSDOpReply m;
	u64 tid = le64_to_cpu(msg->hdr.tid);
	u32 data_len = 0;
	bool already_acked;
	int ret;
	int i;

	dout("%s msg %p tid %llu\n", __func__, msg, tid);

	down_read(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown\n", __func__, osd->o_osd);
		goto out_unlock_osdc;
	}
	WARN_ON(osd->o_osd != le64_to_cpu(msg->hdr.src.num));

	mutex_lock(&osd->lock);
	req = lookup_request(&osd->o_requests, tid);
	if (!req) {
		dout("%s osd%d tid %llu unknown\n", __func__, osd->o_osd, tid);
		goto out_unlock_session;
	}

	m.redirect.oloc.pool_ns = req->r_t.target_oloc.pool_ns;
	ret = decode_MOSDOpReply(msg, &m);
	m.redirect.oloc.pool_ns = NULL;
	if (ret) {
		pr_err("failed to decode MOSDOpReply for tid %llu: %d\n",
		       req->r_tid, ret);
		ceph_msg_dump(msg);
		goto fail_request;
	}
	dout("%s req %p tid %llu flags 0x%llx pgid %llu.%x epoch %u attempt %d v %u'%llu uv %llu\n",
	     __func__, req, req->r_tid, m.flags, m.pgid.pool, m.pgid.seed,
	     m.epoch, m.retry_attempt, le32_to_cpu(m.replay_version.epoch),
	     le64_to_cpu(m.replay_version.version), m.user_version);

	if (m.retry_attempt >= 0) {
		if (m.retry_attempt != req->r_attempts - 1) {
			dout("req %p tid %llu retry_attempt %d != %d, ignoring\n",
			     req, req->r_tid, m.retry_attempt,
			     req->r_attempts - 1);
			goto out_unlock_session;
		}
	} else {
		WARN_ON(1); /* MOSDOpReply v4 is assumed */
	}

	if (!ceph_oloc_empty(&m.redirect.oloc)) {
		dout("req %p tid %llu redirect pool %lld\n", req, req->r_tid,
		     m.redirect.oloc.pool);
		unlink_request(osd, req);
		mutex_unlock(&osd->lock);

		/*
		 * Not ceph_oloc_copy() - changing pool_ns is not
		 * supported.
		 */
		req->r_t.target_oloc.pool = m.redirect.oloc.pool;
		req->r_flags |= CEPH_OSD_FLAG_REDIRECTED |
				CEPH_OSD_FLAG_IGNORE_OVERLAY |
				CEPH_OSD_FLAG_IGNORE_CACHE;
		req->r_tid = 0;
		__submit_request(req, false);
		goto out_unlock_osdc;
	}

	if (m.num_ops != req->r_num_ops) {
		pr_err("num_ops %d != %d for tid %llu\n", m.num_ops,
		       req->r_num_ops, req->r_tid);
		goto fail_request;
	}
	for (i = 0; i < req->r_num_ops; i++) {
		dout(" req %p tid %llu op %d rval %d len %u\n", req,
		     req->r_tid, i, m.rval[i], m.outdata_len[i]);
		req->r_ops[i].rval = m.rval[i];
		req->r_ops[i].outdata_len = m.outdata_len[i];
		data_len += m.outdata_len[i];
	}
	if (data_len != le32_to_cpu(msg->hdr.data_len)) {
		pr_err("sum of lens %u != %u for tid %llu\n", data_len,
		       le32_to_cpu(msg->hdr.data_len), req->r_tid);
		goto fail_request;
	}
	dout("%s req %p tid %llu acked %d result %d data_len %u\n", __func__,
	     req, req->r_tid, req->r_got_reply, m.result, data_len);

	already_acked = req->r_got_reply;
	if (!already_acked) {
		req->r_result = m.result ?: data_len;
		req->r_replay_version = m.replay_version; /* struct */
		req->r_got_reply = true;
	} else if (!(m.flags & CEPH_OSD_FLAG_ONDISK)) {
		dout("req %p tid %llu dup ack\n", req, req->r_tid);
		goto out_unlock_session;
	}

	if (done_request(req, &m)) {
		__finish_request(req);
		if (req->r_linger) {
			WARN_ON(req->r_unsafe_callback);
			dout("req %p tid %llu cb (locked)\n", req, req->r_tid);
			__complete_request(req);
		}
	}

	mutex_unlock(&osd->lock);
	up_read(&osdc->lock);

	if (done_request(req, &m)) {
		if (already_acked && req->r_unsafe_callback) {
			dout("req %p tid %llu safe-cb\n", req, req->r_tid);
			req->r_unsafe_callback(req, false);
		} else if (!req->r_linger) {
			dout("req %p tid %llu cb\n", req, req->r_tid);
			__complete_request(req);
		}
		if (m.flags & CEPH_OSD_FLAG_ONDISK)
			complete_all(&req->r_safe_completion);
		ceph_osdc_put_request(req);
	} else {
		if (req->r_unsafe_callback) {
			dout("req %p tid %llu unsafe-cb\n", req, req->r_tid);
			req->r_unsafe_callback(req, true);
		} else {
			WARN_ON(1);
		}
	}

	return;

fail_request:
	complete_request(req, -EIO);
out_unlock_session:
	mutex_unlock(&osd->lock);
out_unlock_osdc:
	up_read(&osdc->lock);
}

static void set_pool_was_full(struct ceph_osd_client *osdc)
{
	struct rb_node *n;

	for (n = rb_first(&osdc->osdmap->pg_pools); n; n = rb_next(n)) {
		struct ceph_pg_pool_info *pi =
		    rb_entry(n, struct ceph_pg_pool_info, node);

		pi->was_full = __pool_full(pi);
	}
}

static bool pool_cleared_full(struct ceph_osd_client *osdc, s64 pool_id)
{
	struct ceph_pg_pool_info *pi;

	pi = ceph_pg_pool_by_id(osdc->osdmap, pool_id);
	if (!pi)
		return false;

	return pi->was_full && !__pool_full(pi);
}

static enum calc_target_result
recalc_linger_target(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	enum calc_target_result ct_res;

	ct_res = calc_target(osdc, &lreq->t, &lreq->last_force_resend, true);
	if (ct_res == CALC_TARGET_NEED_RESEND) {
		struct ceph_osd *osd;

		osd = lookup_create_osd(osdc, lreq->t.osd, true);
		if (osd != lreq->osd) {
			unlink_linger(lreq->osd, lreq);
			link_linger(osd, lreq);
		}
	}

	return ct_res;
}

/*
 * Requeue requests whose mapping to an OSD has changed.
 */
static void scan_requests(struct ceph_osd *osd,
			  bool force_resend,
			  bool cleared_full,
			  bool check_pool_cleared_full,
			  struct rb_root *need_resend,
			  struct list_head *need_resend_linger)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct rb_node *n;
	bool force_resend_writes;

	for (n = rb_first(&osd->o_linger_requests); n; ) {
		struct ceph_osd_linger_request *lreq =
		    rb_entry(n, struct ceph_osd_linger_request, node);
		enum calc_target_result ct_res;

		n = rb_next(n); /* recalc_linger_target() */

		dout("%s lreq %p linger_id %llu\n", __func__, lreq,
		     lreq->linger_id);
		ct_res = recalc_linger_target(lreq);
		switch (ct_res) {
		case CALC_TARGET_NO_ACTION:
			force_resend_writes = cleared_full ||
			    (check_pool_cleared_full &&
			     pool_cleared_full(osdc, lreq->t.base_oloc.pool));
			if (!force_resend && !force_resend_writes)
				break;

			/* fall through */
		case CALC_TARGET_NEED_RESEND:
			cancel_linger_map_check(lreq);
			/*
			 * scan_requests() for the previous epoch(s)
			 * may have already added it to the list, since
			 * it's not unlinked here.
			 */
			if (list_empty(&lreq->scan_item))
				list_add_tail(&lreq->scan_item, need_resend_linger);
			break;
		case CALC_TARGET_POOL_DNE:
			check_linger_pool_dne(lreq);
			break;
		}
	}

	for (n = rb_first(&osd->o_requests); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);
		enum calc_target_result ct_res;

		n = rb_next(n); /* unlink_request(), check_pool_dne() */

		dout("%s req %p tid %llu\n", __func__, req, req->r_tid);
		ct_res = calc_target(osdc, &req->r_t,
				     &req->r_last_force_resend, false);
		switch (ct_res) {
		case CALC_TARGET_NO_ACTION:
			force_resend_writes = cleared_full ||
			    (check_pool_cleared_full &&
			     pool_cleared_full(osdc, req->r_t.base_oloc.pool));
			if (!force_resend &&
			    (!(req->r_flags & CEPH_OSD_FLAG_WRITE) ||
			     !force_resend_writes))
				break;

			/* fall through */
		case CALC_TARGET_NEED_RESEND:
			cancel_map_check(req);
			unlink_request(osd, req);
			insert_request(need_resend, req);
			break;
		case CALC_TARGET_POOL_DNE:
			check_pool_dne(req);
			break;
		}
	}
}

static int handle_one_map(struct ceph_osd_client *osdc,
			  void *p, void *end, bool incremental,
			  struct rb_root *need_resend,
			  struct list_head *need_resend_linger)
{
	struct ceph_osdmap *newmap;
	struct rb_node *n;
	bool skipped_map = false;
	bool was_full;

	was_full = ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL);
	set_pool_was_full(osdc);

	if (incremental)
		newmap = osdmap_apply_incremental(&p, end, osdc->osdmap);
	else
		newmap = ceph_osdmap_decode(&p, end);
	if (IS_ERR(newmap))
		return PTR_ERR(newmap);

	if (newmap != osdc->osdmap) {
		/*
		 * Preserve ->was_full before destroying the old map.
		 * For pools that weren't in the old map, ->was_full
		 * should be false.
		 */
		for (n = rb_first(&newmap->pg_pools); n; n = rb_next(n)) {
			struct ceph_pg_pool_info *pi =
			    rb_entry(n, struct ceph_pg_pool_info, node);
			struct ceph_pg_pool_info *old_pi;

			old_pi = ceph_pg_pool_by_id(osdc->osdmap, pi->id);
			if (old_pi)
				pi->was_full = old_pi->was_full;
			else
				WARN_ON(pi->was_full);
		}

		if (osdc->osdmap->epoch &&
		    osdc->osdmap->epoch + 1 < newmap->epoch) {
			WARN_ON(incremental);
			skipped_map = true;
		}

		ceph_osdmap_destroy(osdc->osdmap);
		osdc->osdmap = newmap;
	}

	was_full &= !ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL);
	scan_requests(&osdc->homeless_osd, skipped_map, was_full, true,
		      need_resend, need_resend_linger);

	for (n = rb_first(&osdc->osds); n; ) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);

		n = rb_next(n); /* close_osd() */

		scan_requests(osd, skipped_map, was_full, true, need_resend,
			      need_resend_linger);
		if (!ceph_osd_is_up(osdc->osdmap, osd->o_osd) ||
		    memcmp(&osd->o_con.peer_addr,
			   ceph_osd_addr(osdc->osdmap, osd->o_osd),
			   sizeof(struct ceph_entity_addr)))
			close_osd(osd);
	}

	return 0;
}

static void kick_requests(struct ceph_osd_client *osdc,
			  struct rb_root *need_resend,
			  struct list_head *need_resend_linger)
{
	struct ceph_osd_linger_request *lreq, *nlreq;
	struct rb_node *n;

	for (n = rb_first(need_resend); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);
		struct ceph_osd *osd;

		n = rb_next(n);
		erase_request(need_resend, req); /* before link_request() */

		WARN_ON(req->r_osd);
		calc_target(osdc, &req->r_t, NULL, false);
		osd = lookup_create_osd(osdc, req->r_t.osd, true);
		link_request(osd, req);
		if (!req->r_linger) {
			if (!osd_homeless(osd) && !req->r_t.paused)
				send_request(req);
		} else {
			cancel_linger_request(req);
		}
	}

	list_for_each_entry_safe(lreq, nlreq, need_resend_linger, scan_item) {
		if (!osd_homeless(lreq->osd))
			send_linger(lreq);

		list_del_init(&lreq->scan_item);
	}
}
>>>>>>> v4.9.227

/*
 * Process updated osd map.
 *
 * The message contains any number of incremental and full maps, normally
 * indicating some sort of topology change in the cluster.  Kick requests
 * off to different OSDs as needed.
 */
void ceph_osdc_handle_map(struct ceph_osd_client *osdc, struct ceph_msg *msg)
{
<<<<<<< HEAD
	void *p, *end, *next;
	u32 nr_maps, maplen;
	u32 epoch;
	struct ceph_osdmap *newmap = NULL, *oldmap;
	int err;
	struct ceph_fsid fsid;
	bool was_full;

	dout("handle_map have %u\n", osdc->osdmap ? osdc->osdmap->epoch : 0);
	p = msg->front.iov_base;
	end = p + msg->front.iov_len;
=======
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	u32 nr_maps, maplen;
	u32 epoch;
	struct ceph_fsid fsid;
	struct rb_root need_resend = RB_ROOT;
	LIST_HEAD(need_resend_linger);
	bool handled_incremental = false;
	bool was_pauserd, was_pausewr;
	bool pauserd, pausewr;
	int err;

	dout("%s have %u\n", __func__, osdc->osdmap->epoch);
	down_write(&osdc->lock);
>>>>>>> v4.9.227

	/* verify fsid */
	ceph_decode_need(&p, end, sizeof(fsid), bad);
	ceph_decode_copy(&p, &fsid, sizeof(fsid));
	if (ceph_check_fsid(osdc->client, &fsid) < 0)
<<<<<<< HEAD
		return;

	down_write(&osdc->map_sem);

	was_full = ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_FULL);
=======
		goto bad;

	was_pauserd = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD);
	was_pausewr = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR) ||
		      ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		      have_pool_full(osdc);
>>>>>>> v4.9.227

	/* incremental maps */
	ceph_decode_32_safe(&p, end, nr_maps, bad);
	dout(" %d inc maps\n", nr_maps);
	while (nr_maps > 0) {
		ceph_decode_need(&p, end, 2*sizeof(u32), bad);
		epoch = ceph_decode_32(&p);
		maplen = ceph_decode_32(&p);
		ceph_decode_need(&p, end, maplen, bad);
<<<<<<< HEAD
		next = p + maplen;
		if (osdc->osdmap && osdc->osdmap->epoch+1 == epoch) {
			dout("applying incremental map %u len %d\n",
			     epoch, maplen);
			newmap = osdmap_apply_incremental(&p, next,
							  osdc->osdmap,
							  &osdc->client->msgr);
			if (IS_ERR(newmap)) {
				err = PTR_ERR(newmap);
				goto bad;
			}
			BUG_ON(!newmap);
			if (newmap != osdc->osdmap) {
				ceph_osdmap_destroy(osdc->osdmap);
				osdc->osdmap = newmap;
			}
			was_full = was_full ||
				ceph_osdmap_flag(osdc->osdmap,
						 CEPH_OSDMAP_FULL);
			kick_requests(osdc, 0, was_full);
=======
		if (osdc->osdmap->epoch &&
		    osdc->osdmap->epoch + 1 == epoch) {
			dout("applying incremental map %u len %d\n",
			     epoch, maplen);
			err = handle_one_map(osdc, p, p + maplen, true,
					     &need_resend, &need_resend_linger);
			if (err)
				goto bad;
			handled_incremental = true;
>>>>>>> v4.9.227
		} else {
			dout("ignoring incremental map %u len %d\n",
			     epoch, maplen);
		}
<<<<<<< HEAD
		p = next;
		nr_maps--;
	}
	if (newmap)
=======
		p += maplen;
		nr_maps--;
	}
	if (handled_incremental)
>>>>>>> v4.9.227
		goto done;

	/* full maps */
	ceph_decode_32_safe(&p, end, nr_maps, bad);
	dout(" %d full maps\n", nr_maps);
	while (nr_maps) {
		ceph_decode_need(&p, end, 2*sizeof(u32), bad);
		epoch = ceph_decode_32(&p);
		maplen = ceph_decode_32(&p);
		ceph_decode_need(&p, end, maplen, bad);
		if (nr_maps > 1) {
			dout("skipping non-latest full map %u len %d\n",
			     epoch, maplen);
<<<<<<< HEAD
		} else if (osdc->osdmap && osdc->osdmap->epoch >= epoch) {
=======
		} else if (osdc->osdmap->epoch >= epoch) {
>>>>>>> v4.9.227
			dout("skipping full map %u len %d, "
			     "older than our %u\n", epoch, maplen,
			     osdc->osdmap->epoch);
		} else {
<<<<<<< HEAD
			int skipped_map = 0;

			dout("taking full map %u len %d\n", epoch, maplen);
			newmap = ceph_osdmap_decode(&p, p+maplen);
			if (IS_ERR(newmap)) {
				err = PTR_ERR(newmap);
				goto bad;
			}
			BUG_ON(!newmap);
			oldmap = osdc->osdmap;
			osdc->osdmap = newmap;
			if (oldmap) {
				if (oldmap->epoch + 1 < newmap->epoch)
					skipped_map = 1;
				ceph_osdmap_destroy(oldmap);
			}
			was_full = was_full ||
				ceph_osdmap_flag(osdc->osdmap,
						 CEPH_OSDMAP_FULL);
			kick_requests(osdc, skipped_map, was_full);
=======
			dout("taking full map %u len %d\n", epoch, maplen);
			err = handle_one_map(osdc, p, p + maplen, false,
					     &need_resend, &need_resend_linger);
			if (err)
				goto bad;
>>>>>>> v4.9.227
		}
		p += maplen;
		nr_maps--;
	}

<<<<<<< HEAD
	if (!osdc->osdmap)
		goto bad;
done:
	downgrade_write(&osdc->map_sem);
	ceph_monc_got_osdmap(&osdc->client->monc, osdc->osdmap->epoch);

=======
done:
>>>>>>> v4.9.227
	/*
	 * subscribe to subsequent osdmap updates if full to ensure
	 * we find out when we are no longer full and stop returning
	 * ENOSPC.
	 */
<<<<<<< HEAD
	if (ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_FULL) ||
		ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_PAUSERD) ||
		ceph_osdmap_flag(osdc->osdmap, CEPH_OSDMAP_PAUSEWR))
		ceph_monc_request_next_osdmap(&osdc->client->monc);

	mutex_lock(&osdc->request_mutex);
	__send_queued(osdc);
	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);
=======
	pauserd = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD);
	pausewr = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR) ||
		  ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		  have_pool_full(osdc);
	if (was_pauserd || was_pausewr || pauserd || pausewr)
		maybe_request_map(osdc);

	kick_requests(osdc, &need_resend, &need_resend_linger);

	ceph_monc_got_map(&osdc->client->monc, CEPH_SUB_OSDMAP,
			  osdc->osdmap->epoch);
	up_write(&osdc->lock);
>>>>>>> v4.9.227
	wake_up_all(&osdc->client->auth_wq);
	return;

bad:
	pr_err("osdc handle_map corrupt msg\n");
	ceph_msg_dump(msg);
<<<<<<< HEAD
	up_write(&osdc->map_sem);
}

/*
 * watch/notify callback event infrastructure
 *
 * These callbacks are used both for watch and notify operations.
 */
static void __release_event(struct kref *kref)
{
	struct ceph_osd_event *event =
		container_of(kref, struct ceph_osd_event, kref);

	dout("__release_event %p\n", event);
	kfree(event);
}

static void get_event(struct ceph_osd_event *event)
{
	kref_get(&event->kref);
}

void ceph_osdc_put_event(struct ceph_osd_event *event)
{
	kref_put(&event->kref, __release_event);
}
EXPORT_SYMBOL(ceph_osdc_put_event);

static void __insert_event(struct ceph_osd_client *osdc,
			     struct ceph_osd_event *new)
{
	struct rb_node **p = &osdc->event_tree.rb_node;
	struct rb_node *parent = NULL;
	struct ceph_osd_event *event = NULL;

	while (*p) {
		parent = *p;
		event = rb_entry(parent, struct ceph_osd_event, node);
		if (new->cookie < event->cookie)
			p = &(*p)->rb_left;
		else if (new->cookie > event->cookie)
			p = &(*p)->rb_right;
		else
			BUG();
	}

	rb_link_node(&new->node, parent, p);
	rb_insert_color(&new->node, &osdc->event_tree);
}

static struct ceph_osd_event *__find_event(struct ceph_osd_client *osdc,
					        u64 cookie)
{
	struct rb_node **p = &osdc->event_tree.rb_node;
	struct rb_node *parent = NULL;
	struct ceph_osd_event *event = NULL;

	while (*p) {
		parent = *p;
		event = rb_entry(parent, struct ceph_osd_event, node);
		if (cookie < event->cookie)
			p = &(*p)->rb_left;
		else if (cookie > event->cookie)
			p = &(*p)->rb_right;
		else
			return event;
	}
	return NULL;
}

static void __remove_event(struct ceph_osd_event *event)
{
	struct ceph_osd_client *osdc = event->osdc;

	if (!RB_EMPTY_NODE(&event->node)) {
		dout("__remove_event removed %p\n", event);
		rb_erase(&event->node, &osdc->event_tree);
		ceph_osdc_put_event(event);
	} else {
		dout("__remove_event didn't remove %p\n", event);
	}
}

int ceph_osdc_create_event(struct ceph_osd_client *osdc,
			   void (*event_cb)(u64, u64, u8, void *),
			   void *data, struct ceph_osd_event **pevent)
{
	struct ceph_osd_event *event;

	event = kmalloc(sizeof(*event), GFP_NOIO);
	if (!event)
		return -ENOMEM;

	dout("create_event %p\n", event);
	event->cb = event_cb;
	event->one_shot = 0;
	event->data = data;
	event->osdc = osdc;
	INIT_LIST_HEAD(&event->osd_node);
	RB_CLEAR_NODE(&event->node);
	kref_init(&event->kref);   /* one ref for us */
	kref_get(&event->kref);    /* one ref for the caller */

	spin_lock(&osdc->event_lock);
	event->cookie = ++osdc->event_count;
	__insert_event(osdc, event);
	spin_unlock(&osdc->event_lock);

	*pevent = event;
	return 0;
}
EXPORT_SYMBOL(ceph_osdc_create_event);

void ceph_osdc_cancel_event(struct ceph_osd_event *event)
{
	struct ceph_osd_client *osdc = event->osdc;

	dout("cancel_event %p\n", event);
	spin_lock(&osdc->event_lock);
	__remove_event(event);
	spin_unlock(&osdc->event_lock);
	ceph_osdc_put_event(event); /* caller's */
}
EXPORT_SYMBOL(ceph_osdc_cancel_event);


static void do_event_work(struct work_struct *work)
{
	struct ceph_osd_event_work *event_work =
		container_of(work, struct ceph_osd_event_work, work);
	struct ceph_osd_event *event = event_work->event;
	u64 ver = event_work->ver;
	u64 notify_id = event_work->notify_id;
	u8 opcode = event_work->opcode;

	dout("do_event_work completing %p\n", event);
	event->cb(ver, notify_id, opcode, event->data);
	dout("do_event_work completed %p\n", event);
	ceph_osdc_put_event(event);
	kfree(event_work);
}

=======
	up_write(&osdc->lock);
}

/*
 * Resubmit requests pending on the given osd.
 */
static void kick_osd_requests(struct ceph_osd *osd)
{
	struct rb_node *n;

	for (n = rb_first(&osd->o_requests); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);

		n = rb_next(n); /* cancel_linger_request() */

		if (!req->r_linger) {
			if (!req->r_t.paused)
				send_request(req);
		} else {
			cancel_linger_request(req);
		}
	}
	for (n = rb_first(&osd->o_linger_requests); n; n = rb_next(n)) {
		struct ceph_osd_linger_request *lreq =
		    rb_entry(n, struct ceph_osd_linger_request, node);

		send_linger(lreq);
	}
}

/*
 * If the osd connection drops, we need to resubmit all requests.
 */
static void osd_fault(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	struct ceph_osd_client *osdc = osd->o_osdc;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	down_write(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown\n", __func__, osd->o_osd);
		goto out_unlock;
	}

	if (!reopen_osd(osd))
		kick_osd_requests(osd);
	maybe_request_map(osdc);

out_unlock:
	up_write(&osdc->lock);
}
>>>>>>> v4.9.227

/*
 * Process osd watch notifications
 */
static void handle_watch_notify(struct ceph_osd_client *osdc,
				struct ceph_msg *msg)
{
<<<<<<< HEAD
	void *p, *end;
	u8 proto_ver;
	u64 cookie, ver, notify_id;
	u8 opcode;
	struct ceph_osd_event *event;
	struct ceph_osd_event_work *event_work;

	p = msg->front.iov_base;
	end = p + msg->front.iov_len;
=======
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	struct ceph_osd_linger_request *lreq;
	struct linger_work *lwork;
	u8 proto_ver, opcode;
	u64 cookie, notify_id;
	u64 notifier_id = 0;
	s32 return_code = 0;
	void *payload = NULL;
	u32 payload_len = 0;
>>>>>>> v4.9.227

	ceph_decode_8_safe(&p, end, proto_ver, bad);
	ceph_decode_8_safe(&p, end, opcode, bad);
	ceph_decode_64_safe(&p, end, cookie, bad);
<<<<<<< HEAD
	ceph_decode_64_safe(&p, end, ver, bad);
	ceph_decode_64_safe(&p, end, notify_id, bad);

	spin_lock(&osdc->event_lock);
	event = __find_event(osdc, cookie);
	if (event) {
		BUG_ON(event->one_shot);
		get_event(event);
	}
	spin_unlock(&osdc->event_lock);
	dout("handle_watch_notify cookie %lld ver %lld event %p\n",
	     cookie, ver, event);
	if (event) {
		event_work = kmalloc(sizeof(*event_work), GFP_NOIO);
		if (!event_work) {
			pr_err("couldn't allocate event_work\n");
			ceph_osdc_put_event(event);
			return;
		}
		INIT_WORK(&event_work->work, do_event_work);
		event_work->event = event;
		event_work->ver = ver;
		event_work->notify_id = notify_id;
		event_work->opcode = opcode;

		queue_work(osdc->notify_wq, &event_work->work);
	}

=======
	p += 8; /* skip ver */
	ceph_decode_64_safe(&p, end, notify_id, bad);

	if (proto_ver >= 1) {
		ceph_decode_32_safe(&p, end, payload_len, bad);
		ceph_decode_need(&p, end, payload_len, bad);
		payload = p;
		p += payload_len;
	}

	if (le16_to_cpu(msg->hdr.version) >= 2)
		ceph_decode_32_safe(&p, end, return_code, bad);

	if (le16_to_cpu(msg->hdr.version) >= 3)
		ceph_decode_64_safe(&p, end, notifier_id, bad);

	down_read(&osdc->lock);
	lreq = lookup_linger_osdc(&osdc->linger_requests, cookie);
	if (!lreq) {
		dout("%s opcode %d cookie %llu dne\n", __func__, opcode,
		     cookie);
		goto out_unlock_osdc;
	}

	mutex_lock(&lreq->lock);
	dout("%s opcode %d cookie %llu lreq %p is_watch %d\n", __func__,
	     opcode, cookie, lreq, lreq->is_watch);
	if (opcode == CEPH_WATCH_EVENT_DISCONNECT) {
		if (!lreq->last_error) {
			lreq->last_error = -ENOTCONN;
			queue_watch_error(lreq);
		}
	} else if (!lreq->is_watch) {
		/* CEPH_WATCH_EVENT_NOTIFY_COMPLETE */
		if (lreq->notify_id && lreq->notify_id != notify_id) {
			dout("lreq %p notify_id %llu != %llu, ignoring\n", lreq,
			     lreq->notify_id, notify_id);
		} else if (!completion_done(&lreq->notify_finish_wait)) {
			struct ceph_msg_data *data =
			    list_first_entry_or_null(&msg->data,
						     struct ceph_msg_data,
						     links);

			if (data) {
				if (lreq->preply_pages) {
					WARN_ON(data->type !=
							CEPH_MSG_DATA_PAGES);
					*lreq->preply_pages = data->pages;
					*lreq->preply_len = data->length;
				} else {
					ceph_release_page_vector(data->pages,
					       calc_pages_for(0, data->length));
				}
			}
			lreq->notify_finish_error = return_code;
			complete_all(&lreq->notify_finish_wait);
		}
	} else {
		/* CEPH_WATCH_EVENT_NOTIFY */
		lwork = lwork_alloc(lreq, do_watch_notify);
		if (!lwork) {
			pr_err("failed to allocate notify-lwork\n");
			goto out_unlock_lreq;
		}

		lwork->notify.notify_id = notify_id;
		lwork->notify.notifier_id = notifier_id;
		lwork->notify.payload = payload;
		lwork->notify.payload_len = payload_len;
		lwork->notify.msg = ceph_msg_get(msg);
		lwork_queue(lwork);
	}

out_unlock_lreq:
	mutex_unlock(&lreq->lock);
out_unlock_osdc:
	up_read(&osdc->lock);
>>>>>>> v4.9.227
	return;

bad:
	pr_err("osdc handle_watch_notify corrupt msg\n");
}

/*
<<<<<<< HEAD
 * build new request AND message
 *
 */
void ceph_osdc_build_request(struct ceph_osd_request *req, u64 off,
				struct ceph_snap_context *snapc, u64 snap_id,
				struct timespec *mtime)
{
	struct ceph_msg *msg = req->r_request;
	void *p;
	size_t msg_size;
	int flags = req->r_flags;
	u64 data_len;
	unsigned int i;

	req->r_snapid = snap_id;
	req->r_snapc = ceph_get_snap_context(snapc);

	/* encode request */
	msg->hdr.version = cpu_to_le16(4);

	p = msg->front.iov_base;
	ceph_encode_32(&p, 1);   /* client_inc  is always 1 */
	req->r_request_osdmap_epoch = p;
	p += 4;
	req->r_request_flags = p;
	p += 4;
	if (req->r_flags & CEPH_OSD_FLAG_WRITE)
		ceph_encode_timespec(p, mtime);
	p += sizeof(struct ceph_timespec);
	req->r_request_reassert_version = p;
	p += sizeof(struct ceph_eversion); /* will get filled in */

	/* oloc */
	ceph_encode_8(&p, 4);
	ceph_encode_8(&p, 4);
	ceph_encode_32(&p, 8 + 4 + 4);
	req->r_request_pool = p;
	p += 8;
	ceph_encode_32(&p, -1);  /* preferred */
	ceph_encode_32(&p, 0);   /* key len */

	ceph_encode_8(&p, 1);
	req->r_request_pgid = p;
	p += 8 + 4;
	ceph_encode_32(&p, -1);  /* preferred */

	/* oid */
	ceph_encode_32(&p, req->r_base_oid.name_len);
	memcpy(p, req->r_base_oid.name, req->r_base_oid.name_len);
	dout("oid '%.*s' len %d\n", req->r_base_oid.name_len,
	     req->r_base_oid.name, req->r_base_oid.name_len);
	p += req->r_base_oid.name_len;

	/* ops--can imply data */
	ceph_encode_16(&p, (u16)req->r_num_ops);
	data_len = 0;
	for (i = 0; i < req->r_num_ops; i++) {
		data_len += osd_req_encode_op(req, p, i);
		p += sizeof(struct ceph_osd_op);
	}

	/* snaps */
	ceph_encode_64(&p, req->r_snapid);
	ceph_encode_64(&p, req->r_snapc ? req->r_snapc->seq : 0);
	ceph_encode_32(&p, req->r_snapc ? req->r_snapc->num_snaps : 0);
	if (req->r_snapc) {
		for (i = 0; i < snapc->num_snaps; i++) {
			ceph_encode_64(&p, req->r_snapc->snaps[i]);
		}
	}

	req->r_request_attempts = p;
	p += 4;

	/* data */
	if (flags & CEPH_OSD_FLAG_WRITE) {
		u16 data_off;

		/*
		 * The header "data_off" is a hint to the receiver
		 * allowing it to align received data into its
		 * buffers such that there's no need to re-copy
		 * it before writing it to disk (direct I/O).
		 */
		data_off = (u16) (off & 0xffff);
		req->r_request->hdr.data_off = cpu_to_le16(data_off);
	}
	req->r_request->hdr.data_len = cpu_to_le32(data_len);

	BUG_ON(p > msg->front.iov_base + msg->front.iov_len);
	msg_size = p - msg->front.iov_base;
	msg->front.iov_len = msg_size;
	msg->hdr.front_len = cpu_to_le32(msg_size);

	dout("build_request msg_size was %d\n", (int)msg_size);
}
EXPORT_SYMBOL(ceph_osdc_build_request);

/*
=======
>>>>>>> v4.9.227
 * Register request, send initial attempt.
 */
int ceph_osdc_start_request(struct ceph_osd_client *osdc,
			    struct ceph_osd_request *req,
			    bool nofail)
{
<<<<<<< HEAD
	int rc;

	down_read(&osdc->map_sem);
	mutex_lock(&osdc->request_mutex);

	rc = __ceph_osdc_start_request(osdc, req, nofail);

	mutex_unlock(&osdc->request_mutex);
	up_read(&osdc->map_sem);

	return rc;
=======
	down_read(&osdc->lock);
	submit_request(req, false);
	up_read(&osdc->lock);

	return 0;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(ceph_osdc_start_request);

/*
 * Unregister a registered request.  The request is not completed (i.e.
 * no callbacks or wakeups) - higher layers are supposed to know what
 * they are canceling.
 */
void ceph_osdc_cancel_request(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;

<<<<<<< HEAD
	mutex_lock(&osdc->request_mutex);
	if (req->r_linger)
		__unregister_linger_request(osdc, req);
	__unregister_request(osdc, req);
	mutex_unlock(&osdc->request_mutex);

	dout("%s %p tid %llu canceled\n", __func__, req, req->r_tid);
=======
	down_write(&osdc->lock);
	if (req->r_osd)
		cancel_request(req);
	up_write(&osdc->lock);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(ceph_osdc_cancel_request);

/*
<<<<<<< HEAD
=======
 * @timeout: in jiffies, 0 means "wait forever"
 */
static int wait_request_timeout(struct ceph_osd_request *req,
				unsigned long timeout)
{
	long left;

	dout("%s req %p tid %llu\n", __func__, req, req->r_tid);
	left = wait_for_completion_killable_timeout(&req->r_completion,
						ceph_timeout_jiffies(timeout));
	if (left <= 0) {
		left = left ?: -ETIMEDOUT;
		ceph_osdc_cancel_request(req);

		/* kludge - need to to wake ceph_osdc_sync() */
		complete_all(&req->r_safe_completion);
	} else {
		left = req->r_result; /* completed */
	}

	return left;
}

/*
>>>>>>> v4.9.227
 * wait for a request to complete
 */
int ceph_osdc_wait_request(struct ceph_osd_client *osdc,
			   struct ceph_osd_request *req)
{
<<<<<<< HEAD
	int rc;

	dout("%s %p tid %llu\n", __func__, req, req->r_tid);

	rc = wait_for_completion_interruptible(&req->r_completion);
	if (rc < 0) {
		dout("%s %p tid %llu interrupted\n", __func__, req, req->r_tid);
		ceph_osdc_cancel_request(req);
		complete_request(req);
		return rc;
	}

	dout("%s %p tid %llu result %d\n", __func__, req, req->r_tid,
	     req->r_result);
	return req->r_result;
=======
	return wait_request_timeout(req, 0);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(ceph_osdc_wait_request);

/*
 * sync - wait for all in-flight requests to flush.  avoid starvation.
 */
void ceph_osdc_sync(struct ceph_osd_client *osdc)
{
<<<<<<< HEAD
	struct ceph_osd_request *req;
	u64 last_tid, next_tid = 0;

	mutex_lock(&osdc->request_mutex);
	last_tid = osdc->last_tid;
	while (1) {
		req = __lookup_request_ge(osdc, next_tid);
		if (!req)
			break;
		if (req->r_tid > last_tid)
			break;

		next_tid = req->r_tid + 1;
		if ((req->r_flags & CEPH_OSD_FLAG_WRITE) == 0)
			continue;

		ceph_osdc_get_request(req);
		mutex_unlock(&osdc->request_mutex);
		dout("sync waiting on tid %llu (last is %llu)\n",
		     req->r_tid, last_tid);
		wait_for_completion(&req->r_safe_completion);
		mutex_lock(&osdc->request_mutex);
		ceph_osdc_put_request(req);
	}
	mutex_unlock(&osdc->request_mutex);
	dout("sync done (thru tid %llu)\n", last_tid);
}
EXPORT_SYMBOL(ceph_osdc_sync);

=======
	struct rb_node *n, *p;
	u64 last_tid = atomic64_read(&osdc->last_tid);

again:
	down_read(&osdc->lock);
	for (n = rb_first(&osdc->osds); n; n = rb_next(n)) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);

		mutex_lock(&osd->lock);
		for (p = rb_first(&osd->o_requests); p; p = rb_next(p)) {
			struct ceph_osd_request *req =
			    rb_entry(p, struct ceph_osd_request, r_node);

			if (req->r_tid > last_tid)
				break;

			if (!(req->r_flags & CEPH_OSD_FLAG_WRITE))
				continue;

			ceph_osdc_get_request(req);
			mutex_unlock(&osd->lock);
			up_read(&osdc->lock);
			dout("%s waiting on req %p tid %llu last_tid %llu\n",
			     __func__, req, req->r_tid, last_tid);
			wait_for_completion(&req->r_safe_completion);
			ceph_osdc_put_request(req);
			goto again;
		}

		mutex_unlock(&osd->lock);
	}

	up_read(&osdc->lock);
	dout("%s done last_tid %llu\n", __func__, last_tid);
}
EXPORT_SYMBOL(ceph_osdc_sync);

static struct ceph_osd_request *
alloc_linger_request(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_request *req;

	req = ceph_osdc_alloc_request(lreq->osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return NULL;

	ceph_oid_copy(&req->r_base_oid, &lreq->t.base_oid);
	ceph_oloc_copy(&req->r_base_oloc, &lreq->t.base_oloc);

	if (ceph_osdc_alloc_messages(req, GFP_NOIO)) {
		ceph_osdc_put_request(req);
		return NULL;
	}

	return req;
}

/*
 * Returns a handle, caller owns a ref.
 */
struct ceph_osd_linger_request *
ceph_osdc_watch(struct ceph_osd_client *osdc,
		struct ceph_object_id *oid,
		struct ceph_object_locator *oloc,
		rados_watchcb2_t wcb,
		rados_watcherrcb_t errcb,
		void *data)
{
	struct ceph_osd_linger_request *lreq;
	int ret;

	lreq = linger_alloc(osdc);
	if (!lreq)
		return ERR_PTR(-ENOMEM);

	lreq->is_watch = true;
	lreq->wcb = wcb;
	lreq->errcb = errcb;
	lreq->data = data;
	lreq->watch_valid_thru = jiffies;

	ceph_oid_copy(&lreq->t.base_oid, oid);
	ceph_oloc_copy(&lreq->t.base_oloc, oloc);
	lreq->t.flags = CEPH_OSD_FLAG_WRITE | CEPH_OSD_FLAG_ONDISK;
	lreq->mtime = CURRENT_TIME;

	lreq->reg_req = alloc_linger_request(lreq);
	if (!lreq->reg_req) {
		ret = -ENOMEM;
		goto err_put_lreq;
	}

	lreq->ping_req = alloc_linger_request(lreq);
	if (!lreq->ping_req) {
		ret = -ENOMEM;
		goto err_put_lreq;
	}

	down_write(&osdc->lock);
	linger_register(lreq); /* before osd_req_op_* */
	osd_req_op_watch_init(lreq->reg_req, 0, lreq->linger_id,
			      CEPH_OSD_WATCH_OP_WATCH);
	osd_req_op_watch_init(lreq->ping_req, 0, lreq->linger_id,
			      CEPH_OSD_WATCH_OP_PING);
	linger_submit(lreq);
	up_write(&osdc->lock);

	ret = linger_reg_commit_wait(lreq);
	if (ret) {
		linger_cancel(lreq);
		goto err_put_lreq;
	}

	return lreq;

err_put_lreq:
	linger_put(lreq);
	return ERR_PTR(ret);
}
EXPORT_SYMBOL(ceph_osdc_watch);

/*
 * Releases a ref.
 *
 * Times out after mount_timeout to preserve rbd unmap behaviour
 * introduced in 2894e1d76974 ("rbd: timeout watch teardown on unmap
 * with mount_timeout").
 */
int ceph_osdc_unwatch(struct ceph_osd_client *osdc,
		      struct ceph_osd_linger_request *lreq)
{
	struct ceph_options *opts = osdc->client->options;
	struct ceph_osd_request *req;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, &lreq->t.base_oid);
	ceph_oloc_copy(&req->r_base_oloc, &lreq->t.base_oloc);
	req->r_flags = CEPH_OSD_FLAG_WRITE | CEPH_OSD_FLAG_ONDISK;
	req->r_mtime = CURRENT_TIME;
	osd_req_op_watch_init(req, 0, lreq->linger_id,
			      CEPH_OSD_WATCH_OP_UNWATCH);

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	ceph_osdc_start_request(osdc, req, false);
	linger_cancel(lreq);
	linger_put(lreq);
	ret = wait_request_timeout(req, opts->mount_timeout);

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_unwatch);

static int osd_req_op_notify_ack_init(struct ceph_osd_request *req, int which,
				      u64 notify_id, u64 cookie, void *payload,
				      size_t payload_len)
{
	struct ceph_osd_req_op *op;
	struct ceph_pagelist *pl;
	int ret;

	op = _osd_req_op_init(req, which, CEPH_OSD_OP_NOTIFY_ACK, 0);

	pl = kmalloc(sizeof(*pl), GFP_NOIO);
	if (!pl)
		return -ENOMEM;

	ceph_pagelist_init(pl);
	ret = ceph_pagelist_encode_64(pl, notify_id);
	ret |= ceph_pagelist_encode_64(pl, cookie);
	if (payload) {
		ret |= ceph_pagelist_encode_32(pl, payload_len);
		ret |= ceph_pagelist_append(pl, payload, payload_len);
	} else {
		ret |= ceph_pagelist_encode_32(pl, 0);
	}
	if (ret) {
		ceph_pagelist_release(pl);
		return -ENOMEM;
	}

	ceph_osd_data_pagelist_init(&op->notify_ack.request_data, pl);
	op->indata_len = pl->length;
	return 0;
}

int ceph_osdc_notify_ack(struct ceph_osd_client *osdc,
			 struct ceph_object_id *oid,
			 struct ceph_object_locator *oloc,
			 u64 notify_id,
			 u64 cookie,
			 void *payload,
			 size_t payload_len)
{
	struct ceph_osd_request *req;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, oid);
	ceph_oloc_copy(&req->r_base_oloc, oloc);
	req->r_flags = CEPH_OSD_FLAG_READ;

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	ret = osd_req_op_notify_ack_init(req, 0, notify_id, cookie, payload,
					 payload_len);
	if (ret)
		goto out_put_req;

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_notify_ack);

static int osd_req_op_notify_init(struct ceph_osd_request *req, int which,
				  u64 cookie, u32 prot_ver, u32 timeout,
				  void *payload, size_t payload_len)
{
	struct ceph_osd_req_op *op;
	struct ceph_pagelist *pl;
	int ret;

	op = _osd_req_op_init(req, which, CEPH_OSD_OP_NOTIFY, 0);
	op->notify.cookie = cookie;

	pl = kmalloc(sizeof(*pl), GFP_NOIO);
	if (!pl)
		return -ENOMEM;

	ceph_pagelist_init(pl);
	ret = ceph_pagelist_encode_32(pl, 1); /* prot_ver */
	ret |= ceph_pagelist_encode_32(pl, timeout);
	ret |= ceph_pagelist_encode_32(pl, payload_len);
	ret |= ceph_pagelist_append(pl, payload, payload_len);
	if (ret) {
		ceph_pagelist_release(pl);
		return -ENOMEM;
	}

	ceph_osd_data_pagelist_init(&op->notify.request_data, pl);
	op->indata_len = pl->length;
	return 0;
}

/*
 * @timeout: in seconds
 *
 * @preply_{pages,len} are initialized both on success and error.
 * The caller is responsible for:
 *
 *     ceph_release_page_vector(reply_pages, calc_pages_for(0, reply_len))
 */
int ceph_osdc_notify(struct ceph_osd_client *osdc,
		     struct ceph_object_id *oid,
		     struct ceph_object_locator *oloc,
		     void *payload,
		     size_t payload_len,
		     u32 timeout,
		     struct page ***preply_pages,
		     size_t *preply_len)
{
	struct ceph_osd_linger_request *lreq;
	struct page **pages;
	int ret;

	WARN_ON(!timeout);
	if (preply_pages) {
		*preply_pages = NULL;
		*preply_len = 0;
	}

	lreq = linger_alloc(osdc);
	if (!lreq)
		return -ENOMEM;

	lreq->preply_pages = preply_pages;
	lreq->preply_len = preply_len;

	ceph_oid_copy(&lreq->t.base_oid, oid);
	ceph_oloc_copy(&lreq->t.base_oloc, oloc);
	lreq->t.flags = CEPH_OSD_FLAG_READ;

	lreq->reg_req = alloc_linger_request(lreq);
	if (!lreq->reg_req) {
		ret = -ENOMEM;
		goto out_put_lreq;
	}

	/* for notify_id */
	pages = ceph_alloc_page_vector(1, GFP_NOIO);
	if (IS_ERR(pages)) {
		ret = PTR_ERR(pages);
		goto out_put_lreq;
	}

	down_write(&osdc->lock);
	linger_register(lreq); /* before osd_req_op_* */
	ret = osd_req_op_notify_init(lreq->reg_req, 0, lreq->linger_id, 1,
				     timeout, payload, payload_len);
	if (ret) {
		linger_unregister(lreq);
		up_write(&osdc->lock);
		ceph_release_page_vector(pages, 1);
		goto out_put_lreq;
	}
	ceph_osd_data_pages_init(osd_req_op_data(lreq->reg_req, 0, notify,
						 response_data),
				 pages, PAGE_SIZE, 0, false, true);
	linger_submit(lreq);
	up_write(&osdc->lock);

	ret = linger_reg_commit_wait(lreq);
	if (!ret)
		ret = linger_notify_finish_wait(lreq);
	else
		dout("lreq %p failed to initiate notify %d\n", lreq, ret);

	linger_cancel(lreq);
out_put_lreq:
	linger_put(lreq);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_notify);

/*
 * Return the number of milliseconds since the watch was last
 * confirmed, or an error.  If there is an error, the watch is no
 * longer valid, and should be destroyed with ceph_osdc_unwatch().
 */
int ceph_osdc_watch_check(struct ceph_osd_client *osdc,
			  struct ceph_osd_linger_request *lreq)
{
	unsigned long stamp, age;
	int ret;

	down_read(&osdc->lock);
	mutex_lock(&lreq->lock);
	stamp = lreq->watch_valid_thru;
	if (!list_empty(&lreq->pending_lworks)) {
		struct linger_work *lwork =
		    list_first_entry(&lreq->pending_lworks,
				     struct linger_work,
				     pending_item);

		if (time_before(lwork->queued_stamp, stamp))
			stamp = lwork->queued_stamp;
	}
	age = jiffies - stamp;
	dout("%s lreq %p linger_id %llu age %lu last_error %d\n", __func__,
	     lreq, lreq->linger_id, age, lreq->last_error);
	/* we are truncating to msecs, so return a safe upper bound */
	ret = lreq->last_error ?: 1 + jiffies_to_msecs(age);

	mutex_unlock(&lreq->lock);
	up_read(&osdc->lock);
	return ret;
}

static int decode_watcher(void **p, void *end, struct ceph_watch_item *item)
{
	u8 struct_v;
	u32 struct_len;
	int ret;

	ret = ceph_start_decoding(p, end, 2, "watch_item_t",
				  &struct_v, &struct_len);
	if (ret)
		return ret;

	ceph_decode_copy(p, &item->name, sizeof(item->name));
	item->cookie = ceph_decode_64(p);
	*p += 4; /* skip timeout_seconds */
	if (struct_v >= 2) {
		ceph_decode_copy(p, &item->addr, sizeof(item->addr));
		ceph_decode_addr(&item->addr);
	}

	dout("%s %s%llu cookie %llu addr %s\n", __func__,
	     ENTITY_NAME(item->name), item->cookie,
	     ceph_pr_addr(&item->addr.in_addr));
	return 0;
}

static int decode_watchers(void **p, void *end,
			   struct ceph_watch_item **watchers,
			   u32 *num_watchers)
{
	u8 struct_v;
	u32 struct_len;
	int i;
	int ret;

	ret = ceph_start_decoding(p, end, 1, "obj_list_watch_response_t",
				  &struct_v, &struct_len);
	if (ret)
		return ret;

	*num_watchers = ceph_decode_32(p);
	*watchers = kcalloc(*num_watchers, sizeof(**watchers), GFP_NOIO);
	if (!*watchers)
		return -ENOMEM;

	for (i = 0; i < *num_watchers; i++) {
		ret = decode_watcher(p, end, *watchers + i);
		if (ret) {
			kfree(*watchers);
			return ret;
		}
	}

	return 0;
}

/*
 * On success, the caller is responsible for:
 *
 *     kfree(watchers);
 */
int ceph_osdc_list_watchers(struct ceph_osd_client *osdc,
			    struct ceph_object_id *oid,
			    struct ceph_object_locator *oloc,
			    struct ceph_watch_item **watchers,
			    u32 *num_watchers)
{
	struct ceph_osd_request *req;
	struct page **pages;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, oid);
	ceph_oloc_copy(&req->r_base_oloc, oloc);
	req->r_flags = CEPH_OSD_FLAG_READ;

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	pages = ceph_alloc_page_vector(1, GFP_NOIO);
	if (IS_ERR(pages)) {
		ret = PTR_ERR(pages);
		goto out_put_req;
	}

	osd_req_op_init(req, 0, CEPH_OSD_OP_LIST_WATCHERS, 0);
	ceph_osd_data_pages_init(osd_req_op_data(req, 0, list_watchers,
						 response_data),
				 pages, PAGE_SIZE, 0, false, true);

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);
	if (ret >= 0) {
		void *p = page_address(pages[0]);
		void *const end = p + req->r_ops[0].outdata_len;

		ret = decode_watchers(&p, end, watchers, num_watchers);
	}

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_list_watchers);

>>>>>>> v4.9.227
/*
 * Call all pending notify callbacks - for use after a watch is
 * unregistered, to make sure no more callbacks for it will be invoked
 */
void ceph_osdc_flush_notifies(struct ceph_osd_client *osdc)
{
<<<<<<< HEAD
=======
	dout("%s osdc %p\n", __func__, osdc);
>>>>>>> v4.9.227
	flush_workqueue(osdc->notify_wq);
}
EXPORT_SYMBOL(ceph_osdc_flush_notifies);

<<<<<<< HEAD
=======
void ceph_osdc_maybe_request_map(struct ceph_osd_client *osdc)
{
	down_read(&osdc->lock);
	maybe_request_map(osdc);
	up_read(&osdc->lock);
}
EXPORT_SYMBOL(ceph_osdc_maybe_request_map);

/*
 * Execute an OSD class method on an object.
 *
 * @flags: CEPH_OSD_FLAG_*
 * @resp_len: out param for reply length
 */
int ceph_osdc_call(struct ceph_osd_client *osdc,
		   struct ceph_object_id *oid,
		   struct ceph_object_locator *oloc,
		   const char *class, const char *method,
		   unsigned int flags,
		   struct page *req_page, size_t req_len,
		   struct page *resp_page, size_t *resp_len)
{
	struct ceph_osd_request *req;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, oid);
	ceph_oloc_copy(&req->r_base_oloc, oloc);
	req->r_flags = flags;

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	osd_req_op_cls_init(req, 0, CEPH_OSD_OP_CALL, class, method);
	if (req_page)
		osd_req_op_cls_request_data_pages(req, 0, &req_page, req_len,
						  0, false, false);
	if (resp_page)
		osd_req_op_cls_response_data_pages(req, 0, &resp_page,
						   PAGE_SIZE, 0, false, false);

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);
	if (ret >= 0) {
		ret = req->r_ops[0].rval;
		if (resp_page)
			*resp_len = req->r_ops[0].outdata_len;
	}

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_call);
>>>>>>> v4.9.227

/*
 * init, shutdown
 */
int ceph_osdc_init(struct ceph_osd_client *osdc, struct ceph_client *client)
{
	int err;

	dout("init\n");
	osdc->client = client;
<<<<<<< HEAD
	osdc->osdmap = NULL;
	init_rwsem(&osdc->map_sem);
	init_completion(&osdc->map_waiters);
	osdc->last_requested_map = 0;
	mutex_init(&osdc->request_mutex);
	osdc->last_tid = 0;
	osdc->osds = RB_ROOT;
	INIT_LIST_HEAD(&osdc->osd_lru);
	osdc->requests = RB_ROOT;
	INIT_LIST_HEAD(&osdc->req_lru);
	INIT_LIST_HEAD(&osdc->req_unsent);
	INIT_LIST_HEAD(&osdc->req_notarget);
	INIT_LIST_HEAD(&osdc->req_linger);
	osdc->num_requests = 0;
	INIT_DELAYED_WORK(&osdc->timeout_work, handle_timeout);
	INIT_DELAYED_WORK(&osdc->osds_timeout_work, handle_osds_timeout);
	spin_lock_init(&osdc->event_lock);
	osdc->event_tree = RB_ROOT;
	osdc->event_count = 0;

	schedule_delayed_work(&osdc->osds_timeout_work,
	   round_jiffies_relative(osdc->client->options->osd_idle_ttl * HZ));

	err = -ENOMEM;
	osdc->req_mempool = mempool_create_kmalloc_pool(10,
					sizeof(struct ceph_osd_request));
	if (!osdc->req_mempool)
		goto out;

	err = ceph_msgpool_init(&osdc->msgpool_op, CEPH_MSG_OSD_OP,
				OSD_OP_FRONT_LEN, 10, true,
				"osd_op");
	if (err < 0)
		goto out_mempool;
	err = ceph_msgpool_init(&osdc->msgpool_op_reply, CEPH_MSG_OSD_OPREPLY,
				OSD_OPREPLY_FRONT_LEN, 10, true,
				"osd_op_reply");
=======
	init_rwsem(&osdc->lock);
	osdc->osds = RB_ROOT;
	INIT_LIST_HEAD(&osdc->osd_lru);
	spin_lock_init(&osdc->osd_lru_lock);
	osd_init(&osdc->homeless_osd);
	osdc->homeless_osd.o_osdc = osdc;
	osdc->homeless_osd.o_osd = CEPH_HOMELESS_OSD;
	osdc->last_linger_id = CEPH_LINGER_ID_START;
	osdc->linger_requests = RB_ROOT;
	osdc->map_checks = RB_ROOT;
	osdc->linger_map_checks = RB_ROOT;
	INIT_DELAYED_WORK(&osdc->timeout_work, handle_timeout);
	INIT_DELAYED_WORK(&osdc->osds_timeout_work, handle_osds_timeout);

	err = -ENOMEM;
	osdc->osdmap = ceph_osdmap_alloc();
	if (!osdc->osdmap)
		goto out;

	osdc->req_mempool = mempool_create_slab_pool(10,
						     ceph_osd_request_cache);
	if (!osdc->req_mempool)
		goto out_map;

	err = ceph_msgpool_init(&osdc->msgpool_op, CEPH_MSG_OSD_OP,
				PAGE_SIZE, 10, true, "osd_op");
	if (err < 0)
		goto out_mempool;
	err = ceph_msgpool_init(&osdc->msgpool_op_reply, CEPH_MSG_OSD_OPREPLY,
				PAGE_SIZE, 10, true, "osd_op_reply");
>>>>>>> v4.9.227
	if (err < 0)
		goto out_msgpool;

	err = -ENOMEM;
	osdc->notify_wq = create_singlethread_workqueue("ceph-watch-notify");
	if (!osdc->notify_wq)
		goto out_msgpool_reply;

<<<<<<< HEAD
=======
	schedule_delayed_work(&osdc->timeout_work,
			      osdc->client->options->osd_keepalive_timeout);
	schedule_delayed_work(&osdc->osds_timeout_work,
	    round_jiffies_relative(osdc->client->options->osd_idle_ttl));

>>>>>>> v4.9.227
	return 0;

out_msgpool_reply:
	ceph_msgpool_destroy(&osdc->msgpool_op_reply);
out_msgpool:
	ceph_msgpool_destroy(&osdc->msgpool_op);
out_mempool:
	mempool_destroy(osdc->req_mempool);
<<<<<<< HEAD
=======
out_map:
	ceph_osdmap_destroy(osdc->osdmap);
>>>>>>> v4.9.227
out:
	return err;
}

void ceph_osdc_stop(struct ceph_osd_client *osdc)
{
	flush_workqueue(osdc->notify_wq);
	destroy_workqueue(osdc->notify_wq);
	cancel_delayed_work_sync(&osdc->timeout_work);
	cancel_delayed_work_sync(&osdc->osds_timeout_work);
<<<<<<< HEAD
	if (osdc->osdmap) {
		ceph_osdmap_destroy(osdc->osdmap);
		osdc->osdmap = NULL;
	}
	remove_all_osds(osdc);
=======

	down_write(&osdc->lock);
	while (!RB_EMPTY_ROOT(&osdc->osds)) {
		struct ceph_osd *osd = rb_entry(rb_first(&osdc->osds),
						struct ceph_osd, o_node);
		close_osd(osd);
	}
	up_write(&osdc->lock);
	WARN_ON(atomic_read(&osdc->homeless_osd.o_ref) != 1);
	osd_cleanup(&osdc->homeless_osd);

	WARN_ON(!list_empty(&osdc->osd_lru));
	WARN_ON(!RB_EMPTY_ROOT(&osdc->linger_requests));
	WARN_ON(!RB_EMPTY_ROOT(&osdc->map_checks));
	WARN_ON(!RB_EMPTY_ROOT(&osdc->linger_map_checks));
	WARN_ON(atomic_read(&osdc->num_requests));
	WARN_ON(atomic_read(&osdc->num_homeless));

	ceph_osdmap_destroy(osdc->osdmap);
>>>>>>> v4.9.227
	mempool_destroy(osdc->req_mempool);
	ceph_msgpool_destroy(&osdc->msgpool_op);
	ceph_msgpool_destroy(&osdc->msgpool_op_reply);
}

/*
 * Read some contiguous pages.  If we cross a stripe boundary, shorten
 * *plen.  Return number of bytes read, or error.
 */
int ceph_osdc_readpages(struct ceph_osd_client *osdc,
			struct ceph_vino vino, struct ceph_file_layout *layout,
			u64 off, u64 *plen,
			u32 truncate_seq, u64 truncate_size,
			struct page **pages, int num_pages, int page_align)
{
	struct ceph_osd_request *req;
	int rc = 0;

	dout("readpages on ino %llx.%llx on %llu~%llu\n", vino.ino,
	     vino.snap, off, *plen);
<<<<<<< HEAD
	req = ceph_osdc_new_request(osdc, layout, vino, off, plen, 1,
=======
	req = ceph_osdc_new_request(osdc, layout, vino, off, plen, 0, 1,
>>>>>>> v4.9.227
				    CEPH_OSD_OP_READ, CEPH_OSD_FLAG_READ,
				    NULL, truncate_seq, truncate_size,
				    false);
	if (IS_ERR(req))
		return PTR_ERR(req);

	/* it may be a short read due to an object boundary */
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	osd_req_op_extent_osd_data_pages(req, 0,
				pages, *plen, page_align, false, false);

	dout("readpages  final extent is %llu~%llu (%llu bytes align %d)\n",
	     off, *plen, *plen, page_align);

<<<<<<< HEAD
	ceph_osdc_build_request(req, off, NULL, vino.snap, NULL);

=======
>>>>>>> v4.9.227
	rc = ceph_osdc_start_request(osdc, req, false);
	if (!rc)
		rc = ceph_osdc_wait_request(osdc, req);

	ceph_osdc_put_request(req);
	dout("readpages result %d\n", rc);
	return rc;
}
EXPORT_SYMBOL(ceph_osdc_readpages);

/*
 * do a synchronous write on N pages
 */
int ceph_osdc_writepages(struct ceph_osd_client *osdc, struct ceph_vino vino,
			 struct ceph_file_layout *layout,
			 struct ceph_snap_context *snapc,
			 u64 off, u64 len,
			 u32 truncate_seq, u64 truncate_size,
			 struct timespec *mtime,
			 struct page **pages, int num_pages)
{
	struct ceph_osd_request *req;
	int rc = 0;
	int page_align = off & ~PAGE_MASK;

<<<<<<< HEAD
	BUG_ON(vino.snap != CEPH_NOSNAP);	/* snapshots aren't writeable */
	req = ceph_osdc_new_request(osdc, layout, vino, off, &len, 1,
=======
	req = ceph_osdc_new_request(osdc, layout, vino, off, &len, 0, 1,
>>>>>>> v4.9.227
				    CEPH_OSD_OP_WRITE,
				    CEPH_OSD_FLAG_ONDISK | CEPH_OSD_FLAG_WRITE,
				    snapc, truncate_seq, truncate_size,
				    true);
	if (IS_ERR(req))
		return PTR_ERR(req);

	/* it may be a short write due to an object boundary */
	osd_req_op_extent_osd_data_pages(req, 0, pages, len, page_align,
				false, false);
	dout("writepages %llu~%llu (%llu bytes)\n", off, len, len);

<<<<<<< HEAD
	ceph_osdc_build_request(req, off, snapc, CEPH_NOSNAP, mtime);

=======
	req->r_mtime = *mtime;
>>>>>>> v4.9.227
	rc = ceph_osdc_start_request(osdc, req, true);
	if (!rc)
		rc = ceph_osdc_wait_request(osdc, req);

	ceph_osdc_put_request(req);
	if (rc == 0)
		rc = len;
	dout("writepages result %d\n", rc);
	return rc;
}
EXPORT_SYMBOL(ceph_osdc_writepages);

int ceph_osdc_setup(void)
{
<<<<<<< HEAD
	BUG_ON(ceph_osd_request_cache);
	ceph_osd_request_cache = kmem_cache_create("ceph_osd_request",
					sizeof (struct ceph_osd_request),
					__alignof__(struct ceph_osd_request),
					0, NULL);
=======
	size_t size = sizeof(struct ceph_osd_request) +
	    CEPH_OSD_SLAB_OPS * sizeof(struct ceph_osd_req_op);

	BUG_ON(ceph_osd_request_cache);
	ceph_osd_request_cache = kmem_cache_create("ceph_osd_request", size,
						   0, 0, NULL);
>>>>>>> v4.9.227

	return ceph_osd_request_cache ? 0 : -ENOMEM;
}
EXPORT_SYMBOL(ceph_osdc_setup);

void ceph_osdc_cleanup(void)
{
	BUG_ON(!ceph_osd_request_cache);
	kmem_cache_destroy(ceph_osd_request_cache);
	ceph_osd_request_cache = NULL;
}
EXPORT_SYMBOL(ceph_osdc_cleanup);

/*
 * handle incoming message
 */
static void dispatch(struct ceph_connection *con, struct ceph_msg *msg)
{
	struct ceph_osd *osd = con->private;
<<<<<<< HEAD
	struct ceph_osd_client *osdc;
	int type = le16_to_cpu(msg->hdr.type);

	if (!osd)
		goto out;
	osdc = osd->o_osdc;

=======
	struct ceph_osd_client *osdc = osd->o_osdc;
	int type = le16_to_cpu(msg->hdr.type);

>>>>>>> v4.9.227
	switch (type) {
	case CEPH_MSG_OSD_MAP:
		ceph_osdc_handle_map(osdc, msg);
		break;
	case CEPH_MSG_OSD_OPREPLY:
<<<<<<< HEAD
		handle_reply(osdc, msg, con);
=======
		handle_reply(osd, msg);
>>>>>>> v4.9.227
		break;
	case CEPH_MSG_WATCH_NOTIFY:
		handle_watch_notify(osdc, msg);
		break;

	default:
		pr_err("received unknown message type %d %s\n", type,
		       ceph_msg_type_name(type));
	}
<<<<<<< HEAD
out:
=======

>>>>>>> v4.9.227
	ceph_msg_put(msg);
}

/*
<<<<<<< HEAD
 * lookup and return message for incoming reply.  set up reply message
 * pages.
=======
 * Lookup and return message for incoming reply.  Don't try to do
 * anything about a larger than preallocated data portion of the
 * message at the moment - for now, just skip the message.
>>>>>>> v4.9.227
 */
static struct ceph_msg *get_reply(struct ceph_connection *con,
				  struct ceph_msg_header *hdr,
				  int *skip)
{
	struct ceph_osd *osd = con->private;
	struct ceph_osd_client *osdc = osd->o_osdc;
<<<<<<< HEAD
	struct ceph_msg *m;
	struct ceph_osd_request *req;
	int front_len = le32_to_cpu(hdr->front_len);
	int data_len = le32_to_cpu(hdr->data_len);
	u64 tid;

	tid = le64_to_cpu(hdr->tid);
	mutex_lock(&osdc->request_mutex);
	req = __lookup_request(osdc, tid);
	if (!req) {
		*skip = 1;
		m = NULL;
		dout("get_reply unknown tid %llu from osd%d\n", tid,
		     osd->o_osd);
		goto out;
	}

	if (req->r_reply->con)
		dout("%s revoking msg %p from old con %p\n", __func__,
		     req->r_reply, req->r_reply->con);
	ceph_msg_revoke_incoming(req->r_reply);

	if (front_len > req->r_reply->front_alloc_len) {
		pr_warn("get_reply front %d > preallocated %d (%u#%llu)\n",
			front_len, req->r_reply->front_alloc_len,
			(unsigned int)con->peer_name.type,
			le64_to_cpu(con->peer_name.num));
		m = ceph_msg_new(CEPH_MSG_OSD_OPREPLY, front_len, GFP_NOFS,
				 false);
		if (!m)
			goto out;
		ceph_msg_put(req->r_reply);
		req->r_reply = m;
	}
	m = ceph_msg_get(req->r_reply);

	if (data_len > 0) {
		struct ceph_osd_data *osd_data;

		/*
		 * XXX This is assuming there is only one op containing
		 * XXX page data.  Probably OK for reads, but this
		 * XXX ought to be done more generally.
		 */
		osd_data = osd_req_op_extent_osd_data(req, 0);
		if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGES) {
			if (osd_data->pages &&
				unlikely(osd_data->length < data_len)) {

				pr_warn("tid %lld reply has %d bytes we had only %llu bytes ready\n",
					tid, data_len, osd_data->length);
				*skip = 1;
				ceph_msg_put(m);
				m = NULL;
				goto out;
			}
		}
	}
	*skip = 0;
	dout("get_reply tid %lld %p\n", tid, m);

out:
	mutex_unlock(&osdc->request_mutex);
	return m;

=======
	struct ceph_msg *m = NULL;
	struct ceph_osd_request *req;
	int front_len = le32_to_cpu(hdr->front_len);
	int data_len = le32_to_cpu(hdr->data_len);
	u64 tid = le64_to_cpu(hdr->tid);

	down_read(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown, skipping\n", __func__, osd->o_osd);
		*skip = 1;
		goto out_unlock_osdc;
	}
	WARN_ON(osd->o_osd != le64_to_cpu(hdr->src.num));

	mutex_lock(&osd->lock);
	req = lookup_request(&osd->o_requests, tid);
	if (!req) {
		dout("%s osd%d tid %llu unknown, skipping\n", __func__,
		     osd->o_osd, tid);
		*skip = 1;
		goto out_unlock_session;
	}

	ceph_msg_revoke_incoming(req->r_reply);

	if (front_len > req->r_reply->front_alloc_len) {
		pr_warn("%s osd%d tid %llu front %d > preallocated %d\n",
			__func__, osd->o_osd, req->r_tid, front_len,
			req->r_reply->front_alloc_len);
		m = ceph_msg_new(CEPH_MSG_OSD_OPREPLY, front_len, GFP_NOFS,
				 false);
		if (!m)
			goto out_unlock_session;
		ceph_msg_put(req->r_reply);
		req->r_reply = m;
	}

	if (data_len > req->r_reply->data_length) {
		pr_warn("%s osd%d tid %llu data %d > preallocated %zu, skipping\n",
			__func__, osd->o_osd, req->r_tid, data_len,
			req->r_reply->data_length);
		m = NULL;
		*skip = 1;
		goto out_unlock_session;
	}

	m = ceph_msg_get(req->r_reply);
	dout("get_reply tid %lld %p\n", tid, m);

out_unlock_session:
	mutex_unlock(&osd->lock);
out_unlock_osdc:
	up_read(&osdc->lock);
	return m;
}

/*
 * TODO: switch to a msg-owned pagelist
 */
static struct ceph_msg *alloc_msg_with_page_vector(struct ceph_msg_header *hdr)
{
	struct ceph_msg *m;
	int type = le16_to_cpu(hdr->type);
	u32 front_len = le32_to_cpu(hdr->front_len);
	u32 data_len = le32_to_cpu(hdr->data_len);

	m = ceph_msg_new(type, front_len, GFP_NOIO, false);
	if (!m)
		return NULL;

	if (data_len) {
		struct page **pages;
		struct ceph_osd_data osd_data;

		pages = ceph_alloc_page_vector(calc_pages_for(0, data_len),
					       GFP_NOIO);
		if (IS_ERR(pages)) {
			ceph_msg_put(m);
			return NULL;
		}

		ceph_osd_data_pages_init(&osd_data, pages, data_len, 0, false,
					 false);
		ceph_osdc_msg_data_add(m, &osd_data);
	}

	return m;
>>>>>>> v4.9.227
}

static struct ceph_msg *alloc_msg(struct ceph_connection *con,
				  struct ceph_msg_header *hdr,
				  int *skip)
{
	struct ceph_osd *osd = con->private;
	int type = le16_to_cpu(hdr->type);
<<<<<<< HEAD
	int front = le32_to_cpu(hdr->front_len);
=======
>>>>>>> v4.9.227

	*skip = 0;
	switch (type) {
	case CEPH_MSG_OSD_MAP:
	case CEPH_MSG_WATCH_NOTIFY:
<<<<<<< HEAD
		return ceph_msg_new(type, front, GFP_NOFS, false);
	case CEPH_MSG_OSD_OPREPLY:
		return get_reply(con, hdr, skip);
	default:
		pr_info("alloc_msg unexpected msg type %d from osd%d\n", type,
			osd->o_osd);
=======
		return alloc_msg_with_page_vector(hdr);
	case CEPH_MSG_OSD_OPREPLY:
		return get_reply(con, hdr, skip);
	default:
		pr_warn("%s osd%d unknown msg type %d, skipping\n", __func__,
			osd->o_osd, type);
>>>>>>> v4.9.227
		*skip = 1;
		return NULL;
	}
}

/*
 * Wrappers to refcount containing ceph_osd struct
 */
static struct ceph_connection *get_osd_con(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	if (get_osd(osd))
		return con;
	return NULL;
}

static void put_osd_con(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	put_osd(osd);
}

/*
 * authentication
 */
/*
 * Note: returned pointer is the address of a structure that's
 * managed separately.  Caller must *not* attempt to free it.
 */
static struct ceph_auth_handshake *get_authorizer(struct ceph_connection *con,
					int *proto, int force_new)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;
	struct ceph_auth_handshake *auth = &o->o_auth;

	if (force_new && auth->authorizer) {
<<<<<<< HEAD
		ceph_auth_destroy_authorizer(ac, auth->authorizer);
=======
		ceph_auth_destroy_authorizer(auth->authorizer);
>>>>>>> v4.9.227
		auth->authorizer = NULL;
	}
	if (!auth->authorizer) {
		int ret = ceph_auth_create_authorizer(ac, CEPH_ENTITY_TYPE_OSD,
						      auth);
		if (ret)
			return ERR_PTR(ret);
	} else {
		int ret = ceph_auth_update_authorizer(ac, CEPH_ENTITY_TYPE_OSD,
						     auth);
		if (ret)
			return ERR_PTR(ret);
	}
	*proto = ac->protocol;

	return auth;
}

<<<<<<< HEAD

static int verify_authorizer_reply(struct ceph_connection *con, int len)
=======
static int add_authorizer_challenge(struct ceph_connection *con,
				    void *challenge_buf, int challenge_buf_len)
>>>>>>> v4.9.227
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;

<<<<<<< HEAD
	return ceph_auth_verify_authorizer_reply(ac, o->o_auth.authorizer, len);
=======
	return ceph_auth_add_authorizer_challenge(ac, o->o_auth.authorizer,
					    challenge_buf, challenge_buf_len);
}

static int verify_authorizer_reply(struct ceph_connection *con)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;

	return ceph_auth_verify_authorizer_reply(ac, o->o_auth.authorizer);
>>>>>>> v4.9.227
}

static int invalidate_authorizer(struct ceph_connection *con)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;

	ceph_auth_invalidate_authorizer(ac, CEPH_ENTITY_TYPE_OSD);
	return ceph_monc_validate_auth(&osdc->client->monc);
}

<<<<<<< HEAD
=======
static int osd_sign_message(struct ceph_msg *msg)
{
	struct ceph_osd *o = msg->con->private;
	struct ceph_auth_handshake *auth = &o->o_auth;

	return ceph_auth_sign_message(auth, msg);
}

static int osd_check_message_signature(struct ceph_msg *msg)
{
	struct ceph_osd *o = msg->con->private;
	struct ceph_auth_handshake *auth = &o->o_auth;

	return ceph_auth_check_message_signature(auth, msg);
}

>>>>>>> v4.9.227
static const struct ceph_connection_operations osd_con_ops = {
	.get = get_osd_con,
	.put = put_osd_con,
	.dispatch = dispatch,
	.get_authorizer = get_authorizer,
<<<<<<< HEAD
	.verify_authorizer_reply = verify_authorizer_reply,
	.invalidate_authorizer = invalidate_authorizer,
	.alloc_msg = alloc_msg,
	.fault = osd_reset,
=======
	.add_authorizer_challenge = add_authorizer_challenge,
	.verify_authorizer_reply = verify_authorizer_reply,
	.invalidate_authorizer = invalidate_authorizer,
	.alloc_msg = alloc_msg,
	.sign_message = osd_sign_message,
	.check_message_signature = osd_check_message_signature,
	.fault = osd_fault,
>>>>>>> v4.9.227
};
