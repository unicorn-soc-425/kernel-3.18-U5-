/*
<<<<<<< HEAD
 *  binfmt_misc.c
 *
 *  Copyright (C) 1997 Richard Günther
 *
 *  binfmt_misc detects binaries via a magic or filename extension and invokes
 *  a specified wrapper. This should obsolete binfmt_java, binfmt_em86 and
 *  binfmt_mz.
 *
 *  1997-04-25 first version
 *  [...]
 *  1997-05-19 cleanup
 *  1997-06-26 hpa: pass the real filename rather than argv[0]
 *  1997-06-30 minor cleanup
 *  1997-08-09 removed extension stripping, locking cleanup
 *  2001-02-28 AV: rewritten into something that resembles C. Original didn't.
 */

=======
 * binfmt_misc.c
 *
 * Copyright (C) 1997 Richard Günther
 *
 * binfmt_misc detects binaries via a magic or filename extension and invokes
 * a specified wrapper. See Documentation/binfmt_misc.txt for more details.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
>>>>>>> v4.9.227
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/magic.h>
#include <linux/binfmts.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/string_helpers.h>
#include <linux/file.h>
#include <linux/pagemap.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
<<<<<<< HEAD

#include <asm/uaccess.h>
=======
#include <linux/uaccess.h>

#include "internal.h"

#ifdef DEBUG
# define USE_DEBUG 1
#else
# define USE_DEBUG 0
#endif
>>>>>>> v4.9.227

enum {
	VERBOSE_STATUS = 1 /* make it zero to save 400 bytes kernel memory */
};

static LIST_HEAD(entries);
static int enabled = 1;

enum {Enabled, Magic};
<<<<<<< HEAD
#define MISC_FMT_PRESERVE_ARGV0 (1<<31)
#define MISC_FMT_OPEN_BINARY (1<<30)
#define MISC_FMT_CREDENTIALS (1<<29)
=======
#define MISC_FMT_PRESERVE_ARGV0 (1 << 31)
#define MISC_FMT_OPEN_BINARY (1 << 30)
#define MISC_FMT_CREDENTIALS (1 << 29)
#define MISC_FMT_OPEN_FILE (1 << 28)
>>>>>>> v4.9.227

typedef struct {
	struct list_head list;
	unsigned long flags;		/* type, status, etc. */
	int offset;			/* offset of magic */
	int size;			/* size of magic/mask */
	char *magic;			/* magic or filename extension */
	char *mask;			/* mask, NULL for exact match */
	char *interpreter;		/* filename of interpreter */
	char *name;
	struct dentry *dentry;
<<<<<<< HEAD
=======
	struct file *interp_file;
>>>>>>> v4.9.227
} Node;

static DEFINE_RWLOCK(entries_lock);
static struct file_system_type bm_fs_type;
static struct vfsmount *bm_mnt;
static int entry_count;

/*
 * Max length of the register string.  Determined by:
 *  - 7 delimiters
 *  - name:   ~50 bytes
 *  - type:   1 byte
 *  - offset: 3 bytes (has to be smaller than BINPRM_BUF_SIZE)
 *  - magic:  128 bytes (512 in escaped form)
 *  - mask:   128 bytes (512 in escaped form)
 *  - interp: ~50 bytes
 *  - flags:  5 bytes
 * Round that up a bit, and then back off to hold the internal data
 * (like struct Node).
 */
#define MAX_REGISTER_LENGTH 1920

/*
 * Check if we support the binfmt
 * if we do, return the node, else NULL
 * locking is done in load_misc_binary
 */
static Node *check_file(struct linux_binprm *bprm)
{
	char *p = strrchr(bprm->interp, '.');
	struct list_head *l;

<<<<<<< HEAD
=======
	/* Walk all the registered handlers. */
>>>>>>> v4.9.227
	list_for_each(l, &entries) {
		Node *e = list_entry(l, Node, list);
		char *s;
		int j;

<<<<<<< HEAD
		if (!test_bit(Enabled, &e->flags))
			continue;

=======
		/* Make sure this one is currently enabled. */
		if (!test_bit(Enabled, &e->flags))
			continue;

		/* Do matching based on extension if applicable. */
>>>>>>> v4.9.227
		if (!test_bit(Magic, &e->flags)) {
			if (p && !strcmp(e->magic, p + 1))
				return e;
			continue;
		}

<<<<<<< HEAD
=======
		/* Do matching based on magic & mask. */
>>>>>>> v4.9.227
		s = bprm->buf + e->offset;
		if (e->mask) {
			for (j = 0; j < e->size; j++)
				if ((*s++ ^ e->magic[j]) & e->mask[j])
					break;
		} else {
			for (j = 0; j < e->size; j++)
				if ((*s++ ^ e->magic[j]))
					break;
		}
		if (j == e->size)
			return e;
	}
	return NULL;
}

/*
 * the loader itself
 */
static int load_misc_binary(struct linux_binprm *bprm)
{
	Node *fmt;
<<<<<<< HEAD
	struct file * interp_file = NULL;
=======
	struct file *interp_file = NULL;
>>>>>>> v4.9.227
	char iname[BINPRM_BUF_SIZE];
	const char *iname_addr = iname;
	int retval;
	int fd_binary = -1;

	retval = -ENOEXEC;
	if (!enabled)
<<<<<<< HEAD
		goto _ret;
=======
		goto ret;
>>>>>>> v4.9.227

	/* to keep locking time low, we copy the interpreter string */
	read_lock(&entries_lock);
	fmt = check_file(bprm);
	if (fmt)
		strlcpy(iname, fmt->interpreter, BINPRM_BUF_SIZE);
	read_unlock(&entries_lock);
	if (!fmt)
<<<<<<< HEAD
		goto _ret;
=======
		goto ret;

	/* Need to be able to load the file after exec */
	if (bprm->interp_flags & BINPRM_FLAGS_PATH_INACCESSIBLE)
		return -ENOENT;
>>>>>>> v4.9.227

	if (!(fmt->flags & MISC_FMT_PRESERVE_ARGV0)) {
		retval = remove_arg_zero(bprm);
		if (retval)
<<<<<<< HEAD
			goto _ret;
=======
			goto ret;
>>>>>>> v4.9.227
	}

	if (fmt->flags & MISC_FMT_OPEN_BINARY) {

		/* if the binary should be opened on behalf of the
		 * interpreter than keep it open and assign descriptor
<<<<<<< HEAD
		 * to it */
 		fd_binary = get_unused_fd();
 		if (fd_binary < 0) {
 			retval = fd_binary;
 			goto _ret;
 		}
 		fd_install(fd_binary, bprm->file);
=======
		 * to it
		 */
		fd_binary = get_unused_fd_flags(0);
		if (fd_binary < 0) {
			retval = fd_binary;
			goto ret;
		}
		fd_install(fd_binary, bprm->file);
>>>>>>> v4.9.227

		/* if the binary is not readable than enforce mm->dumpable=0
		   regardless of the interpreter's permissions */
		would_dump(bprm, bprm->file);

		allow_write_access(bprm->file);
		bprm->file = NULL;

		/* mark the bprm that fd should be passed to interp */
		bprm->interp_flags |= BINPRM_FLAGS_EXECFD;
		bprm->interp_data = fd_binary;

<<<<<<< HEAD
 	} else {
 		allow_write_access(bprm->file);
 		fput(bprm->file);
 		bprm->file = NULL;
 	}
	/* make argv[1] be the path to the binary */
	retval = copy_strings_kernel (1, &bprm->interp, bprm);
	if (retval < 0)
		goto _error;
	bprm->argc++;

	/* add the interp as argv[0] */
	retval = copy_strings_kernel (1, &iname_addr, bprm);
	if (retval < 0)
		goto _error;
	bprm->argc ++;
=======
	} else {
		allow_write_access(bprm->file);
		fput(bprm->file);
		bprm->file = NULL;
	}
	/* make argv[1] be the path to the binary */
	retval = copy_strings_kernel(1, &bprm->interp, bprm);
	if (retval < 0)
		goto error;
	bprm->argc++;

	/* add the interp as argv[0] */
	retval = copy_strings_kernel(1, &iname_addr, bprm);
	if (retval < 0)
		goto error;
	bprm->argc++;
>>>>>>> v4.9.227

	/* Update interp in case binfmt_script needs it. */
	retval = bprm_change_interp(iname, bprm);
	if (retval < 0)
<<<<<<< HEAD
		goto _error;

	interp_file = open_exec (iname);
	retval = PTR_ERR (interp_file);
	if (IS_ERR (interp_file))
		goto _error;
=======
		goto error;

	if (fmt->flags & MISC_FMT_OPEN_FILE && fmt->interp_file) {
		interp_file = filp_clone_open(fmt->interp_file);
		if (!IS_ERR(interp_file))
			deny_write_access(interp_file);
	} else {
		interp_file = open_exec(iname);
	}
	retval = PTR_ERR(interp_file);
	if (IS_ERR(interp_file))
		goto error;
>>>>>>> v4.9.227

	bprm->file = interp_file;
	if (fmt->flags & MISC_FMT_CREDENTIALS) {
		/*
		 * No need to call prepare_binprm(), it's already been
		 * done.  bprm->buf is stale, update from interp_file.
		 */
		memset(bprm->buf, 0, BINPRM_BUF_SIZE);
		retval = kernel_read(bprm->file, 0, bprm->buf, BINPRM_BUF_SIZE);
	} else
<<<<<<< HEAD
		retval = prepare_binprm (bprm);

	if (retval < 0)
		goto _error;

	retval = search_binary_handler(bprm);
	if (retval < 0)
		goto _error;

_ret:
	return retval;
_error:
=======
		retval = prepare_binprm(bprm);

	if (retval < 0)
		goto error;

	retval = search_binary_handler(bprm);
	if (retval < 0)
		goto error;

ret:
	return retval;
error:
>>>>>>> v4.9.227
	if (fd_binary > 0)
		sys_close(fd_binary);
	bprm->interp_flags = 0;
	bprm->interp_data = 0;
<<<<<<< HEAD
	goto _ret;
=======
	goto ret;
>>>>>>> v4.9.227
}

/* Command parsers */

/*
 * parses and copies one argument enclosed in del from *sp to *dp,
 * recognising the \x special.
 * returns pointer to the copied argument or NULL in case of an
 * error (and sets err) or null argument length.
 */
static char *scanarg(char *s, char del)
{
	char c;

	while ((c = *s++) != del) {
		if (c == '\\' && *s == 'x') {
			s++;
			if (!isxdigit(*s++))
				return NULL;
			if (!isxdigit(*s++))
				return NULL;
		}
	}
<<<<<<< HEAD
	return s;
}

static char * check_special_flags (char * sfs, Node * e)
{
	char * p = sfs;
=======
	s[-1] ='\0';
	return s;
}

static char *check_special_flags(char *sfs, Node *e)
{
	char *p = sfs;
>>>>>>> v4.9.227
	int cont = 1;

	/* special flags */
	while (cont) {
		switch (*p) {
<<<<<<< HEAD
			case 'P':
				p++;
				e->flags |= MISC_FMT_PRESERVE_ARGV0;
				break;
			case 'O':
				p++;
				e->flags |= MISC_FMT_OPEN_BINARY;
				break;
			case 'C':
				p++;
				/* this flags also implies the
				   open-binary flag */
				e->flags |= (MISC_FMT_CREDENTIALS |
						MISC_FMT_OPEN_BINARY);
				break;
			default:
				cont = 0;
=======
		case 'P':
			pr_debug("register: flag: P (preserve argv0)\n");
			p++;
			e->flags |= MISC_FMT_PRESERVE_ARGV0;
			break;
		case 'O':
			pr_debug("register: flag: O (open binary)\n");
			p++;
			e->flags |= MISC_FMT_OPEN_BINARY;
			break;
		case 'C':
			pr_debug("register: flag: C (preserve creds)\n");
			p++;
			/* this flags also implies the
			   open-binary flag */
			e->flags |= (MISC_FMT_CREDENTIALS |
					MISC_FMT_OPEN_BINARY);
			break;
		case 'F':
			pr_debug("register: flag: F: open interpreter file now\n");
			p++;
			e->flags |= MISC_FMT_OPEN_FILE;
			break;
		default:
			cont = 0;
>>>>>>> v4.9.227
		}
	}

	return p;
}
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
/*
 * This registers a new binary format, it recognises the syntax
 * ':name:type:offset:magic:mask:interpreter:flags'
 * where the ':' is the IFS, that can be chosen with the first char
 */
static Node *create_entry(const char __user *buffer, size_t count)
{
	Node *e;
	int memsize, err;
	char *buf, *p;
	char del;

<<<<<<< HEAD
=======
	pr_debug("register: received %zu bytes\n", count);

>>>>>>> v4.9.227
	/* some sanity checks */
	err = -EINVAL;
	if ((count < 11) || (count > MAX_REGISTER_LENGTH))
		goto out;

	err = -ENOMEM;
	memsize = sizeof(Node) + count + 8;
<<<<<<< HEAD
	e = kmalloc(memsize, GFP_USER);
=======
	e = kmalloc(memsize, GFP_KERNEL);
>>>>>>> v4.9.227
	if (!e)
		goto out;

	p = buf = (char *)e + sizeof(Node);

	memset(e, 0, sizeof(Node));
	if (copy_from_user(buf, buffer, count))
<<<<<<< HEAD
		goto Efault;

	del = *p++;	/* delimeter */

	memset(buf+count, del, 8);

	e->name = p;
	p = strchr(p, del);
	if (!p)
		goto Einval;
=======
		goto efault;

	del = *p++;	/* delimeter */

	pr_debug("register: delim: %#x {%c}\n", del, del);

	/* Pad the buffer with the delim to simplify parsing below. */
	memset(buf + count, del, 8);

	/* Parse the 'name' field. */
	e->name = p;
	p = strchr(p, del);
	if (!p)
		goto einval;
>>>>>>> v4.9.227
	*p++ = '\0';
	if (!e->name[0] ||
	    !strcmp(e->name, ".") ||
	    !strcmp(e->name, "..") ||
	    strchr(e->name, '/'))
<<<<<<< HEAD
		goto Einval;
	switch (*p++) {
		case 'E': e->flags = 1<<Enabled; break;
		case 'M': e->flags = (1<<Enabled) | (1<<Magic); break;
		default: goto Einval;
	}
	if (*p++ != del)
		goto Einval;
	if (test_bit(Magic, &e->flags)) {
		char *s = strchr(p, del);
		if (!s)
			goto Einval;
=======
		goto einval;

	pr_debug("register: name: {%s}\n", e->name);

	/* Parse the 'type' field. */
	switch (*p++) {
	case 'E':
		pr_debug("register: type: E (extension)\n");
		e->flags = 1 << Enabled;
		break;
	case 'M':
		pr_debug("register: type: M (magic)\n");
		e->flags = (1 << Enabled) | (1 << Magic);
		break;
	default:
		goto einval;
	}
	if (*p++ != del)
		goto einval;

	if (test_bit(Magic, &e->flags)) {
		/* Handle the 'M' (magic) format. */
		char *s;

		/* Parse the 'offset' field. */
		s = strchr(p, del);
		if (!s)
			goto einval;
>>>>>>> v4.9.227
		*s = '\0';
		if (p != s) {
			int r = kstrtoint(p, 10, &e->offset);
			if (r != 0 || e->offset < 0)
<<<<<<< HEAD
				goto Einval;
		}
		p = s;
		if (*p++)
			goto Einval;
		e->magic = p;
		p = scanarg(p, del);
		if (!p)
			goto Einval;
		p[-1] = '\0';
		if (!e->magic[0])
			goto Einval;
		e->mask = p;
		p = scanarg(p, del);
		if (!p)
			goto Einval;
		p[-1] = '\0';
		if (!e->mask[0])
			e->mask = NULL;
		e->size = string_unescape_inplace(e->magic, UNESCAPE_HEX);
		if (e->mask &&
		    string_unescape_inplace(e->mask, UNESCAPE_HEX) != e->size)
			goto Einval;
		if (e->size > BINPRM_BUF_SIZE ||
		    BINPRM_BUF_SIZE - e->size < e->offset)
			goto Einval;
	} else {
		p = strchr(p, del);
		if (!p)
			goto Einval;
		*p++ = '\0';
		e->magic = p;
		p = strchr(p, del);
		if (!p)
			goto Einval;
		*p++ = '\0';
		if (!e->magic[0] || strchr(e->magic, '/'))
			goto Einval;
		p = strchr(p, del);
		if (!p)
			goto Einval;
		*p++ = '\0';
	}
	e->interpreter = p;
	p = strchr(p, del);
	if (!p)
		goto Einval;
	*p++ = '\0';
	if (!e->interpreter[0])
		goto Einval;


	p = check_special_flags (p, e);

	if (*p == '\n')
		p++;
	if (p != buf + count)
		goto Einval;
=======
				goto einval;
		}
		p = s;
		if (*p++)
			goto einval;
		pr_debug("register: offset: %#x\n", e->offset);

		/* Parse the 'magic' field. */
		e->magic = p;
		p = scanarg(p, del);
		if (!p)
			goto einval;
		if (!e->magic[0])
			goto einval;
		if (USE_DEBUG)
			print_hex_dump_bytes(
				KBUILD_MODNAME ": register: magic[raw]: ",
				DUMP_PREFIX_NONE, e->magic, p - e->magic);

		/* Parse the 'mask' field. */
		e->mask = p;
		p = scanarg(p, del);
		if (!p)
			goto einval;
		if (!e->mask[0]) {
			e->mask = NULL;
			pr_debug("register:  mask[raw]: none\n");
		} else if (USE_DEBUG)
			print_hex_dump_bytes(
				KBUILD_MODNAME ": register:  mask[raw]: ",
				DUMP_PREFIX_NONE, e->mask, p - e->mask);

		/*
		 * Decode the magic & mask fields.
		 * Note: while we might have accepted embedded NUL bytes from
		 * above, the unescape helpers here will stop at the first one
		 * it encounters.
		 */
		e->size = string_unescape_inplace(e->magic, UNESCAPE_HEX);
		if (e->mask &&
		    string_unescape_inplace(e->mask, UNESCAPE_HEX) != e->size)
			goto einval;
		if (e->size > BINPRM_BUF_SIZE ||
		    BINPRM_BUF_SIZE - e->size < e->offset)
			goto einval;
		pr_debug("register: magic/mask length: %i\n", e->size);
		if (USE_DEBUG) {
			print_hex_dump_bytes(
				KBUILD_MODNAME ": register: magic[decoded]: ",
				DUMP_PREFIX_NONE, e->magic, e->size);

			if (e->mask) {
				int i;
				char *masked = kmalloc(e->size, GFP_KERNEL);

				print_hex_dump_bytes(
					KBUILD_MODNAME ": register:  mask[decoded]: ",
					DUMP_PREFIX_NONE, e->mask, e->size);

				if (masked) {
					for (i = 0; i < e->size; ++i)
						masked[i] = e->magic[i] & e->mask[i];
					print_hex_dump_bytes(
						KBUILD_MODNAME ": register:  magic[masked]: ",
						DUMP_PREFIX_NONE, masked, e->size);

					kfree(masked);
				}
			}
		}
	} else {
		/* Handle the 'E' (extension) format. */

		/* Skip the 'offset' field. */
		p = strchr(p, del);
		if (!p)
			goto einval;
		*p++ = '\0';

		/* Parse the 'magic' field. */
		e->magic = p;
		p = strchr(p, del);
		if (!p)
			goto einval;
		*p++ = '\0';
		if (!e->magic[0] || strchr(e->magic, '/'))
			goto einval;
		pr_debug("register: extension: {%s}\n", e->magic);

		/* Skip the 'mask' field. */
		p = strchr(p, del);
		if (!p)
			goto einval;
		*p++ = '\0';
	}

	/* Parse the 'interpreter' field. */
	e->interpreter = p;
	p = strchr(p, del);
	if (!p)
		goto einval;
	*p++ = '\0';
	if (!e->interpreter[0])
		goto einval;
	pr_debug("register: interpreter: {%s}\n", e->interpreter);

	/* Parse the 'flags' field. */
	p = check_special_flags(p, e);
	if (*p == '\n')
		p++;
	if (p != buf + count)
		goto einval;

>>>>>>> v4.9.227
	return e;

out:
	return ERR_PTR(err);

<<<<<<< HEAD
Efault:
	kfree(e);
	return ERR_PTR(-EFAULT);
Einval:
=======
efault:
	kfree(e);
	return ERR_PTR(-EFAULT);
einval:
>>>>>>> v4.9.227
	kfree(e);
	return ERR_PTR(-EINVAL);
}

/*
 * Set status of entry/binfmt_misc:
 * '1' enables, '0' disables and '-1' clears entry/binfmt_misc
 */
static int parse_command(const char __user *buffer, size_t count)
{
	char s[4];

	if (count > 3)
		return -EINVAL;
	if (copy_from_user(s, buffer, count))
		return -EFAULT;
	if (!count)
		return 0;
<<<<<<< HEAD
	if (s[count-1] == '\n')
=======
	if (s[count - 1] == '\n')
>>>>>>> v4.9.227
		count--;
	if (count == 1 && s[0] == '0')
		return 1;
	if (count == 1 && s[0] == '1')
		return 2;
	if (count == 2 && s[0] == '-' && s[1] == '1')
		return 3;
	return -EINVAL;
}

/* generic stuff */

static void entry_status(Node *e, char *page)
{
<<<<<<< HEAD
	char *dp;
	char *status = "disabled";
	const char * flags = "flags: ";
=======
	char *dp = page;
	const char *status = "disabled";
>>>>>>> v4.9.227

	if (test_bit(Enabled, &e->flags))
		status = "enabled";

	if (!VERBOSE_STATUS) {
		sprintf(page, "%s\n", status);
		return;
	}

<<<<<<< HEAD
	sprintf(page, "%s\ninterpreter %s\n", status, e->interpreter);
	dp = page + strlen(page);

	/* print the special flags */
	sprintf (dp, "%s", flags);
	dp += strlen (flags);
	if (e->flags & MISC_FMT_PRESERVE_ARGV0) {
		*dp ++ = 'P';
	}
	if (e->flags & MISC_FMT_OPEN_BINARY) {
		*dp ++ = 'O';
	}
	if (e->flags & MISC_FMT_CREDENTIALS) {
		*dp ++ = 'C';
	}
	*dp ++ = '\n';

=======
	dp += sprintf(dp, "%s\ninterpreter %s\n", status, e->interpreter);

	/* print the special flags */
	dp += sprintf(dp, "flags: ");
	if (e->flags & MISC_FMT_PRESERVE_ARGV0)
		*dp++ = 'P';
	if (e->flags & MISC_FMT_OPEN_BINARY)
		*dp++ = 'O';
	if (e->flags & MISC_FMT_CREDENTIALS)
		*dp++ = 'C';
	if (e->flags & MISC_FMT_OPEN_FILE)
		*dp++ = 'F';
	*dp++ = '\n';
>>>>>>> v4.9.227

	if (!test_bit(Magic, &e->flags)) {
		sprintf(dp, "extension .%s\n", e->magic);
	} else {
<<<<<<< HEAD
		int i;

		sprintf(dp, "offset %i\nmagic ", e->offset);
		dp = page + strlen(page);
		for (i = 0; i < e->size; i++) {
			sprintf(dp, "%02x", 0xff & (int) (e->magic[i]));
			dp += 2;
		}
		if (e->mask) {
			sprintf(dp, "\nmask ");
			dp += 6;
			for (i = 0; i < e->size; i++) {
				sprintf(dp, "%02x", 0xff & (int) (e->mask[i]));
				dp += 2;
			}
=======
		dp += sprintf(dp, "offset %i\nmagic ", e->offset);
		dp = bin2hex(dp, e->magic, e->size);
		if (e->mask) {
			dp += sprintf(dp, "\nmask ");
			dp = bin2hex(dp, e->mask, e->size);
>>>>>>> v4.9.227
		}
		*dp++ = '\n';
		*dp = '\0';
	}
}

static struct inode *bm_get_inode(struct super_block *sb, int mode)
{
<<<<<<< HEAD
	struct inode * inode = new_inode(sb);
=======
	struct inode *inode = new_inode(sb);
>>>>>>> v4.9.227

	if (inode) {
		inode->i_ino = get_next_ino();
		inode->i_mode = mode;
		inode->i_atime = inode->i_mtime = inode->i_ctime =
<<<<<<< HEAD
			current_fs_time(inode->i_sb);
=======
			current_time(inode);
>>>>>>> v4.9.227
	}
	return inode;
}

static void bm_evict_inode(struct inode *inode)
{
	clear_inode(inode);
	kfree(inode->i_private);
}

static void kill_node(Node *e)
{
	struct dentry *dentry;

	write_lock(&entries_lock);
	dentry = e->dentry;
	if (dentry) {
		list_del_init(&e->list);
		e->dentry = NULL;
	}
	write_unlock(&entries_lock);

<<<<<<< HEAD
	if (dentry) {
		drop_nlink(dentry->d_inode);
=======
	if ((e->flags & MISC_FMT_OPEN_FILE) && e->interp_file) {
		filp_close(e->interp_file, NULL);
		e->interp_file = NULL;
	}

	if (dentry) {
		drop_nlink(d_inode(dentry));
>>>>>>> v4.9.227
		d_drop(dentry);
		dput(dentry);
		simple_release_fs(&bm_mnt, &entry_count);
	}
}

/* /<entry> */

static ssize_t
<<<<<<< HEAD
bm_entry_read(struct file * file, char __user * buf, size_t nbytes, loff_t *ppos)
=======
bm_entry_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
>>>>>>> v4.9.227
{
	Node *e = file_inode(file)->i_private;
	ssize_t res;
	char *page;

<<<<<<< HEAD
	if (!(page = (char*) __get_free_page(GFP_KERNEL)))
=======
	page = (char *) __get_free_page(GFP_KERNEL);
	if (!page)
>>>>>>> v4.9.227
		return -ENOMEM;

	entry_status(e, page);

	res = simple_read_from_buffer(buf, nbytes, ppos, page, strlen(page));

	free_page((unsigned long) page);
	return res;
}

static ssize_t bm_entry_write(struct file *file, const char __user *buffer,
				size_t count, loff_t *ppos)
{
	struct dentry *root;
	Node *e = file_inode(file)->i_private;
	int res = parse_command(buffer, count);

	switch (res) {
<<<<<<< HEAD
		case 1: clear_bit(Enabled, &e->flags);
			break;
		case 2: set_bit(Enabled, &e->flags);
			break;
		case 3: root = dget(file->f_path.dentry->d_sb->s_root);
			mutex_lock(&root->d_inode->i_mutex);

			kill_node(e);

			mutex_unlock(&root->d_inode->i_mutex);
			dput(root);
			break;
		default: return res;
	}
=======
	case 1:
		/* Disable this handler. */
		clear_bit(Enabled, &e->flags);
		break;
	case 2:
		/* Enable this handler. */
		set_bit(Enabled, &e->flags);
		break;
	case 3:
		/* Delete this handler. */
		root = file_inode(file)->i_sb->s_root;
		inode_lock(d_inode(root));

		kill_node(e);

		inode_unlock(d_inode(root));
		break;
	default:
		return res;
	}

>>>>>>> v4.9.227
	return count;
}

static const struct file_operations bm_entry_operations = {
	.read		= bm_entry_read,
	.write		= bm_entry_write,
	.llseek		= default_llseek,
};

/* /register */

static ssize_t bm_register_write(struct file *file, const char __user *buffer,
			       size_t count, loff_t *ppos)
{
	Node *e;
	struct inode *inode;
<<<<<<< HEAD
	struct dentry *root, *dentry;
	struct super_block *sb = file->f_path.dentry->d_sb;
=======
	struct super_block *sb = file_inode(file)->i_sb;
	struct dentry *root = sb->s_root, *dentry;
>>>>>>> v4.9.227
	int err = 0;

	e = create_entry(buffer, count);

	if (IS_ERR(e))
		return PTR_ERR(e);

<<<<<<< HEAD
	root = dget(sb->s_root);
	mutex_lock(&root->d_inode->i_mutex);
=======
	inode_lock(d_inode(root));
>>>>>>> v4.9.227
	dentry = lookup_one_len(e->name, root, strlen(e->name));
	err = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out;

	err = -EEXIST;
<<<<<<< HEAD
	if (dentry->d_inode)
=======
	if (d_really_is_positive(dentry))
>>>>>>> v4.9.227
		goto out2;

	inode = bm_get_inode(sb, S_IFREG | 0644);

	err = -ENOMEM;
	if (!inode)
		goto out2;

	err = simple_pin_fs(&bm_fs_type, &bm_mnt, &entry_count);
	if (err) {
		iput(inode);
		inode = NULL;
		goto out2;
	}

<<<<<<< HEAD
=======
	if (e->flags & MISC_FMT_OPEN_FILE) {
		struct file *f;

		f = open_exec(e->interpreter);
		if (IS_ERR(f)) {
			err = PTR_ERR(f);
			pr_notice("register: failed to install interpreter file %s\n", e->interpreter);
			simple_release_fs(&bm_mnt, &entry_count);
			iput(inode);
			inode = NULL;
			goto out2;
		}
		e->interp_file = f;
	}

>>>>>>> v4.9.227
	e->dentry = dget(dentry);
	inode->i_private = e;
	inode->i_fop = &bm_entry_operations;

	d_instantiate(dentry, inode);
	write_lock(&entries_lock);
	list_add(&e->list, &entries);
	write_unlock(&entries_lock);

	err = 0;
out2:
	dput(dentry);
out:
<<<<<<< HEAD
	mutex_unlock(&root->d_inode->i_mutex);
	dput(root);

	if (err) {
		kfree(e);
		return -EINVAL;
=======
	inode_unlock(d_inode(root));

	if (err) {
		kfree(e);
		return err;
>>>>>>> v4.9.227
	}
	return count;
}

static const struct file_operations bm_register_operations = {
	.write		= bm_register_write,
	.llseek		= noop_llseek,
};

/* /status */

static ssize_t
bm_status_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	char *s = enabled ? "enabled\n" : "disabled\n";

	return simple_read_from_buffer(buf, nbytes, ppos, s, strlen(s));
}

<<<<<<< HEAD
static ssize_t bm_status_write(struct file * file, const char __user * buffer,
=======
static ssize_t bm_status_write(struct file *file, const char __user *buffer,
>>>>>>> v4.9.227
		size_t count, loff_t *ppos)
{
	int res = parse_command(buffer, count);
	struct dentry *root;

	switch (res) {
<<<<<<< HEAD
		case 1: enabled = 0; break;
		case 2: enabled = 1; break;
		case 3: root = dget(file->f_path.dentry->d_sb->s_root);
			mutex_lock(&root->d_inode->i_mutex);

			while (!list_empty(&entries))
				kill_node(list_entry(entries.next, Node, list));

			mutex_unlock(&root->d_inode->i_mutex);
			dput(root);
			break;
		default: return res;
	}
=======
	case 1:
		/* Disable all handlers. */
		enabled = 0;
		break;
	case 2:
		/* Enable all handlers. */
		enabled = 1;
		break;
	case 3:
		/* Delete all handlers. */
		root = file_inode(file)->i_sb->s_root;
		inode_lock(d_inode(root));

		while (!list_empty(&entries))
			kill_node(list_entry(entries.next, Node, list));

		inode_unlock(d_inode(root));
		break;
	default:
		return res;
	}

>>>>>>> v4.9.227
	return count;
}

static const struct file_operations bm_status_operations = {
	.read		= bm_status_read,
	.write		= bm_status_write,
	.llseek		= default_llseek,
};

/* Superblock handling */

static const struct super_operations s_ops = {
	.statfs		= simple_statfs,
	.evict_inode	= bm_evict_inode,
};

<<<<<<< HEAD
static int bm_fill_super(struct super_block * sb, void * data, int silent)
{
=======
static int bm_fill_super(struct super_block *sb, void *data, int silent)
{
	int err;
>>>>>>> v4.9.227
	static struct tree_descr bm_files[] = {
		[2] = {"status", &bm_status_operations, S_IWUSR|S_IRUGO},
		[3] = {"register", &bm_register_operations, S_IWUSR},
		/* last one */ {""}
	};
<<<<<<< HEAD
	int err = simple_fill_super(sb, BINFMTFS_MAGIC, bm_files);
=======

	err = simple_fill_super(sb, BINFMTFS_MAGIC, bm_files);
>>>>>>> v4.9.227
	if (!err)
		sb->s_op = &s_ops;
	return err;
}

static struct dentry *bm_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	return mount_single(fs_type, flags, data, bm_fill_super);
}

static struct linux_binfmt misc_format = {
	.module = THIS_MODULE,
	.load_binary = load_misc_binary,
};

static struct file_system_type bm_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "binfmt_misc",
	.mount		= bm_mount,
	.kill_sb	= kill_litter_super,
};
MODULE_ALIAS_FS("binfmt_misc");

static int __init init_misc_binfmt(void)
{
	int err = register_filesystem(&bm_fs_type);
	if (!err)
		insert_binfmt(&misc_format);
	return err;
}

static void __exit exit_misc_binfmt(void)
{
	unregister_binfmt(&misc_format);
	unregister_filesystem(&bm_fs_type);
}

core_initcall(init_misc_binfmt);
module_exit(exit_misc_binfmt);
MODULE_LICENSE("GPL");
