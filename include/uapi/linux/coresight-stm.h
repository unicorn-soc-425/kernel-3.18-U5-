#ifndef __UAPI_CORESIGHT_STM_H_
#define __UAPI_CORESIGHT_STM_H_

<<<<<<< HEAD
enum {
	OST_ENTITY_NONE			= 0x00,
	OST_ENTITY_FTRACE_EVENTS	= 0x01,
	OST_ENTITY_TRACE_PRINTK		= 0x02,
	OST_ENTITY_TRACE_MARKER		= 0x04,
	OST_ENTITY_DEV_NODE		= 0x08,
	OST_ENTITY_DIAG			= 0xEE,
	OST_ENTITY_QVIEW		= 0xFE,
	OST_ENTITY_MAX			= 0xFF,
};

enum {
	STM_OPTION_NONE			= 0x0,
	STM_OPTION_TIMESTAMPED		= 0x08,
	STM_OPTION_GUARANTEED		= 0x80,
=======
#include <linux/const.h>

#define STM_FLAG_TIMESTAMPED   _BITUL(3)
#define STM_FLAG_GUARANTEED    _BITUL(7)

/*
 * The CoreSight STM supports guaranteed and invariant timing
 * transactions.  Guaranteed transactions are guaranteed to be
 * traced, this might involve stalling the bus or system to
 * ensure the transaction is accepted by the STM.  While invariant
 * timing transactions are not guaranteed to be traced, they
 * will take an invariant amount of time regardless of the
 * state of the STM.
 */
enum {
	STM_OPTION_GUARANTEED = 0,
	STM_OPTION_INVARIANT,
>>>>>>> v4.9.227
};

#endif
