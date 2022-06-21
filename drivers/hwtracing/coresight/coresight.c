<<<<<<< HEAD
/* Copyright (c) 2012, 2016 The Linux Foundation. All rights reserved.
=======
/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
<<<<<<< HEAD
#include <linux/module.h>
=======
>>>>>>> v4.9.227
#include <linux/init.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/coresight.h>
#include <linux/of_platform.h>
#include <linux/delay.h>
<<<<<<< HEAD
=======
#include <linux/pm_runtime.h>
>>>>>>> v4.9.227

#include "coresight-priv.h"

static DEFINE_MUTEX(coresight_mutex);
<<<<<<< HEAD
static struct coresight_device *curr_sink;
=======

/**
 * struct coresight_node - elements of a path, from source to sink
 * @csdev:	Address of an element.
 * @link:	hook to the list.
 */
struct coresight_node {
	struct coresight_device *csdev;
	struct list_head link;
};

/*
 * When operating Coresight drivers from the sysFS interface, only a single
 * path can exist from a tracer (associated to a CPU) to a sink.
 */
static DEFINE_PER_CPU(struct list_head *, tracer_path);

/*
 * As of this writing only a single STM can be found in CS topologies.  Since
 * there is no way to know if we'll ever see more and what kind of
 * configuration they will enact, for the time being only define a single path
 * for STM.
 */
static struct list_head *stm_path;
>>>>>>> v4.9.227

static int coresight_id_match(struct device *dev, void *data)
{
	int trace_id, i_trace_id;
	struct coresight_device *csdev, *i_csdev;

	csdev = data;
	i_csdev = to_coresight_device(dev);

	/*
	 * No need to care about oneself and components that are not
	 * sources or not enabled
	 */
	if (i_csdev == csdev || !i_csdev->enable ||
	    i_csdev->type != CORESIGHT_DEV_TYPE_SOURCE)
		return 0;

	/* Get the source ID for both compoment */
	trace_id = source_ops(csdev)->trace_id(csdev);
	i_trace_id = source_ops(i_csdev)->trace_id(i_csdev);

	/* All you need is one */
	if (trace_id == i_trace_id)
		return 1;

	return 0;
}

static int coresight_source_is_unique(struct coresight_device *csdev)
{
	int trace_id = source_ops(csdev)->trace_id(csdev);

	/* this shouldn't happen */
	if (trace_id < 0)
		return 0;

	return !bus_for_each_dev(&coresight_bustype, NULL,
				 csdev, coresight_id_match);
}

<<<<<<< HEAD
static int coresight_find_link_inport(struct coresight_device *csdev)
{
	int i;
	struct coresight_device *parent;
	struct coresight_connection *conn;

	parent = container_of(csdev->path_link.next,
			      struct coresight_device, path_link);

=======
static int coresight_find_link_inport(struct coresight_device *csdev,
				      struct coresight_device *parent)
{
	int i;
	struct coresight_connection *conn;

>>>>>>> v4.9.227
	for (i = 0; i < parent->nr_outport; i++) {
		conn = &parent->conns[i];
		if (conn->child_dev == csdev)
			return conn->child_port;
	}

	dev_err(&csdev->dev, "couldn't find inport, parent: %s, child: %s\n",
		dev_name(&parent->dev), dev_name(&csdev->dev));

<<<<<<< HEAD
	return 0;
}

static int coresight_find_link_outport(struct coresight_device *csdev)
{
	int i;
	struct coresight_device *child;
	struct coresight_connection *conn;

	child = container_of(csdev->path_link.prev,
			     struct coresight_device, path_link);

=======
	return -ENODEV;
}

static int coresight_find_link_outport(struct coresight_device *csdev,
				       struct coresight_device *child)
{
	int i;
	struct coresight_connection *conn;

>>>>>>> v4.9.227
	for (i = 0; i < csdev->nr_outport; i++) {
		conn = &csdev->conns[i];
		if (conn->child_dev == child)
			return conn->outport;
	}

	dev_err(&csdev->dev, "couldn't find outport, parent: %s, child: %s\n",
		dev_name(&csdev->dev), dev_name(&child->dev));

<<<<<<< HEAD
	return 0;
}

static int coresight_enable_sink(struct coresight_device *csdev)
{
	int ret;

	if (!csdev->enable) {
		if (sink_ops(csdev)->enable) {
			ret = sink_ops(csdev)->enable(csdev);
			if (ret)
				return ret;
		}
=======
	return -ENODEV;
}

static int coresight_enable_sink(struct coresight_device *csdev, u32 mode)
{
	int ret;

	/*
	 * We need to make sure the "new" session is compatible with the
	 * existing "mode" of operation.
	 */
	if (sink_ops(csdev)->enable) {
		ret = sink_ops(csdev)->enable(csdev, mode);
		if (ret)
			return ret;
>>>>>>> v4.9.227
		csdev->enable = true;
	}

	atomic_inc(csdev->refcnt);

	return 0;
}

static void coresight_disable_sink(struct coresight_device *csdev)
{
	if (atomic_dec_return(csdev->refcnt) == 0) {
		if (sink_ops(csdev)->disable) {
			sink_ops(csdev)->disable(csdev);
			csdev->enable = false;
		}
	}
}

<<<<<<< HEAD
static int coresight_enable_link(struct coresight_device *csdev)
=======
static int coresight_enable_link(struct coresight_device *csdev,
				 struct coresight_device *parent,
				 struct coresight_device *child)
>>>>>>> v4.9.227
{
	int ret;
	int link_subtype;
	int refport, inport, outport;

<<<<<<< HEAD
	inport = coresight_find_link_inport(csdev);
	outport = coresight_find_link_outport(csdev);
=======
	if (!parent || !child)
		return -EINVAL;

	inport = coresight_find_link_inport(csdev, parent);
	outport = coresight_find_link_outport(csdev, child);
>>>>>>> v4.9.227
	link_subtype = csdev->subtype.link_subtype;

	if (link_subtype == CORESIGHT_DEV_SUBTYPE_LINK_MERG)
		refport = inport;
	else if (link_subtype == CORESIGHT_DEV_SUBTYPE_LINK_SPLIT)
		refport = outport;
	else
		refport = 0;

<<<<<<< HEAD
=======
	if (refport < 0)
		return refport;

>>>>>>> v4.9.227
	if (atomic_inc_return(&csdev->refcnt[refport]) == 1) {
		if (link_ops(csdev)->enable) {
			ret = link_ops(csdev)->enable(csdev, inport, outport);
			if (ret)
				return ret;
		}
	}

	csdev->enable = true;

	return 0;
}

<<<<<<< HEAD
static void coresight_disable_link(struct coresight_device *csdev)
=======
static void coresight_disable_link(struct coresight_device *csdev,
				   struct coresight_device *parent,
				   struct coresight_device *child)
>>>>>>> v4.9.227
{
	int i, nr_conns;
	int link_subtype;
	int refport, inport, outport;

<<<<<<< HEAD
	inport = coresight_find_link_inport(csdev);
	outport = coresight_find_link_outport(csdev);
=======
	if (!parent || !child)
		return;

	inport = coresight_find_link_inport(csdev, parent);
	outport = coresight_find_link_outport(csdev, child);
>>>>>>> v4.9.227
	link_subtype = csdev->subtype.link_subtype;

	if (link_subtype == CORESIGHT_DEV_SUBTYPE_LINK_MERG) {
		refport = inport;
		nr_conns = csdev->nr_inport;
	} else if (link_subtype == CORESIGHT_DEV_SUBTYPE_LINK_SPLIT) {
		refport = outport;
		nr_conns = csdev->nr_outport;
	} else {
		refport = 0;
		nr_conns = 1;
	}

	if (atomic_dec_return(&csdev->refcnt[refport]) == 0) {
		if (link_ops(csdev)->disable)
			link_ops(csdev)->disable(csdev, inport, outport);
	}

	for (i = 0; i < nr_conns; i++)
		if (atomic_read(&csdev->refcnt[i]) != 0)
			return;

	csdev->enable = false;
}

<<<<<<< HEAD
static int coresight_enable_source(struct coresight_device *csdev)
=======
static int coresight_enable_source(struct coresight_device *csdev, u32 mode)
>>>>>>> v4.9.227
{
	int ret;

	if (!coresight_source_is_unique(csdev)) {
		dev_warn(&csdev->dev, "traceID %d not unique\n",
			 source_ops(csdev)->trace_id(csdev));
		return -EINVAL;
	}

	if (!csdev->enable) {
		if (source_ops(csdev)->enable) {
<<<<<<< HEAD
			ret = source_ops(csdev)->enable(csdev);
=======
			ret = source_ops(csdev)->enable(csdev, NULL, mode);
>>>>>>> v4.9.227
			if (ret)
				return ret;
		}
		csdev->enable = true;
	}

	atomic_inc(csdev->refcnt);

	return 0;
}

static void coresight_disable_source(struct coresight_device *csdev)
{
	if (atomic_dec_return(csdev->refcnt) == 0) {
		if (source_ops(csdev)->disable) {
<<<<<<< HEAD
			source_ops(csdev)->disable(csdev);
=======
			source_ops(csdev)->disable(csdev, NULL);
>>>>>>> v4.9.227
			csdev->enable = false;
		}
	}
}

<<<<<<< HEAD
static int coresight_enable_path(struct list_head *path)
{
	int ret = 0;
	struct coresight_device *cd;

	/*
	 * At this point we have a full @path, from source to sink.  The
	 * sink is the first entry and the source the last one.  Go through
	 * all the components and enable them one by one.
	 */
	list_for_each_entry(cd, path, path_link) {
		if (cd == list_first_entry(path, struct coresight_device,
					   path_link)) {
			ret = coresight_enable_sink(cd);
		} else if (list_is_last(&cd->path_link, path)) {
			/*
			 * Don't enable the source just yet - this needs to
			 * happen at the very end when all links and sink
			 * along the path have been configured properly.
			 */
			;
		} else {
			ret = coresight_enable_link(cd);
		}
		if (ret)
			goto err;
	}

	return 0;
err:
	list_for_each_entry_continue_reverse(cd, path, path_link) {
		if (cd == list_first_entry(path, struct coresight_device,
					   path_link)) {
			coresight_disable_sink(cd);
		} else if (list_is_last(&cd->path_link, path)) {
			;
		} else {
			coresight_disable_link(cd);
		}
	}

	return ret;
}

static int coresight_disable_path(struct list_head *path)
{
	struct coresight_device *cd;

	list_for_each_entry_reverse(cd, path, path_link) {
		if (cd == list_first_entry(path, struct coresight_device,
					   path_link)) {
			coresight_disable_sink(cd);
		} else if (list_is_last(&cd->path_link, path)) {
			/*
			 * The source has already been stopped, no need
			 * to do it again here.
			 */
			;
		} else {
			coresight_disable_link(cd);
		}
	}

	return 0;
}

static int coresight_build_paths(struct coresight_device *csdev,
				 struct list_head *path,
				 bool enable)
{
	int i, ret = -EINVAL;
	struct coresight_connection *conn;

	list_add(&csdev->path_link, path);

	if ((csdev->type == CORESIGHT_DEV_TYPE_SINK ||
	    csdev->type == CORESIGHT_DEV_TYPE_LINKSINK) &&
	    csdev->activated) {
		if (enable)
			ret = coresight_enable_path(path);
		else
			ret = coresight_disable_path(path);
	} else {
		for (i = 0; i < csdev->nr_outport; i++) {
			conn = &csdev->conns[i];
			if (coresight_build_paths(conn->child_dev,
						    path, enable) == 0)
				ret = 0;
		}
	}

	if (list_first_entry(path, struct coresight_device, path_link) != csdev)
		dev_err(&csdev->dev, "wrong device in %s\n", __func__);

	list_del(&csdev->path_link);

	return ret;
=======
void coresight_disable_path(struct list_head *path)
{
	u32 type;
	struct coresight_node *nd;
	struct coresight_device *csdev, *parent, *child;

	list_for_each_entry(nd, path, link) {
		csdev = nd->csdev;
		type = csdev->type;

		/*
		 * ETF devices are tricky... They can be a link or a sink,
		 * depending on how they are configured.  If an ETF has been
		 * "activated" it will be configured as a sink, otherwise
		 * go ahead with the link configuration.
		 */
		if (type == CORESIGHT_DEV_TYPE_LINKSINK)
			type = (csdev == coresight_get_sink(path)) ?
						CORESIGHT_DEV_TYPE_SINK :
						CORESIGHT_DEV_TYPE_LINK;

		switch (type) {
		case CORESIGHT_DEV_TYPE_SINK:
			coresight_disable_sink(csdev);
			break;
		case CORESIGHT_DEV_TYPE_SOURCE:
			/* sources are disabled from either sysFS or Perf */
			break;
		case CORESIGHT_DEV_TYPE_LINK:
			parent = list_prev_entry(nd, link)->csdev;
			child = list_next_entry(nd, link)->csdev;
			coresight_disable_link(csdev, parent, child);
			break;
		default:
			break;
		}
	}
}

int coresight_enable_path(struct list_head *path, u32 mode)
{

	int ret = 0;
	u32 type;
	struct coresight_node *nd;
	struct coresight_device *csdev, *parent, *child;

	list_for_each_entry_reverse(nd, path, link) {
		csdev = nd->csdev;
		type = csdev->type;

		/*
		 * ETF devices are tricky... They can be a link or a sink,
		 * depending on how they are configured.  If an ETF has been
		 * "activated" it will be configured as a sink, otherwise
		 * go ahead with the link configuration.
		 */
		if (type == CORESIGHT_DEV_TYPE_LINKSINK)
			type = (csdev == coresight_get_sink(path)) ?
						CORESIGHT_DEV_TYPE_SINK :
						CORESIGHT_DEV_TYPE_LINK;

		switch (type) {
		case CORESIGHT_DEV_TYPE_SINK:
			ret = coresight_enable_sink(csdev, mode);
			/*
			 * Sink is the first component turned on. If we
			 * failed to enable the sink, there are no components
			 * that need disabling. Disabling the path here
			 * would mean we could disrupt an existing session.
			 */
			if (ret)
				goto out;
			break;
		case CORESIGHT_DEV_TYPE_SOURCE:
			/* sources are enabled from either sysFS or Perf */
			break;
		case CORESIGHT_DEV_TYPE_LINK:
			parent = list_prev_entry(nd, link)->csdev;
			child = list_next_entry(nd, link)->csdev;
			ret = coresight_enable_link(csdev, parent, child);
			if (ret)
				goto err;
			break;
		default:
			goto err;
		}
	}

out:
	return ret;
err:
	coresight_disable_path(path);
	goto out;
}

struct coresight_device *coresight_get_sink(struct list_head *path)
{
	struct coresight_device *csdev;

	if (!path)
		return NULL;

	csdev = list_last_entry(path, struct coresight_node, link)->csdev;
	if (csdev->type != CORESIGHT_DEV_TYPE_SINK &&
	    csdev->type != CORESIGHT_DEV_TYPE_LINKSINK)
		return NULL;

	return csdev;
}

/**
 * _coresight_build_path - recursively build a path from a @csdev to a sink.
 * @csdev:	The device to start from.
 * @path:	The list to add devices to.
 *
 * The tree of Coresight device is traversed until an activated sink is
 * found.  From there the sink is added to the list along with all the
 * devices that led to that point - the end result is a list from source
 * to sink. In that list the source is the first device and the sink the
 * last one.
 */
static int _coresight_build_path(struct coresight_device *csdev,
				 struct list_head *path)
{
	int i;
	bool found = false;
	struct coresight_node *node;

	/* An activated sink has been found.  Enqueue the element */
	if ((csdev->type == CORESIGHT_DEV_TYPE_SINK ||
	     csdev->type == CORESIGHT_DEV_TYPE_LINKSINK) && csdev->activated)
		goto out;

	/* Not a sink - recursively explore each port found on this element */
	for (i = 0; i < csdev->nr_outport; i++) {
		struct coresight_device *child_dev = csdev->conns[i].child_dev;

		if (child_dev && _coresight_build_path(child_dev, path) == 0) {
			found = true;
			break;
		}
	}

	if (!found)
		return -ENODEV;

out:
	/*
	 * A path from this element to a sink has been found.  The elements
	 * leading to the sink are already enqueued, all that is left to do
	 * is tell the PM runtime core we need this element and add a node
	 * for it.
	 */
	node = kzalloc(sizeof(struct coresight_node), GFP_KERNEL);
	if (!node)
		return -ENOMEM;

	node->csdev = csdev;
	list_add(&node->link, path);
	pm_runtime_get_sync(csdev->dev.parent);

	return 0;
}

struct list_head *coresight_build_path(struct coresight_device *csdev)
{
	struct list_head *path;
	int rc;

	path = kzalloc(sizeof(struct list_head), GFP_KERNEL);
	if (!path)
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(path);

	rc = _coresight_build_path(csdev, path);
	if (rc) {
		kfree(path);
		return ERR_PTR(rc);
	}

	return path;
}

/**
 * coresight_release_path - release a previously built path.
 * @path:	the path to release.
 *
 * Go through all the elements of a path and 1) removed it from the list and
 * 2) free the memory allocated for each node.
 */
void coresight_release_path(struct list_head *path)
{
	struct coresight_device *csdev;
	struct coresight_node *nd, *next;

	list_for_each_entry_safe(nd, next, path, link) {
		csdev = nd->csdev;

		pm_runtime_put_sync(csdev->dev.parent);
		list_del(&nd->link);
		kfree(nd);
	}

	kfree(path);
	path = NULL;
}

/** coresight_validate_source - make sure a source has the right credentials
 *  @csdev:	the device structure for a source.
 *  @function:	the function this was called from.
 *
 * Assumes the coresight_mutex is held.
 */
static int coresight_validate_source(struct coresight_device *csdev,
				     const char *function)
{
	u32 type, subtype;

	type = csdev->type;
	subtype = csdev->subtype.source_subtype;

	if (type != CORESIGHT_DEV_TYPE_SOURCE) {
		dev_err(&csdev->dev, "wrong device type in %s\n", function);
		return -EINVAL;
	}

	if (subtype != CORESIGHT_DEV_SUBTYPE_SOURCE_PROC &&
	    subtype != CORESIGHT_DEV_SUBTYPE_SOURCE_SOFTWARE) {
		dev_err(&csdev->dev, "wrong device subtype in %s\n", function);
		return -EINVAL;
	}

	return 0;
>>>>>>> v4.9.227
}

int coresight_enable(struct coresight_device *csdev)
{
<<<<<<< HEAD
	int ret = 0;
	LIST_HEAD(path);

	mutex_lock(&coresight_mutex);
	if (csdev->type != CORESIGHT_DEV_TYPE_SOURCE) {
		ret = -EINVAL;
		dev_err(&csdev->dev, "wrong device type in %s\n", __func__);
		goto out;
	}
	if (csdev->enable)
		goto out;

	if (coresight_build_paths(csdev, &path, true)) {
		dev_err(&csdev->dev, "building path(s) failed\n");
		goto out;
	}

	if (coresight_enable_source(csdev))
		dev_err(&csdev->dev, "source enable failed\n");
out:
	mutex_unlock(&coresight_mutex);
	return ret;
=======
	int cpu, ret = 0;
	struct list_head *path;
	enum coresight_dev_subtype_source subtype;

	subtype = csdev->subtype.source_subtype;

	mutex_lock(&coresight_mutex);

	ret = coresight_validate_source(csdev, __func__);
	if (ret)
		goto out;

	if (csdev->enable) {
		/*
		 * There could be multiple applications driving the software
		 * source. So keep the refcount for each such user when the
		 * source is already enabled.
		 */
		if (subtype == CORESIGHT_DEV_SUBTYPE_SOURCE_SOFTWARE)
			atomic_inc(csdev->refcnt);
		goto out;
	}

	path = coresight_build_path(csdev);
	if (IS_ERR(path)) {
		pr_err("building path(s) failed\n");
		ret = PTR_ERR(path);
		goto out;
	}

	ret = coresight_enable_path(path, CS_MODE_SYSFS);
	if (ret)
		goto err_path;

	ret = coresight_enable_source(csdev, CS_MODE_SYSFS);
	if (ret)
		goto err_source;

	switch (subtype) {
	case CORESIGHT_DEV_SUBTYPE_SOURCE_PROC:
		/*
		 * When working from sysFS it is important to keep track
		 * of the paths that were created so that they can be
		 * undone in 'coresight_disable()'.  Since there can only
		 * be a single session per tracer (when working from sysFS)
		 * a per-cpu variable will do just fine.
		 */
		cpu = source_ops(csdev)->cpu_id(csdev);
		per_cpu(tracer_path, cpu) = path;
		break;
	case CORESIGHT_DEV_SUBTYPE_SOURCE_SOFTWARE:
		stm_path = path;
		break;
	default:
		/* We can't be here */
		break;
	}

out:
	mutex_unlock(&coresight_mutex);
	return ret;

err_source:
	coresight_disable_path(path);

err_path:
	coresight_release_path(path);
	goto out;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(coresight_enable);

void coresight_disable(struct coresight_device *csdev)
{
<<<<<<< HEAD
	LIST_HEAD(path);

	mutex_lock(&coresight_mutex);
	if (csdev->type != CORESIGHT_DEV_TYPE_SOURCE) {
		dev_err(&csdev->dev, "wrong device type in %s\n", __func__);
		goto out;
	}
	if (!csdev->enable)
		goto out;

	coresight_disable_source(csdev);
	if (coresight_build_paths(csdev, &path, false))
		dev_err(&csdev->dev, "releasing path(s) failed\n");
=======
	int cpu, ret;
	struct list_head *path = NULL;

	mutex_lock(&coresight_mutex);

	ret = coresight_validate_source(csdev, __func__);
	if (ret)
		goto out;

	if (!csdev->enable)
		goto out;

	switch (csdev->subtype.source_subtype) {
	case CORESIGHT_DEV_SUBTYPE_SOURCE_PROC:
		cpu = source_ops(csdev)->cpu_id(csdev);
		path = per_cpu(tracer_path, cpu);
		per_cpu(tracer_path, cpu) = NULL;
		break;
	case CORESIGHT_DEV_SUBTYPE_SOURCE_SOFTWARE:
		path = stm_path;
		stm_path = NULL;
		break;
	default:
		/* We can't be here */
		break;
	}

	coresight_disable_source(csdev);
	coresight_disable_path(path);
	coresight_release_path(path);
>>>>>>> v4.9.227

out:
	mutex_unlock(&coresight_mutex);
}
EXPORT_SYMBOL_GPL(coresight_disable);

<<<<<<< HEAD
static int coresight_disable_all_source(struct device *dev, void *data)
{
	struct coresight_device *csdev;
	LIST_HEAD(path);

	csdev = to_coresight_device(dev);

	/*
	 * No need to care about components that are not sources or not enabled
	 */
	if (!csdev->enable || csdev->type != CORESIGHT_DEV_TYPE_SOURCE)
		return 0;

	coresight_disable_source(csdev);

	return 0;
}

static int coresight_toggle_source_path(struct device *dev, void *data)
{
	struct coresight_device *csdev;
	bool *enable = data;
	int ret;
	LIST_HEAD(path);

	csdev = to_coresight_device(dev);

	/*
	 * No need to care about components that are not sources or not enabled
	 */
	if (!csdev->enable || csdev->type != CORESIGHT_DEV_TYPE_SOURCE)
		return 0;

	if (*enable) {
		ret = coresight_build_paths(csdev, &path, true);
		if (ret) {
			dev_err(&csdev->dev, "building path(s) failed\n");
			return ret;
		}
	} else {
		if (coresight_build_paths(csdev, &path, false))
			dev_err(&csdev->dev, "releasing path(s) failed\n");
	}

	return 0;
}

static int coresight_switch_sink(struct coresight_device *csdev)
{
	int ret;
	LIST_HEAD(slist);
	bool enable = false;

	mutex_lock(&coresight_mutex);

	/* If curr_sink is same as new requested sink then do nothing. */
	if (curr_sink == csdev)
		goto out;

	/*
	 * If curr_sink is NULL then sink is getting set for the first time.
	 * No source should be enabled at this time.
	 */
	if (!curr_sink) {
		csdev->activated = true;
		goto out;
	}

	/* curr_sink is different from csdev */
	bus_for_each_dev(&coresight_bustype, NULL,
			 &enable, coresight_toggle_source_path);

	csdev->activated = true;
	curr_sink->activated = false;

	enable = true;
	ret = bus_for_each_dev(&coresight_bustype, NULL, &enable,
			       coresight_toggle_source_path);
	if (ret)
		goto err;
out:
	curr_sink = csdev;
	mutex_unlock(&coresight_mutex);
	return 0;

err:
	/* Disable sources */
	bus_for_each_dev(&coresight_bustype, NULL,
			 &enable, coresight_disable_all_source);

	enable = false;
	bus_for_each_dev(&coresight_bustype, NULL,
			 &enable, coresight_toggle_source_path);

	csdev->activated = false;
	curr_sink->activated = true;

	mutex_unlock(&coresight_mutex);
	return ret;
}

static ssize_t curr_sink_show(struct device *dev,
=======
static ssize_t enable_sink_show(struct device *dev,
>>>>>>> v4.9.227
				struct device_attribute *attr, char *buf)
{
	struct coresight_device *csdev = to_coresight_device(dev);

<<<<<<< HEAD
	return scnprintf(buf, PAGE_SIZE, "%u\n", (unsigned)csdev->activated);
}

static ssize_t curr_sink_store(struct device *dev,
=======
	return scnprintf(buf, PAGE_SIZE, "%u\n", csdev->activated);
}

static ssize_t enable_sink_store(struct device *dev,
>>>>>>> v4.9.227
				 struct device_attribute *attr,
				 const char *buf, size_t size)
{
	int ret;
	unsigned long val;
	struct coresight_device *csdev = to_coresight_device(dev);

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return ret;

	if (val)
<<<<<<< HEAD
		coresight_switch_sink(csdev);

	return size;
}
static DEVICE_ATTR_RW(curr_sink);

static ssize_t enable_show(struct device *dev,
=======
		csdev->activated = true;
	else
		csdev->activated = false;

	return size;

}
static DEVICE_ATTR_RW(enable_sink);

static ssize_t enable_source_show(struct device *dev,
>>>>>>> v4.9.227
				  struct device_attribute *attr, char *buf)
{
	struct coresight_device *csdev = to_coresight_device(dev);

<<<<<<< HEAD
	return scnprintf(buf, PAGE_SIZE, "%u\n", (unsigned)csdev->enable);
}

static ssize_t enable_store(struct device *dev,
=======
	return scnprintf(buf, PAGE_SIZE, "%u\n", csdev->enable);
}

static ssize_t enable_source_store(struct device *dev,
>>>>>>> v4.9.227
				   struct device_attribute *attr,
				   const char *buf, size_t size)
{
	int ret = 0;
	unsigned long val;
	struct coresight_device *csdev = to_coresight_device(dev);

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return ret;

	if (val) {
		ret = coresight_enable(csdev);
		if (ret)
			return ret;
	} else {
		coresight_disable(csdev);
	}

	return size;
}
<<<<<<< HEAD
static DEVICE_ATTR_RW(enable);

static struct attribute *coresight_sink_attrs[] = {
	&dev_attr_curr_sink.attr,
=======
static DEVICE_ATTR_RW(enable_source);

static struct attribute *coresight_sink_attrs[] = {
	&dev_attr_enable_sink.attr,
>>>>>>> v4.9.227
	NULL,
};
ATTRIBUTE_GROUPS(coresight_sink);

static struct attribute *coresight_source_attrs[] = {
<<<<<<< HEAD
	&dev_attr_enable.attr,
=======
	&dev_attr_enable_source.attr,
>>>>>>> v4.9.227
	NULL,
};
ATTRIBUTE_GROUPS(coresight_source);

static struct device_type coresight_dev_type[] = {
	{
		.name = "none",
	},
	{
		.name = "sink",
		.groups = coresight_sink_groups,
	},
	{
		.name = "link",
	},
	{
		.name = "linksink",
		.groups = coresight_sink_groups,
	},
	{
		.name = "source",
		.groups = coresight_source_groups,
	},
};

static void coresight_device_release(struct device *dev)
{
	struct coresight_device *csdev = to_coresight_device(dev);

<<<<<<< HEAD
=======
	kfree(csdev->conns);
	kfree(csdev->refcnt);
>>>>>>> v4.9.227
	kfree(csdev);
}

static int coresight_orphan_match(struct device *dev, void *data)
{
	int i;
	bool still_orphan = false;
	struct coresight_device *csdev, *i_csdev;
	struct coresight_connection *conn;

	csdev = data;
	i_csdev = to_coresight_device(dev);

	/* No need to check oneself */
	if (csdev == i_csdev)
		return 0;

	/* Move on to another component if no connection is orphan */
	if (!i_csdev->orphan)
		return 0;
	/*
	 * Circle throuch all the connection of that component.  If we find
	 * an orphan connection whose name matches @csdev, link it.
	 */
	for (i = 0; i < i_csdev->nr_outport; i++) {
		conn = &i_csdev->conns[i];

		/* We have found at least one orphan connection */
		if (conn->child_dev == NULL) {
			/* Does it match this newly added device? */
<<<<<<< HEAD
			if (!strcmp(dev_name(&csdev->dev), conn->child_name)) {
=======
			if (conn->child_name &&
			    !strcmp(dev_name(&csdev->dev), conn->child_name)) {
>>>>>>> v4.9.227
				conn->child_dev = csdev;
			} else {
				/* This component still has an orphan */
				still_orphan = true;
			}
		}
	}

	i_csdev->orphan = still_orphan;

	/*
	 * Returning '0' ensures that all known component on the
	 * bus will be checked.
	 */
	return 0;
}

static void coresight_fixup_orphan_conns(struct coresight_device *csdev)
{
	/*
	 * No need to check for a return value as orphan connection(s)
	 * are hooked-up with each newly added component.
	 */
	bus_for_each_dev(&coresight_bustype, NULL,
<<<<<<< HEAD
				 csdev, coresight_orphan_match);
=======
			 csdev, coresight_orphan_match);
>>>>>>> v4.9.227
}


static int coresight_name_match(struct device *dev, void *data)
{
	char *to_match;
	struct coresight_device *i_csdev;

	to_match = data;
	i_csdev = to_coresight_device(dev);

<<<<<<< HEAD
	if (!strcmp(to_match, dev_name(&i_csdev->dev)))
=======
	if (to_match && !strcmp(to_match, dev_name(&i_csdev->dev)))
>>>>>>> v4.9.227
		return 1;

	return 0;
}

static void coresight_fixup_device_conns(struct coresight_device *csdev)
{
	int i;
	struct device *dev = NULL;
	struct coresight_connection *conn;

	for (i = 0; i < csdev->nr_outport; i++) {
		conn = &csdev->conns[i];
		dev = bus_find_device(&coresight_bustype, NULL,
				      (void *)conn->child_name,
				      coresight_name_match);

		if (dev) {
			conn->child_dev = to_coresight_device(dev);
<<<<<<< HEAD
=======
			/* and put reference from 'bus_find_device()' */
			put_device(dev);
>>>>>>> v4.9.227
		} else {
			csdev->orphan = true;
			conn->child_dev = NULL;
		}
	}
}

<<<<<<< HEAD
=======
static int coresight_remove_match(struct device *dev, void *data)
{
	int i;
	struct coresight_device *csdev, *iterator;
	struct coresight_connection *conn;

	csdev = data;
	iterator = to_coresight_device(dev);

	/* No need to check oneself */
	if (csdev == iterator)
		return 0;

	/*
	 * Circle throuch all the connection of that component.  If we find
	 * a connection whose name matches @csdev, remove it.
	 */
	for (i = 0; i < iterator->nr_outport; i++) {
		conn = &iterator->conns[i];

		if (conn->child_dev == NULL)
			continue;

		if (!strcmp(dev_name(&csdev->dev), conn->child_name)) {
			iterator->orphan = true;
			conn->child_dev = NULL;
			/* No need to continue */
			break;
		}
	}

	/*
	 * Returning '0' ensures that all known component on the
	 * bus will be checked.
	 */
	return 0;
}

static void coresight_remove_conns(struct coresight_device *csdev)
{
	bus_for_each_dev(&coresight_bustype, NULL,
			 csdev, coresight_remove_match);
}

>>>>>>> v4.9.227
/**
 * coresight_timeout - loop until a bit has changed to a specific state.
 * @addr: base address of the area of interest.
 * @offset: address of a register, starting from @addr.
 * @position: the position of the bit of interest.
 * @value: the value the bit should have.
 *
 * Return: 0 as soon as the bit has taken the desired state or -EAGAIN if
 * TIMEOUT_US has elapsed, which ever happens first.
 */

int coresight_timeout(void __iomem *addr, u32 offset, int position, int value)
{
	int i;
	u32 val;

	for (i = TIMEOUT_US; i > 0; i--) {
		val = __raw_readl(addr + offset);
		/* waiting on the bit to go from 0 to 1 */
		if (value) {
			if (val & BIT(position))
				return 0;
		/* waiting on the bit to go from 1 to 0 */
		} else {
			if (!(val & BIT(position)))
				return 0;
		}

		/*
		 * Delay is arbitrary - the specification doesn't say how long
		 * we are expected to wait.  Extra check required to make sure
		 * we don't wait needlessly on the last iteration.
		 */
		if (i - 1)
			udelay(1);
	}

	return -EAGAIN;
}

struct bus_type coresight_bustype = {
	.name	= "coresight",
};

static int __init coresight_init(void)
{
	return bus_register(&coresight_bustype);
}
postcore_initcall(coresight_init);

struct coresight_device *coresight_register(struct coresight_desc *desc)
{
	int i;
	int ret;
	int link_subtype;
	int nr_refcnts = 1;
	atomic_t *refcnts = NULL;
	struct coresight_device *csdev;
<<<<<<< HEAD
	struct coresight_connection *conns;
=======
	struct coresight_connection *conns = NULL;
>>>>>>> v4.9.227

	csdev = kzalloc(sizeof(*csdev), GFP_KERNEL);
	if (!csdev) {
		ret = -ENOMEM;
		goto err_kzalloc_csdev;
	}

	if (desc->type == CORESIGHT_DEV_TYPE_LINK ||
	    desc->type == CORESIGHT_DEV_TYPE_LINKSINK) {
		link_subtype = desc->subtype.link_subtype;

		if (link_subtype == CORESIGHT_DEV_SUBTYPE_LINK_MERG)
			nr_refcnts = desc->pdata->nr_inport;
		else if (link_subtype == CORESIGHT_DEV_SUBTYPE_LINK_SPLIT)
			nr_refcnts = desc->pdata->nr_outport;
	}

	refcnts = kcalloc(nr_refcnts, sizeof(*refcnts), GFP_KERNEL);
	if (!refcnts) {
		ret = -ENOMEM;
		goto err_kzalloc_refcnts;
	}

	csdev->refcnt = refcnts;

	csdev->nr_inport = desc->pdata->nr_inport;
	csdev->nr_outport = desc->pdata->nr_outport;
<<<<<<< HEAD
	conns = kcalloc(csdev->nr_outport, sizeof(*conns), GFP_KERNEL);
	if (!conns) {
		ret = -ENOMEM;
		goto err_kzalloc_conns;
	}

	for (i = 0; i < csdev->nr_outport; i++) {
		conns[i].outport = desc->pdata->outports[i];
		conns[i].child_name = desc->pdata->child_names[i];
		conns[i].child_port = desc->pdata->child_ports[i];
=======

	/* Initialise connections if there is at least one outport */
	if (csdev->nr_outport) {
		conns = kcalloc(csdev->nr_outport, sizeof(*conns), GFP_KERNEL);
		if (!conns) {
			ret = -ENOMEM;
			goto err_kzalloc_conns;
		}

		for (i = 0; i < csdev->nr_outport; i++) {
			conns[i].outport = desc->pdata->outports[i];
			conns[i].child_name = desc->pdata->child_names[i];
			conns[i].child_port = desc->pdata->child_ports[i];
		}
>>>>>>> v4.9.227
	}

	csdev->conns = conns;

	csdev->type = desc->type;
	csdev->subtype = desc->subtype;
	csdev->ops = desc->ops;
	csdev->orphan = false;

	csdev->dev.type = &coresight_dev_type[desc->type];
	csdev->dev.groups = desc->groups;
	csdev->dev.parent = desc->dev;
	csdev->dev.release = coresight_device_release;
	csdev->dev.bus = &coresight_bustype;
	dev_set_name(&csdev->dev, "%s", desc->pdata->name);

	ret = device_register(&csdev->dev);
	if (ret)
		goto err_device_register;

	mutex_lock(&coresight_mutex);

	coresight_fixup_device_conns(csdev);
	coresight_fixup_orphan_conns(csdev);

<<<<<<< HEAD
	if (csdev->type == CORESIGHT_DEV_TYPE_SINK ||
	    csdev->type == CORESIGHT_DEV_TYPE_LINKSINK) {
		if (desc->pdata->default_sink) {
			if (curr_sink) {
				dev_warn(&csdev->dev,
					 "overwritting curr sink %s",
					 dev_name(&curr_sink->dev));
				curr_sink->activated = false;
			}

			curr_sink = csdev;
			curr_sink->activated = true;
		}
	}

=======
>>>>>>> v4.9.227
	mutex_unlock(&coresight_mutex);

	return csdev;

err_device_register:
	kfree(conns);
err_kzalloc_conns:
	kfree(refcnts);
err_kzalloc_refcnts:
	kfree(csdev);
err_kzalloc_csdev:
	return ERR_PTR(ret);
}
EXPORT_SYMBOL_GPL(coresight_register);

void coresight_unregister(struct coresight_device *csdev)
{
<<<<<<< HEAD
	mutex_lock(&coresight_mutex);

	kfree(csdev->conns);
	device_unregister(&csdev->dev);

	mutex_unlock(&coresight_mutex);
}
EXPORT_SYMBOL_GPL(coresight_unregister);

MODULE_LICENSE("GPL v2");
=======
	/* Remove references of that device in the topology */
	coresight_remove_conns(csdev);
	device_unregister(&csdev->dev);
}
EXPORT_SYMBOL_GPL(coresight_unregister);
>>>>>>> v4.9.227
