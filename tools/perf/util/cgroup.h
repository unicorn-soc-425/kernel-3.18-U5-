#ifndef __CGROUP_H__
#define __CGROUP_H__

<<<<<<< HEAD
=======
#include <linux/atomic.h>

>>>>>>> v4.9.227
struct option;

struct cgroup_sel {
	char *name;
	int fd;
<<<<<<< HEAD
	int refcnt;
=======
	atomic_t refcnt;
>>>>>>> v4.9.227
};


extern int nr_cgroups; /* number of explicit cgroups defined */
<<<<<<< HEAD
extern void close_cgroup(struct cgroup_sel *cgrp);
extern int parse_cgroups(const struct option *opt, const char *str, int unset);
=======
void close_cgroup(struct cgroup_sel *cgrp);
int parse_cgroups(const struct option *opt, const char *str, int unset);
>>>>>>> v4.9.227

#endif /* __CGROUP_H__ */
