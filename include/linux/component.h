#ifndef COMPONENT_H
#define COMPONENT_H

<<<<<<< HEAD
struct device;

struct component_ops {
	int (*bind)(struct device *, struct device *, void *);
	void (*unbind)(struct device *, struct device *, void *);
=======
#include <linux/stddef.h>

struct device;

struct component_ops {
	int (*bind)(struct device *comp, struct device *master,
		    void *master_data);
	void (*unbind)(struct device *comp, struct device *master,
		       void *master_data);
>>>>>>> v4.9.227
};

int component_add(struct device *, const struct component_ops *);
void component_del(struct device *, const struct component_ops *);

<<<<<<< HEAD
int component_bind_all(struct device *, void *);
void component_unbind_all(struct device *, void *);
=======
int component_bind_all(struct device *master, void *master_data);
void component_unbind_all(struct device *master, void *master_data);
>>>>>>> v4.9.227

struct master;

struct component_master_ops {
<<<<<<< HEAD
	int (*add_components)(struct device *, struct master *);
	int (*bind)(struct device *);
	void (*unbind)(struct device *);
};

int component_master_add(struct device *, const struct component_master_ops *);
void component_master_del(struct device *,
	const struct component_master_ops *);

int component_master_add_child(struct master *master,
	int (*compare)(struct device *, void *), void *compare_data);

=======
	int (*bind)(struct device *master);
	void (*unbind)(struct device *master);
};

void component_master_del(struct device *,
	const struct component_master_ops *);

>>>>>>> v4.9.227
struct component_match;

int component_master_add_with_match(struct device *,
	const struct component_master_ops *, struct component_match *);
<<<<<<< HEAD
void component_match_add(struct device *, struct component_match **,
	int (*compare)(struct device *, void *), void *compare_data);

=======
void component_match_add_release(struct device *master,
	struct component_match **matchptr,
	void (*release)(struct device *, void *),
	int (*compare)(struct device *, void *), void *compare_data);

static inline void component_match_add(struct device *master,
	struct component_match **matchptr,
	int (*compare)(struct device *, void *), void *compare_data)
{
	component_match_add_release(master, matchptr, NULL, compare,
				    compare_data);
}

>>>>>>> v4.9.227
#endif
