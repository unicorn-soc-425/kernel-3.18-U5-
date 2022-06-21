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
#ifndef _VXFS_FSHEAD_H_
#define _VXFS_FSHEAD_H_

/*
 * Veritas filesystem driver - fileset header structures.
 *
 * This file contains the physical structure of the VxFS
 * fileset header.
 */


/*
 * Fileset header 
 */
struct vxfs_fsh {
<<<<<<< HEAD
	u_int32_t	fsh_version;		/* fileset header version */
	u_int32_t	fsh_fsindex;		/* fileset index */
	u_int32_t	fsh_time;		/* modification time - sec */
	u_int32_t	fsh_utime;		/* modification time - usec */
	u_int32_t	fsh_extop;		/* extop flags */
	vx_ino_t	fsh_ninodes;		/* allocated inodes */
	u_int32_t	fsh_nau;		/* number of IAUs */
	u_int32_t	fsh_old_ilesize;	/* old size of ilist */
	u_int32_t	fsh_dflags;		/* flags */
	u_int32_t	fsh_quota;		/* quota limit */
	vx_ino_t	fsh_maxinode;		/* maximum inode number */
	vx_ino_t	fsh_iauino;		/* IAU inode */
	vx_ino_t	fsh_ilistino[2];	/* ilist inodes */
	vx_ino_t	fsh_lctino;		/* link count table inode */
=======
	__fs32		fsh_version;		/* fileset header version */
	__fs32		fsh_fsindex;		/* fileset index */
	__fs32		fsh_time;		/* modification time - sec */
	__fs32		fsh_utime;		/* modification time - usec */
	__fs32		fsh_extop;		/* extop flags */
	__fs32		fsh_ninodes;		/* allocated inodes */
	__fs32		fsh_nau;		/* number of IAUs */
	__fs32		fsh_old_ilesize;	/* old size of ilist */
	__fs32		fsh_dflags;		/* flags */
	__fs32		fsh_quota;		/* quota limit */
	__fs32		fsh_maxinode;		/* maximum inode number */
	__fs32		fsh_iauino;		/* IAU inode */
	__fs32		fsh_ilistino[2];	/* ilist inodes */
	__fs32		fsh_lctino;		/* link count table inode */
>>>>>>> v4.9.227

	/*
	 * Slightly more fields follow, but they
	 *  a) are not of any interest for us, and
	 *  b) differ a lot in different vxfs versions/ports
	 */
};

#endif /* _VXFS_FSHEAD_H_ */
