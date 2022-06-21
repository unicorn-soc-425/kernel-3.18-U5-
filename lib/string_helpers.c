/*
 * Helpers for formatting and printing strings
 *
 * Copyright 31 August 2008 James Bottomley
 * Copyright (C) 2013, Intel Corporation
 */
<<<<<<< HEAD
=======
#include <linux/bug.h>
>>>>>>> v4.9.227
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/export.h>
#include <linux/ctype.h>
#include <linux/errno.h>
<<<<<<< HEAD
=======
#include <linux/fs.h>
#include <linux/limits.h>
#include <linux/mm.h>
#include <linux/slab.h>
>>>>>>> v4.9.227
#include <linux/string.h>
#include <linux/string_helpers.h>

/**
 * string_get_size - get the size in the specified units
<<<<<<< HEAD
 * @size:	The size to be converted
=======
 * @size:	The size to be converted in blocks
 * @blk_size:	Size of the block (use 1 for size in bytes)
>>>>>>> v4.9.227
 * @units:	units to use (powers of 1000 or 1024)
 * @buf:	buffer to format to
 * @len:	length of buffer
 *
 * This function returns a string formatted to 3 significant figures
<<<<<<< HEAD
 * giving the size in the required units.  Returns 0 on success or
 * error on failure.  @buf is always zero terminated.
 *
 */
int string_get_size(u64 size, const enum string_size_units units,
		    char *buf, int len)
{
	static const char *const units_10[] = {
		"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB", NULL
	};
	static const char *const units_2[] = {
		"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB",
		NULL
=======
 * giving the size in the required units.  @buf should have room for
 * at least 9 bytes and will always be zero terminated.
 *
 */
void string_get_size(u64 size, u64 blk_size, const enum string_size_units units,
		     char *buf, int len)
{
	static const char *const units_10[] = {
		"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"
	};
	static const char *const units_2[] = {
		"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"
>>>>>>> v4.9.227
	};
	static const char *const *const units_str[] = {
		[STRING_UNITS_10] = units_10,
		[STRING_UNITS_2] = units_2,
	};
	static const unsigned int divisor[] = {
		[STRING_UNITS_10] = 1000,
		[STRING_UNITS_2] = 1024,
	};
<<<<<<< HEAD
	int i, j;
	u64 remainder = 0, sf_cap;
	char tmp[8];

	tmp[0] = '\0';
	i = 0;
	if (size >= divisor[units]) {
		while (size >= divisor[units] && units_str[units][i]) {
			remainder = do_div(size, divisor[units]);
			i++;
		}

		sf_cap = size;
		for (j = 0; sf_cap*10 < 1000; j++)
			sf_cap *= 10;

		if (j) {
			remainder *= 1000;
			do_div(remainder, divisor[units]);
			snprintf(tmp, sizeof(tmp), ".%03lld",
				 (unsigned long long)remainder);
			tmp[j+1] = '\0';
		}
	}

	snprintf(buf, len, "%lld%s %s", (unsigned long long)size,
		 tmp, units_str[units][i]);

	return 0;
=======
	static const unsigned int rounding[] = { 500, 50, 5 };
	int i = 0, j;
	u32 remainder = 0, sf_cap;
	char tmp[8];
	const char *unit;

	tmp[0] = '\0';

	if (blk_size == 0)
		size = 0;
	if (size == 0)
		goto out;

	/* This is Napier's algorithm.  Reduce the original block size to
	 *
	 * coefficient * divisor[units]^i
	 *
	 * we do the reduction so both coefficients are just under 32 bits so
	 * that multiplying them together won't overflow 64 bits and we keep
	 * as much precision as possible in the numbers.
	 *
	 * Note: it's safe to throw away the remainders here because all the
	 * precision is in the coefficients.
	 */
	while (blk_size >> 32) {
		do_div(blk_size, divisor[units]);
		i++;
	}

	while (size >> 32) {
		do_div(size, divisor[units]);
		i++;
	}

	/* now perform the actual multiplication keeping i as the sum of the
	 * two logarithms */
	size *= blk_size;

	/* and logarithmically reduce it until it's just under the divisor */
	while (size >= divisor[units]) {
		remainder = do_div(size, divisor[units]);
		i++;
	}

	/* work out in j how many digits of precision we need from the
	 * remainder */
	sf_cap = size;
	for (j = 0; sf_cap*10 < 1000; j++)
		sf_cap *= 10;

	if (units == STRING_UNITS_2) {
		/* express the remainder as a decimal.  It's currently the
		 * numerator of a fraction whose denominator is
		 * divisor[units], which is 1 << 10 for STRING_UNITS_2 */
		remainder *= 1000;
		remainder >>= 10;
	}

	/* add a 5 to the digit below what will be printed to ensure
	 * an arithmetical round up and carry it through to size */
	remainder += rounding[j];
	if (remainder >= 1000) {
		remainder -= 1000;
		size += 1;
	}

	if (j) {
		snprintf(tmp, sizeof(tmp), ".%03u", remainder);
		tmp[j+1] = '\0';
	}

 out:
	if (i >= ARRAY_SIZE(units_2))
		unit = "UNK";
	else
		unit = units_str[units][i];

	snprintf(buf, len, "%u%s %s", (u32)size,
		 tmp, unit);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(string_get_size);

static bool unescape_space(char **src, char **dst)
{
	char *p = *dst, *q = *src;

	switch (*q) {
	case 'n':
		*p = '\n';
		break;
	case 'r':
		*p = '\r';
		break;
	case 't':
		*p = '\t';
		break;
	case 'v':
		*p = '\v';
		break;
	case 'f':
		*p = '\f';
		break;
	default:
		return false;
	}
	*dst += 1;
	*src += 1;
	return true;
}

static bool unescape_octal(char **src, char **dst)
{
	char *p = *dst, *q = *src;
	u8 num;

	if (isodigit(*q) == 0)
		return false;

	num = (*q++) & 7;
	while (num < 32 && isodigit(*q) && (q - *src < 3)) {
		num <<= 3;
		num += (*q++) & 7;
	}
	*p = num;
	*dst += 1;
	*src = q;
	return true;
}

static bool unescape_hex(char **src, char **dst)
{
	char *p = *dst, *q = *src;
	int digit;
	u8 num;

	if (*q++ != 'x')
		return false;

	num = digit = hex_to_bin(*q++);
	if (digit < 0)
		return false;

	digit = hex_to_bin(*q);
	if (digit >= 0) {
		q++;
		num = (num << 4) | digit;
	}
	*p = num;
	*dst += 1;
	*src = q;
	return true;
}

static bool unescape_special(char **src, char **dst)
{
	char *p = *dst, *q = *src;

	switch (*q) {
	case '\"':
		*p = '\"';
		break;
	case '\\':
		*p = '\\';
		break;
	case 'a':
		*p = '\a';
		break;
	case 'e':
		*p = '\e';
		break;
	default:
		return false;
	}
	*dst += 1;
	*src += 1;
	return true;
}

/**
 * string_unescape - unquote characters in the given string
 * @src:	source buffer (escaped)
 * @dst:	destination buffer (unescaped)
 * @size:	size of the destination buffer (0 to unlimit)
 * @flags:	combination of the flags (bitwise OR):
 *	%UNESCAPE_SPACE:
 *		'\f' - form feed
 *		'\n' - new line
 *		'\r' - carriage return
 *		'\t' - horizontal tab
 *		'\v' - vertical tab
 *	%UNESCAPE_OCTAL:
 *		'\NNN' - byte with octal value NNN (1 to 3 digits)
 *	%UNESCAPE_HEX:
 *		'\xHH' - byte with hexadecimal value HH (1 to 2 digits)
 *	%UNESCAPE_SPECIAL:
 *		'\"' - double quote
 *		'\\' - backslash
 *		'\a' - alert (BEL)
 *		'\e' - escape
 *	%UNESCAPE_ANY:
 *		all previous together
 *
 * Description:
 * The function unquotes characters in the given string.
 *
 * Because the size of the output will be the same as or less than the size of
 * the input, the transformation may be performed in place.
 *
 * Caller must provide valid source and destination pointers. Be aware that
 * destination buffer will always be NULL-terminated. Source string must be
 * NULL-terminated as well.
 *
 * Return:
 * The amount of the characters processed to the destination buffer excluding
 * trailing '\0' is returned.
 */
int string_unescape(char *src, char *dst, size_t size, unsigned int flags)
{
	char *out = dst;

	while (*src && --size) {
		if (src[0] == '\\' && src[1] != '\0' && size > 1) {
			src++;
			size--;

			if (flags & UNESCAPE_SPACE &&
					unescape_space(&src, &out))
				continue;

			if (flags & UNESCAPE_OCTAL &&
					unescape_octal(&src, &out))
				continue;

			if (flags & UNESCAPE_HEX &&
					unescape_hex(&src, &out))
				continue;

			if (flags & UNESCAPE_SPECIAL &&
					unescape_special(&src, &out))
				continue;

			*out++ = '\\';
		}
		*out++ = *src++;
	}
	*out = '\0';

	return out - dst;
}
EXPORT_SYMBOL(string_unescape);

<<<<<<< HEAD
static int escape_passthrough(unsigned char c, char **dst, size_t *osz)
{
	char *out = *dst;

	if (*osz < 1)
		return -ENOMEM;

	*out++ = c;

	*dst = out;
	*osz -= 1;

	return 1;
}

static int escape_space(unsigned char c, char **dst, size_t *osz)
=======
static bool escape_passthrough(unsigned char c, char **dst, char *end)
{
	char *out = *dst;

	if (out < end)
		*out = c;
	*dst = out + 1;
	return true;
}

static bool escape_space(unsigned char c, char **dst, char *end)
>>>>>>> v4.9.227
{
	char *out = *dst;
	unsigned char to;

<<<<<<< HEAD
	if (*osz < 2)
		return -ENOMEM;

=======
>>>>>>> v4.9.227
	switch (c) {
	case '\n':
		to = 'n';
		break;
	case '\r':
		to = 'r';
		break;
	case '\t':
		to = 't';
		break;
	case '\v':
		to = 'v';
		break;
	case '\f':
		to = 'f';
		break;
	default:
<<<<<<< HEAD
		return 0;
	}

	*out++ = '\\';
	*out++ = to;

	*dst = out;
	*osz -= 2;

	return 1;
}

static int escape_special(unsigned char c, char **dst, size_t *osz)
=======
		return false;
	}

	if (out < end)
		*out = '\\';
	++out;
	if (out < end)
		*out = to;
	++out;

	*dst = out;
	return true;
}

static bool escape_special(unsigned char c, char **dst, char *end)
>>>>>>> v4.9.227
{
	char *out = *dst;
	unsigned char to;

<<<<<<< HEAD
	if (*osz < 2)
		return -ENOMEM;

=======
>>>>>>> v4.9.227
	switch (c) {
	case '\\':
		to = '\\';
		break;
	case '\a':
		to = 'a';
		break;
	case '\e':
		to = 'e';
		break;
	default:
<<<<<<< HEAD
		return 0;
	}

	*out++ = '\\';
	*out++ = to;

	*dst = out;
	*osz -= 2;

	return 1;
}

static int escape_null(unsigned char c, char **dst, size_t *osz)
{
	char *out = *dst;

	if (*osz < 2)
		return -ENOMEM;

	if (c)
		return 0;

	*out++ = '\\';
	*out++ = '0';

	*dst = out;
	*osz -= 2;

	return 1;
}

static int escape_octal(unsigned char c, char **dst, size_t *osz)
{
	char *out = *dst;

	if (*osz < 4)
		return -ENOMEM;

	*out++ = '\\';
	*out++ = ((c >> 6) & 0x07) + '0';
	*out++ = ((c >> 3) & 0x07) + '0';
	*out++ = ((c >> 0) & 0x07) + '0';

	*dst = out;
	*osz -= 4;

	return 1;
}

static int escape_hex(unsigned char c, char **dst, size_t *osz)
{
	char *out = *dst;

	if (*osz < 4)
		return -ENOMEM;

	*out++ = '\\';
	*out++ = 'x';
	*out++ = hex_asc_hi(c);
	*out++ = hex_asc_lo(c);

	*dst = out;
	*osz -= 4;

	return 1;
=======
		return false;
	}

	if (out < end)
		*out = '\\';
	++out;
	if (out < end)
		*out = to;
	++out;

	*dst = out;
	return true;
}

static bool escape_null(unsigned char c, char **dst, char *end)
{
	char *out = *dst;

	if (c)
		return false;

	if (out < end)
		*out = '\\';
	++out;
	if (out < end)
		*out = '0';
	++out;

	*dst = out;
	return true;
}

static bool escape_octal(unsigned char c, char **dst, char *end)
{
	char *out = *dst;

	if (out < end)
		*out = '\\';
	++out;
	if (out < end)
		*out = ((c >> 6) & 0x07) + '0';
	++out;
	if (out < end)
		*out = ((c >> 3) & 0x07) + '0';
	++out;
	if (out < end)
		*out = ((c >> 0) & 0x07) + '0';
	++out;

	*dst = out;
	return true;
}

static bool escape_hex(unsigned char c, char **dst, char *end)
{
	char *out = *dst;

	if (out < end)
		*out = '\\';
	++out;
	if (out < end)
		*out = 'x';
	++out;
	if (out < end)
		*out = hex_asc_hi(c);
	++out;
	if (out < end)
		*out = hex_asc_lo(c);
	++out;

	*dst = out;
	return true;
>>>>>>> v4.9.227
}

/**
 * string_escape_mem - quote characters in the given memory buffer
 * @src:	source buffer (unescaped)
 * @isz:	source buffer size
 * @dst:	destination buffer (escaped)
 * @osz:	destination buffer size
 * @flags:	combination of the flags (bitwise OR):
<<<<<<< HEAD
 *	%ESCAPE_SPACE:
=======
 *	%ESCAPE_SPACE: (special white space, not space itself)
>>>>>>> v4.9.227
 *		'\f' - form feed
 *		'\n' - new line
 *		'\r' - carriage return
 *		'\t' - horizontal tab
 *		'\v' - vertical tab
 *	%ESCAPE_SPECIAL:
 *		'\\' - backslash
 *		'\a' - alert (BEL)
 *		'\e' - escape
 *	%ESCAPE_NULL:
 *		'\0' - null
 *	%ESCAPE_OCTAL:
 *		'\NNN' - byte with octal value NNN (3 digits)
 *	%ESCAPE_ANY:
 *		all previous together
 *	%ESCAPE_NP:
 *		escape only non-printable characters (checked by isprint)
 *	%ESCAPE_ANY_NP:
 *		all previous together
 *	%ESCAPE_HEX:
 *		'\xHH' - byte with hexadecimal value HH (2 digits)
<<<<<<< HEAD
 * @esc:	NULL-terminated string of characters any of which, if found in
 *		the source, has to be escaped
=======
 * @only:	NULL-terminated string containing characters used to limit
 *		the selected escape class. If characters are included in @only
 *		that would not normally be escaped by the classes selected
 *		in @flags, they will be copied to @dst unescaped.
>>>>>>> v4.9.227
 *
 * Description:
 * The process of escaping byte buffer includes several parts. They are applied
 * in the following sequence.
 *	1. The character is matched to the printable class, if asked, and in
 *	   case of match it passes through to the output.
<<<<<<< HEAD
 *	2. The character is not matched to the one from @esc string and thus
 *	   must go as is to the output.
=======
 *	2. The character is not matched to the one from @only string and thus
 *	   must go as-is to the output.
>>>>>>> v4.9.227
 *	3. The character is checked if it falls into the class given by @flags.
 *	   %ESCAPE_OCTAL and %ESCAPE_HEX are going last since they cover any
 *	   character. Note that they actually can't go together, otherwise
 *	   %ESCAPE_HEX will be ignored.
 *
 * Caller must provide valid source and destination pointers. Be aware that
 * destination buffer will not be NULL-terminated, thus caller have to append
 * it if needs.
 *
 * Return:
<<<<<<< HEAD
 * The amount of the characters processed to the destination buffer, or
 * %-ENOMEM if the size of buffer is not enough to put an escaped character is
 * returned.
 *
 * Even in the case of error @dst pointer will be updated to point to the byte
 * after the last processed character.
 */
int string_escape_mem(const char *src, size_t isz, char **dst, size_t osz,
		      unsigned int flags, const char *esc)
{
	char *out = *dst, *p = out;
	bool is_dict = esc && *esc;
	int ret = 0;
=======
 * The total size of the escaped output that would be generated for
 * the given input and flags. To check whether the output was
 * truncated, compare the return value to osz. There is room left in
 * dst for a '\0' terminator if and only if ret < osz.
 */
int string_escape_mem(const char *src, size_t isz, char *dst, size_t osz,
		      unsigned int flags, const char *only)
{
	char *p = dst;
	char *end = p + osz;
	bool is_dict = only && *only;
>>>>>>> v4.9.227

	while (isz--) {
		unsigned char c = *src++;

		/*
		 * Apply rules in the following sequence:
		 *	- the character is printable, when @flags has
		 *	  %ESCAPE_NP bit set
<<<<<<< HEAD
		 *	- the @esc string is supplied and does not contain a
=======
		 *	- the @only string is supplied and does not contain a
>>>>>>> v4.9.227
		 *	  character under question
		 *	- the character doesn't fall into a class of symbols
		 *	  defined by given @flags
		 * In these cases we just pass through a character to the
		 * output buffer.
		 */
		if ((flags & ESCAPE_NP && isprint(c)) ||
<<<<<<< HEAD
		    (is_dict && !strchr(esc, c))) {
			/* do nothing */
		} else {
			if (flags & ESCAPE_SPACE) {
				ret = escape_space(c, &p, &osz);
				if (ret < 0)
					break;
				if (ret > 0)
					continue;
			}

			if (flags & ESCAPE_SPECIAL) {
				ret = escape_special(c, &p, &osz);
				if (ret < 0)
					break;
				if (ret > 0)
					continue;
			}

			if (flags & ESCAPE_NULL) {
				ret = escape_null(c, &p, &osz);
				if (ret < 0)
					break;
				if (ret > 0)
					continue;
			}

			/* ESCAPE_OCTAL and ESCAPE_HEX always go last */
			if (flags & ESCAPE_OCTAL) {
				ret = escape_octal(c, &p, &osz);
				if (ret < 0)
					break;
				continue;
			}
			if (flags & ESCAPE_HEX) {
				ret = escape_hex(c, &p, &osz);
				if (ret < 0)
					break;
				continue;
			}
		}

		ret = escape_passthrough(c, &p, &osz);
		if (ret < 0)
			break;
	}

	*dst = p;

	if (ret < 0)
		return ret;

	return p - out;
}
EXPORT_SYMBOL(string_escape_mem);
=======
		    (is_dict && !strchr(only, c))) {
			/* do nothing */
		} else {
			if (flags & ESCAPE_SPACE && escape_space(c, &p, end))
				continue;

			if (flags & ESCAPE_SPECIAL && escape_special(c, &p, end))
				continue;

			if (flags & ESCAPE_NULL && escape_null(c, &p, end))
				continue;

			/* ESCAPE_OCTAL and ESCAPE_HEX always go last */
			if (flags & ESCAPE_OCTAL && escape_octal(c, &p, end))
				continue;

			if (flags & ESCAPE_HEX && escape_hex(c, &p, end))
				continue;
		}

		escape_passthrough(c, &p, end);
	}

	return p - dst;
}
EXPORT_SYMBOL(string_escape_mem);

/*
 * Return an allocated string that has been escaped of special characters
 * and double quotes, making it safe to log in quotes.
 */
char *kstrdup_quotable(const char *src, gfp_t gfp)
{
	size_t slen, dlen;
	char *dst;
	const int flags = ESCAPE_HEX;
	const char esc[] = "\f\n\r\t\v\a\e\\\"";

	if (!src)
		return NULL;
	slen = strlen(src);

	dlen = string_escape_mem(src, slen, NULL, 0, flags, esc);
	dst = kmalloc(dlen + 1, gfp);
	if (!dst)
		return NULL;

	WARN_ON(string_escape_mem(src, slen, dst, dlen, flags, esc) != dlen);
	dst[dlen] = '\0';

	return dst;
}
EXPORT_SYMBOL_GPL(kstrdup_quotable);

/*
 * Returns allocated NULL-terminated string containing process
 * command line, with inter-argument NULLs replaced with spaces,
 * and other special characters escaped.
 */
char *kstrdup_quotable_cmdline(struct task_struct *task, gfp_t gfp)
{
	char *buffer, *quoted;
	int i, res;

	buffer = kmalloc(PAGE_SIZE, GFP_TEMPORARY);
	if (!buffer)
		return NULL;

	res = get_cmdline(task, buffer, PAGE_SIZE - 1);
	buffer[res] = '\0';

	/* Collapse trailing NULLs, leave res pointing to last non-NULL. */
	while (--res >= 0 && buffer[res] == '\0')
		;

	/* Replace inter-argument NULLs. */
	for (i = 0; i <= res; i++)
		if (buffer[i] == '\0')
			buffer[i] = ' ';

	/* Make sure result is printable. */
	quoted = kstrdup_quotable(buffer, gfp);
	kfree(buffer);
	return quoted;
}
EXPORT_SYMBOL_GPL(kstrdup_quotable_cmdline);

/*
 * Returns allocated NULL-terminated string containing pathname,
 * with special characters escaped, able to be safely logged. If
 * there is an error, the leading character will be "<".
 */
char *kstrdup_quotable_file(struct file *file, gfp_t gfp)
{
	char *temp, *pathname;

	if (!file)
		return kstrdup("<unknown>", gfp);

	/* We add 11 spaces for ' (deleted)' to be appended */
	temp = kmalloc(PATH_MAX + 11, GFP_TEMPORARY);
	if (!temp)
		return kstrdup("<no_memory>", gfp);

	pathname = file_path(file, temp, PATH_MAX + 11);
	if (IS_ERR(pathname))
		pathname = kstrdup("<too_long>", gfp);
	else
		pathname = kstrdup_quotable(pathname, gfp);

	kfree(temp);
	return pathname;
}
EXPORT_SYMBOL_GPL(kstrdup_quotable_file);
>>>>>>> v4.9.227
