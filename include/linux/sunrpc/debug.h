/*
 * linux/include/linux/sunrpc/debug.h
 *
 * Debugging support for sunrpc module
 *
 * Copyright (C) 1996, Olaf Kirch <okir@monad.swb.de>
 */
#ifndef _LINUX_SUNRPC_DEBUG_H_
#define _LINUX_SUNRPC_DEBUG_H_

#include <uapi/linux/sunrpc/debug.h>

<<<<<<< HEAD

/*
 * Enable RPC debugging/profiling.
 */
#ifdef CONFIG_SUNRPC_DEBUG
#define  RPC_DEBUG
#endif
#ifdef CONFIG_TRACEPOINTS
#define RPC_TRACEPOINTS
#endif
/* #define  RPC_PROFILE */

/*
 * Debugging macros etc
 */
#ifdef RPC_DEBUG
=======
/*
 * Debugging macros etc
 */
#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
>>>>>>> v4.9.227
extern unsigned int		rpc_debug;
extern unsigned int		nfs_debug;
extern unsigned int		nfsd_debug;
extern unsigned int		nlm_debug;
#endif

#define dprintk(args...)	dfprintk(FACILITY, ## args)
#define dprintk_rcu(args...)	dfprintk_rcu(FACILITY, ## args)

#undef ifdebug
<<<<<<< HEAD
#ifdef RPC_DEBUG			
=======
#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
>>>>>>> v4.9.227
# define ifdebug(fac)		if (unlikely(rpc_debug & RPCDBG_##fac))

# define dfprintk(fac, args...)	\
	do { \
		ifdebug(fac) \
			printk(KERN_DEFAULT args); \
	} while (0)

# define dfprintk_rcu(fac, args...)	\
	do { \
		ifdebug(fac) { \
			rcu_read_lock(); \
			printk(KERN_DEFAULT args); \
			rcu_read_unlock(); \
		} \
	} while (0)

# define RPC_IFDEBUG(x)		x
#else
# define ifdebug(fac)		if (0)
# define dfprintk(fac, args...)	do {} while (0)
# define dfprintk_rcu(fac, args...)	do {} while (0)
# define RPC_IFDEBUG(x)
#endif

/*
 * Sysctl interface for RPC debugging
 */
<<<<<<< HEAD
#ifdef RPC_DEBUG
void		rpc_register_sysctl(void);
void		rpc_unregister_sysctl(void);
=======

struct rpc_clnt;
struct rpc_xprt;

#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
void		rpc_register_sysctl(void);
void		rpc_unregister_sysctl(void);
void		sunrpc_debugfs_init(void);
void		sunrpc_debugfs_exit(void);
void		rpc_clnt_debugfs_register(struct rpc_clnt *);
void		rpc_clnt_debugfs_unregister(struct rpc_clnt *);
void		rpc_xprt_debugfs_register(struct rpc_xprt *);
void		rpc_xprt_debugfs_unregister(struct rpc_xprt *);
#else
static inline void
sunrpc_debugfs_init(void)
{
	return;
}

static inline void
sunrpc_debugfs_exit(void)
{
	return;
}

static inline void
rpc_clnt_debugfs_register(struct rpc_clnt *clnt)
{
	return;
}

static inline void
rpc_clnt_debugfs_unregister(struct rpc_clnt *clnt)
{
	return;
}

static inline void
rpc_xprt_debugfs_register(struct rpc_xprt *xprt)
{
	return;
}

static inline void
rpc_xprt_debugfs_unregister(struct rpc_xprt *xprt)
{
	return;
}
>>>>>>> v4.9.227
#endif

#endif /* _LINUX_SUNRPC_DEBUG_H_ */
