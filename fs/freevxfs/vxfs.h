/*
 * Copyright (c) 2000-2001 Christoph Hellwig.
<<<<<<< HEAD
=======
 * Copyright (c) 2016 Krzysztof Blaszkowski
>>>>>>> v4.9.227
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL").
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef _VXFS_SUPER_H_
#define _VXFS_SUPER_H_

/*
 * Veritas filesystem driver - superblock structure.
 *
 * This file contains the definition of the disk and core
 * superblocks of the Veritas Filesystem.
 */
#include <linux/types.h>

<<<<<<< HEAD

/*
 * Data types for use with the VxFS ondisk format.
 */
typedef	int32_t		vx_daddr_t;
typedef int32_t		vx_ino_t;

=======
>>>>>>> v4.9.227
/*
 * Superblock magic number (vxfs_super->vs_magic).
 */
#define VXFS_SUPER_MAGIC	0xa501FCF5

/*
 * The root inode.
 */
#define VXFS_ROOT_INO		2

/*
 * Num of entries in free extent array
 */
#define VXFS_NEFREE		32

<<<<<<< HEAD
=======
enum vxfs_byte_order {
	VXFS_BO_LE,
	VXFS_BO_BE,
};

typedef __u16 __bitwise __fs16;
typedef __u32 __bitwise __fs32;
typedef __u64 __bitwise __fs64;
>>>>>>> v4.9.227

/*
 * VxFS superblock (disk).
 */
struct vxfs_sb {
	/*
	 * Readonly fields for the version 1 superblock.
	 *
	 * Lots of this fields are no more used by version 2
	 * and never filesystems.
	 */
<<<<<<< HEAD
	u_int32_t	vs_magic;		/* Magic number */
	int32_t		vs_version;		/* VxFS version */
	u_int32_t	vs_ctime;		/* create time - secs */
	u_int32_t	vs_cutime;		/* create time - usecs */
	int32_t		__unused1;		/* unused */
	int32_t		__unused2;		/* unused */
	vx_daddr_t	vs_old_logstart;	/* obsolete */
	vx_daddr_t	vs_old_logend;		/* obsolete */
	int32_t		vs_bsize;		/* block size */
	int32_t		vs_size;		/* number of blocks */
	int32_t		vs_dsize;		/* number of data blocks */
	u_int32_t	vs_old_ninode;		/* obsolete */
	int32_t		vs_old_nau;		/* obsolete */
	int32_t		__unused3;		/* unused */
	int32_t		vs_old_defiextsize;	/* obsolete */
	int32_t		vs_old_ilbsize;		/* obsolete */
	int32_t		vs_immedlen;		/* size of immediate data area */
	int32_t		vs_ndaddr;		/* number of direct extentes */
	vx_daddr_t	vs_firstau;		/* address of first AU */
	vx_daddr_t	vs_emap;		/* offset of extent map in AU */
	vx_daddr_t	vs_imap;		/* offset of inode map in AU */
	vx_daddr_t	vs_iextop;		/* offset of ExtOp. map in AU */
	vx_daddr_t	vs_istart;		/* offset of inode list in AU */
	vx_daddr_t	vs_bstart;		/* offset of fdblock in AU */
	vx_daddr_t	vs_femap;		/* aufirst + emap */
	vx_daddr_t	vs_fimap;		/* aufirst + imap */
	vx_daddr_t	vs_fiextop;		/* aufirst + iextop */
	vx_daddr_t	vs_fistart;		/* aufirst + istart */
	vx_daddr_t	vs_fbstart;		/* aufirst + bstart */
	int32_t		vs_nindir;		/* number of entries in indir */
	int32_t		vs_aulen;		/* length of AU in blocks */
	int32_t		vs_auimlen;		/* length of imap in blocks */
	int32_t		vs_auemlen;		/* length of emap in blocks */
	int32_t		vs_auilen;		/* length of ilist in blocks */
	int32_t		vs_aupad;		/* length of pad in blocks */
	int32_t		vs_aublocks;		/* data blocks in AU */
	int32_t		vs_maxtier;		/* log base 2 of aublocks */
	int32_t		vs_inopb;		/* number of inodes per blk */
	int32_t		vs_old_inopau;		/* obsolete */
	int32_t		vs_old_inopilb;		/* obsolete */
	int32_t		vs_old_ndiripau;	/* obsolete */
	int32_t		vs_iaddrlen;		/* size of indirect addr ext. */
	int32_t		vs_bshift;		/* log base 2 of bsize */
	int32_t		vs_inoshift;		/* log base 2 of inobp */
	int32_t		vs_bmask;		/* ~( bsize - 1 ) */
	int32_t		vs_boffmask;		/* bsize - 1 */
	int32_t		vs_old_inomask;		/* old_inopilb - 1 */
	int32_t		vs_checksum;		/* checksum of V1 data */
=======
	__fs32		vs_magic;		/* Magic number */
	__fs32		vs_version;		/* VxFS version */
	__fs32		vs_ctime;		/* create time - secs */
	__fs32		vs_cutime;		/* create time - usecs */
	__fs32		__unused1;		/* unused */
	__fs32		__unused2;		/* unused */
	__fs32		vs_old_logstart;	/* obsolete */
	__fs32		vs_old_logend;		/* obsolete */
	__fs32		vs_bsize;		/* block size */
	__fs32		vs_size;		/* number of blocks */
	__fs32		vs_dsize;		/* number of data blocks */
	__fs32		vs_old_ninode;		/* obsolete */
	__fs32		vs_old_nau;		/* obsolete */
	__fs32		__unused3;		/* unused */
	__fs32		vs_old_defiextsize;	/* obsolete */
	__fs32		vs_old_ilbsize;		/* obsolete */
	__fs32		vs_immedlen;		/* size of immediate data area */
	__fs32		vs_ndaddr;		/* number of direct extentes */
	__fs32		vs_firstau;		/* address of first AU */
	__fs32		vs_emap;		/* offset of extent map in AU */
	__fs32		vs_imap;		/* offset of inode map in AU */
	__fs32		vs_iextop;		/* offset of ExtOp. map in AU */
	__fs32		vs_istart;		/* offset of inode list in AU */
	__fs32		vs_bstart;		/* offset of fdblock in AU */
	__fs32		vs_femap;		/* aufirst + emap */
	__fs32		vs_fimap;		/* aufirst + imap */
	__fs32		vs_fiextop;		/* aufirst + iextop */
	__fs32		vs_fistart;		/* aufirst + istart */
	__fs32		vs_fbstart;		/* aufirst + bstart */
	__fs32		vs_nindir;		/* number of entries in indir */
	__fs32		vs_aulen;		/* length of AU in blocks */
	__fs32		vs_auimlen;		/* length of imap in blocks */
	__fs32		vs_auemlen;		/* length of emap in blocks */
	__fs32		vs_auilen;		/* length of ilist in blocks */
	__fs32		vs_aupad;		/* length of pad in blocks */
	__fs32		vs_aublocks;		/* data blocks in AU */
	__fs32		vs_maxtier;		/* log base 2 of aublocks */
	__fs32		vs_inopb;		/* number of inodes per blk */
	__fs32		vs_old_inopau;		/* obsolete */
	__fs32		vs_old_inopilb;		/* obsolete */
	__fs32		vs_old_ndiripau;	/* obsolete */
	__fs32		vs_iaddrlen;		/* size of indirect addr ext. */
	__fs32		vs_bshift;		/* log base 2 of bsize */
	__fs32		vs_inoshift;		/* log base 2 of inobp */
	__fs32		vs_bmask;		/* ~( bsize - 1 ) */
	__fs32		vs_boffmask;		/* bsize - 1 */
	__fs32		vs_old_inomask;		/* old_inopilb - 1 */
	__fs32		vs_checksum;		/* checksum of V1 data */
>>>>>>> v4.9.227
	
	/*
	 * Version 1, writable
	 */
<<<<<<< HEAD
	int32_t		vs_free;		/* number of free blocks */
	int32_t		vs_ifree;		/* number of free inodes */
	int32_t		vs_efree[VXFS_NEFREE];	/* number of free extents by size */
	int32_t		vs_flags;		/* flags ?!? */
	u_int8_t	vs_mod;			/* filesystem has been changed */
	u_int8_t	vs_clean;		/* clean FS */
	u_int16_t	__unused4;		/* unused */
	u_int32_t	vs_firstlogid;		/* mount time log ID */
	u_int32_t	vs_wtime;		/* last time written - sec */
	u_int32_t	vs_wutime;		/* last time written - usec */
	u_int8_t	vs_fname[6];		/* FS name */
	u_int8_t	vs_fpack[6];		/* FS pack name */
	int32_t		vs_logversion;		/* log format version */
	int32_t		__unused5;		/* unused */
=======
	__fs32		vs_free;		/* number of free blocks */
	__fs32		vs_ifree;		/* number of free inodes */
	__fs32		vs_efree[VXFS_NEFREE];	/* number of free extents by size */
	__fs32		vs_flags;		/* flags ?!? */
	__u8		vs_mod;			/* filesystem has been changed */
	__u8		vs_clean;		/* clean FS */
	__fs16		__unused4;		/* unused */
	__fs32		vs_firstlogid;		/* mount time log ID */
	__fs32		vs_wtime;		/* last time written - sec */
	__fs32		vs_wutime;		/* last time written - usec */
	__u8		vs_fname[6];		/* FS name */
	__u8		vs_fpack[6];		/* FS pack name */
	__fs32		vs_logversion;		/* log format version */
	__u32		__unused5;		/* unused */
>>>>>>> v4.9.227
	
	/*
	 * Version 2, Read-only
	 */
<<<<<<< HEAD
	vx_daddr_t	vs_oltext[2];		/* OLT extent and replica */
	int32_t		vs_oltsize;		/* OLT extent size */
	int32_t		vs_iauimlen;		/* size of inode map */
	int32_t		vs_iausize;		/* size of IAU in blocks */
	int32_t		vs_dinosize;		/* size of inode in bytes */
	int32_t		vs_old_dniaddr;		/* indir levels per inode */
	int32_t		vs_checksum2;		/* checksum of V2 RO */
=======
	__fs32		vs_oltext[2];		/* OLT extent and replica */
	__fs32		vs_oltsize;		/* OLT extent size */
	__fs32		vs_iauimlen;		/* size of inode map */
	__fs32		vs_iausize;		/* size of IAU in blocks */
	__fs32		vs_dinosize;		/* size of inode in bytes */
	__fs32		vs_old_dniaddr;		/* indir levels per inode */
	__fs32		vs_checksum2;		/* checksum of V2 RO */
>>>>>>> v4.9.227

	/*
	 * Actually much more...
	 */
};


/*
 * In core superblock filesystem private data for VxFS.
 */
struct vxfs_sb_info {
	struct vxfs_sb		*vsi_raw;	/* raw (on disk) superblock */
	struct buffer_head	*vsi_bp;	/* buffer for raw superblock*/
	struct inode		*vsi_fship;	/* fileset header inode */
	struct inode		*vsi_ilist;	/* inode list inode */
	struct inode		*vsi_stilist;	/* structural inode list inode */
	u_long			vsi_iext;	/* initial inode list */
	ino_t			vsi_fshino;	/* fileset header inode */
	daddr_t			vsi_oltext;	/* OLT extent */
	daddr_t			vsi_oltsize;	/* OLT size */
<<<<<<< HEAD
};

=======
	enum vxfs_byte_order	byte_order;
};

static inline u16 fs16_to_cpu(struct vxfs_sb_info *sbi, __fs16 a)
{
	if (sbi->byte_order == VXFS_BO_BE)
		return be16_to_cpu((__force __be16)a);
	else
		return le16_to_cpu((__force __le16)a);
}

static inline u32 fs32_to_cpu(struct vxfs_sb_info *sbi, __fs32 a)
{
	if (sbi->byte_order == VXFS_BO_BE)
		return be32_to_cpu((__force __be32)a);
	else
		return le32_to_cpu((__force __le32)a);
}

static inline u64 fs64_to_cpu(struct vxfs_sb_info *sbi, __fs64 a)
{
	if (sbi->byte_order == VXFS_BO_BE)
		return be64_to_cpu((__force __be64)a);
	else
		return le64_to_cpu((__force __le64)a);
}
>>>>>>> v4.9.227

/*
 * File modes.  File types above 0xf000 are vxfs internal only, they should
 * not be passed back to higher levels of the system.  vxfs file types must
 * never have one of the regular file type bits set.
 */
enum vxfs_mode {
	VXFS_ISUID = 0x00000800,	/* setuid */
	VXFS_ISGID = 0x00000400,	/* setgid */
	VXFS_ISVTX = 0x00000200,	/* sticky bit */
	VXFS_IREAD = 0x00000100,	/* read */
	VXFS_IWRITE = 0x00000080,	/* write */
	VXFS_IEXEC = 0x00000040,	/* exec */

	VXFS_IFIFO = 0x00001000,	/* Named pipe */
	VXFS_IFCHR = 0x00002000,	/* Character device */
	VXFS_IFDIR = 0x00004000,	/* Directory */
	VXFS_IFNAM = 0x00005000,	/* Xenix device ?? */
	VXFS_IFBLK = 0x00006000,	/* Block device */
	VXFS_IFREG = 0x00008000,	/* Regular file */
	VXFS_IFCMP = 0x00009000,	/* Compressed file ?!? */
	VXFS_IFLNK = 0x0000a000,	/* Symlink */
	VXFS_IFSOC = 0x0000c000,	/* Socket */

	/* VxFS internal */
	VXFS_IFFSH = 0x10000000,	/* Fileset header */
	VXFS_IFILT = 0x20000000,	/* Inode list */
	VXFS_IFIAU = 0x30000000,	/* Inode allocation unit */
	VXFS_IFCUT = 0x40000000,	/* Current usage table */
	VXFS_IFATT = 0x50000000,	/* Attr. inode */
	VXFS_IFLCT = 0x60000000,	/* Link count table */
	VXFS_IFIAT = 0x70000000,	/* Indirect attribute file */
	VXFS_IFEMR = 0x80000000,	/* Extent map reorg file */
	VXFS_IFQUO = 0x90000000,	/* BSD quota file */
	VXFS_IFPTI = 0xa0000000,	/* "Pass through" inode */
	VXFS_IFLAB = 0x11000000,	/* Device label file */
	VXFS_IFOLT = 0x12000000,	/* OLT file */
	VXFS_IFLOG = 0x13000000,	/* Log file */
	VXFS_IFEMP = 0x14000000,	/* Extent map file */
	VXFS_IFEAU = 0x15000000,	/* Extent AU file */
	VXFS_IFAUS = 0x16000000,	/* Extent AU summary file */
	VXFS_IFDEV = 0x17000000,	/* Device config file */

};

#define	VXFS_TYPE_MASK		0xfffff000

#define VXFS_IS_TYPE(ip,type)	(((ip)->vii_mode & VXFS_TYPE_MASK) == (type))
#define VXFS_ISFIFO(x)		VXFS_IS_TYPE((x),VXFS_IFIFO)
#define VXFS_ISCHR(x)		VXFS_IS_TYPE((x),VXFS_IFCHR)
#define VXFS_ISDIR(x)		VXFS_IS_TYPE((x),VXFS_IFDIR)
#define VXFS_ISNAM(x)		VXFS_IS_TYPE((x),VXFS_IFNAM)
#define VXFS_ISBLK(x)		VXFS_IS_TYPE((x),VXFS_IFBLK)
#define VXFS_ISLNK(x)		VXFS_IS_TYPE((x),VXFS_IFLNK)
#define VXFS_ISREG(x)		VXFS_IS_TYPE((x),VXFS_IFREG)
#define VXFS_ISCMP(x)		VXFS_IS_TYPE((x),VXFS_IFCMP)
#define VXFS_ISSOC(x)		VXFS_IS_TYPE((x),VXFS_IFSOC)

#define VXFS_ISFSH(x)		VXFS_IS_TYPE((x),VXFS_IFFSH)
#define VXFS_ISILT(x)		VXFS_IS_TYPE((x),VXFS_IFILT)

/*
 * Inmode organisation types.
 */
enum {
	VXFS_ORG_NONE	= 0,	/* Inode has *no* format ?!? */
	VXFS_ORG_EXT4	= 1,	/* Ext4 */
	VXFS_ORG_IMMED	= 2,	/* All data stored in inode */
	VXFS_ORG_TYPED	= 3,	/* Typed extents */
};

#define VXFS_IS_ORG(ip,org)	((ip)->vii_orgtype == (org))
#define VXFS_ISNONE(ip)		VXFS_IS_ORG((ip), VXFS_ORG_NONE)
#define VXFS_ISEXT4(ip)		VXFS_IS_ORG((ip), VXFS_ORG_EXT4)
#define VXFS_ISIMMED(ip)	VXFS_IS_ORG((ip), VXFS_ORG_IMMED)
#define VXFS_ISTYPED(ip)	VXFS_IS_ORG((ip), VXFS_ORG_TYPED)

<<<<<<< HEAD

/*
 * Get filesystem private data from VFS inode.
 */
#define VXFS_INO(ip) \
	((struct vxfs_inode_info *)(ip)->i_private)

=======
>>>>>>> v4.9.227
/*
 * Get filesystem private data from VFS superblock.
 */
#define VXFS_SBI(sbp) \
	((struct vxfs_sb_info *)(sbp)->s_fs_info)

#endif /* _VXFS_SUPER_H_ */
