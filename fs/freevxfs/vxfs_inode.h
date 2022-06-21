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
#ifndef _VXFS_INODE_H_
#define _VXFS_INODE_H_

/*
 * Veritas filesystem driver - inode structure.
 *
 * This file contains the definition of the disk and core
 * inodes of the Veritas Filesystem.
 */


#define VXFS_ISIZE		0x100		/* Inode size */

#define VXFS_NDADDR		10		/* Number of direct addrs in inode */
#define VXFS_NIADDR		2		/* Number of indirect addrs in inode */
#define VXFS_NIMMED		96		/* Size of immediate data in inode */
#define VXFS_NTYPED		6		/* Num of typed extents */

#define VXFS_TYPED_OFFSETMASK	(0x00FFFFFFFFFFFFFFULL)
#define VXFS_TYPED_TYPEMASK	(0xFF00000000000000ULL)
#define VXFS_TYPED_TYPESHIFT	56

#define VXFS_TYPED_PER_BLOCK(sbp) \
	((sbp)->s_blocksize / sizeof(struct vxfs_typed))

/*
 * Possible extent descriptor types for %VXFS_ORG_TYPED extents.
 */
enum {
	VXFS_TYPED_INDIRECT		= 1,
	VXFS_TYPED_DATA			= 2,
	VXFS_TYPED_INDIRECT_DEV4	= 3,
	VXFS_TYPED_DATA_DEV4		= 4,
};

/*
 * Data stored immediately in the inode.
 */
struct vxfs_immed {
<<<<<<< HEAD
	u_int8_t	vi_immed[VXFS_NIMMED];
};

struct vxfs_ext4 {
	u_int32_t		ve4_spare;		/* ?? */
	u_int32_t		ve4_indsize;		/* Indirect extent size */
	vx_daddr_t		ve4_indir[VXFS_NIADDR];	/* Indirect extents */
	struct direct {					/* Direct extents */
		vx_daddr_t	extent;			/* Extent number */
		int32_t		size;			/* Size of extent */
=======
	__u8			vi_immed[VXFS_NIMMED];
};

struct vxfs_ext4 {
	__fs32			ve4_spare;		/* ?? */
	__fs32			ve4_indsize;		/* Indirect extent size */
	__fs32			ve4_indir[VXFS_NIADDR];	/* Indirect extents */
	struct direct {					/* Direct extents */
		__fs32		extent;			/* Extent number */
		__fs32		size;			/* Size of extent */
>>>>>>> v4.9.227
	} ve4_direct[VXFS_NDADDR];
};

struct vxfs_typed {
<<<<<<< HEAD
	u_int64_t	vt_hdr;		/* Header, 0xTTOOOOOOOOOOOOOO; T=type,O=offs */
	vx_daddr_t	vt_block;	/* Extent block */
	int32_t		vt_size;	/* Size in blocks */
};

struct vxfs_typed_dev4 {
	u_int64_t	vd4_hdr;	/* Header, 0xTTOOOOOOOOOOOOOO; T=type,O=offs */
	u_int64_t	vd4_block;	/* Extent block */
	u_int64_t	vd4_size;	/* Size in blocks */
	int32_t		vd4_dev;	/* Device ID */
	u_int32_t	__pad1;
=======
	__fs64		vt_hdr;		/* Header, 0xTTOOOOOOOOOOOOOO; T=type,O=offs */
	__fs32		vt_block;	/* Extent block */
	__fs32		vt_size;	/* Size in blocks */
};

struct vxfs_typed_dev4 {
	__fs64		vd4_hdr;	/* Header, 0xTTOOOOOOOOOOOOOO; T=type,O=offs */
	__fs64		vd4_block;	/* Extent block */
	__fs64		vd4_size;	/* Size in blocks */
	__fs32		vd4_dev;	/* Device ID */
	__u8		__pad1;
>>>>>>> v4.9.227
};

/*
 * The inode as contained on the physical device.
 */
struct vxfs_dinode {
<<<<<<< HEAD
	int32_t		vdi_mode;
	u_int32_t	vdi_nlink;	/* Link count */
	u_int32_t	vdi_uid;	/* UID */
	u_int32_t	vdi_gid;	/* GID */
	u_int64_t	vdi_size;	/* Inode size in bytes */
	u_int32_t	vdi_atime;	/* Last time accessed - sec */
	u_int32_t	vdi_autime;	/* Last time accessed - usec */
	u_int32_t	vdi_mtime;	/* Last modify time - sec */
	u_int32_t	vdi_mutime;	/* Last modify time - usec */
	u_int32_t	vdi_ctime;	/* Create time - sec */
	u_int32_t	vdi_cutime;	/* Create time - usec */
	u_int8_t	vdi_aflags;	/* Allocation flags */
	u_int8_t	vdi_orgtype;	/* Organisation type */
	u_int16_t	vdi_eopflags;
	u_int32_t	vdi_eopdata;
	union {
		u_int32_t		rdev;
		u_int32_t		dotdot;
		struct {
			u_int32_t	reserved;
			u_int32_t	fixextsize;
		} i_regular;
		struct {
			u_int32_t	matchino;
			u_int32_t	fsetindex;
		} i_vxspec;
		u_int64_t		align;
	} vdi_ftarea;
	u_int32_t	vdi_blocks;	/* How much blocks does inode occupy */
	u_int32_t	vdi_gen;	/* Inode generation */
	u_int64_t	vdi_version;	/* Version */
=======
	__fs32		vdi_mode;
	__fs32		vdi_nlink;	/* Link count */
	__fs32		vdi_uid;	/* UID */
	__fs32		vdi_gid;	/* GID */
	__fs64		vdi_size;	/* Inode size in bytes */
	__fs32		vdi_atime;	/* Last time accessed - sec */
	__fs32		vdi_autime;	/* Last time accessed - usec */
	__fs32		vdi_mtime;	/* Last modify time - sec */
	__fs32		vdi_mutime;	/* Last modify time - usec */
	__fs32		vdi_ctime;	/* Create time - sec */
	__fs32		vdi_cutime;	/* Create time - usec */
	__u8		vdi_aflags;	/* Allocation flags */
	__u8		vdi_orgtype;	/* Organisation type */
	__fs16		vdi_eopflags;
	__fs32		vdi_eopdata;
	union {
		__fs32			rdev;
		__fs32			dotdot;
		struct {
			__u32		reserved;
			__fs32		fixextsize;
		} i_regular;
		struct {
			__fs32		matchino;
			__fs32		fsetindex;
		} i_vxspec;
		__u64			align;
	} vdi_ftarea;
	__fs32		vdi_blocks;	/* How much blocks does inode occupy */
	__fs32		vdi_gen;	/* Inode generation */
	__fs64		vdi_version;	/* Version */
>>>>>>> v4.9.227
	union {
		struct vxfs_immed	immed;
		struct vxfs_ext4	ext4;
		struct vxfs_typed	typed[VXFS_NTYPED];
	} vdi_org;
<<<<<<< HEAD
	u_int32_t	vdi_iattrino;
=======
	__fs32		vdi_iattrino;
>>>>>>> v4.9.227
};

#define vdi_rdev	vdi_ftarea.rdev
#define vdi_dotdot	vdi_ftarea.dotdot
#define vdi_fixextsize	vdi_ftarea.regular.fixextsize
#define vdi_matchino	vdi_ftarea.vxspec.matchino
#define vdi_fsetindex	vdi_ftarea.vxspec.fsetindex

#define vdi_immed	vdi_org.immed
#define vdi_ext4	vdi_org.ext4
#define vdi_typed	vdi_org.typed


/*
 * The inode as represented in the main memory.
<<<<<<< HEAD
 *
 * TBD: This should become a separate structure...
 */
#define vxfs_inode_info	vxfs_dinode

#define vii_mode	vdi_mode
#define vii_uid		vdi_uid
#define vii_gid		vdi_gid
#define vii_nlink	vdi_nlink
#define vii_size	vdi_size
#define vii_atime	vdi_atime
#define vii_ctime	vdi_ctime
#define vii_mtime	vdi_mtime
#define vii_blocks	vdi_blocks
#define vii_org		vdi_org
#define vii_orgtype	vdi_orgtype
#define vii_gen		vdi_gen

#define vii_rdev	vdi_ftarea.rdev
#define vii_dotdot	vdi_ftarea.dotdot
#define vii_fixextsize	vdi_ftarea.regular.fixextsize
#define vii_matchino	vdi_ftarea.vxspec.matchino
#define vii_fsetindex	vdi_ftarea.vxspec.fsetindex

#define vii_immed	vdi_org.immed
#define vii_ext4	vdi_org.ext4
#define vii_typed	vdi_org.typed
=======
 */
struct vxfs_inode_info {
	struct inode	vfs_inode;

	__u32		vii_mode;
	__u32		vii_nlink;	/* Link count */
	__u32		vii_uid;	/* UID */
	__u32		vii_gid;	/* GID */
	__u64		vii_size;	/* Inode size in bytes */
	__u32		vii_atime;	/* Last time accessed - sec */
	__u32		vii_autime;	/* Last time accessed - usec */
	__u32		vii_mtime;	/* Last modify time - sec */
	__u32		vii_mutime;	/* Last modify time - usec */
	__u32		vii_ctime;	/* Create time - sec */
	__u32		vii_cutime;	/* Create time - usec */
	__u8		vii_orgtype;	/* Organisation type */
	union {
		__u32			rdev;
		__u32			dotdot;
	} vii_ftarea;
	__u32		vii_blocks;	/* How much blocks does inode occupy */
	__u32		vii_gen;	/* Inode generation */
	union {
		struct vxfs_immed	immed;
		struct vxfs_ext4	ext4;
		struct vxfs_typed	typed[VXFS_NTYPED];
	} vii_org;
};

#define vii_rdev	vii_ftarea.rdev
#define vii_dotdot	vii_ftarea.dotdot

#define vii_immed	vii_org.immed
#define vii_ext4	vii_org.ext4
#define vii_typed	vii_org.typed

static inline struct vxfs_inode_info *VXFS_INO(struct inode *inode)
{
	return container_of(inode, struct vxfs_inode_info, vfs_inode);
}
>>>>>>> v4.9.227

#endif /* _VXFS_INODE_H_ */
