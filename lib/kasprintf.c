/*
 *  linux/lib/kasprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <stdarg.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/string.h>

/* Simplified asprintf. */
char *kvasprintf(gfp_t gfp, const char *fmt, va_list ap)
{
<<<<<<< HEAD
	unsigned int len;
=======
	unsigned int first, second;
>>>>>>> v4.9.227
	char *p;
	va_list aq;

	va_copy(aq, ap);
<<<<<<< HEAD
	len = vsnprintf(NULL, 0, fmt, aq);
	va_end(aq);

	p = kmalloc_track_caller(len+1, gfp);
	if (!p)
		return NULL;

	vsnprintf(p, len+1, fmt, ap);
=======
	first = vsnprintf(NULL, 0, fmt, aq);
	va_end(aq);

	p = kmalloc_track_caller(first+1, gfp);
	if (!p)
		return NULL;

	second = vsnprintf(p, first+1, fmt, ap);
	WARN(first != second, "different return values (%u and %u) from vsnprintf(\"%s\", ...)",
	     first, second, fmt);
>>>>>>> v4.9.227

	return p;
}
EXPORT_SYMBOL(kvasprintf);

<<<<<<< HEAD
=======
/*
 * If fmt contains no % (or is exactly %s), use kstrdup_const. If fmt
 * (or the sole vararg) points to rodata, we will then save a memory
 * allocation and string copy. In any case, the return value should be
 * freed using kfree_const().
 */
const char *kvasprintf_const(gfp_t gfp, const char *fmt, va_list ap)
{
	if (!strchr(fmt, '%'))
		return kstrdup_const(fmt, gfp);
	if (!strcmp(fmt, "%s"))
		return kstrdup_const(va_arg(ap, const char*), gfp);
	return kvasprintf(gfp, fmt, ap);
}
EXPORT_SYMBOL(kvasprintf_const);

>>>>>>> v4.9.227
char *kasprintf(gfp_t gfp, const char *fmt, ...)
{
	va_list ap;
	char *p;

	va_start(ap, fmt);
	p = kvasprintf(gfp, fmt, ap);
	va_end(ap);

	return p;
}
EXPORT_SYMBOL(kasprintf);
