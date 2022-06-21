/*
 * List of cgroup subsystems.
 *
 * DO NOT ADD ANY SUBSYSTEM WITHOUT EXPLICIT ACKS FROM CGROUP MAINTAINERS.
 */
<<<<<<< HEAD
=======

/*
 * This file *must* be included with SUBSYS() defined.
 */

>>>>>>> v4.9.227
#if IS_ENABLED(CONFIG_CPUSETS)
SUBSYS(cpuset)
#endif

#if IS_ENABLED(CONFIG_CGROUP_SCHED)
SUBSYS(cpu)
#endif

#if IS_ENABLED(CONFIG_CGROUP_CPUACCT)
SUBSYS(cpuacct)
#endif

<<<<<<< HEAD
#if IS_ENABLED(CONFIG_CGROUP_SCHEDTUNE)
SUBSYS(schedtune)
#endif

#if IS_ENABLED(CONFIG_BLK_CGROUP)
SUBSYS(blkio)
=======
#if IS_ENABLED(CONFIG_BLK_CGROUP)
SUBSYS(io)
>>>>>>> v4.9.227
#endif

#if IS_ENABLED(CONFIG_MEMCG)
SUBSYS(memory)
#endif

#if IS_ENABLED(CONFIG_CGROUP_DEVICE)
SUBSYS(devices)
#endif

#if IS_ENABLED(CONFIG_CGROUP_FREEZER)
SUBSYS(freezer)
#endif

#if IS_ENABLED(CONFIG_CGROUP_NET_CLASSID)
SUBSYS(net_cls)
#endif

#if IS_ENABLED(CONFIG_CGROUP_PERF)
SUBSYS(perf_event)
#endif

#if IS_ENABLED(CONFIG_CGROUP_NET_PRIO)
SUBSYS(net_prio)
#endif

#if IS_ENABLED(CONFIG_CGROUP_HUGETLB)
SUBSYS(hugetlb)
#endif

<<<<<<< HEAD
=======
#if IS_ENABLED(CONFIG_CGROUP_PIDS)
SUBSYS(pids)
#endif

>>>>>>> v4.9.227
/*
 * The following subsystems are not supported on the default hierarchy.
 */
#if IS_ENABLED(CONFIG_CGROUP_DEBUG)
SUBSYS(debug)
#endif
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
/*
 * DO NOT ADD ANY SUBSYSTEM WITHOUT EXPLICIT ACKS FROM CGROUP MAINTAINERS.
 */
