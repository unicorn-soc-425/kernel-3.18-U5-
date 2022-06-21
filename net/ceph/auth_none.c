
#include <linux/ceph/ceph_debug.h>

#include <linux/err.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>

#include <linux/ceph/decode.h>
#include <linux/ceph/auth.h>

#include "auth_none.h"

static void reset(struct ceph_auth_client *ac)
{
	struct ceph_auth_none_info *xi = ac->private;

	xi->starting = true;
<<<<<<< HEAD
	xi->built_authorizer = false;
=======
>>>>>>> v4.9.227
}

static void destroy(struct ceph_auth_client *ac)
{
	kfree(ac->private);
	ac->private = NULL;
}

static int is_authenticated(struct ceph_auth_client *ac)
{
	struct ceph_auth_none_info *xi = ac->private;

	return !xi->starting;
}

static int should_authenticate(struct ceph_auth_client *ac)
{
	struct ceph_auth_none_info *xi = ac->private;

	return xi->starting;
}

<<<<<<< HEAD
=======
static int ceph_auth_none_build_authorizer(struct ceph_auth_client *ac,
					   struct ceph_none_authorizer *au)
{
	void *p = au->buf;
	void *const end = p + sizeof(au->buf);
	int ret;

	ceph_encode_8_safe(&p, end, 1, e_range);
	ret = ceph_auth_entity_name_encode(ac->name, &p, end);
	if (ret < 0)
		return ret;

	ceph_encode_64_safe(&p, end, ac->global_id, e_range);
	au->buf_len = p - (void *)au->buf;
	dout("%s built authorizer len %d\n", __func__, au->buf_len);
	return 0;

e_range:
	return -ERANGE;
}

>>>>>>> v4.9.227
static int build_request(struct ceph_auth_client *ac, void *buf, void *end)
{
	return 0;
}

/*
 * the generic auth code decode the global_id, and we carry no actual
 * authenticate state, so nothing happens here.
 */
static int handle_reply(struct ceph_auth_client *ac, int result,
			void *buf, void *end)
{
	struct ceph_auth_none_info *xi = ac->private;

	xi->starting = false;
	return result;
}

<<<<<<< HEAD
/*
 * build an 'authorizer' with our entity_name and global_id.  we can
 * reuse a single static copy since it is identical for all services
 * we connect to.
=======
static void ceph_auth_none_destroy_authorizer(struct ceph_authorizer *a)
{
	kfree(a);
}

/*
 * build an 'authorizer' with our entity_name and global_id.  it is
 * identical for all services we connect to.
>>>>>>> v4.9.227
 */
static int ceph_auth_none_create_authorizer(
	struct ceph_auth_client *ac, int peer_type,
	struct ceph_auth_handshake *auth)
{
<<<<<<< HEAD
	struct ceph_auth_none_info *ai = ac->private;
	struct ceph_none_authorizer *au = &ai->au;
	void *p, *end;
	int ret;

	if (!ai->built_authorizer) {
		p = au->buf;
		end = p + sizeof(au->buf);
		ceph_encode_8(&p, 1);
		ret = ceph_entity_name_encode(ac->name, &p, end - 8);
		if (ret < 0)
			goto bad;
		ceph_decode_need(&p, end, sizeof(u64), bad2);
		ceph_encode_64(&p, ac->global_id);
		au->buf_len = p - (void *)au->buf;
		ai->built_authorizer = true;
		dout("built authorizer len %d\n", au->buf_len);
=======
	struct ceph_none_authorizer *au;
	int ret;

	au = kmalloc(sizeof(*au), GFP_NOFS);
	if (!au)
		return -ENOMEM;

	au->base.destroy = ceph_auth_none_destroy_authorizer;

	ret = ceph_auth_none_build_authorizer(ac, au);
	if (ret) {
		kfree(au);
		return ret;
>>>>>>> v4.9.227
	}

	auth->authorizer = (struct ceph_authorizer *) au;
	auth->authorizer_buf = au->buf;
	auth->authorizer_buf_len = au->buf_len;
	auth->authorizer_reply_buf = au->reply_buf;
	auth->authorizer_reply_buf_len = sizeof (au->reply_buf);

	return 0;
<<<<<<< HEAD

bad2:
	ret = -ERANGE;
bad:
	return ret;
}

static void ceph_auth_none_destroy_authorizer(struct ceph_auth_client *ac,
				      struct ceph_authorizer *a)
{
	/* nothing to do */
=======
>>>>>>> v4.9.227
}

static const struct ceph_auth_client_ops ceph_auth_none_ops = {
	.name = "none",
	.reset = reset,
	.destroy = destroy,
	.is_authenticated = is_authenticated,
	.should_authenticate = should_authenticate,
	.build_request = build_request,
	.handle_reply = handle_reply,
	.create_authorizer = ceph_auth_none_create_authorizer,
<<<<<<< HEAD
	.destroy_authorizer = ceph_auth_none_destroy_authorizer,
=======
>>>>>>> v4.9.227
};

int ceph_auth_none_init(struct ceph_auth_client *ac)
{
	struct ceph_auth_none_info *xi;

	dout("ceph_auth_none_init %p\n", ac);
	xi = kzalloc(sizeof(*xi), GFP_NOFS);
	if (!xi)
		return -ENOMEM;

	xi->starting = true;
<<<<<<< HEAD
	xi->built_authorizer = false;
=======
>>>>>>> v4.9.227

	ac->protocol = CEPH_AUTH_NONE;
	ac->private = xi;
	ac->ops = &ceph_auth_none_ops;
	return 0;
}

