#ifndef _IP_SET_COMMENT_H
#define _IP_SET_COMMENT_H

/* Copyright (C) 2013 Oliver Smith <oliver@8.c.9.b.0.7.4.0.1.0.0.2.ip6.arpa>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifdef __KERNEL__

static inline char*
ip_set_comment_uget(struct nlattr *tb)
{
	return nla_data(tb);
}

<<<<<<< HEAD
=======
/* Called from uadd only, protected by the set spinlock.
 * The kadt functions don't use the comment extensions in any way.
 */
>>>>>>> v4.9.227
static inline void
ip_set_init_comment(struct ip_set_comment *comment,
		    const struct ip_set_ext *ext)
{
<<<<<<< HEAD
	size_t len = ext->comment ? strlen(ext->comment) : 0;

	if (unlikely(comment->str)) {
		kfree(comment->str);
		comment->str = NULL;
=======
	struct ip_set_comment_rcu *c = rcu_dereference_protected(comment->c, 1);
	size_t len = ext->comment ? strlen(ext->comment) : 0;

	if (unlikely(c)) {
		kfree_rcu(c, rcu);
		rcu_assign_pointer(comment->c, NULL);
>>>>>>> v4.9.227
	}
	if (!len)
		return;
	if (unlikely(len > IPSET_MAX_COMMENT_SIZE))
		len = IPSET_MAX_COMMENT_SIZE;
<<<<<<< HEAD
	comment->str = kzalloc(len + 1, GFP_ATOMIC);
	if (unlikely(!comment->str))
		return;
	strlcpy(comment->str, ext->comment, len + 1);
}

static inline int
ip_set_put_comment(struct sk_buff *skb, struct ip_set_comment *comment)
{
	if (!comment->str)
		return 0;
	return nla_put_string(skb, IPSET_ATTR_COMMENT, comment->str);
}

static inline void
ip_set_comment_free(struct ip_set_comment *comment)
{
	if (unlikely(!comment->str))
		return;
	kfree(comment->str);
	comment->str = NULL;
=======
	c = kzalloc(sizeof(*c) + len + 1, GFP_ATOMIC);
	if (unlikely(!c))
		return;
	strlcpy(c->str, ext->comment, len + 1);
	rcu_assign_pointer(comment->c, c);
}

/* Used only when dumping a set, protected by rcu_read_lock() */
static inline int
ip_set_put_comment(struct sk_buff *skb, struct ip_set_comment *comment)
{
	struct ip_set_comment_rcu *c = rcu_dereference(comment->c);

	if (!c)
		return 0;
	return nla_put_string(skb, IPSET_ATTR_COMMENT, c->str);
}

/* Called from uadd/udel, flush or the garbage collectors protected
 * by the set spinlock.
 * Called when the set is destroyed and when there can't be any user
 * of the set data anymore.
 */
static inline void
ip_set_comment_free(struct ip_set_comment *comment)
{
	struct ip_set_comment_rcu *c;

	c = rcu_dereference_protected(comment->c, 1);
	if (unlikely(!c))
		return;
	kfree_rcu(c, rcu);
	rcu_assign_pointer(comment->c, NULL);
>>>>>>> v4.9.227
}

#endif
#endif
