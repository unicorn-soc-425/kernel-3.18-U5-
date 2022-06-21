#ifndef REBOOT_H
#define REBOOT_H

<<<<<<< HEAD
=======
extern void call_with_stack(void (*fn)(void *), void *arg, void *sp);
>>>>>>> v4.9.227
extern void _soft_restart(unsigned long addr, bool disable_l2);

#endif
