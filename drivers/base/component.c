/*
 * Componentized device handling.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This is work in progress.  We gather up the component devices into a list,
 * and bind them when instructed.  At the moment, we're specific to the DRM
 * subsystem, and only handles one master device, but this doesn't have to be
 * the case.
 */
#include <linux/component.h>
#include <linux/device.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>

<<<<<<< HEAD
struct component_match {
	size_t alloc;
	size_t num;
	struct {
		void *data;
		int (*fn)(struct device *, void *);
	} compare[0];
=======
struct component;

struct component_match_array {
	void *data;
	int (*compare)(struct device *, void *);
	void (*release)(struct device *, void *);
	struct component *component;
	bool duplicate;
};

struct component_match {
	size_t alloc;
	size_t num;
	struct component_match_array *compare;
>>>>>>> v4.9.227
};

struct master {
	struct list_head node;
<<<<<<< HEAD
	struct list_head components;
=======
>>>>>>> v4.9.227
	bool bound;

	const struct component_master_ops *ops;
	struct device *dev;
	struct component_match *match;
};

struct component {
	struct list_head node;
<<<<<<< HEAD
	struct list_head master_node;
=======
>>>>>>> v4.9.227
	struct master *master;
	bool bound;

	const struct component_ops *ops;
	struct device *dev;
};

static DEFINE_MUTEX(component_mutex);
static LIST_HEAD(component_list);
static LIST_HEAD(masters);

static struct master *__master_find(struct device *dev,
	const struct component_master_ops *ops)
{
	struct master *m;

	list_for_each_entry(m, &masters, node)
		if (m->dev == dev && (!ops || m->ops == ops))
			return m;

	return NULL;
}

<<<<<<< HEAD
/* Attach an unattached component to a master. */
static void component_attach_master(struct master *master, struct component *c)
{
	c->master = master;

	list_add_tail(&c->master_node, &master->components);
}

/* Detach a component from a master. */
static void component_detach_master(struct master *master, struct component *c)
{
	list_del(&c->master_node);

	c->master = NULL;
}

/*
 * Add a component to a master, finding the component via the compare
 * function and compare data.  This is safe to call for duplicate matches
 * and will not result in the same component being added multiple times.
 */
int component_master_add_child(struct master *master,
	int (*compare)(struct device *, void *), void *compare_data)
{
	struct component *c;
	int ret = -ENXIO;
=======
static struct component *find_component(struct master *master,
	int (*compare)(struct device *, void *), void *compare_data)
{
	struct component *c;
>>>>>>> v4.9.227

	list_for_each_entry(c, &component_list, node) {
		if (c->master && c->master != master)
			continue;

<<<<<<< HEAD
		if (compare(c->dev, compare_data)) {
			if (!c->master)
				component_attach_master(master, c);
			ret = 0;
			break;
		}
	}

	return ret;
}
EXPORT_SYMBOL_GPL(component_master_add_child);
=======
		if (compare(c->dev, compare_data))
			return c;
	}

	return NULL;
}
>>>>>>> v4.9.227

static int find_components(struct master *master)
{
	struct component_match *match = master->match;
	size_t i;
	int ret = 0;

<<<<<<< HEAD
	if (!match) {
		/*
		 * Search the list of components, looking for components that
		 * belong to this master, and attach them to the master.
		 */
		return master->ops->add_components(master->dev, master);
	}

=======
>>>>>>> v4.9.227
	/*
	 * Scan the array of match functions and attach
	 * any components which are found to this master.
	 */
	for (i = 0; i < match->num; i++) {
<<<<<<< HEAD
		ret = component_master_add_child(master,
						 match->compare[i].fn,
						 match->compare[i].data);
		if (ret)
			break;
=======
		struct component_match_array *mc = &match->compare[i];
		struct component *c;

		dev_dbg(master->dev, "Looking for component %zu\n", i);

		if (match->compare[i].component)
			continue;

		c = find_component(master, mc->compare, mc->data);
		if (!c) {
			ret = -ENXIO;
			break;
		}

		dev_dbg(master->dev, "found component %s, duplicate %u\n", dev_name(c->dev), !!c->master);

		/* Attach this component to the master */
		match->compare[i].duplicate = !!c->master;
		match->compare[i].component = c;
		c->master = master;
>>>>>>> v4.9.227
	}
	return ret;
}

<<<<<<< HEAD
/* Detach all attached components from this master */
static void master_remove_components(struct master *master)
{
	while (!list_empty(&master->components)) {
		struct component *c = list_first_entry(&master->components,
					struct component, master_node);

		WARN_ON(c->master != master);

		component_detach_master(master, c);
	}
=======
/* Detach component from associated master */
static void remove_component(struct master *master, struct component *c)
{
	size_t i;

	/* Detach the component from this master. */
	for (i = 0; i < master->match->num; i++)
		if (master->match->compare[i].component == c)
			master->match->compare[i].component = NULL;
>>>>>>> v4.9.227
}

/*
 * Try to bring up a master.  If component is NULL, we're interested in
 * this master, otherwise it's a component which must be present to try
 * and bring up the master.
 *
 * Returns 1 for successful bringup, 0 if not ready, or -ve errno.
 */
static int try_to_bring_up_master(struct master *master,
	struct component *component)
{
	int ret;

<<<<<<< HEAD
	if (master->bound)
		return 0;

	/*
	 * Search the list of components, looking for components that
	 * belong to this master, and attach them to the master.
	 */
	if (find_components(master)) {
		/* Failed to find all components */
		ret = 0;
		goto out;
	}

	if (component && component->master != master) {
		ret = 0;
		goto out;
	}

	if (!devres_open_group(master->dev, NULL, GFP_KERNEL)) {
		ret = -ENOMEM;
		goto out;
	}
=======
	dev_dbg(master->dev, "trying to bring up master\n");

	if (find_components(master)) {
		dev_dbg(master->dev, "master has incomplete components\n");
		return 0;
	}

	if (component && component->master != master) {
		dev_dbg(master->dev, "master is not for this component (%s)\n",
			dev_name(component->dev));
		return 0;
	}

	if (!devres_open_group(master->dev, NULL, GFP_KERNEL))
		return -ENOMEM;
>>>>>>> v4.9.227

	/* Found all components */
	ret = master->ops->bind(master->dev);
	if (ret < 0) {
		devres_release_group(master->dev, NULL);
<<<<<<< HEAD
		dev_info(master->dev, "master bind failed: %d\n", ret);
		goto out;
=======
		if (ret != -EPROBE_DEFER)
			dev_info(master->dev, "master bind failed: %d\n", ret);
		return ret;
>>>>>>> v4.9.227
	}

	master->bound = true;
	return 1;
<<<<<<< HEAD

out:
	master_remove_components(master);

	return ret;
=======
>>>>>>> v4.9.227
}

static int try_to_bring_up_masters(struct component *component)
{
	struct master *m;
	int ret = 0;

	list_for_each_entry(m, &masters, node) {
<<<<<<< HEAD
		ret = try_to_bring_up_master(m, component);
		if (ret != 0)
			break;
=======
		if (!m->bound) {
			ret = try_to_bring_up_master(m, component);
			if (ret != 0)
				break;
		}
>>>>>>> v4.9.227
	}

	return ret;
}

static void take_down_master(struct master *master)
{
	if (master->bound) {
		master->ops->unbind(master->dev);
		devres_release_group(master->dev, NULL);
		master->bound = false;
	}
<<<<<<< HEAD

	master_remove_components(master);
}

static size_t component_match_size(size_t num)
{
	return offsetof(struct component_match, compare[num]);
}

static struct component_match *component_match_realloc(struct device *dev,
	struct component_match *match, size_t num)
{
	struct component_match *new;

	if (match && match->alloc == num)
		return match;

	new = devm_kmalloc(dev, component_match_size(num), GFP_KERNEL);
	if (!new)
		return ERR_PTR(-ENOMEM);

	if (match) {
		memcpy(new, match, component_match_size(min(match->num, num)));
		devm_kfree(dev, match);
	} else {
		new->num = 0;
	}

	new->alloc = num;

	return new;
}

/*
 * Add a component to be matched.
 *
 * The match array is first created or extended if necessary.
 */
void component_match_add(struct device *dev, struct component_match **matchptr,
=======
}

static void component_match_release(struct device *master,
	struct component_match *match)
{
	unsigned int i;

	for (i = 0; i < match->num; i++) {
		struct component_match_array *mc = &match->compare[i];

		if (mc->release)
			mc->release(master, mc->data);
	}

	kfree(match->compare);
}

static void devm_component_match_release(struct device *dev, void *res)
{
	component_match_release(dev, res);
}

static int component_match_realloc(struct device *dev,
	struct component_match *match, size_t num)
{
	struct component_match_array *new;

	if (match->alloc == num)
		return 0;

	new = kmalloc_array(num, sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	if (match->compare) {
		memcpy(new, match->compare, sizeof(*new) *
					    min(match->num, num));
		kfree(match->compare);
	}
	match->compare = new;
	match->alloc = num;

	return 0;
}

/*
 * Add a component to be matched, with a release function.
 *
 * The match array is first created or extended if necessary.
 */
void component_match_add_release(struct device *master,
	struct component_match **matchptr,
	void (*release)(struct device *, void *),
>>>>>>> v4.9.227
	int (*compare)(struct device *, void *), void *compare_data)
{
	struct component_match *match = *matchptr;

	if (IS_ERR(match))
		return;

<<<<<<< HEAD
	if (!match || match->num == match->alloc) {
		size_t new_size = match ? match->alloc + 16 : 15;

		match = component_match_realloc(dev, match, new_size);

		*matchptr = match;

		if (IS_ERR(match))
			return;
	}

	match->compare[match->num].fn = compare;
	match->compare[match->num].data = compare_data;
	match->num++;
}
EXPORT_SYMBOL(component_match_add);
=======
	if (!match) {
		match = devres_alloc(devm_component_match_release,
				     sizeof(*match), GFP_KERNEL);
		if (!match) {
			*matchptr = ERR_PTR(-ENOMEM);
			return;
		}

		devres_add(master, match);

		*matchptr = match;
	}

	if (match->num == match->alloc) {
		size_t new_size = match->alloc + 16;
		int ret;

		ret = component_match_realloc(master, match, new_size);
		if (ret) {
			*matchptr = ERR_PTR(ret);
			return;
		}
	}

	match->compare[match->num].compare = compare;
	match->compare[match->num].release = release;
	match->compare[match->num].data = compare_data;
	match->compare[match->num].component = NULL;
	match->num++;
}
EXPORT_SYMBOL(component_match_add_release);

static void free_master(struct master *master)
{
	struct component_match *match = master->match;
	int i;

	list_del(&master->node);

	if (match) {
		for (i = 0; i < match->num; i++) {
			struct component *c = match->compare[i].component;
			if (c)
				c->master = NULL;
		}
	}

	kfree(master);
}
>>>>>>> v4.9.227

int component_master_add_with_match(struct device *dev,
	const struct component_master_ops *ops,
	struct component_match *match)
{
	struct master *master;
	int ret;

<<<<<<< HEAD
	if (ops->add_components && match)
		return -EINVAL;

	if (match) {
		/* Reallocate the match array for its true size */
		match = component_match_realloc(dev, match, match->num);
		if (IS_ERR(match))
			return PTR_ERR(match);
	}
=======
	/* Reallocate the match array for its true size */
	ret = component_match_realloc(dev, match, match->num);
	if (ret)
		return ret;
>>>>>>> v4.9.227

	master = kzalloc(sizeof(*master), GFP_KERNEL);
	if (!master)
		return -ENOMEM;

	master->dev = dev;
	master->ops = ops;
	master->match = match;
<<<<<<< HEAD
	INIT_LIST_HEAD(&master->components);
=======
>>>>>>> v4.9.227

	/* Add to the list of available masters. */
	mutex_lock(&component_mutex);
	list_add(&master->node, &masters);

	ret = try_to_bring_up_master(master, NULL);

<<<<<<< HEAD
	if (ret < 0) {
		/* Delete off the list if we weren't successful */
		list_del(&master->node);
		kfree(master);
	}
=======
	if (ret < 0)
		free_master(master);

>>>>>>> v4.9.227
	mutex_unlock(&component_mutex);

	return ret < 0 ? ret : 0;
}
EXPORT_SYMBOL_GPL(component_master_add_with_match);

<<<<<<< HEAD
int component_master_add(struct device *dev,
	const struct component_master_ops *ops)
{
	return component_master_add_with_match(dev, ops, NULL);
}
EXPORT_SYMBOL_GPL(component_master_add);

=======
>>>>>>> v4.9.227
void component_master_del(struct device *dev,
	const struct component_master_ops *ops)
{
	struct master *master;

	mutex_lock(&component_mutex);
	master = __master_find(dev, ops);
	if (master) {
		take_down_master(master);
<<<<<<< HEAD

		list_del(&master->node);
		kfree(master);
=======
		free_master(master);
>>>>>>> v4.9.227
	}
	mutex_unlock(&component_mutex);
}
EXPORT_SYMBOL_GPL(component_master_del);

static void component_unbind(struct component *component,
	struct master *master, void *data)
{
	WARN_ON(!component->bound);

	component->ops->unbind(component->dev, master->dev, data);
	component->bound = false;

	/* Release all resources claimed in the binding of this component */
	devres_release_group(component->dev, component);
}

void component_unbind_all(struct device *master_dev, void *data)
{
	struct master *master;
	struct component *c;
<<<<<<< HEAD
=======
	size_t i;
>>>>>>> v4.9.227

	WARN_ON(!mutex_is_locked(&component_mutex));

	master = __master_find(master_dev, NULL);
	if (!master)
		return;

<<<<<<< HEAD
	list_for_each_entry_reverse(c, &master->components, master_node)
		component_unbind(c, master, data);
=======
	/* Unbind components in reverse order */
	for (i = master->match->num; i--; )
		if (!master->match->compare[i].duplicate) {
			c = master->match->compare[i].component;
			component_unbind(c, master, data);
		}
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(component_unbind_all);

static int component_bind(struct component *component, struct master *master,
	void *data)
{
	int ret;

	/*
	 * Each component initialises inside its own devres group.
	 * This allows us to roll-back a failed component without
	 * affecting anything else.
	 */
	if (!devres_open_group(master->dev, NULL, GFP_KERNEL))
		return -ENOMEM;

	/*
	 * Also open a group for the device itself: this allows us
	 * to release the resources claimed against the sub-device
	 * at the appropriate moment.
	 */
	if (!devres_open_group(component->dev, component, GFP_KERNEL)) {
		devres_release_group(master->dev, NULL);
		return -ENOMEM;
	}

	dev_dbg(master->dev, "binding %s (ops %ps)\n",
		dev_name(component->dev), component->ops);

	ret = component->ops->bind(component->dev, master->dev, data);
	if (!ret) {
		component->bound = true;

		/*
		 * Close the component device's group so that resources
		 * allocated in the binding are encapsulated for removal
		 * at unbind.  Remove the group on the DRM device as we
		 * can clean those resources up independently.
		 */
		devres_close_group(component->dev, NULL);
		devres_remove_group(master->dev, NULL);

		dev_info(master->dev, "bound %s (ops %ps)\n",
			 dev_name(component->dev), component->ops);
	} else {
		devres_release_group(component->dev, NULL);
		devres_release_group(master->dev, NULL);

<<<<<<< HEAD
		dev_err(master->dev, "failed to bind %s (ops %ps): %d\n",
			dev_name(component->dev), component->ops, ret);
=======
		if (ret != -EPROBE_DEFER)
			dev_err(master->dev, "failed to bind %s (ops %ps): %d\n",
				dev_name(component->dev), component->ops, ret);
>>>>>>> v4.9.227
	}

	return ret;
}

int component_bind_all(struct device *master_dev, void *data)
{
	struct master *master;
	struct component *c;
<<<<<<< HEAD
=======
	size_t i;
>>>>>>> v4.9.227
	int ret = 0;

	WARN_ON(!mutex_is_locked(&component_mutex));

	master = __master_find(master_dev, NULL);
	if (!master)
		return -EINVAL;

<<<<<<< HEAD
	list_for_each_entry(c, &master->components, master_node) {
		ret = component_bind(c, master, data);
		if (ret)
			break;
	}

	if (ret != 0) {
		list_for_each_entry_continue_reverse(c, &master->components,
						     master_node)
			component_unbind(c, master, data);
=======
	/* Bind components in match order */
	for (i = 0; i < master->match->num; i++)
		if (!master->match->compare[i].duplicate) {
			c = master->match->compare[i].component;
			ret = component_bind(c, master, data);
			if (ret)
				break;
		}

	if (ret != 0) {
		for (; i > 0; i--)
			if (!master->match->compare[i - 1].duplicate) {
				c = master->match->compare[i - 1].component;
				component_unbind(c, master, data);
			}
>>>>>>> v4.9.227
	}

	return ret;
}
EXPORT_SYMBOL_GPL(component_bind_all);

int component_add(struct device *dev, const struct component_ops *ops)
{
	struct component *component;
	int ret;

	component = kzalloc(sizeof(*component), GFP_KERNEL);
	if (!component)
		return -ENOMEM;

	component->ops = ops;
	component->dev = dev;

	dev_dbg(dev, "adding component (ops %ps)\n", ops);

	mutex_lock(&component_mutex);
	list_add_tail(&component->node, &component_list);

	ret = try_to_bring_up_masters(component);
	if (ret < 0) {
<<<<<<< HEAD
=======
		if (component->master)
			remove_component(component->master, component);
>>>>>>> v4.9.227
		list_del(&component->node);

		kfree(component);
	}
	mutex_unlock(&component_mutex);

	return ret < 0 ? ret : 0;
}
EXPORT_SYMBOL_GPL(component_add);

void component_del(struct device *dev, const struct component_ops *ops)
{
	struct component *c, *component = NULL;

	mutex_lock(&component_mutex);
	list_for_each_entry(c, &component_list, node)
		if (c->dev == dev && c->ops == ops) {
			list_del(&c->node);
			component = c;
			break;
		}

<<<<<<< HEAD
	if (component && component->master)
		take_down_master(component->master);
=======
	if (component && component->master) {
		take_down_master(component->master);
		remove_component(component->master, component);
	}
>>>>>>> v4.9.227

	mutex_unlock(&component_mutex);

	WARN_ON(!component);
	kfree(component);
}
EXPORT_SYMBOL_GPL(component_del);

MODULE_LICENSE("GPL v2");
