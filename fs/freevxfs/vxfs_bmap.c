/*
 * Copyright (c) 2000-2001 Christoph Hellwig.
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
 */

/*
 * Veritas filesystem driver - filesystem to disk block mapping.
 */
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/kernel.h>

#include "vxfs.h"
#include "vxfs_inode.h"
#include "vxfs_extern.h"


#ifdef DIAGNOSTIC
static void
vxfs_typdump(struct vxfs_typed *typ)
{
	printk(KERN_DEBUG "type=%Lu ", typ->vt_hdr >> VXFS_TYPED_TYPESHIFT);
	printk("offset=%Lx ", typ->vt_hdr & VXFS_TYPED_OFFSETMASK);
	printk("block=%x ", typ->vt_block);
	printk("size=%x\n", typ->vt_size);
}
#endif

/**
 * vxfs_bmap_ext4 - do bmap for ext4 extents
 * @ip:		pointer to the inode we do bmap for
 * @iblock:	logical block.
 *
 * Description:
 *   vxfs_bmap_ext4 performs the bmap operation for inodes with
 *   ext4-style extents (which are much like the traditional UNIX
 *   inode organisation).
 *
 * Returns:
 *   The physical block number on success, else Zero.
 */
static daddr_t
vxfs_bmap_ext4(struct inode *ip, long bn)
{
	struct super_block *sb = ip->i_sb;
	struct vxfs_inode_info *vip = VXFS_INO(ip);
<<<<<<< HEAD
	unsigned long bsize = sb->s_blocksize;
	u32 indsize = vip->vii_ext4.ve4_indsize;
=======
	struct vxfs_sb_info *sbi = VXFS_SBI(sb);
	unsigned long bsize = sb->s_blocksize;
	u32 indsize = fs32_to_cpu(sbi, vip->vii_ext4.ve4_indsize);
>>>>>>> v4.9.227
	int i;

	if (indsize > sb->s_blocksize)
		goto fail_size;

	for (i = 0; i < VXFS_NDADDR; i++) {
		struct direct *d = vip->vii_ext4.ve4_direct + i;
<<<<<<< HEAD
		if (bn >= 0 && bn < d->size)
			return (bn + d->extent);
		bn -= d->size;
=======
		if (bn >= 0 && bn < fs32_to_cpu(sbi, d->size))
			return (bn + fs32_to_cpu(sbi, d->extent));
		bn -= fs32_to_cpu(sbi, d->size);
>>>>>>> v4.9.227
	}

	if ((bn / (indsize * indsize * bsize / 4)) == 0) {
		struct buffer_head *buf;
		daddr_t	bno;
<<<<<<< HEAD
		u32 *indir;

		buf = sb_bread(sb, vip->vii_ext4.ve4_indir[0]);
		if (!buf || !buffer_mapped(buf))
			goto fail_buf;

		indir = (u32 *)buf->b_data;
		bno = indir[(bn/indsize) % (indsize*bn)] + (bn%indsize);
=======
		__fs32 *indir;

		buf = sb_bread(sb,
			fs32_to_cpu(sbi, vip->vii_ext4.ve4_indir[0]));
		if (!buf || !buffer_mapped(buf))
			goto fail_buf;

		indir = (__fs32 *)buf->b_data;
		bno = fs32_to_cpu(sbi, indir[(bn / indsize) % (indsize * bn)]) +
			(bn % indsize);
>>>>>>> v4.9.227

		brelse(buf);
		return bno;
	} else
		printk(KERN_WARNING "no matching indir?");

	return 0;

fail_size:
	printk("vxfs: indirect extent too big!\n");
fail_buf:
	return 0;
}

/**
 * vxfs_bmap_indir - recursion for vxfs_bmap_typed
 * @ip:		pointer to the inode we do bmap for
 * @indir:	indirect block we start reading at
 * @size:	size of the typed area to search
 * @block:	partially result from further searches
 *
 * Description:
 *   vxfs_bmap_indir reads a &struct vxfs_typed at @indir
 *   and performs the type-defined action.
 *
 * Return Value:
 *   The physical block number on success, else Zero.
 *
 * Note:
 *   Kernelstack is rare.  Unrecurse?
 */
static daddr_t
vxfs_bmap_indir(struct inode *ip, long indir, int size, long block)
{
<<<<<<< HEAD
=======
	struct vxfs_sb_info		*sbi = VXFS_SBI(ip->i_sb);
>>>>>>> v4.9.227
	struct buffer_head		*bp = NULL;
	daddr_t				pblock = 0;
	int				i;

	for (i = 0; i < size * VXFS_TYPED_PER_BLOCK(ip->i_sb); i++) {
		struct vxfs_typed	*typ;
		int64_t			off;

		bp = sb_bread(ip->i_sb,
				indir + (i / VXFS_TYPED_PER_BLOCK(ip->i_sb)));
		if (!bp || !buffer_mapped(bp))
			return 0;

		typ = ((struct vxfs_typed *)bp->b_data) +
			(i % VXFS_TYPED_PER_BLOCK(ip->i_sb));
<<<<<<< HEAD
		off = (typ->vt_hdr & VXFS_TYPED_OFFSETMASK);
=======
		off = fs64_to_cpu(sbi, typ->vt_hdr) & VXFS_TYPED_OFFSETMASK;
>>>>>>> v4.9.227

		if (block < off) {
			brelse(bp);
			continue;
		}

<<<<<<< HEAD
		switch ((u_int32_t)(typ->vt_hdr >> VXFS_TYPED_TYPESHIFT)) {
		case VXFS_TYPED_INDIRECT:
			pblock = vxfs_bmap_indir(ip, typ->vt_block,
					typ->vt_size, block - off);
=======
		switch ((u_int32_t)(fs64_to_cpu(sbi, typ->vt_hdr) >>
				VXFS_TYPED_TYPESHIFT)) {
		case VXFS_TYPED_INDIRECT:
			pblock = vxfs_bmap_indir(ip,
					fs32_to_cpu(sbi, typ->vt_block),
					fs32_to_cpu(sbi, typ->vt_size),
					block - off);
>>>>>>> v4.9.227
			if (pblock == -2)
				break;
			goto out;
		case VXFS_TYPED_DATA:
<<<<<<< HEAD
			if ((block - off) >= typ->vt_size)
				break;
			pblock = (typ->vt_block + block - off);
=======
			if ((block - off) >= fs32_to_cpu(sbi, typ->vt_size))
				break;
			pblock = fs32_to_cpu(sbi, typ->vt_block) + block - off;
>>>>>>> v4.9.227
			goto out;
		case VXFS_TYPED_INDIRECT_DEV4:
		case VXFS_TYPED_DATA_DEV4: {
			struct vxfs_typed_dev4	*typ4 =
				(struct vxfs_typed_dev4 *)typ;

			printk(KERN_INFO "\n\nTYPED_DEV4 detected!\n");
<<<<<<< HEAD
			printk(KERN_INFO "block: %Lu\tsize: %Ld\tdev: %d\n",
			       (unsigned long long) typ4->vd4_block,
			       (unsigned long long) typ4->vd4_size,
			       typ4->vd4_dev);
			goto fail;
		}
		default:
=======
			printk(KERN_INFO "block: %llu\tsize: %lld\tdev: %d\n",
			       fs64_to_cpu(sbi, typ4->vd4_block),
			       fs64_to_cpu(sbi, typ4->vd4_size),
			       fs32_to_cpu(sbi, typ4->vd4_dev));
			goto fail;
		}
		default:
			printk(KERN_ERR "%s:%d vt_hdr %llu\n", __func__,
				__LINE__, fs64_to_cpu(sbi, typ->vt_hdr));
>>>>>>> v4.9.227
			BUG();
		}
		brelse(bp);
	}

fail:
	pblock = 0;
out:
	brelse(bp);
	return (pblock);
}

/**
 * vxfs_bmap_typed - bmap for typed extents
 * @ip:		pointer to the inode we do bmap for
 * @iblock:	logical block
 *
 * Description:
 *   Performs the bmap operation for typed extents.
 *
 * Return Value:
 *   The physical block number on success, else Zero.
 */
static daddr_t
vxfs_bmap_typed(struct inode *ip, long iblock)
{
	struct vxfs_inode_info		*vip = VXFS_INO(ip);
<<<<<<< HEAD
=======
	struct vxfs_sb_info		*sbi = VXFS_SBI(ip->i_sb);
>>>>>>> v4.9.227
	daddr_t				pblock = 0;
	int				i;

	for (i = 0; i < VXFS_NTYPED; i++) {
		struct vxfs_typed	*typ = vip->vii_org.typed + i;
<<<<<<< HEAD
		int64_t			off = (typ->vt_hdr & VXFS_TYPED_OFFSETMASK);
=======
		u64			hdr = fs64_to_cpu(sbi, typ->vt_hdr);
		int64_t			off = (hdr & VXFS_TYPED_OFFSETMASK);
>>>>>>> v4.9.227

#ifdef DIAGNOSTIC
		vxfs_typdump(typ);
#endif
		if (iblock < off)
			continue;
<<<<<<< HEAD
		switch ((u_int32_t)(typ->vt_hdr >> VXFS_TYPED_TYPESHIFT)) {
		case VXFS_TYPED_INDIRECT:
			pblock = vxfs_bmap_indir(ip, typ->vt_block,
					typ->vt_size, iblock - off);
=======
		switch ((u32)(hdr >> VXFS_TYPED_TYPESHIFT)) {
		case VXFS_TYPED_INDIRECT:
			pblock = vxfs_bmap_indir(ip,
					fs32_to_cpu(sbi, typ->vt_block),
					fs32_to_cpu(sbi, typ->vt_size),
					iblock - off);
>>>>>>> v4.9.227
			if (pblock == -2)
				break;
			return (pblock);
		case VXFS_TYPED_DATA:
<<<<<<< HEAD
			if ((iblock - off) < typ->vt_size)
				return (typ->vt_block + iblock - off);
=======
			if ((iblock - off) < fs32_to_cpu(sbi, typ->vt_size))
				return (fs32_to_cpu(sbi, typ->vt_block) +
						iblock - off);
>>>>>>> v4.9.227
			break;
		case VXFS_TYPED_INDIRECT_DEV4:
		case VXFS_TYPED_DATA_DEV4: {
			struct vxfs_typed_dev4	*typ4 =
				(struct vxfs_typed_dev4 *)typ;

			printk(KERN_INFO "\n\nTYPED_DEV4 detected!\n");
<<<<<<< HEAD
			printk(KERN_INFO "block: %Lu\tsize: %Ld\tdev: %d\n",
			       (unsigned long long) typ4->vd4_block,
			       (unsigned long long) typ4->vd4_size,
			       typ4->vd4_dev);
=======
			printk(KERN_INFO "block: %llu\tsize: %lld\tdev: %d\n",
			       fs64_to_cpu(sbi, typ4->vd4_block),
			       fs64_to_cpu(sbi, typ4->vd4_size),
			       fs32_to_cpu(sbi, typ4->vd4_dev));
>>>>>>> v4.9.227
			return 0;
		}
		default:
			BUG();
		}
	}

	return 0;
}

/**
 * vxfs_bmap1 - vxfs-internal bmap operation
 * @ip:			pointer to the inode we do bmap for
 * @iblock:		logical block
 *
 * Description:
 *   vxfs_bmap1 perfoms a logical to physical block mapping
 *   for vxfs-internal purposes.
 *
 * Return Value:
 *   The physical block number on success, else Zero.
 */
daddr_t
vxfs_bmap1(struct inode *ip, long iblock)
{
	struct vxfs_inode_info		*vip = VXFS_INO(ip);

	if (VXFS_ISEXT4(vip))
		return vxfs_bmap_ext4(ip, iblock);
	if (VXFS_ISTYPED(vip))
		return vxfs_bmap_typed(ip, iblock);
	if (VXFS_ISNONE(vip))
		goto unsupp;
	if (VXFS_ISIMMED(vip))
		goto unsupp;

	printk(KERN_WARNING "vxfs: inode %ld has no valid orgtype (%x)\n",
			ip->i_ino, vip->vii_orgtype);
	BUG();

unsupp:
	printk(KERN_WARNING "vxfs: inode %ld has an unsupported orgtype (%x)\n",
			ip->i_ino, vip->vii_orgtype);
	return 0;
}
