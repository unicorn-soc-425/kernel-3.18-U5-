/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
<<<<<<< HEAD
 * http://www.sun.com/software/products/lustre/docs/GPLv2.pdf
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
=======
 * http://www.gnu.org/licenses/gpl-2.0.html
>>>>>>> v4.9.227
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
<<<<<<< HEAD
 * Copyright (c) 2011, 2012, Intel Corporation.
=======
 * Copyright (c) 2011, 2015, Intel Corporation.
>>>>>>> v4.9.227
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * Implementation of cl_io for VVP layer.
 *
 *   Author: Nikita Danilov <nikita.danilov@sun.com>
 *   Author: Jinshan Xiong <jinshan.xiong@whamcloud.com>
 */

#define DEBUG_SUBSYSTEM S_LLITE

<<<<<<< HEAD

#include "../include/obd.h"
#include "../include/lustre_lite.h"

#include "vvp_internal.h"

static struct vvp_io *cl2vvp_io(const struct lu_env *env,
				const struct cl_io_slice *slice);

/**
 * True, if \a io is a normal io, False for splice_{read,write}
 */
int cl_is_normalio(const struct lu_env *env, const struct cl_io *io)
{
	struct vvp_io *vio = vvp_env_io(env);

	LASSERT(io->ci_type == CIT_READ || io->ci_type == CIT_WRITE);

	return vio->cui_io_subtype == IO_NORMAL;
=======
#include "../include/obd.h"

#include "llite_internal.h"
#include "vvp_internal.h"

static struct vvp_io *cl2vvp_io(const struct lu_env *env,
				const struct cl_io_slice *slice)
{
	struct vvp_io *vio;

	vio = container_of(slice, struct vvp_io, vui_cl);
	LASSERT(vio == vvp_env_io(env));

	return vio;
>>>>>>> v4.9.227
}

/**
 * For swapping layout. The file's layout may have changed.
 * To avoid populating pages to a wrong stripe, we have to verify the
 * correctness of layout. It works because swapping layout processes
 * have to acquire group lock.
 */
static bool can_populate_pages(const struct lu_env *env, struct cl_io *io,
<<<<<<< HEAD
				struct inode *inode)
{
	struct ll_inode_info	*lli = ll_i2info(inode);
	struct ccc_io		*cio = ccc_env_io(env);
=======
			       struct inode *inode)
{
	struct ll_inode_info	*lli = ll_i2info(inode);
	struct vvp_io		*vio = vvp_env_io(env);
>>>>>>> v4.9.227
	bool rc = true;

	switch (io->ci_type) {
	case CIT_READ:
	case CIT_WRITE:
		/* don't need lock here to check lli_layout_gen as we have held
<<<<<<< HEAD
		 * extent lock and GROUP lock has to hold to swap layout */
		if (ll_layout_version_get(lli) != cio->cui_layout_gen) {
=======
		 * extent lock and GROUP lock has to hold to swap layout
		 */
		if (ll_layout_version_get(lli) != vio->vui_layout_gen) {
>>>>>>> v4.9.227
			io->ci_need_restart = 1;
			/* this will return application a short read/write */
			io->ci_continue = 0;
			rc = false;
		}
	case CIT_FAULT:
		/* fault is okay because we've already had a page. */
	default:
		break;
	}

	return rc;
}

<<<<<<< HEAD
=======
static void vvp_object_size_lock(struct cl_object *obj)
{
	struct inode *inode = vvp_object_inode(obj);

	ll_inode_size_lock(inode);
	cl_object_attr_lock(obj);
}

static void vvp_object_size_unlock(struct cl_object *obj)
{
	struct inode *inode = vvp_object_inode(obj);

	cl_object_attr_unlock(obj);
	ll_inode_size_unlock(inode);
}

/**
 * Helper function that if necessary adjusts file size (inode->i_size), when
 * position at the offset \a pos is accessed. File size can be arbitrary stale
 * on a Lustre client, but client at least knows KMS. If accessed area is
 * inside [0, KMS], set file size to KMS, otherwise glimpse file size.
 *
 * Locking: cl_isize_lock is used to serialize changes to inode size and to
 * protect consistency between inode size and cl_object
 * attributes. cl_object_size_lock() protects consistency between cl_attr's of
 * top-object and sub-objects.
 */
static int vvp_prep_size(const struct lu_env *env, struct cl_object *obj,
			 struct cl_io *io, loff_t start, size_t count,
			 int *exceed)
{
	struct cl_attr *attr  = vvp_env_thread_attr(env);
	struct inode   *inode = vvp_object_inode(obj);
	loff_t	  pos   = start + count - 1;
	loff_t kms;
	int result;

	/*
	 * Consistency guarantees: following possibilities exist for the
	 * relation between region being accessed and real file size at this
	 * moment:
	 *
	 *  (A): the region is completely inside of the file;
	 *
	 *  (B-x): x bytes of region are inside of the file, the rest is
	 *  outside;
	 *
	 *  (C): the region is completely outside of the file.
	 *
	 * This classification is stable under DLM lock already acquired by
	 * the caller, because to change the class, other client has to take
	 * DLM lock conflicting with our lock. Also, any updates to ->i_size
	 * by other threads on this client are serialized by
	 * ll_inode_size_lock(). This guarantees that short reads are handled
	 * correctly in the face of concurrent writes and truncates.
	 */
	vvp_object_size_lock(obj);
	result = cl_object_attr_get(env, obj, attr);
	if (result == 0) {
		kms = attr->cat_kms;
		if (pos > kms) {
			/*
			 * A glimpse is necessary to determine whether we
			 * return a short read (B) or some zeroes at the end
			 * of the buffer (C)
			 */
			vvp_object_size_unlock(obj);
			result = cl_glimpse_lock(env, io, inode, obj, 0);
			if (result == 0 && exceed) {
				/* If objective page index exceed end-of-file
				 * page index, return directly. Do not expect
				 * kernel will check such case correctly.
				 * linux-2.6.18-128.1.1 miss to do that.
				 * --bug 17336
				 */
				loff_t size = i_size_read(inode);
				loff_t cur_index = start >> PAGE_SHIFT;
				loff_t size_index = (size - 1) >> PAGE_SHIFT;

				if ((size == 0 && cur_index != 0) ||
				    size_index < cur_index)
					*exceed = 1;
			}
			return result;
		}
		/*
		 * region is within kms and, hence, within real file
		 * size (A). We need to increase i_size to cover the
		 * read region so that generic_file_read() will do its
		 * job, but that doesn't mean the kms size is
		 * _correct_, it is only the _minimum_ size. If
		 * someone does a stat they will get the correct size
		 * which will always be >= the kms value here.
		 * b=11081
		 */
		if (i_size_read(inode) < kms) {
			i_size_write(inode, kms);
			CDEBUG(D_VFSTRACE, DFID " updating i_size %llu\n",
			       PFID(lu_object_fid(&obj->co_lu)),
			       (__u64)i_size_read(inode));
		}
	}

	vvp_object_size_unlock(obj);

	return result;
}

>>>>>>> v4.9.227
/*****************************************************************************
 *
 * io operations.
 *
 */

<<<<<<< HEAD
=======
static int vvp_io_one_lock_index(const struct lu_env *env, struct cl_io *io,
				 __u32 enqflags, enum cl_lock_mode mode,
			  pgoff_t start, pgoff_t end)
{
	struct vvp_io          *vio   = vvp_env_io(env);
	struct cl_lock_descr   *descr = &vio->vui_link.cill_descr;
	struct cl_object       *obj   = io->ci_obj;

	CLOBINVRNT(env, obj, vvp_object_invariant(obj));

	CDEBUG(D_VFSTRACE, "lock: %d [%lu, %lu]\n", mode, start, end);

	memset(&vio->vui_link, 0, sizeof(vio->vui_link));

	if (vio->vui_fd && (vio->vui_fd->fd_flags & LL_FILE_GROUP_LOCKED)) {
		descr->cld_mode = CLM_GROUP;
		descr->cld_gid  = vio->vui_fd->fd_grouplock.lg_gid;
	} else {
		descr->cld_mode  = mode;
	}
	descr->cld_obj   = obj;
	descr->cld_start = start;
	descr->cld_end   = end;
	descr->cld_enq_flags = enqflags;

	cl_io_lock_add(env, io, &vio->vui_link);
	return 0;
}

static int vvp_io_one_lock(const struct lu_env *env, struct cl_io *io,
			   __u32 enqflags, enum cl_lock_mode mode,
			   loff_t start, loff_t end)
{
	struct cl_object *obj = io->ci_obj;

	return vvp_io_one_lock_index(env, io, enqflags, mode,
				     cl_index(obj, start), cl_index(obj, end));
}

static int vvp_io_write_iter_init(const struct lu_env *env,
				  const struct cl_io_slice *ios)
{
	struct vvp_io *vio = cl2vvp_io(env, ios);

	cl_page_list_init(&vio->u.write.vui_queue);
	vio->u.write.vui_written = 0;
	vio->u.write.vui_from = 0;
	vio->u.write.vui_to = PAGE_SIZE;

	return 0;
}

static void vvp_io_write_iter_fini(const struct lu_env *env,
				   const struct cl_io_slice *ios)
{
	struct vvp_io *vio = cl2vvp_io(env, ios);

	LASSERT(vio->u.write.vui_queue.pl_nr == 0);
}

>>>>>>> v4.9.227
static int vvp_io_fault_iter_init(const struct lu_env *env,
				  const struct cl_io_slice *ios)
{
	struct vvp_io *vio   = cl2vvp_io(env, ios);
<<<<<<< HEAD
	struct inode  *inode = ccc_object_inode(ios->cis_obj);

	LASSERT(inode ==
		cl2ccc_io(env, ios)->cui_fd->fd_file->f_dentry->d_inode);
	vio->u.fault.ft_mtime = LTIME_S(inode->i_mtime);
=======
	struct inode  *inode = vvp_object_inode(ios->cis_obj);

	LASSERT(inode == file_inode(vio->vui_fd->fd_file));
	vio->u.fault.ft_mtime = inode->i_mtime.tv_sec;
>>>>>>> v4.9.227
	return 0;
}

static void vvp_io_fini(const struct lu_env *env, const struct cl_io_slice *ios)
{
	struct cl_io     *io  = ios->cis_io;
	struct cl_object *obj = io->ci_obj;
<<<<<<< HEAD
	struct ccc_io    *cio = cl2ccc_io(env, ios);

	CLOBINVRNT(env, obj, ccc_object_invariant(obj));
=======
	struct vvp_io    *vio = cl2vvp_io(env, ios);
	struct inode *inode = vvp_object_inode(obj);

	CLOBINVRNT(env, obj, vvp_object_invariant(obj));
>>>>>>> v4.9.227

	CDEBUG(D_VFSTRACE, DFID
	       " ignore/verify layout %d/%d, layout version %d restore needed %d\n",
	       PFID(lu_object_fid(&obj->co_lu)),
	       io->ci_ignore_layout, io->ci_verify_layout,
<<<<<<< HEAD
	       cio->cui_layout_gen, io->ci_restore_needed);
=======
	       vio->vui_layout_gen, io->ci_restore_needed);
>>>>>>> v4.9.227

	if (io->ci_restore_needed == 1) {
		int	rc;

		/* file was detected release, we need to restore it
		 * before finishing the io
		 */
<<<<<<< HEAD
		rc = ll_layout_restore(ccc_object_inode(obj));
		/* if restore registration failed, no restart,
		 * we will return -ENODATA */
=======
		rc = ll_layout_restore(inode, 0, OBD_OBJECT_EOF);
		/* if restore registration failed, no restart,
		 * we will return -ENODATA
		 */
>>>>>>> v4.9.227
		/* The layout will change after restore, so we need to
		 * block on layout lock hold by the MDT
		 * as MDT will not send new layout in lvb (see LU-3124)
		 * we have to explicitly fetch it, all this will be done
		 * by ll_layout_refresh()
		 */
		if (rc == 0) {
			io->ci_restore_needed = 0;
			io->ci_need_restart = 1;
			io->ci_verify_layout = 1;
		} else {
			io->ci_restore_needed = 1;
			io->ci_need_restart = 0;
			io->ci_verify_layout = 0;
			io->ci_result = rc;
		}
	}

	if (!io->ci_ignore_layout && io->ci_verify_layout) {
		__u32 gen = 0;

		/* check layout version */
<<<<<<< HEAD
		ll_layout_refresh(ccc_object_inode(obj), &gen);
		io->ci_need_restart = cio->cui_layout_gen != gen;
=======
		ll_layout_refresh(inode, &gen);
		io->ci_need_restart = vio->vui_layout_gen != gen;
>>>>>>> v4.9.227
		if (io->ci_need_restart) {
			CDEBUG(D_VFSTRACE,
			       DFID" layout changed from %d to %d.\n",
			       PFID(lu_object_fid(&obj->co_lu)),
<<<<<<< HEAD
			       cio->cui_layout_gen, gen);
			/* today successful restore is the only possible
			 * case */
			/* restore was done, clear restoring state */
			ll_i2info(ccc_object_inode(obj))->lli_flags &=
=======
			       vio->vui_layout_gen, gen);
			/* today successful restore is the only possible case */
			/* restore was done, clear restoring state */
			ll_i2info(vvp_object_inode(obj))->lli_flags &=
>>>>>>> v4.9.227
				~LLIF_FILE_RESTORING;
		}
	}
}

static void vvp_io_fault_fini(const struct lu_env *env,
			      const struct cl_io_slice *ios)
{
	struct cl_io   *io   = ios->cis_io;
	struct cl_page *page = io->u.ci_fault.ft_page;

<<<<<<< HEAD
	CLOBINVRNT(env, io->ci_obj, ccc_object_invariant(io->ci_obj));

	if (page != NULL) {
=======
	CLOBINVRNT(env, io->ci_obj, vvp_object_invariant(io->ci_obj));

	if (page) {
>>>>>>> v4.9.227
		lu_ref_del(&page->cp_reference, "fault", io);
		cl_page_put(env, page);
		io->u.ci_fault.ft_page = NULL;
	}
	vvp_io_fini(env, ios);
}

static enum cl_lock_mode vvp_mode_from_vma(struct vm_area_struct *vma)
{
	/*
	 * we only want to hold PW locks if the mmap() can generate
	 * writes back to the file and that only happens in shared
	 * writable vmas
	 */
	if ((vma->vm_flags & VM_SHARED) && (vma->vm_flags & VM_WRITE))
		return CLM_WRITE;
	return CLM_READ;
}

static int vvp_mmap_locks(const struct lu_env *env,
<<<<<<< HEAD
			  struct ccc_io *vio, struct cl_io *io)
{
	struct ccc_thread_info *cti = ccc_env_info(env);
	struct mm_struct       *mm = current->mm;
	struct vm_area_struct  *vma;
	struct cl_lock_descr   *descr = &cti->cti_descr;
	ldlm_policy_data_t      policy;
	unsigned long	   addr;
	ssize_t		 count;
	int		     result;
=======
			  struct vvp_io *vio, struct cl_io *io)
{
	struct vvp_thread_info *cti = vvp_env_info(env);
	struct mm_struct       *mm = current->mm;
	struct vm_area_struct  *vma;
	struct cl_lock_descr   *descr = &cti->vti_descr;
	ldlm_policy_data_t      policy;
	unsigned long	   addr;
	ssize_t		 count;
	int		 result = 0;
>>>>>>> v4.9.227
	struct iov_iter i;
	struct iovec iov;

	LASSERT(io->ci_type == CIT_READ || io->ci_type == CIT_WRITE);

<<<<<<< HEAD
	if (!cl_is_normalio(env, io))
		return 0;

	if (vio->cui_iter == NULL) /* nfs or loop back device write */
		return 0;

	/* No MM (e.g. NFS)? No vmas too. */
	if (mm == NULL)
		return 0;

	iov_for_each(iov, i, *(vio->cui_iter)) {
=======
	if (!vio->vui_iter) /* nfs or loop back device write */
		return 0;

	/* No MM (e.g. NFS)? No vmas too. */
	if (!mm)
		return 0;

	iov_for_each(iov, i, *vio->vui_iter) {
>>>>>>> v4.9.227
		addr = (unsigned long)iov.iov_base;
		count = iov.iov_len;
		if (count == 0)
			continue;

<<<<<<< HEAD
		count += addr & (~CFS_PAGE_MASK);
		addr &= CFS_PAGE_MASK;

		down_read(&mm->mmap_sem);
		while ((vma = our_vma(mm, addr, count)) != NULL) {
			struct inode *inode = vma->vm_file->f_dentry->d_inode;
=======
		count += addr & (~PAGE_MASK);
		addr &= PAGE_MASK;

		down_read(&mm->mmap_sem);
		while ((vma = our_vma(mm, addr, count)) != NULL) {
			struct inode *inode = file_inode(vma->vm_file);
>>>>>>> v4.9.227
			int flags = CEF_MUST;

			if (ll_file_nolock(vma->vm_file)) {
				/*
<<<<<<< HEAD
				 * For no lock case, a lockless lock will be
				 * generated.
				 */
				flags = CEF_NEVER;
=======
				 * For no lock case is not allowed for mmap
				 */
				result = -EINVAL;
				break;
>>>>>>> v4.9.227
			}

			/*
			 * XXX: Required lock mode can be weakened: CIT_WRITE
			 * io only ever reads user level buffer, and CIT_READ
			 * only writes on it.
			 */
			policy_from_vma(&policy, vma, addr, count);
			descr->cld_mode = vvp_mode_from_vma(vma);
			descr->cld_obj = ll_i2info(inode)->lli_clob;
			descr->cld_start = cl_index(descr->cld_obj,
						    policy.l_extent.start);
			descr->cld_end = cl_index(descr->cld_obj,
						  policy.l_extent.end);
			descr->cld_enq_flags = flags;
			result = cl_io_lock_alloc_add(env, io, descr);

			CDEBUG(D_VFSTRACE, "lock: %d: [%lu, %lu]\n",
			       descr->cld_mode, descr->cld_start,
			       descr->cld_end);

<<<<<<< HEAD
			if (result < 0) {
				up_read(&mm->mmap_sem);
				return result;
			}
=======
			if (result < 0)
				break;
>>>>>>> v4.9.227

			if (vma->vm_end - addr >= count)
				break;

			count -= vma->vm_end - addr;
			addr = vma->vm_end;
		}
		up_read(&mm->mmap_sem);
<<<<<<< HEAD
	}
	return 0;
=======
		if (result < 0)
			break;
	}
	return result;
}

static void vvp_io_advance(const struct lu_env *env,
			   const struct cl_io_slice *ios,
			   size_t nob)
{
	struct cl_object *obj = ios->cis_io->ci_obj;
	struct vvp_io	 *vio = cl2vvp_io(env, ios);
	CLOBINVRNT(env, obj, vvp_object_invariant(obj));

	iov_iter_reexpand(vio->vui_iter, vio->vui_tot_count  -= nob);
}

static void vvp_io_update_iov(const struct lu_env *env,
			      struct vvp_io *vio, struct cl_io *io)
{
	size_t size = io->u.ci_rw.crw_count;

	if (!vio->vui_iter)
		return;

	iov_iter_truncate(vio->vui_iter, size);
>>>>>>> v4.9.227
}

static int vvp_io_rw_lock(const struct lu_env *env, struct cl_io *io,
			  enum cl_lock_mode mode, loff_t start, loff_t end)
{
<<<<<<< HEAD
	struct ccc_io *cio = ccc_env_io(env);
=======
	struct vvp_io *vio = vvp_env_io(env);
>>>>>>> v4.9.227
	int result;
	int ast_flags = 0;

	LASSERT(io->ci_type == CIT_READ || io->ci_type == CIT_WRITE);

<<<<<<< HEAD
	ccc_io_update_iov(env, cio, io);

	if (io->u.ci_rw.crw_nonblock)
		ast_flags |= CEF_NONBLOCK;
	result = vvp_mmap_locks(env, cio, io);
	if (result == 0)
		result = ccc_io_one_lock(env, io, ast_flags, mode, start, end);
=======
	vvp_io_update_iov(env, vio, io);

	if (io->u.ci_rw.crw_nonblock)
		ast_flags |= CEF_NONBLOCK;
	result = vvp_mmap_locks(env, vio, io);
	if (result == 0)
		result = vvp_io_one_lock(env, io, ast_flags, mode, start, end);
>>>>>>> v4.9.227
	return result;
}

static int vvp_io_read_lock(const struct lu_env *env,
			    const struct cl_io_slice *ios)
{
<<<<<<< HEAD
	struct cl_io	 *io  = ios->cis_io;
	struct ll_inode_info *lli = ll_i2info(ccc_object_inode(io->ci_obj));
	int result;

	/* XXX: Layer violation, we shouldn't see lsm at llite level. */
	if (lli->lli_has_smd) /* lsm-less file doesn't need to lock */
		result = vvp_io_rw_lock(env, io, CLM_READ,
					io->u.ci_rd.rd.crw_pos,
					io->u.ci_rd.rd.crw_pos +
					io->u.ci_rd.rd.crw_count - 1);
	else
		result = 0;
=======
	struct cl_io	 *io = ios->cis_io;
	struct cl_io_rw_common *rd = &io->u.ci_rd.rd;
	int result;

	result = vvp_io_rw_lock(env, io, CLM_READ, rd->crw_pos,
				rd->crw_pos + rd->crw_count - 1);

>>>>>>> v4.9.227
	return result;
}

static int vvp_io_fault_lock(const struct lu_env *env,
			     const struct cl_io_slice *ios)
{
	struct cl_io *io   = ios->cis_io;
	struct vvp_io *vio = cl2vvp_io(env, ios);
	/*
	 * XXX LDLM_FL_CBPENDING
	 */
<<<<<<< HEAD
	return ccc_io_one_lock_index
		(env, io, 0, vvp_mode_from_vma(vio->u.fault.ft_vma),
		 io->u.ci_fault.ft_index, io->u.ci_fault.ft_index);
=======
	return vvp_io_one_lock_index(env,
				     io, 0,
				     vvp_mode_from_vma(vio->u.fault.ft_vma),
				     io->u.ci_fault.ft_index,
				     io->u.ci_fault.ft_index);
>>>>>>> v4.9.227
}

static int vvp_io_write_lock(const struct lu_env *env,
			     const struct cl_io_slice *ios)
{
	struct cl_io *io = ios->cis_io;
	loff_t start;
	loff_t end;

	if (io->u.ci_wr.wr_append) {
		start = 0;
		end   = OBD_OBJECT_EOF;
	} else {
		start = io->u.ci_wr.wr.crw_pos;
		end   = start + io->u.ci_wr.wr.crw_count - 1;
	}
	return vvp_io_rw_lock(env, io, CLM_WRITE, start, end);
}

static int vvp_io_setattr_iter_init(const struct lu_env *env,
				    const struct cl_io_slice *ios)
{
	return 0;
}

/**
<<<<<<< HEAD
 * Implementation of cl_io_operations::cio_lock() method for CIT_SETATTR io.
=======
 * Implementation of cl_io_operations::vio_lock() method for CIT_SETATTR io.
>>>>>>> v4.9.227
 *
 * Handles "lockless io" mode when extent locking is done by server.
 */
static int vvp_io_setattr_lock(const struct lu_env *env,
			       const struct cl_io_slice *ios)
{
<<<<<<< HEAD
	struct ccc_io *cio = ccc_env_io(env);
=======
>>>>>>> v4.9.227
	struct cl_io  *io  = ios->cis_io;
	__u64 new_size;
	__u32 enqflags = 0;

	if (cl_io_is_trunc(io)) {
		new_size = io->u.ci_setattr.sa_attr.lvb_size;
		if (new_size == 0)
			enqflags = CEF_DISCARD_DATA;
	} else {
		if ((io->u.ci_setattr.sa_attr.lvb_mtime >=
		     io->u.ci_setattr.sa_attr.lvb_ctime) ||
		    (io->u.ci_setattr.sa_attr.lvb_atime >=
		     io->u.ci_setattr.sa_attr.lvb_ctime))
			return 0;
		new_size = 0;
	}
<<<<<<< HEAD
	cio->u.setattr.cui_local_lock = SETATTR_EXTENT_LOCK;
	return ccc_io_one_lock(env, io, enqflags, CLM_WRITE,
=======

	return vvp_io_one_lock(env, io, enqflags, CLM_WRITE,
>>>>>>> v4.9.227
			       new_size, OBD_OBJECT_EOF);
}

static int vvp_do_vmtruncate(struct inode *inode, size_t size)
{
	int     result;
	/*
	 * Only ll_inode_size_lock is taken at this level.
	 */
	ll_inode_size_lock(inode);
	result = inode_newsize_ok(inode, size);
	if (result < 0) {
		ll_inode_size_unlock(inode);
		return result;
	}
	truncate_setsize(inode, size);
	ll_inode_size_unlock(inode);
	return result;
}

static int vvp_io_setattr_trunc(const struct lu_env *env,
				const struct cl_io_slice *ios,
				struct inode *inode, loff_t size)
{
	inode_dio_wait(inode);
	return 0;
}

static int vvp_io_setattr_time(const struct lu_env *env,
			       const struct cl_io_slice *ios)
{
	struct cl_io       *io    = ios->cis_io;
	struct cl_object   *obj   = io->ci_obj;
<<<<<<< HEAD
	struct cl_attr     *attr  = ccc_env_thread_attr(env);
=======
	struct cl_attr     *attr  = vvp_env_thread_attr(env);
>>>>>>> v4.9.227
	int result;
	unsigned valid = CAT_CTIME;

	cl_object_attr_lock(obj);
	attr->cat_ctime = io->u.ci_setattr.sa_attr.lvb_ctime;
	if (io->u.ci_setattr.sa_valid & ATTR_ATIME_SET) {
		attr->cat_atime = io->u.ci_setattr.sa_attr.lvb_atime;
		valid |= CAT_ATIME;
	}
	if (io->u.ci_setattr.sa_valid & ATTR_MTIME_SET) {
		attr->cat_mtime = io->u.ci_setattr.sa_attr.lvb_mtime;
		valid |= CAT_MTIME;
	}
<<<<<<< HEAD
	result = cl_object_attr_set(env, obj, attr, valid);
=======
	result = cl_object_attr_update(env, obj, attr, valid);
>>>>>>> v4.9.227
	cl_object_attr_unlock(obj);

	return result;
}

static int vvp_io_setattr_start(const struct lu_env *env,
				const struct cl_io_slice *ios)
{
	struct cl_io	*io    = ios->cis_io;
<<<<<<< HEAD
	struct inode	*inode = ccc_object_inode(io->ci_obj);
	int result = 0;

	mutex_lock(&inode->i_mutex);
=======
	struct inode	*inode = vvp_object_inode(io->ci_obj);
	int result = 0;

	inode_lock(inode);
>>>>>>> v4.9.227
	if (cl_io_is_trunc(io))
		result = vvp_io_setattr_trunc(env, ios, inode,
					io->u.ci_setattr.sa_attr.lvb_size);
	if (result == 0)
		result = vvp_io_setattr_time(env, ios);
	return result;
}

static void vvp_io_setattr_end(const struct lu_env *env,
			       const struct cl_io_slice *ios)
{
	struct cl_io *io    = ios->cis_io;
<<<<<<< HEAD
	struct inode *inode = ccc_object_inode(io->ci_obj);

	if (cl_io_is_trunc(io)) {
		/* Truncate in memory pages - they must be clean pages
		 * because osc has already notified to destroy osc_extents. */
		vvp_do_vmtruncate(inode, io->u.ci_setattr.sa_attr.lvb_size);
		inode_dio_write_done(inode);
	}
	mutex_unlock(&inode->i_mutex);
=======
	struct inode *inode = vvp_object_inode(io->ci_obj);

	if (cl_io_is_trunc(io))
		/* Truncate in memory pages - they must be clean pages
		 * because osc has already notified to destroy osc_extents.
		 */
		vvp_do_vmtruncate(inode, io->u.ci_setattr.sa_attr.lvb_size);

	inode_unlock(inode);
>>>>>>> v4.9.227
}

static void vvp_io_setattr_fini(const struct lu_env *env,
				const struct cl_io_slice *ios)
{
	vvp_io_fini(env, ios);
}

static int vvp_io_read_start(const struct lu_env *env,
			     const struct cl_io_slice *ios)
{
	struct vvp_io     *vio   = cl2vvp_io(env, ios);
<<<<<<< HEAD
	struct ccc_io     *cio   = cl2ccc_io(env, ios);
	struct cl_io      *io    = ios->cis_io;
	struct cl_object  *obj   = io->ci_obj;
	struct inode      *inode = ccc_object_inode(obj);
	struct ll_ra_read *bead  = &vio->cui_bead;
	struct file       *file  = cio->cui_fd->fd_file;
=======
	struct cl_io      *io    = ios->cis_io;
	struct cl_object  *obj   = io->ci_obj;
	struct inode      *inode = vvp_object_inode(obj);
	struct file       *file  = vio->vui_fd->fd_file;
>>>>>>> v4.9.227

	int     result;
	loff_t  pos = io->u.ci_rd.rd.crw_pos;
	long    cnt = io->u.ci_rd.rd.crw_count;
<<<<<<< HEAD
	long    tot = cio->cui_tot_count;
	int     exceed = 0;

	CLOBINVRNT(env, obj, ccc_object_invariant(obj));
=======
	long    tot = vio->vui_tot_count;
	int     exceed = 0;

	CLOBINVRNT(env, obj, vvp_object_invariant(obj));
>>>>>>> v4.9.227

	CDEBUG(D_VFSTRACE, "read: -> [%lli, %lli)\n", pos, pos + cnt);

	if (!can_populate_pages(env, io, inode))
		return 0;

<<<<<<< HEAD
	result = ccc_prep_size(env, obj, io, pos, tot, &exceed);
=======
	result = vvp_prep_size(env, obj, io, pos, tot, &exceed);
>>>>>>> v4.9.227
	if (result != 0)
		return result;
	else if (exceed != 0)
		goto out;

	LU_OBJECT_HEADER(D_INODE, env, &obj->co_lu,
<<<<<<< HEAD
			"Read ino %lu, %lu bytes, offset %lld, size %llu\n",
			inode->i_ino, cnt, pos, i_size_read(inode));

	/* turn off the kernel's read-ahead */
	cio->cui_fd->fd_file->f_ra.ra_pages = 0;

	/* initialize read-ahead window once per syscall */
	if (!vio->cui_ra_window_set) {
		vio->cui_ra_window_set = 1;
		bead->lrr_start = cl_index(obj, pos);
		/*
		 * XXX: explicit PAGE_CACHE_SIZE
		 */
		bead->lrr_count = cl_index(obj, tot + PAGE_CACHE_SIZE - 1);
		ll_ra_read_in(file, bead);
=======
			 "Read ino %lu, %lu bytes, offset %lld, size %llu\n",
			 inode->i_ino, cnt, pos, i_size_read(inode));

	/* turn off the kernel's read-ahead */
	vio->vui_fd->fd_file->f_ra.ra_pages = 0;

	/* initialize read-ahead window once per syscall */
	if (!vio->vui_ra_valid) {
		vio->vui_ra_valid = true;
		vio->vui_ra_start = cl_index(obj, pos);
		vio->vui_ra_count = cl_index(obj, tot + PAGE_SIZE - 1);
		ll_ras_enter(file);
>>>>>>> v4.9.227
	}

	/* BUG: 5972 */
	file_accessed(file);
<<<<<<< HEAD
	switch (vio->cui_io_subtype) {
	case IO_NORMAL:
		LASSERT(cio->cui_iocb->ki_pos == pos);
		result = generic_file_read_iter(cio->cui_iocb, cio->cui_iter);
		break;
	case IO_SPLICE:
		result = generic_file_splice_read(file, &pos,
				vio->u.splice.cui_pipe, cnt,
				vio->u.splice.cui_flags);
		/* LU-1109: do splice read stripe by stripe otherwise if it
		 * may make nfsd stuck if this read occupied all internal pipe
		 * buffers. */
		io->ci_continue = 0;
		break;
	default:
		CERROR("Wrong IO type %u\n", vio->cui_io_subtype);
		LBUG();
	}
=======
	LASSERT(vio->vui_iocb->ki_pos == pos);
	result = generic_file_read_iter(vio->vui_iocb, vio->vui_iter);
>>>>>>> v4.9.227

out:
	if (result >= 0) {
		if (result < cnt)
			io->ci_continue = 0;
		io->ci_nob += result;
		ll_rw_stats_tally(ll_i2sbi(inode), current->pid,
<<<<<<< HEAD
				  cio->cui_fd, pos, result, READ);
=======
				  vio->vui_fd, pos, result, READ);
>>>>>>> v4.9.227
		result = 0;
	}
	return result;
}

<<<<<<< HEAD
static void vvp_io_read_fini(const struct lu_env *env, const struct cl_io_slice *ios)
{
	struct vvp_io *vio = cl2vvp_io(env, ios);
	struct ccc_io *cio = cl2ccc_io(env, ios);

	if (vio->cui_ra_window_set)
		ll_ra_read_ex(cio->cui_fd->fd_file, &vio->cui_bead);

	vvp_io_fini(env, ios);
=======
static int vvp_io_commit_sync(const struct lu_env *env, struct cl_io *io,
			      struct cl_page_list *plist, int from, int to)
{
	struct cl_2queue *queue = &io->ci_queue;
	struct cl_page *page;
	unsigned int bytes = 0;
	int rc = 0;

	if (plist->pl_nr == 0)
		return 0;

	if (from > 0 || to != PAGE_SIZE) {
		page = cl_page_list_first(plist);
		if (plist->pl_nr == 1) {
			cl_page_clip(env, page, from, to);
		} else {
			if (from > 0)
				cl_page_clip(env, page, from, PAGE_SIZE);
			if (to != PAGE_SIZE) {
				page = cl_page_list_last(plist);
				cl_page_clip(env, page, 0, to);
			}
		}
	}

	cl_2queue_init(queue);
	cl_page_list_splice(plist, &queue->c2_qin);
	rc = cl_io_submit_sync(env, io, CRT_WRITE, queue, 0);

	/* plist is not sorted any more */
	cl_page_list_splice(&queue->c2_qin, plist);
	cl_page_list_splice(&queue->c2_qout, plist);
	cl_2queue_fini(env, queue);

	if (rc == 0) {
		/* calculate bytes */
		bytes = plist->pl_nr << PAGE_SHIFT;
		bytes -= from + PAGE_SIZE - to;

		while (plist->pl_nr > 0) {
			page = cl_page_list_first(plist);
			cl_page_list_del(env, plist, page);

			cl_page_clip(env, page, 0, PAGE_SIZE);

			SetPageUptodate(cl_page_vmpage(page));
			cl_page_disown(env, io, page);

			/* held in ll_cl_init() */
			lu_ref_del(&page->cp_reference, "cl_io", io);
			cl_page_put(env, page);
		}
	}

	return bytes > 0 ? bytes : rc;
}

static void write_commit_callback(const struct lu_env *env, struct cl_io *io,
				  struct cl_page *page)
{
	struct vvp_page *vpg;
	struct page *vmpage = page->cp_vmpage;
	struct cl_object *clob = cl_io_top(io)->ci_obj;

	SetPageUptodate(vmpage);
	set_page_dirty(vmpage);

	vpg = cl2vvp_page(cl_object_page_slice(clob, page));
	vvp_write_pending(cl2vvp(clob), vpg);

	cl_page_disown(env, io, page);

	/* held in ll_cl_init() */
	lu_ref_del(&page->cp_reference, "cl_io", cl_io_top(io));
	cl_page_put(env, page);
}

/* make sure the page list is contiguous */
static bool page_list_sanity_check(struct cl_object *obj,
				   struct cl_page_list *plist)
{
	struct cl_page *page;
	pgoff_t index = CL_PAGE_EOF;

	cl_page_list_for_each(page, plist) {
		struct vvp_page *vpg = cl_object_page_slice(obj, page);

		if (index == CL_PAGE_EOF) {
			index = vvp_index(vpg);
			continue;
		}

		++index;
		if (index == vvp_index(vpg))
			continue;

		return false;
	}
	return true;
}

/* Return how many bytes have queued or written */
int vvp_io_write_commit(const struct lu_env *env, struct cl_io *io)
{
	struct cl_object *obj = io->ci_obj;
	struct inode *inode = vvp_object_inode(obj);
	struct vvp_io *vio = vvp_env_io(env);
	struct cl_page_list *queue = &vio->u.write.vui_queue;
	struct cl_page *page;
	int rc = 0;
	int bytes = 0;
	unsigned int npages = vio->u.write.vui_queue.pl_nr;

	if (npages == 0)
		return 0;

	CDEBUG(D_VFSTRACE, "commit async pages: %d, from %d, to %d\n",
	       npages, vio->u.write.vui_from, vio->u.write.vui_to);

	LASSERT(page_list_sanity_check(obj, queue));

	/* submit IO with async write */
	rc = cl_io_commit_async(env, io, queue,
				vio->u.write.vui_from, vio->u.write.vui_to,
				write_commit_callback);
	npages -= queue->pl_nr; /* already committed pages */
	if (npages > 0) {
		/* calculate how many bytes were written */
		bytes = npages << PAGE_SHIFT;

		/* first page */
		bytes -= vio->u.write.vui_from;
		if (queue->pl_nr == 0) /* last page */
			bytes -= PAGE_SIZE - vio->u.write.vui_to;
		LASSERTF(bytes > 0, "bytes = %d, pages = %d\n", bytes, npages);

		vio->u.write.vui_written += bytes;

		CDEBUG(D_VFSTRACE, "Committed %d pages %d bytes, tot: %ld\n",
		       npages, bytes, vio->u.write.vui_written);

		/* the first page must have been written. */
		vio->u.write.vui_from = 0;
	}
	LASSERT(page_list_sanity_check(obj, queue));
	LASSERT(ergo(rc == 0, queue->pl_nr == 0));

	/* out of quota, try sync write */
	if (rc == -EDQUOT && !cl_io_is_mkwrite(io)) {
		rc = vvp_io_commit_sync(env, io, queue,
					vio->u.write.vui_from,
					vio->u.write.vui_to);
		if (rc > 0) {
			vio->u.write.vui_written += rc;
			rc = 0;
		}
	}

	/* update inode size */
	ll_merge_attr(env, inode);

	/* Now the pages in queue were failed to commit, discard them
	 * unless they were dirtied before.
	 */
	while (queue->pl_nr > 0) {
		page = cl_page_list_first(queue);
		cl_page_list_del(env, queue, page);

		if (!PageDirty(cl_page_vmpage(page)))
			cl_page_discard(env, io, page);

		cl_page_disown(env, io, page);

		/* held in ll_cl_init() */
		lu_ref_del(&page->cp_reference, "cl_io", io);
		cl_page_put(env, page);
	}
	cl_page_list_fini(env, queue);

	return rc;
>>>>>>> v4.9.227
}

static int vvp_io_write_start(const struct lu_env *env,
			      const struct cl_io_slice *ios)
{
<<<<<<< HEAD
	struct ccc_io      *cio   = cl2ccc_io(env, ios);
	struct cl_io       *io    = ios->cis_io;
	struct cl_object   *obj   = io->ci_obj;
	struct inode       *inode = ccc_object_inode(obj);
=======
	struct vvp_io      *vio   = cl2vvp_io(env, ios);
	struct cl_io       *io    = ios->cis_io;
	struct cl_object   *obj   = io->ci_obj;
	struct inode       *inode = vvp_object_inode(obj);
>>>>>>> v4.9.227
	ssize_t result = 0;
	loff_t pos = io->u.ci_wr.wr.crw_pos;
	size_t cnt = io->u.ci_wr.wr.crw_count;

	if (!can_populate_pages(env, io, inode))
		return 0;

	if (cl_io_is_append(io)) {
		/*
		 * PARALLEL IO This has to be changed for parallel IO doing
		 * out-of-order writes.
		 */
<<<<<<< HEAD
		pos = io->u.ci_wr.wr.crw_pos = i_size_read(inode);
		cio->cui_iocb->ki_pos = pos;
	} else {
		LASSERT(cio->cui_iocb->ki_pos == pos);
=======
		ll_merge_attr(env, inode);
		pos = i_size_read(inode);
		io->u.ci_wr.wr.crw_pos = pos;
		vio->vui_iocb->ki_pos = pos;
	} else {
		LASSERT(vio->vui_iocb->ki_pos == pos);
>>>>>>> v4.9.227
	}

	CDEBUG(D_VFSTRACE, "write: [%lli, %lli)\n", pos, pos + (long long)cnt);

<<<<<<< HEAD
	if (cio->cui_iter == NULL) /* from a temp io in ll_cl_init(). */
		result = 0;
	else
		result = generic_file_write_iter(cio->cui_iocb, cio->cui_iter);

	if (result > 0) {
		if (result < cnt)
			io->ci_continue = 0;
		io->ci_nob += result;
		ll_rw_stats_tally(ll_i2sbi(inode), current->pid,
				  cio->cui_fd, pos, result, WRITE);
=======
	if (!vio->vui_iter) {
		/* from a temp io in ll_cl_init(). */
		result = 0;
	} else {
		/*
		 * When using the locked AIO function (generic_file_aio_write())
		 * testing has shown the inode mutex to be a limiting factor
		 * with multi-threaded single shared file performance. To get
		 * around this, we now use the lockless version. To maintain
		 * consistency, proper locking to protect against writes,
		 * trucates, etc. is handled in the higher layers of lustre.
		 */
		bool lock_node = !IS_NOSEC(inode);

		if (lock_node)
			inode_lock(inode);
		result = __generic_file_write_iter(vio->vui_iocb,
						   vio->vui_iter);
		if (lock_node)
			inode_unlock(inode);

		if (result > 0 || result == -EIOCBQUEUED)
			result = generic_write_sync(vio->vui_iocb, result);
	}

	if (result > 0) {
		result = vvp_io_write_commit(env, io);
		if (vio->u.write.vui_written > 0) {
			result = vio->u.write.vui_written;
			io->ci_nob += result;

			CDEBUG(D_VFSTRACE, "write: nob %zd, result: %zd\n",
			       io->ci_nob, result);
		}
	}
	if (result > 0) {
		struct ll_inode_info *lli = ll_i2info(inode);

		spin_lock(&lli->lli_lock);
		lli->lli_flags |= LLIF_DATA_MODIFIED;
		spin_unlock(&lli->lli_lock);

		if (result < cnt)
			io->ci_continue = 0;
		ll_rw_stats_tally(ll_i2sbi(inode), current->pid,
				  vio->vui_fd, pos, result, WRITE);
>>>>>>> v4.9.227
		result = 0;
	}
	return result;
}

static int vvp_io_kernel_fault(struct vvp_fault_io *cfio)
{
<<<<<<< HEAD
	struct vm_fault *vmf = cfio->fault.ft_vmf;

	cfio->fault.ft_flags = filemap_fault(cfio->ft_vma, vmf);
	cfio->fault.ft_flags_valid = 1;
=======
	struct vm_fault *vmf = cfio->ft_vmf;

	cfio->ft_flags = filemap_fault(cfio->ft_vma, vmf);
	cfio->ft_flags_valid = 1;
>>>>>>> v4.9.227

	if (vmf->page) {
		CDEBUG(D_PAGE,
		       "page %p map %p index %lu flags %lx count %u priv %0lx: got addr %p type NOPAGE\n",
		       vmf->page, vmf->page->mapping, vmf->page->index,
		       (long)vmf->page->flags, page_count(vmf->page),
		       page_private(vmf->page), vmf->virtual_address);
<<<<<<< HEAD
		if (unlikely(!(cfio->fault.ft_flags & VM_FAULT_LOCKED))) {
			lock_page(vmf->page);
			cfio->fault.ft_flags |= VM_FAULT_LOCKED;
=======
		if (unlikely(!(cfio->ft_flags & VM_FAULT_LOCKED))) {
			lock_page(vmf->page);
			cfio->ft_flags |= VM_FAULT_LOCKED;
>>>>>>> v4.9.227
		}

		cfio->ft_vmpage = vmf->page;
		return 0;
	}

<<<<<<< HEAD
	if (cfio->fault.ft_flags & (VM_FAULT_SIGBUS | VM_FAULT_SIGSEGV)) {
=======
	if (cfio->ft_flags & (VM_FAULT_SIGBUS | VM_FAULT_SIGSEGV)) {
>>>>>>> v4.9.227
		CDEBUG(D_PAGE, "got addr %p - SIGBUS\n", vmf->virtual_address);
		return -EFAULT;
	}

<<<<<<< HEAD
	if (cfio->fault.ft_flags & VM_FAULT_OOM) {
=======
	if (cfio->ft_flags & VM_FAULT_OOM) {
>>>>>>> v4.9.227
		CDEBUG(D_PAGE, "got addr %p - OOM\n", vmf->virtual_address);
		return -ENOMEM;
	}

<<<<<<< HEAD
	if (cfio->fault.ft_flags & VM_FAULT_RETRY)
		return -EAGAIN;

	CERROR("Unknown error in page fault %d!\n", cfio->fault.ft_flags);
	return -EINVAL;
}

=======
	if (cfio->ft_flags & VM_FAULT_RETRY)
		return -EAGAIN;

	CERROR("Unknown error in page fault %d!\n", cfio->ft_flags);
	return -EINVAL;
}

static void mkwrite_commit_callback(const struct lu_env *env, struct cl_io *io,
				    struct cl_page *page)
{
	struct vvp_page *vpg;
	struct cl_object *clob = cl_io_top(io)->ci_obj;

	set_page_dirty(page->cp_vmpage);

	vpg = cl2vvp_page(cl_object_page_slice(clob, page));
	vvp_write_pending(cl2vvp(clob), vpg);
}
>>>>>>> v4.9.227

static int vvp_io_fault_start(const struct lu_env *env,
			      const struct cl_io_slice *ios)
{
	struct vvp_io       *vio     = cl2vvp_io(env, ios);
	struct cl_io	*io      = ios->cis_io;
	struct cl_object    *obj     = io->ci_obj;
<<<<<<< HEAD
	struct inode	*inode   = ccc_object_inode(obj);
=======
	struct inode        *inode   = vvp_object_inode(obj);
>>>>>>> v4.9.227
	struct cl_fault_io  *fio     = &io->u.ci_fault;
	struct vvp_fault_io *cfio    = &vio->u.fault;
	loff_t	       offset;
	int		  result  = 0;
	struct page	  *vmpage  = NULL;
	struct cl_page      *page;
	loff_t	       size;
<<<<<<< HEAD
	pgoff_t	      last; /* last page in a file data region */

	if (fio->ft_executable &&
	    LTIME_S(inode->i_mtime) != vio->u.fault.ft_mtime)
=======
	pgoff_t		     last_index;

	if (fio->ft_executable &&
	    inode->i_mtime.tv_sec != vio->u.fault.ft_mtime)
>>>>>>> v4.9.227
		CWARN("binary "DFID
		      " changed while waiting for the page fault lock\n",
		      PFID(lu_object_fid(&obj->co_lu)));

	/* offset of the last byte on the page */
	offset = cl_offset(obj, fio->ft_index + 1) - 1;
	LASSERT(cl_index(obj, offset) == fio->ft_index);
<<<<<<< HEAD
	result = ccc_prep_size(env, obj, io, 0, offset + 1, NULL);
=======
	result = vvp_prep_size(env, obj, io, 0, offset + 1, NULL);
>>>>>>> v4.9.227
	if (result != 0)
		return result;

	/* must return locked page */
	if (fio->ft_mkwrite) {
<<<<<<< HEAD
		LASSERT(cfio->ft_vmpage != NULL);
=======
		LASSERT(cfio->ft_vmpage);
>>>>>>> v4.9.227
		lock_page(cfio->ft_vmpage);
	} else {
		result = vvp_io_kernel_fault(cfio);
		if (result != 0)
			return result;
	}

	vmpage = cfio->ft_vmpage;
	LASSERT(PageLocked(vmpage));

	if (OBD_FAIL_CHECK(OBD_FAIL_LLITE_FAULT_TRUNC_RACE))
		ll_invalidate_page(vmpage);

	size = i_size_read(inode);
	/* Though we have already held a cl_lock upon this page, but
<<<<<<< HEAD
	 * it still can be truncated locally. */
=======
	 * it still can be truncated locally.
	 */
>>>>>>> v4.9.227
	if (unlikely((vmpage->mapping != inode->i_mapping) ||
		     (page_offset(vmpage) > size))) {
		CDEBUG(D_PAGE, "llite: fault and truncate race happened!\n");

		/* return +1 to stop cl_io_loop() and ll_fault() will catch
<<<<<<< HEAD
		 * and retry. */
		result = +1;
		goto out;
	}


	if (fio->ft_mkwrite ) {
		pgoff_t last_index;
=======
		 * and retry.
		 */
		result = 1;
		goto out;
	}

	last_index = cl_index(obj, size - 1);

	if (fio->ft_mkwrite) {
>>>>>>> v4.9.227
		/*
		 * Capture the size while holding the lli_trunc_sem from above
		 * we want to make sure that we complete the mkwrite action
		 * while holding this lock. We need to make sure that we are
		 * not past the end of the file.
		 */
<<<<<<< HEAD
		last_index = cl_index(obj, size - 1);
		if (last_index < fio->ft_index) {
			CDEBUG(D_PAGE,
				"llite: mkwrite and truncate race happened: "
				"%p: 0x%lx 0x%lx\n",
				vmpage->mapping, fio->ft_index, last_index);
=======
		if (last_index < fio->ft_index) {
			CDEBUG(D_PAGE,
			       "llite: mkwrite and truncate race happened: %p: 0x%lx 0x%lx\n",
			       vmpage->mapping, fio->ft_index, last_index);
>>>>>>> v4.9.227
			/*
			 * We need to return if we are
			 * passed the end of the file. This will propagate
			 * up the call stack to ll_page_mkwrite where
			 * we will return VM_FAULT_NOPAGE. Any non-negative
			 * value returned here will be silently
			 * converted to 0. If the vmpage->mapping is null
			 * the error code would be converted back to ENODATA
			 * in ll_page_mkwrite0. Thus we return -ENODATA
			 * to handle both cases
			 */
			result = -ENODATA;
			goto out;
		}
	}

	page = cl_page_find(env, obj, fio->ft_index, vmpage, CPT_CACHEABLE);
	if (IS_ERR(page)) {
		result = PTR_ERR(page);
		goto out;
	}

	/* if page is going to be written, we should add this page into cache
<<<<<<< HEAD
	 * earlier. */
	if (fio->ft_mkwrite) {
		wait_on_page_writeback(vmpage);
		if (set_page_dirty(vmpage)) {
			struct ccc_page *cp;
=======
	 * earlier.
	 */
	if (fio->ft_mkwrite) {
		wait_on_page_writeback(vmpage);
		if (!PageDirty(vmpage)) {
			struct cl_page_list *plist = &io->ci_queue.c2_qin;
			struct vvp_page *vpg = cl_object_page_slice(obj, page);
			int to = PAGE_SIZE;
>>>>>>> v4.9.227

			/* vvp_page_assume() calls wait_on_page_writeback(). */
			cl_page_assume(env, io, page);

<<<<<<< HEAD
			cp = cl2ccc_page(cl_page_at(page, &vvp_device_type));
			vvp_write_pending(cl2ccc(obj), cp);

			/* Do not set Dirty bit here so that in case IO is
			 * started before the page is really made dirty, we
			 * still have chance to detect it. */
			result = cl_page_cache_add(env, io, page, CRT_WRITE);
			LASSERT(cl_page_is_owned(page, io));

			vmpage = NULL;
			if (result < 0) {
				cl_page_unmap(env, io, page);
=======
			cl_page_list_init(plist);
			cl_page_list_add(plist, page);

			/* size fixup */
			if (last_index == vvp_index(vpg))
				to = size & ~PAGE_MASK;

			/* Do not set Dirty bit here so that in case IO is
			 * started before the page is really made dirty, we
			 * still have chance to detect it.
			 */
			result = cl_io_commit_async(env, io, plist, 0, to,
						    mkwrite_commit_callback);
			LASSERT(cl_page_is_owned(page, io));
			cl_page_list_fini(env, plist);

			vmpage = NULL;
			if (result < 0) {
>>>>>>> v4.9.227
				cl_page_discard(env, io, page);
				cl_page_disown(env, io, page);

				cl_page_put(env, page);

				/* we're in big trouble, what can we do now? */
				if (result == -EDQUOT)
					result = -ENOSPC;
				goto out;
<<<<<<< HEAD
			} else
				cl_page_disown(env, io, page);
		}
	}

	last = cl_index(obj, size - 1);
=======
			} else {
				cl_page_disown(env, io, page);
			}
		}
	}

>>>>>>> v4.9.227
	/*
	 * The ft_index is only used in the case of
	 * a mkwrite action. We need to check
	 * our assertions are correct, since
	 * we should have caught this above
	 */
<<<<<<< HEAD
	LASSERT(!fio->ft_mkwrite || fio->ft_index <= last);
	if (fio->ft_index == last)
=======
	LASSERT(!fio->ft_mkwrite || fio->ft_index <= last_index);
	if (fio->ft_index == last_index)
>>>>>>> v4.9.227
		/*
		 * Last page is mapped partially.
		 */
		fio->ft_nob = size - cl_offset(obj, fio->ft_index);
	else
		fio->ft_nob = cl_page_size(obj);

	lu_ref_add(&page->cp_reference, "fault", io);
	fio->ft_page = page;

out:
	/* return unlocked vmpage to avoid deadlocking */
<<<<<<< HEAD
	if (vmpage != NULL)
		unlock_page(vmpage);
	cfio->fault.ft_flags &= ~VM_FAULT_LOCKED;
=======
	if (vmpage)
		unlock_page(vmpage);

	cfio->ft_flags &= ~VM_FAULT_LOCKED;

>>>>>>> v4.9.227
	return result;
}

static int vvp_io_fsync_start(const struct lu_env *env,
			      const struct cl_io_slice *ios)
{
	/* we should mark TOWRITE bit to each dirty page in radix tree to
	 * verify pages have been written, but this is difficult because of
<<<<<<< HEAD
	 * race. */
=======
	 * race.
	 */
>>>>>>> v4.9.227
	return 0;
}

static int vvp_io_read_page(const struct lu_env *env,
			    const struct cl_io_slice *ios,
			    const struct cl_page_slice *slice)
{
	struct cl_io	      *io     = ios->cis_io;
<<<<<<< HEAD
	struct cl_object	  *obj    = slice->cpl_obj;
	struct ccc_page	   *cp     = cl2ccc_page(slice);
	struct cl_page	    *page   = slice->cpl_page;
	struct inode	      *inode  = ccc_object_inode(obj);
	struct ll_sb_info	 *sbi    = ll_i2sbi(inode);
	struct ll_file_data       *fd     = cl2ccc_io(env, ios)->cui_fd;
	struct ll_readahead_state *ras    = &fd->fd_ras;
	struct page		*vmpage = cp->cpg_page;
	struct cl_2queue	  *queue  = &io->ci_queue;
	int rc;

	CLOBINVRNT(env, obj, ccc_object_invariant(obj));
	LASSERT(slice->cpl_obj == obj);

	if (sbi->ll_ra_info.ra_max_pages_per_file &&
	    sbi->ll_ra_info.ra_max_pages)
		ras_update(sbi, inode, ras, page->cp_index,
			   cp->cpg_defer_uptodate);

	/* Sanity check whether the page is protected by a lock. */
	rc = cl_page_is_under_lock(env, io, page);
	if (rc != -EBUSY) {
		CL_PAGE_HEADER(D_WARNING, env, page, "%s: %d\n",
			       rc == -ENODATA ? "without a lock" :
			       "match failed", rc);
		if (rc != -ENODATA)
			return rc;
	}

	if (cp->cpg_defer_uptodate) {
		cp->cpg_ra_used = 1;
=======
	struct vvp_page           *vpg    = cl2vvp_page(slice);
	struct cl_page	    *page   = slice->cpl_page;
	struct inode              *inode  = vvp_object_inode(slice->cpl_obj);
	struct ll_sb_info	 *sbi    = ll_i2sbi(inode);
	struct ll_file_data       *fd     = cl2vvp_io(env, ios)->vui_fd;
	struct ll_readahead_state *ras    = &fd->fd_ras;
	struct cl_2queue	  *queue  = &io->ci_queue;

	if (sbi->ll_ra_info.ra_max_pages_per_file &&
	    sbi->ll_ra_info.ra_max_pages)
		ras_update(sbi, inode, ras, vvp_index(vpg),
			   vpg->vpg_defer_uptodate);

	if (vpg->vpg_defer_uptodate) {
		vpg->vpg_ra_used = 1;
>>>>>>> v4.9.227
		cl_page_export(env, page, 1);
	}
	/*
	 * Add page into the queue even when it is marked uptodate above.
	 * this will unlock it automatically as part of cl_page_list_disown().
	 */
<<<<<<< HEAD
	cl_2queue_add(queue, page);
	if (sbi->ll_ra_info.ra_max_pages_per_file &&
	    sbi->ll_ra_info.ra_max_pages)
		ll_readahead(env, io, ras,
			     vmpage->mapping, &queue->c2_qin, fd->fd_flags);
=======

	cl_page_list_add(&queue->c2_qin, page);
	if (sbi->ll_ra_info.ra_max_pages_per_file &&
	    sbi->ll_ra_info.ra_max_pages)
		ll_readahead(env, io, &queue->c2_qin, ras,
			     vpg->vpg_defer_uptodate);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int vvp_page_sync_io(const struct lu_env *env, struct cl_io *io,
			    struct cl_page *page, struct ccc_page *cp,
			    enum cl_req_type crt)
{
	struct cl_2queue  *queue;
	int result;

	LASSERT(io->ci_type == CIT_READ || io->ci_type == CIT_WRITE);

	queue = &io->ci_queue;
	cl_2queue_init_page(queue, page);

	result = cl_io_submit_sync(env, io, crt, queue, 0);
	LASSERT(cl_page_is_owned(page, io));

	if (crt == CRT_READ)
		/*
		 * in CRT_WRITE case page is left locked even in case of
		 * error.
		 */
		cl_page_list_disown(env, io, &queue->c2_qin);
	cl_2queue_fini(env, queue);

	return result;
}

/**
 * Prepare partially written-to page for a write.
 */
static int vvp_io_prepare_partial(const struct lu_env *env, struct cl_io *io,
				  struct cl_object *obj, struct cl_page *pg,
				  struct ccc_page *cp,
				  unsigned from, unsigned to)
{
	struct cl_attr *attr   = ccc_env_thread_attr(env);
	loff_t	  offset = cl_offset(obj, pg->cp_index);
	int	     result;

	cl_object_attr_lock(obj);
	result = cl_object_attr_get(env, obj, attr);
	cl_object_attr_unlock(obj);
	if (result == 0) {
		/*
		 * If are writing to a new page, no need to read old data.
		 * The extent locking will have updated the KMS, and for our
		 * purposes here we can treat it like i_size.
		 */
		if (attr->cat_kms <= offset) {
			char *kaddr = kmap_atomic(cp->cpg_page);

			memset(kaddr, 0, cl_page_size(obj));
			kunmap_atomic(kaddr);
		} else if (cp->cpg_defer_uptodate)
			cp->cpg_ra_used = 1;
		else
			result = vvp_page_sync_io(env, io, pg, cp, CRT_READ);
		/*
		 * In older implementations, obdo_refresh_inode is called here
		 * to update the inode because the write might modify the
		 * object info at OST. However, this has been proven useless,
		 * since LVB functions will be called when user space program
		 * tries to retrieve inode attribute.  Also, see bug 15909 for
		 * details. -jay
		 */
		if (result == 0)
			cl_page_export(env, pg, 1);
	}
	return result;
}

static int vvp_io_prepare_write(const struct lu_env *env,
				const struct cl_io_slice *ios,
				const struct cl_page_slice *slice,
				unsigned from, unsigned to)
{
	struct cl_object *obj    = slice->cpl_obj;
	struct ccc_page  *cp     = cl2ccc_page(slice);
	struct cl_page   *pg     = slice->cpl_page;
	struct page       *vmpage = cp->cpg_page;

	int result;

	LINVRNT(cl_page_is_vmlocked(env, pg));
	LASSERT(vmpage->mapping->host == ccc_object_inode(obj));

	result = 0;

	CL_PAGE_HEADER(D_PAGE, env, pg, "preparing: [%d, %d]\n", from, to);
	if (!PageUptodate(vmpage)) {
		/*
		 * We're completely overwriting an existing page, so _don't_
		 * set it up to date until commit_write
		 */
		if (from == 0 && to == PAGE_CACHE_SIZE) {
			CL_PAGE_HEADER(D_PAGE, env, pg, "full page write\n");
			POISON_PAGE(page, 0x11);
		} else
			result = vvp_io_prepare_partial(env, ios->cis_io, obj,
							pg, cp, from, to);
	} else
		CL_PAGE_HEADER(D_PAGE, env, pg, "uptodate\n");
	return result;
}

static int vvp_io_commit_write(const struct lu_env *env,
			       const struct cl_io_slice *ios,
			       const struct cl_page_slice *slice,
			       unsigned from, unsigned to)
{
	struct cl_object  *obj    = slice->cpl_obj;
	struct cl_io      *io     = ios->cis_io;
	struct ccc_page   *cp     = cl2ccc_page(slice);
	struct cl_page    *pg     = slice->cpl_page;
	struct inode      *inode  = ccc_object_inode(obj);
	struct ll_sb_info *sbi    = ll_i2sbi(inode);
	struct ll_inode_info *lli = ll_i2info(inode);
	struct page	*vmpage = cp->cpg_page;

	int    result;
	int    tallyop;
	loff_t size;

	LINVRNT(cl_page_is_vmlocked(env, pg));
	LASSERT(vmpage->mapping->host == inode);

	LU_OBJECT_HEADER(D_INODE, env, &obj->co_lu, "committing page write\n");
	CL_PAGE_HEADER(D_PAGE, env, pg, "committing: [%d, %d]\n", from, to);

	/*
	 * queue a write for some time in the future the first time we
	 * dirty the page.
	 *
	 * This is different from what other file systems do: they usually
	 * just mark page (and some of its buffers) dirty and rely on
	 * balance_dirty_pages() to start a write-back. Lustre wants write-back
	 * to be started earlier for the following reasons:
	 *
	 *     (1) with a large number of clients we need to limit the amount
	 *     of cached data on the clients a lot;
	 *
	 *     (2) large compute jobs generally want compute-only then io-only
	 *     and the IO should complete as quickly as possible;
	 *
	 *     (3) IO is batched up to the RPC size and is async until the
	 *     client max cache is hit
	 *     (/proc/fs/lustre/osc/OSC.../max_dirty_mb)
	 *
	 */
	if (!PageDirty(vmpage)) {
		tallyop = LPROC_LL_DIRTY_MISSES;
		result = cl_page_cache_add(env, io, pg, CRT_WRITE);
		if (result == 0) {
			/* page was added into cache successfully. */
			set_page_dirty(vmpage);
			vvp_write_pending(cl2ccc(obj), cp);
		} else if (result == -EDQUOT) {
			pgoff_t last_index = i_size_read(inode) >> PAGE_CACHE_SHIFT;
			bool need_clip = true;

			/*
			 * Client ran out of disk space grant. Possible
			 * strategies are:
			 *
			 *     (a) do a sync write, renewing grant;
			 *
			 *     (b) stop writing on this stripe, switch to the
			 *     next one.
			 *
			 * (b) is a part of "parallel io" design that is the
			 * ultimate goal. (a) is what "old" client did, and
			 * what the new code continues to do for the time
			 * being.
			 */
			if (last_index > pg->cp_index) {
				to = PAGE_CACHE_SIZE;
				need_clip = false;
			} else if (last_index == pg->cp_index) {
				int size_to = i_size_read(inode) & ~CFS_PAGE_MASK;
				if (to < size_to)
					to = size_to;
			}
			if (need_clip)
				cl_page_clip(env, pg, 0, to);
			result = vvp_page_sync_io(env, io, pg, cp, CRT_WRITE);
			if (result)
				CERROR("Write page %lu of inode %p failed %d\n",
				       pg->cp_index, inode, result);
		}
	} else {
		tallyop = LPROC_LL_DIRTY_HITS;
		result = 0;
	}
	ll_stats_ops_tally(sbi, tallyop, 1);

	/* Inode should be marked DIRTY even if no new page was marked DIRTY
	 * because page could have been not flushed between 2 modifications.
	 * It is important the file is marked DIRTY as soon as the I/O is done
	 * Indeed, when cache is flushed, file could be already closed and it
	 * is too late to warn the MDT.
	 * It is acceptable that file is marked DIRTY even if I/O is dropped
	 * for some reasons before being flushed to OST.
	 */
	if (result == 0) {
		spin_lock(&lli->lli_lock);
		lli->lli_flags |= LLIF_DATA_MODIFIED;
		spin_unlock(&lli->lli_lock);
	}

	size = cl_offset(obj, pg->cp_index) + to;

	ll_inode_size_lock(inode);
	if (result == 0) {
		if (size > i_size_read(inode)) {
			cl_isize_write_nolock(inode, size);
			CDEBUG(D_VFSTRACE, DFID" updating i_size %lu\n",
			       PFID(lu_object_fid(&obj->co_lu)),
			       (unsigned long)size);
		}
		cl_page_export(env, pg, 1);
	} else {
		if (size > i_size_read(inode))
			cl_page_discard(env, io, pg);
	}
	ll_inode_size_unlock(inode);
	return result;
=======
static void vvp_io_end(const struct lu_env *env, const struct cl_io_slice *ios)
{
	CLOBINVRNT(env, ios->cis_io->ci_obj,
		   vvp_object_invariant(ios->cis_io->ci_obj));
>>>>>>> v4.9.227
}

static const struct cl_io_operations vvp_io_ops = {
	.op = {
		[CIT_READ] = {
<<<<<<< HEAD
			.cio_fini      = vvp_io_read_fini,
			.cio_lock      = vvp_io_read_lock,
			.cio_start     = vvp_io_read_start,
			.cio_advance   = ccc_io_advance
		},
		[CIT_WRITE] = {
			.cio_fini      = vvp_io_fini,
			.cio_lock      = vvp_io_write_lock,
			.cio_start     = vvp_io_write_start,
			.cio_advance   = ccc_io_advance
=======
			.cio_fini	= vvp_io_fini,
			.cio_lock      = vvp_io_read_lock,
			.cio_start     = vvp_io_read_start,
			.cio_advance	= vvp_io_advance,
		},
		[CIT_WRITE] = {
			.cio_fini      = vvp_io_fini,
			.cio_iter_init = vvp_io_write_iter_init,
			.cio_iter_fini = vvp_io_write_iter_fini,
			.cio_lock      = vvp_io_write_lock,
			.cio_start     = vvp_io_write_start,
			.cio_advance   = vvp_io_advance,
>>>>>>> v4.9.227
		},
		[CIT_SETATTR] = {
			.cio_fini       = vvp_io_setattr_fini,
			.cio_iter_init  = vvp_io_setattr_iter_init,
			.cio_lock       = vvp_io_setattr_lock,
			.cio_start      = vvp_io_setattr_start,
			.cio_end	= vvp_io_setattr_end
		},
		[CIT_FAULT] = {
			.cio_fini      = vvp_io_fault_fini,
			.cio_iter_init = vvp_io_fault_iter_init,
			.cio_lock      = vvp_io_fault_lock,
			.cio_start     = vvp_io_fault_start,
<<<<<<< HEAD
			.cio_end       = ccc_io_end
=======
			.cio_end       = vvp_io_end,
>>>>>>> v4.9.227
		},
		[CIT_FSYNC] = {
			.cio_start  = vvp_io_fsync_start,
			.cio_fini   = vvp_io_fini
		},
		[CIT_MISC] = {
			.cio_fini   = vvp_io_fini
		}
	},
	.cio_read_page     = vvp_io_read_page,
<<<<<<< HEAD
	.cio_prepare_write = vvp_io_prepare_write,
	.cio_commit_write  = vvp_io_commit_write
=======
>>>>>>> v4.9.227
};

int vvp_io_init(const struct lu_env *env, struct cl_object *obj,
		struct cl_io *io)
{
	struct vvp_io      *vio   = vvp_env_io(env);
<<<<<<< HEAD
	struct ccc_io      *cio   = ccc_env_io(env);
	struct inode       *inode = ccc_object_inode(obj);
	int		 result;

	CLOBINVRNT(env, obj, ccc_object_invariant(obj));
=======
	struct inode       *inode = vvp_object_inode(obj);
	int		 result;

	CLOBINVRNT(env, obj, vvp_object_invariant(obj));
>>>>>>> v4.9.227

	CDEBUG(D_VFSTRACE, DFID
	       " ignore/verify layout %d/%d, layout version %d restore needed %d\n",
	       PFID(lu_object_fid(&obj->co_lu)),
	       io->ci_ignore_layout, io->ci_verify_layout,
<<<<<<< HEAD
	       cio->cui_layout_gen, io->ci_restore_needed);

	CL_IO_SLICE_CLEAN(cio, cui_cl);
	cl_io_slice_add(io, &cio->cui_cl, obj, &vvp_io_ops);
	vio->cui_ra_window_set = 0;
=======
	       vio->vui_layout_gen, io->ci_restore_needed);

	CL_IO_SLICE_CLEAN(vio, vui_cl);
	cl_io_slice_add(io, &vio->vui_cl, obj, &vvp_io_ops);
	vio->vui_ra_valid = false;
>>>>>>> v4.9.227
	result = 0;
	if (io->ci_type == CIT_READ || io->ci_type == CIT_WRITE) {
		size_t count;
		struct ll_inode_info *lli = ll_i2info(inode);

		count = io->u.ci_rw.crw_count;
		/* "If nbyte is 0, read() will return 0 and have no other
<<<<<<< HEAD
		 *  results."  -- Single Unix Spec */
		if (count == 0)
			result = 1;
		else
			cio->cui_tot_count = count;
=======
		 *  results."  -- Single Unix Spec
		 */
		if (count == 0)
			result = 1;
		else
			vio->vui_tot_count = count;
>>>>>>> v4.9.227

		/* for read/write, we store the jobid in the inode, and
		 * it'll be fetched by osc when building RPC.
		 *
		 * it's not accurate if the file is shared by different
		 * jobs.
		 */
		lustre_get_jobid(lli->lli_jobid);
	} else if (io->ci_type == CIT_SETATTR) {
		if (!cl_io_is_trunc(io))
			io->ci_lockreq = CILR_MANDATORY;
	}

	/* ignore layout change for generic CIT_MISC but not for glimpse.
	 * io context for glimpse must set ci_verify_layout to true,
<<<<<<< HEAD
	 * see cl_glimpse_size0() for details. */
=======
	 * see cl_glimpse_size0() for details.
	 */
>>>>>>> v4.9.227
	if (io->ci_type == CIT_MISC && !io->ci_verify_layout)
		io->ci_ignore_layout = 1;

	/* Enqueue layout lock and get layout version. We need to do this
	 * even for operations requiring to open file, such as read and write,
<<<<<<< HEAD
	 * because it might not grant layout lock in IT_OPEN. */
	if (result == 0 && !io->ci_ignore_layout) {
		result = ll_layout_refresh(inode, &cio->cui_layout_gen);
=======
	 * because it might not grant layout lock in IT_OPEN.
	 */
	if (result == 0 && !io->ci_ignore_layout) {
		result = ll_layout_refresh(inode, &vio->vui_layout_gen);
>>>>>>> v4.9.227
		if (result == -ENOENT)
			/* If the inode on MDS has been removed, but the objects
			 * on OSTs haven't been destroyed (async unlink), layout
			 * fetch will return -ENOENT, we'd ignore this error
<<<<<<< HEAD
			 * and continue with dirty flush. LU-3230. */
			result = 0;
		if (result < 0)
			CERROR("%s: refresh file layout " DFID " error %d.\n",
				ll_get_fsname(inode->i_sb, NULL, 0),
				PFID(lu_object_fid(&obj->co_lu)), result);
=======
			 * and continue with dirty flush. LU-3230.
			 */
			result = 0;
		if (result < 0)
			CERROR("%s: refresh file layout " DFID " error %d.\n",
			       ll_get_fsname(inode->i_sb, NULL, 0),
			       PFID(lu_object_fid(&obj->co_lu)), result);
>>>>>>> v4.9.227
	}

	return result;
}
<<<<<<< HEAD

static struct vvp_io *cl2vvp_io(const struct lu_env *env,
				const struct cl_io_slice *slice)
{
	/* Calling just for assertion */
	cl2ccc_io(env, slice);
	return vvp_env_io(env);
}
=======
>>>>>>> v4.9.227
