/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Miscellaneous ARCS PROM routines.
 *
 * Copyright (C) 1996 David S. Miller (davem@davemloft.net)
 * Copyright (C) 1999 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 1999 Silicon Graphics, Inc.
 */
<<<<<<< HEAD
=======
#include <linux/compiler.h>
>>>>>>> v4.9.227
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/irqflags.h>

#include <asm/bcache.h>

#include <asm/fw/arc/types.h>
#include <asm/sgialib.h>
#include <asm/bootinfo.h>

<<<<<<< HEAD
VOID
=======
VOID __noreturn
>>>>>>> v4.9.227
ArcHalt(VOID)
{
	bc_disable();
	local_irq_disable();
	ARC_CALL0(halt);
<<<<<<< HEAD
never:	goto never;
}

VOID
=======

	unreachable();
}

VOID __noreturn
>>>>>>> v4.9.227
ArcPowerDown(VOID)
{
	bc_disable();
	local_irq_disable();
	ARC_CALL0(pdown);
<<<<<<< HEAD
never:	goto never;
}

/* XXX is this a soft reset basically? XXX */
VOID
=======

	unreachable();
}

/* XXX is this a soft reset basically? XXX */
VOID __noreturn
>>>>>>> v4.9.227
ArcRestart(VOID)
{
	bc_disable();
	local_irq_disable();
	ARC_CALL0(restart);
<<<<<<< HEAD
never:	goto never;
}

VOID
=======

	unreachable();
}

VOID __noreturn
>>>>>>> v4.9.227
ArcReboot(VOID)
{
	bc_disable();
	local_irq_disable();
	ARC_CALL0(reboot);
<<<<<<< HEAD
never:	goto never;
}

VOID
=======

	unreachable();
}

VOID __noreturn
>>>>>>> v4.9.227
ArcEnterInteractiveMode(VOID)
{
	bc_disable();
	local_irq_disable();
	ARC_CALL0(imode);
<<<<<<< HEAD
never:	goto never;
=======

	unreachable();
>>>>>>> v4.9.227
}

LONG
ArcSaveConfiguration(VOID)
{
	return ARC_CALL0(cfg_save);
}

struct linux_sysid *
ArcGetSystemId(VOID)
{
	return (struct linux_sysid *) ARC_CALL0(get_sysid);
}

VOID __init
ArcFlushAllCaches(VOID)
{
	ARC_CALL0(cache_flush);
}

DISPLAY_STATUS * __init ArcGetDisplayStatus(ULONG FileID)
{
	return (DISPLAY_STATUS *) ARC_CALL1(GetDisplayStatus, FileID);
}
