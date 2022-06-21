#ifndef __LINUX_KBUILD_H
#define __LINUX_KBUILD_H

#define DEFINE(sym, val) \
<<<<<<< HEAD
        asm volatile("\n->" #sym " %0 " #val : : "i" (val))

#define BLANK() asm volatile("\n->" : : )
=======
	asm volatile("\n.ascii \"->" #sym " %0 " #val "\"" : : "i" (val))

#define BLANK() asm volatile("\n.ascii \"->\"" : : )
>>>>>>> v4.9.227

#define OFFSET(sym, str, mem) \
	DEFINE(sym, offsetof(struct str, mem))

#define COMMENT(x) \
<<<<<<< HEAD
	asm volatile("\n->#" x)
=======
	asm volatile("\n.ascii \"->#" x "\"")
>>>>>>> v4.9.227

#endif
