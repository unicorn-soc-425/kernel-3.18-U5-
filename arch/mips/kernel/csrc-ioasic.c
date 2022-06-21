/*
 *  DEC I/O ASIC's counter clocksource
 *
 *  Copyright (C) 2008	Yoichi Yuasa <yuasa@linux-mips.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
<<<<<<< HEAD
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <linux/clocksource.h>
=======
 */
#include <linux/clocksource.h>
#include <linux/sched_clock.h>
>>>>>>> v4.9.227
#include <linux/init.h>

#include <asm/ds1287.h>
#include <asm/time.h>
#include <asm/dec/ioasic.h>
#include <asm/dec/ioasic_addrs.h>

static cycle_t dec_ioasic_hpt_read(struct clocksource *cs)
{
	return ioasic_read(IO_REG_FCTR);
}

static struct clocksource clocksource_dec = {
	.name		= "dec-ioasic",
	.read		= dec_ioasic_hpt_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

<<<<<<< HEAD
=======
static u64 notrace dec_ioasic_read_sched_clock(void)
{
	return ioasic_read(IO_REG_FCTR);
}

>>>>>>> v4.9.227
int __init dec_ioasic_clocksource_init(void)
{
	unsigned int freq;
	u32 start, end;
	int i = HZ / 8;

	ds1287_timer_state();
	while (!ds1287_timer_state())
		;

	start = dec_ioasic_hpt_read(&clocksource_dec);

	while (i--)
		while (!ds1287_timer_state())
			;

	end = dec_ioasic_hpt_read(&clocksource_dec);

	freq = (end - start) * 8;

	/* An early revision of the I/O ASIC didn't have the counter.  */
	if (!freq)
		return -ENXIO;

	printk(KERN_INFO "I/O ASIC clock frequency %dHz\n", freq);

	clocksource_dec.rating = 200 + freq / 10000000;
	clocksource_register_hz(&clocksource_dec, freq);
<<<<<<< HEAD
=======

	sched_clock_register(dec_ioasic_read_sched_clock, 32, freq);

>>>>>>> v4.9.227
	return 0;
}
