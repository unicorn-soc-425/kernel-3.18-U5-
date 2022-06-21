#ifndef _LINUX_CONTEXT_TRACKING_STATE_H
#define _LINUX_CONTEXT_TRACKING_STATE_H

#include <linux/percpu.h>
#include <linux/static_key.h>

struct context_tracking {
	/*
	 * When active is false, probes are unset in order
	 * to minimize overhead: TIF flags are cleared
	 * and calls to user_enter/exit are ignored. This
	 * may be further optimized using static keys.
	 */
	bool active;
<<<<<<< HEAD
	enum ctx_state {
		IN_KERNEL = 0,
		IN_USER,
=======
	int recursion;
	enum ctx_state {
		CONTEXT_DISABLED = -1,	/* returned by ct_state() if unknown */
		CONTEXT_KERNEL = 0,
		CONTEXT_USER,
		CONTEXT_GUEST,
>>>>>>> v4.9.227
	} state;
};

#ifdef CONFIG_CONTEXT_TRACKING
<<<<<<< HEAD
extern struct static_key context_tracking_enabled;
=======
extern struct static_key_false context_tracking_enabled;
>>>>>>> v4.9.227
DECLARE_PER_CPU(struct context_tracking, context_tracking);

static inline bool context_tracking_is_enabled(void)
{
<<<<<<< HEAD
	return static_key_false(&context_tracking_enabled);
=======
	return static_branch_unlikely(&context_tracking_enabled);
>>>>>>> v4.9.227
}

static inline bool context_tracking_cpu_is_enabled(void)
{
	return __this_cpu_read(context_tracking.active);
}

static inline bool context_tracking_in_user(void)
{
<<<<<<< HEAD
	return __this_cpu_read(context_tracking.state) == IN_USER;
=======
	return __this_cpu_read(context_tracking.state) == CONTEXT_USER;
>>>>>>> v4.9.227
}
#else
static inline bool context_tracking_in_user(void) { return false; }
static inline bool context_tracking_active(void) { return false; }
<<<<<<< HEAD
=======
static inline bool context_tracking_is_enabled(void) { return false; }
static inline bool context_tracking_cpu_is_enabled(void) { return false; }
>>>>>>> v4.9.227
#endif /* CONFIG_CONTEXT_TRACKING */

#endif
