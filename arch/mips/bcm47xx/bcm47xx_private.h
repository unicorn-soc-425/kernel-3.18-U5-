#ifndef LINUX_BCM47XX_PRIVATE_H_
#define LINUX_BCM47XX_PRIVATE_H_

<<<<<<< HEAD
=======
#ifndef pr_fmt
#define pr_fmt(fmt)		"bcm47xx: " fmt
#endif

>>>>>>> v4.9.227
#include <linux/kernel.h>

/* prom.c */
void __init bcm47xx_prom_highmem_init(void);

/* buttons.c */
int __init bcm47xx_buttons_register(void);

/* leds.c */
void __init bcm47xx_leds_register(void);

<<<<<<< HEAD
=======
/* setup.c */
void __init bcm47xx_bus_setup(void);

>>>>>>> v4.9.227
/* workarounds.c */
void __init bcm47xx_workarounds(void);

#endif
