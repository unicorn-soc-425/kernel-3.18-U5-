/*
 * Machine specific setup for xen
 *
 * Jeremy Fitzhardinge <jeremy@xensource.com>, XenSource Inc, 2007
 */

<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/init.h>
>>>>>>> v4.9.227
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/pm.h>
#include <linux/memblock.h>
#include <linux/cpuidle.h>
#include <linux/cpufreq.h>

#include <asm/elf.h>
#include <asm/vdso.h>
#include <asm/e820.h>
#include <asm/setup.h>
#include <asm/acpi.h>
#include <asm/numa.h>
<<<<<<< HEAD
=======
#include <asm/sections.h>
>>>>>>> v4.9.227
#include <asm/xen/hypervisor.h>
#include <asm/xen/hypercall.h>

#include <xen/xen.h>
#include <xen/page.h>
#include <xen/interface/callback.h>
#include <xen/interface/memory.h>
#include <xen/interface/physdev.h>
#include <xen/features.h>
<<<<<<< HEAD
#include "xen-ops.h"
#include "vdso.h"
#include "p2m.h"

/* These are code, but not functions.  Defined in entry.S */
extern const char xen_hypervisor_callback[];
extern const char xen_failsafe_callback[];
#ifdef CONFIG_X86_64
extern asmlinkage void nmi(void);
#endif
extern void xen_sysenter_target(void);
extern void xen_syscall_target(void);
extern void xen_syscall32_target(void);
=======
#include <xen/hvc-console.h>
#include "xen-ops.h"
#include "vdso.h"
#include "mmu.h"

#define GB(x) ((uint64_t)(x) * 1024 * 1024 * 1024)
>>>>>>> v4.9.227

/* Amount of extra memory space we add to the e820 ranges */
struct xen_memory_region xen_extra_mem[XEN_EXTRA_MEM_MAX_REGIONS] __initdata;

/* Number of pages released from the initial allocation. */
unsigned long xen_released_pages;

<<<<<<< HEAD
/* Buffer used to remap identity mapped pages */
unsigned long xen_remap_buf[P2M_PER_PAGE] __initdata;
=======
/* E820 map used during setting up memory. */
static struct e820entry xen_e820_map[E820MAX] __initdata;
static u32 xen_e820_map_entries __initdata;

/*
 * Buffer used to remap identity mapped pages. We only need the virtual space.
 * The physical page behind this address is remapped as needed to different
 * buffer pages.
 */
#define REMAP_SIZE	(P2M_PER_PAGE - 3)
static struct {
	unsigned long	next_area_mfn;
	unsigned long	target_pfn;
	unsigned long	size;
	unsigned long	mfns[REMAP_SIZE];
} xen_remap_buf __initdata __aligned(PAGE_SIZE);
static unsigned long xen_remap_mfn __initdata = INVALID_P2M_ENTRY;
>>>>>>> v4.9.227

/* 
 * The maximum amount of extra memory compared to the base size.  The
 * main scaling factor is the size of struct page.  At extreme ratios
 * of base:extra, all the base memory can be filled with page
 * structures for the extra memory, leaving no space for anything
 * else.
 * 
 * 10x seems like a reasonable balance between scaling flexibility and
 * leaving a practically usable system.
 */
#define EXTRA_MEM_RATIO		(10)

<<<<<<< HEAD
static void __init xen_add_extra_mem(u64 start, u64 size)
{
	unsigned long pfn;
	int i;

	for (i = 0; i < XEN_EXTRA_MEM_MAX_REGIONS; i++) {
		/* Add new region. */
		if (xen_extra_mem[i].size == 0) {
			xen_extra_mem[i].start = start;
			xen_extra_mem[i].size  = size;
			break;
		}
		/* Append to existing region. */
		if (xen_extra_mem[i].start + xen_extra_mem[i].size == start) {
			xen_extra_mem[i].size += size;
=======
static bool xen_512gb_limit __initdata = IS_ENABLED(CONFIG_XEN_512GB);

static void __init xen_parse_512gb(void)
{
	bool val = false;
	char *arg;

	arg = strstr(xen_start_info->cmd_line, "xen_512gb_limit");
	if (!arg)
		return;

	arg = strstr(xen_start_info->cmd_line, "xen_512gb_limit=");
	if (!arg)
		val = true;
	else if (strtobool(arg + strlen("xen_512gb_limit="), &val))
		return;

	xen_512gb_limit = val;
}

static void __init xen_add_extra_mem(unsigned long start_pfn,
				     unsigned long n_pfns)
{
	int i;

	/*
	 * No need to check for zero size, should happen rarely and will only
	 * write a new entry regarded to be unused due to zero size.
	 */
	for (i = 0; i < XEN_EXTRA_MEM_MAX_REGIONS; i++) {
		/* Add new region. */
		if (xen_extra_mem[i].n_pfns == 0) {
			xen_extra_mem[i].start_pfn = start_pfn;
			xen_extra_mem[i].n_pfns = n_pfns;
			break;
		}
		/* Append to existing region. */
		if (xen_extra_mem[i].start_pfn + xen_extra_mem[i].n_pfns ==
		    start_pfn) {
			xen_extra_mem[i].n_pfns += n_pfns;
>>>>>>> v4.9.227
			break;
		}
	}
	if (i == XEN_EXTRA_MEM_MAX_REGIONS)
		printk(KERN_WARNING "Warning: not enough extra memory regions\n");

<<<<<<< HEAD
	memblock_reserve(start, size);

	xen_max_p2m_pfn = PFN_DOWN(start + size);
	for (pfn = PFN_DOWN(start); pfn < xen_max_p2m_pfn; pfn++) {
		unsigned long mfn = pfn_to_mfn(pfn);

		if (WARN_ONCE(mfn == pfn, "Trying to over-write 1-1 mapping (pfn: %lx)\n", pfn))
			continue;
		WARN_ONCE(mfn != INVALID_P2M_ENTRY, "Trying to remove %lx which has %lx mfn!\n",
			  pfn, mfn);

		__set_phys_to_machine(pfn, INVALID_P2M_ENTRY);
	}
}

static unsigned long __init xen_do_chunk(unsigned long start,
					 unsigned long end, bool release)
{
	struct xen_memory_reservation reservation = {
		.address_bits = 0,
		.extent_order = 0,
		.domid        = DOMID_SELF
	};
	unsigned long len = 0;
	unsigned long pfn;
	int ret;

	for (pfn = start; pfn < end; pfn++) {
		unsigned long frame;
		unsigned long mfn = pfn_to_mfn(pfn);

		if (release) {
			/* Make sure pfn exists to start with */
			if (mfn == INVALID_P2M_ENTRY || mfn_to_pfn(mfn) != pfn)
				continue;
			frame = mfn;
		} else {
			if (mfn != INVALID_P2M_ENTRY)
				continue;
			frame = pfn;
		}
		set_xen_guest_handle(reservation.extent_start, &frame);
		reservation.nr_extents = 1;

		ret = HYPERVISOR_memory_op(release ? XENMEM_decrease_reservation : XENMEM_populate_physmap,
					   &reservation);
		WARN(ret != 1, "Failed to %s pfn %lx err=%d\n",
		     release ? "release" : "populate", pfn, ret);

		if (ret == 1) {
			if (!early_set_phys_to_machine(pfn, release ? INVALID_P2M_ENTRY : frame)) {
				if (release)
					break;
				set_xen_guest_handle(reservation.extent_start, &frame);
				reservation.nr_extents = 1;
				ret = HYPERVISOR_memory_op(XENMEM_decrease_reservation,
							   &reservation);
				break;
			}
			len++;
		} else
			break;
	}
	if (len)
		printk(KERN_INFO "%s %lx-%lx pfn range: %lu pages %s\n",
		       release ? "Freeing" : "Populating",
		       start, end, len,
		       release ? "freed" : "added");

	return len;
=======
	memblock_reserve(PFN_PHYS(start_pfn), PFN_PHYS(n_pfns));
}

static void __init xen_del_extra_mem(unsigned long start_pfn,
				     unsigned long n_pfns)
{
	int i;
	unsigned long start_r, size_r;

	for (i = 0; i < XEN_EXTRA_MEM_MAX_REGIONS; i++) {
		start_r = xen_extra_mem[i].start_pfn;
		size_r = xen_extra_mem[i].n_pfns;

		/* Start of region. */
		if (start_r == start_pfn) {
			BUG_ON(n_pfns > size_r);
			xen_extra_mem[i].start_pfn += n_pfns;
			xen_extra_mem[i].n_pfns -= n_pfns;
			break;
		}
		/* End of region. */
		if (start_r + size_r == start_pfn + n_pfns) {
			BUG_ON(n_pfns > size_r);
			xen_extra_mem[i].n_pfns -= n_pfns;
			break;
		}
		/* Mid of region. */
		if (start_pfn > start_r && start_pfn < start_r + size_r) {
			BUG_ON(start_pfn + n_pfns > start_r + size_r);
			xen_extra_mem[i].n_pfns = start_pfn - start_r;
			/* Calling memblock_reserve() again is okay. */
			xen_add_extra_mem(start_pfn + n_pfns, start_r + size_r -
					  (start_pfn + n_pfns));
			break;
		}
	}
	memblock_free(PFN_PHYS(start_pfn), PFN_PHYS(n_pfns));
}

/*
 * Called during boot before the p2m list can take entries beyond the
 * hypervisor supplied p2m list. Entries in extra mem are to be regarded as
 * invalid.
 */
unsigned long __ref xen_chk_extra_mem(unsigned long pfn)
{
	int i;

	for (i = 0; i < XEN_EXTRA_MEM_MAX_REGIONS; i++) {
		if (pfn >= xen_extra_mem[i].start_pfn &&
		    pfn < xen_extra_mem[i].start_pfn + xen_extra_mem[i].n_pfns)
			return INVALID_P2M_ENTRY;
	}

	return IDENTITY_FRAME(pfn);
}

/*
 * Mark all pfns of extra mem as invalid in p2m list.
 */
void __init xen_inv_extra_mem(void)
{
	unsigned long pfn, pfn_s, pfn_e;
	int i;

	for (i = 0; i < XEN_EXTRA_MEM_MAX_REGIONS; i++) {
		if (!xen_extra_mem[i].n_pfns)
			continue;
		pfn_s = xen_extra_mem[i].start_pfn;
		pfn_e = pfn_s + xen_extra_mem[i].n_pfns;
		for (pfn = pfn_s; pfn < pfn_e; pfn++)
			set_phys_to_machine(pfn, INVALID_P2M_ENTRY);
	}
>>>>>>> v4.9.227
}

/*
 * Finds the next RAM pfn available in the E820 map after min_pfn.
 * This function updates min_pfn with the pfn found and returns
 * the size of that range or zero if not found.
 */
<<<<<<< HEAD
static unsigned long __init xen_find_pfn_range(
	const struct e820entry *list, size_t map_size,
	unsigned long *min_pfn)
{
	const struct e820entry *entry;
	unsigned int i;
	unsigned long done = 0;

	for (i = 0, entry = list; i < map_size; i++, entry++) {
=======
static unsigned long __init xen_find_pfn_range(unsigned long *min_pfn)
{
	const struct e820entry *entry = xen_e820_map;
	unsigned int i;
	unsigned long done = 0;

	for (i = 0; i < xen_e820_map_entries; i++, entry++) {
>>>>>>> v4.9.227
		unsigned long s_pfn;
		unsigned long e_pfn;

		if (entry->type != E820_RAM)
			continue;

		e_pfn = PFN_DOWN(entry->addr + entry->size);

		/* We only care about E820 after this */
<<<<<<< HEAD
		if (e_pfn < *min_pfn)
=======
		if (e_pfn <= *min_pfn)
>>>>>>> v4.9.227
			continue;

		s_pfn = PFN_UP(entry->addr);

		/* If min_pfn falls within the E820 entry, we want to start
		 * at the min_pfn PFN.
		 */
		if (s_pfn <= *min_pfn) {
			done = e_pfn - *min_pfn;
		} else {
			done = e_pfn - s_pfn;
			*min_pfn = s_pfn;
		}
		break;
	}

	return done;
}

<<<<<<< HEAD
/*
 * This releases a chunk of memory and then does the identity map. It's used as
 * as a fallback if the remapping fails.
 */
static void __init xen_set_identity_and_release_chunk(unsigned long start_pfn,
	unsigned long end_pfn, unsigned long nr_pages, unsigned long *identity,
	unsigned long *released)
{
	WARN_ON(start_pfn > end_pfn);

	/* Need to release pages first */
	*released += xen_do_chunk(start_pfn, min(end_pfn, nr_pages), true);
	*identity += set_phys_range_identity(start_pfn, end_pfn);
}

/*
 * Helper function to update both the p2m and m2p tables.
 */
static unsigned long __init xen_update_mem_tables(unsigned long pfn,
						  unsigned long mfn)
{
	struct mmu_update update = {
		.ptr = ((unsigned long long)mfn << PAGE_SHIFT) | MMU_MACHPHYS_UPDATE,
=======
static int __init xen_free_mfn(unsigned long mfn)
{
	struct xen_memory_reservation reservation = {
		.address_bits = 0,
		.extent_order = 0,
		.domid        = DOMID_SELF
	};

	set_xen_guest_handle(reservation.extent_start, &mfn);
	reservation.nr_extents = 1;

	return HYPERVISOR_memory_op(XENMEM_decrease_reservation, &reservation);
}

/*
 * This releases a chunk of memory and then does the identity map. It's used
 * as a fallback if the remapping fails.
 */
static void __init xen_set_identity_and_release_chunk(unsigned long start_pfn,
			unsigned long end_pfn, unsigned long nr_pages)
{
	unsigned long pfn, end;
	int ret;

	WARN_ON(start_pfn > end_pfn);

	/* Release pages first. */
	end = min(end_pfn, nr_pages);
	for (pfn = start_pfn; pfn < end; pfn++) {
		unsigned long mfn = pfn_to_mfn(pfn);

		/* Make sure pfn exists to start with */
		if (mfn == INVALID_P2M_ENTRY || mfn_to_pfn(mfn) != pfn)
			continue;

		ret = xen_free_mfn(mfn);
		WARN(ret != 1, "Failed to release pfn %lx err=%d\n", pfn, ret);

		if (ret == 1) {
			xen_released_pages++;
			if (!__set_phys_to_machine(pfn, INVALID_P2M_ENTRY))
				break;
		} else
			break;
	}

	set_phys_range_identity(start_pfn, end_pfn);
}

/*
 * Helper function to update the p2m and m2p tables and kernel mapping.
 */
static void __init xen_update_mem_tables(unsigned long pfn, unsigned long mfn)
{
	struct mmu_update update = {
		.ptr = ((uint64_t)mfn << PAGE_SHIFT) | MMU_MACHPHYS_UPDATE,
>>>>>>> v4.9.227
		.val = pfn
	};

	/* Update p2m */
<<<<<<< HEAD
	if (!early_set_phys_to_machine(pfn, mfn)) {
		WARN(1, "Failed to set p2m mapping for pfn=%ld mfn=%ld\n",
		     pfn, mfn);
		return false;
=======
	if (!set_phys_to_machine(pfn, mfn)) {
		WARN(1, "Failed to set p2m mapping for pfn=%ld mfn=%ld\n",
		     pfn, mfn);
		BUG();
>>>>>>> v4.9.227
	}

	/* Update m2p */
	if (HYPERVISOR_mmu_update(&update, 1, NULL, DOMID_SELF) < 0) {
		WARN(1, "Failed to set m2p mapping for mfn=%ld pfn=%ld\n",
		     mfn, pfn);
<<<<<<< HEAD
		return false;
	}

	return true;
=======
		BUG();
	}

	/* Update kernel mapping, but not for highmem. */
	if (pfn >= PFN_UP(__pa(high_memory - 1)))
		return;

	if (HYPERVISOR_update_va_mapping((unsigned long)__va(pfn << PAGE_SHIFT),
					 mfn_pte(mfn, PAGE_KERNEL), 0)) {
		WARN(1, "Failed to update kernel mapping for mfn=%ld pfn=%ld\n",
		      mfn, pfn);
		BUG();
	}
>>>>>>> v4.9.227
}

/*
 * This function updates the p2m and m2p tables with an identity map from
<<<<<<< HEAD
 * start_pfn to start_pfn+size and remaps the underlying RAM of the original
 * allocation at remap_pfn. It must do so carefully in P2M_PER_PAGE sized blocks
 * to not exhaust the reserved brk space. Doing it in properly aligned blocks
 * ensures we only allocate the minimum required leaf pages in the p2m table. It
 * copies the existing mfns from the p2m table under the 1:1 map, overwrites
 * them with the identity map and then updates the p2m and m2p tables with the
 * remapped memory.
 */
static unsigned long __init xen_do_set_identity_and_remap_chunk(
        unsigned long start_pfn, unsigned long size, unsigned long remap_pfn)
{
	unsigned long ident_pfn_iter, remap_pfn_iter;
	unsigned long ident_start_pfn_align, remap_start_pfn_align;
	unsigned long ident_end_pfn_align, remap_end_pfn_align;
	unsigned long ident_boundary_pfn, remap_boundary_pfn;
	unsigned long ident_cnt = 0;
	unsigned long remap_cnt = 0;
	unsigned long left = size;
	unsigned long mod;
	int i;
=======
 * start_pfn to start_pfn+size and prepares remapping the underlying RAM of the
 * original allocation at remap_pfn. The information needed for remapping is
 * saved in the memory itself to avoid the need for allocating buffers. The
 * complete remap information is contained in a list of MFNs each containing
 * up to REMAP_SIZE MFNs and the start target PFN for doing the remap.
 * This enables us to preserve the original mfn sequence while doing the
 * remapping at a time when the memory management is capable of allocating
 * virtual and physical memory in arbitrary amounts, see 'xen_remap_memory' and
 * its callers.
 */
static void __init xen_do_set_identity_and_remap_chunk(
        unsigned long start_pfn, unsigned long size, unsigned long remap_pfn)
{
	unsigned long buf = (unsigned long)&xen_remap_buf;
	unsigned long mfn_save, mfn;
	unsigned long ident_pfn_iter, remap_pfn_iter;
	unsigned long ident_end_pfn = start_pfn + size;
	unsigned long left = size;
	unsigned int i, chunk;
>>>>>>> v4.9.227

	WARN_ON(size == 0);

	BUG_ON(xen_feature(XENFEAT_auto_translated_physmap));

<<<<<<< HEAD
	/*
	 * Determine the proper alignment to remap memory in P2M_PER_PAGE sized
	 * blocks. We need to keep track of both the existing pfn mapping and
	 * the new pfn remapping.
	 */
	mod = start_pfn % P2M_PER_PAGE;
	ident_start_pfn_align =
		mod ? (start_pfn - mod + P2M_PER_PAGE) : start_pfn;
	mod = remap_pfn % P2M_PER_PAGE;
	remap_start_pfn_align =
		mod ? (remap_pfn - mod + P2M_PER_PAGE) : remap_pfn;
	mod = (start_pfn + size) % P2M_PER_PAGE;
	ident_end_pfn_align = start_pfn + size - mod;
	mod = (remap_pfn + size) % P2M_PER_PAGE;
	remap_end_pfn_align = remap_pfn + size - mod;

	/* Iterate over each p2m leaf node in each range */
	for (ident_pfn_iter = ident_start_pfn_align, remap_pfn_iter = remap_start_pfn_align;
	     ident_pfn_iter < ident_end_pfn_align && remap_pfn_iter < remap_end_pfn_align;
	     ident_pfn_iter += P2M_PER_PAGE, remap_pfn_iter += P2M_PER_PAGE) {
		/* Check we aren't past the end */
		BUG_ON(ident_pfn_iter + P2M_PER_PAGE > start_pfn + size);
		BUG_ON(remap_pfn_iter + P2M_PER_PAGE > remap_pfn + size);

		/* Save p2m mappings */
		for (i = 0; i < P2M_PER_PAGE; i++)
			xen_remap_buf[i] = pfn_to_mfn(ident_pfn_iter + i);

		/* Set identity map which will free a p2m leaf */
		ident_cnt += set_phys_range_identity(ident_pfn_iter,
			ident_pfn_iter + P2M_PER_PAGE);

#ifdef DEBUG
		/* Helps verify a p2m leaf has been freed */
		for (i = 0; i < P2M_PER_PAGE; i++) {
			unsigned int pfn = ident_pfn_iter + i;
			BUG_ON(pfn_to_mfn(pfn) != pfn);
		}
#endif
		/* Now remap memory */
		for (i = 0; i < P2M_PER_PAGE; i++) {
			unsigned long mfn = xen_remap_buf[i];

			/* This will use the p2m leaf freed above */
			if (!xen_update_mem_tables(remap_pfn_iter + i, mfn)) {
				WARN(1, "Failed to update mem mapping for pfn=%ld mfn=%ld\n",
					remap_pfn_iter + i, mfn);
				return 0;
			}

			remap_cnt++;
		}

		left -= P2M_PER_PAGE;
	}

	/* Max boundary space possible */
	BUG_ON(left > (P2M_PER_PAGE - 1) * 2);

	/* Now handle the boundary conditions */
	ident_boundary_pfn = start_pfn;
	remap_boundary_pfn = remap_pfn;
	for (i = 0; i < left; i++) {
		unsigned long mfn;

		/* These two checks move from the start to end boundaries */
		if (ident_boundary_pfn == ident_start_pfn_align)
			ident_boundary_pfn = ident_pfn_iter;
		if (remap_boundary_pfn == remap_start_pfn_align)
			remap_boundary_pfn = remap_pfn_iter;

		/* Check we aren't past the end */
		BUG_ON(ident_boundary_pfn >= start_pfn + size);
		BUG_ON(remap_boundary_pfn >= remap_pfn + size);

		mfn = pfn_to_mfn(ident_boundary_pfn);

		if (!xen_update_mem_tables(remap_boundary_pfn, mfn)) {
			WARN(1, "Failed to update mem mapping for pfn=%ld mfn=%ld\n",
				remap_pfn_iter + i, mfn);
			return 0;
		}
		remap_cnt++;

		ident_boundary_pfn++;
		remap_boundary_pfn++;
	}

	/* Finish up the identity map */
	if (ident_start_pfn_align >= ident_end_pfn_align) {
		/*
                 * In this case we have an identity range which does not span an
                 * aligned block so everything needs to be identity mapped here.
                 * If we didn't check this we might remap too many pages since
                 * the align boundaries are not meaningful in this case.
	         */
		ident_cnt += set_phys_range_identity(start_pfn,
			start_pfn + size);
	} else {
		/* Remapped above so check each end of the chunk */
		if (start_pfn < ident_start_pfn_align)
			ident_cnt += set_phys_range_identity(start_pfn,
				ident_start_pfn_align);
		if (start_pfn + size > ident_pfn_iter)
			ident_cnt += set_phys_range_identity(ident_pfn_iter,
				start_pfn + size);
	}

	BUG_ON(ident_cnt != size);
	BUG_ON(remap_cnt != size);

	return size;
=======
	mfn_save = virt_to_mfn(buf);

	for (ident_pfn_iter = start_pfn, remap_pfn_iter = remap_pfn;
	     ident_pfn_iter < ident_end_pfn;
	     ident_pfn_iter += REMAP_SIZE, remap_pfn_iter += REMAP_SIZE) {
		chunk = (left < REMAP_SIZE) ? left : REMAP_SIZE;

		/* Map first pfn to xen_remap_buf */
		mfn = pfn_to_mfn(ident_pfn_iter);
		set_pte_mfn(buf, mfn, PAGE_KERNEL);

		/* Save mapping information in page */
		xen_remap_buf.next_area_mfn = xen_remap_mfn;
		xen_remap_buf.target_pfn = remap_pfn_iter;
		xen_remap_buf.size = chunk;
		for (i = 0; i < chunk; i++)
			xen_remap_buf.mfns[i] = pfn_to_mfn(ident_pfn_iter + i);

		/* Put remap buf into list. */
		xen_remap_mfn = mfn;

		/* Set identity map */
		set_phys_range_identity(ident_pfn_iter, ident_pfn_iter + chunk);

		left -= chunk;
	}

	/* Restore old xen_remap_buf mapping */
	set_pte_mfn(buf, mfn_save, PAGE_KERNEL);
>>>>>>> v4.9.227
}

/*
 * This function takes a contiguous pfn range that needs to be identity mapped
 * and:
 *
 *  1) Finds a new range of pfns to use to remap based on E820 and remap_pfn.
 *  2) Calls the do_ function to actually do the mapping/remapping work.
 *
 * The goal is to not allocate additional memory but to remap the existing
 * pages. In the case of an error the underlying memory is simply released back
 * to Xen and not remapped.
 */
static unsigned long __init xen_set_identity_and_remap_chunk(
<<<<<<< HEAD
        const struct e820entry *list, size_t map_size, unsigned long start_pfn,
	unsigned long end_pfn, unsigned long nr_pages, unsigned long remap_pfn,
	unsigned long *identity, unsigned long *remapped,
	unsigned long *released)
=======
	unsigned long start_pfn, unsigned long end_pfn, unsigned long nr_pages,
	unsigned long remap_pfn)
>>>>>>> v4.9.227
{
	unsigned long pfn;
	unsigned long i = 0;
	unsigned long n = end_pfn - start_pfn;

<<<<<<< HEAD
=======
	if (remap_pfn == 0)
		remap_pfn = nr_pages;

>>>>>>> v4.9.227
	while (i < n) {
		unsigned long cur_pfn = start_pfn + i;
		unsigned long left = n - i;
		unsigned long size = left;
		unsigned long remap_range_size;

		/* Do not remap pages beyond the current allocation */
		if (cur_pfn >= nr_pages) {
			/* Identity map remaining pages */
<<<<<<< HEAD
			*identity += set_phys_range_identity(cur_pfn,
				cur_pfn + size);
=======
			set_phys_range_identity(cur_pfn, cur_pfn + size);
>>>>>>> v4.9.227
			break;
		}
		if (cur_pfn + size > nr_pages)
			size = nr_pages - cur_pfn;

<<<<<<< HEAD
		remap_range_size = xen_find_pfn_range(list, map_size,
						      &remap_pfn);
		if (!remap_range_size) {
			pr_warning("Unable to find available pfn range, not remapping identity pages\n");
			xen_set_identity_and_release_chunk(cur_pfn,
				cur_pfn + left, nr_pages, identity, released);
=======
		remap_range_size = xen_find_pfn_range(&remap_pfn);
		if (!remap_range_size) {
			pr_warning("Unable to find available pfn range, not remapping identity pages\n");
			xen_set_identity_and_release_chunk(cur_pfn,
						cur_pfn + left, nr_pages);
>>>>>>> v4.9.227
			break;
		}
		/* Adjust size to fit in current e820 RAM region */
		if (size > remap_range_size)
			size = remap_range_size;

<<<<<<< HEAD
		if (!xen_do_set_identity_and_remap_chunk(cur_pfn, size, remap_pfn)) {
			WARN(1, "Failed to remap 1:1 memory cur_pfn=%ld size=%ld remap_pfn=%ld\n",
				cur_pfn, size, remap_pfn);
			xen_set_identity_and_release_chunk(cur_pfn,
				cur_pfn + left, nr_pages, identity, released);
			break;
		}
=======
		xen_do_set_identity_and_remap_chunk(cur_pfn, size, remap_pfn);
>>>>>>> v4.9.227

		/* Update variables to reflect new mappings. */
		i += size;
		remap_pfn += size;
<<<<<<< HEAD
		*identity += size;
		*remapped += size;
=======
>>>>>>> v4.9.227
	}

	/*
	 * If the PFNs are currently mapped, the VA mapping also needs
	 * to be updated to be 1:1.
	 */
	for (pfn = start_pfn; pfn <= max_pfn_mapped && pfn < end_pfn; pfn++)
		(void)HYPERVISOR_update_va_mapping(
			(unsigned long)__va(pfn << PAGE_SHIFT),
			mfn_pte(pfn, PAGE_KERNEL_IO), 0);

	return remap_pfn;
}

<<<<<<< HEAD
static unsigned long __init xen_set_identity_and_remap(
	const struct e820entry *list, size_t map_size, unsigned long nr_pages,
	unsigned long *released)
{
	phys_addr_t start = 0;
	unsigned long identity = 0;
	unsigned long remapped = 0;
	unsigned long last_pfn = nr_pages;
	const struct e820entry *entry;
	unsigned long num_released = 0;
=======
static unsigned long __init xen_count_remap_pages(
	unsigned long start_pfn, unsigned long end_pfn, unsigned long nr_pages,
	unsigned long remap_pages)
{
	if (start_pfn >= nr_pages)
		return remap_pages;

	return remap_pages + min(end_pfn, nr_pages) - start_pfn;
}

static unsigned long __init xen_foreach_remap_area(unsigned long nr_pages,
	unsigned long (*func)(unsigned long start_pfn, unsigned long end_pfn,
			      unsigned long nr_pages, unsigned long last_val))
{
	phys_addr_t start = 0;
	unsigned long ret_val = 0;
	const struct e820entry *entry = xen_e820_map;
>>>>>>> v4.9.227
	int i;

	/*
	 * Combine non-RAM regions and gaps until a RAM region (or the
<<<<<<< HEAD
	 * end of the map) is reached, then set the 1:1 map and
	 * remap the memory in those non-RAM regions.
=======
	 * end of the map) is reached, then call the provided function
	 * to perform its duty on the non-RAM region.
>>>>>>> v4.9.227
	 *
	 * The combined non-RAM regions are rounded to a whole number
	 * of pages so any partial pages are accessible via the 1:1
	 * mapping.  This is needed for some BIOSes that put (for
	 * example) the DMI tables in a reserved region that begins on
	 * a non-page boundary.
	 */
<<<<<<< HEAD
	for (i = 0, entry = list; i < map_size; i++, entry++) {
		phys_addr_t end = entry->addr + entry->size;
		if (entry->type == E820_RAM || i == map_size - 1) {
=======
	for (i = 0; i < xen_e820_map_entries; i++, entry++) {
		phys_addr_t end = entry->addr + entry->size;
		if (entry->type == E820_RAM || i == xen_e820_map_entries - 1) {
>>>>>>> v4.9.227
			unsigned long start_pfn = PFN_DOWN(start);
			unsigned long end_pfn = PFN_UP(end);

			if (entry->type == E820_RAM)
				end_pfn = PFN_UP(entry->addr);

			if (start_pfn < end_pfn)
<<<<<<< HEAD
				last_pfn = xen_set_identity_and_remap_chunk(
						list, map_size, start_pfn,
						end_pfn, nr_pages, last_pfn,
						&identity, &remapped,
						&num_released);
=======
				ret_val = func(start_pfn, end_pfn, nr_pages,
					       ret_val);
>>>>>>> v4.9.227
			start = end;
		}
	}

<<<<<<< HEAD
	*released = num_released;

	pr_info("Set %ld page(s) to 1-1 mapping\n", identity);
	pr_info("Remapped %ld page(s), last_pfn=%ld\n", remapped,
		last_pfn);
	pr_info("Released %ld page(s)\n", num_released);

	return last_pfn;
}
static unsigned long __init xen_get_max_pages(void)
{
	unsigned long max_pages = MAX_DOMAIN_PAGES;
	domid_t domid = DOMID_SELF;
	int ret;
=======
	return ret_val;
}

/*
 * Remap the memory prepared in xen_do_set_identity_and_remap_chunk().
 * The remap information (which mfn remap to which pfn) is contained in the
 * to be remapped memory itself in a linked list anchored at xen_remap_mfn.
 * This scheme allows to remap the different chunks in arbitrary order while
 * the resulting mapping will be independant from the order.
 */
void __init xen_remap_memory(void)
{
	unsigned long buf = (unsigned long)&xen_remap_buf;
	unsigned long mfn_save, mfn, pfn;
	unsigned long remapped = 0;
	unsigned int i;
	unsigned long pfn_s = ~0UL;
	unsigned long len = 0;

	mfn_save = virt_to_mfn(buf);

	while (xen_remap_mfn != INVALID_P2M_ENTRY) {
		/* Map the remap information */
		set_pte_mfn(buf, xen_remap_mfn, PAGE_KERNEL);

		BUG_ON(xen_remap_mfn != xen_remap_buf.mfns[0]);

		pfn = xen_remap_buf.target_pfn;
		for (i = 0; i < xen_remap_buf.size; i++) {
			mfn = xen_remap_buf.mfns[i];
			xen_update_mem_tables(pfn, mfn);
			remapped++;
			pfn++;
		}
		if (pfn_s == ~0UL || pfn == pfn_s) {
			pfn_s = xen_remap_buf.target_pfn;
			len += xen_remap_buf.size;
		} else if (pfn_s + len == xen_remap_buf.target_pfn) {
			len += xen_remap_buf.size;
		} else {
			xen_del_extra_mem(pfn_s, len);
			pfn_s = xen_remap_buf.target_pfn;
			len = xen_remap_buf.size;
		}

		mfn = xen_remap_mfn;
		xen_remap_mfn = xen_remap_buf.next_area_mfn;
	}

	if (pfn_s != ~0UL && len)
		xen_del_extra_mem(pfn_s, len);

	set_pte_mfn(buf, mfn_save, PAGE_KERNEL);

	pr_info("Remapped %ld page(s)\n", remapped);
}

static unsigned long __init xen_get_pages_limit(void)
{
	unsigned long limit;

#ifdef CONFIG_X86_32
	limit = GB(64) / PAGE_SIZE;
#else
	limit = MAXMEM / PAGE_SIZE;
	if (!xen_initial_domain() && xen_512gb_limit)
		limit = GB(512) / PAGE_SIZE;
#endif
	return limit;
}

static unsigned long __init xen_get_max_pages(void)
{
	unsigned long max_pages, limit;
	domid_t domid = DOMID_SELF;
	long ret;

	limit = xen_get_pages_limit();
	max_pages = limit;
>>>>>>> v4.9.227

	/*
	 * For the initial domain we use the maximum reservation as
	 * the maximum page.
	 *
	 * For guest domains the current maximum reservation reflects
	 * the current maximum rather than the static maximum. In this
	 * case the e820 map provided to us will cover the static
	 * maximum region.
	 */
	if (xen_initial_domain()) {
		ret = HYPERVISOR_memory_op(XENMEM_maximum_reservation, &domid);
		if (ret > 0)
			max_pages = ret;
	}

<<<<<<< HEAD
	return min(max_pages, MAX_DOMAIN_PAGES);
}

static void xen_align_and_add_e820_region(u64 start, u64 size, int type)
{
	u64 end = start + size;
=======
	return min(max_pages, limit);
}

static void __init xen_align_and_add_e820_region(phys_addr_t start,
						 phys_addr_t size, int type)
{
	phys_addr_t end = start + size;
>>>>>>> v4.9.227

	/* Align RAM regions to page boundaries. */
	if (type == E820_RAM) {
		start = PAGE_ALIGN(start);
<<<<<<< HEAD
		end &= ~((u64)PAGE_SIZE - 1);
=======
		end &= ~((phys_addr_t)PAGE_SIZE - 1);
>>>>>>> v4.9.227
	}

	e820_add_region(start, end - start, type);
}

<<<<<<< HEAD
void xen_ignore_unusable(struct e820entry *list, size_t map_size)
{
	struct e820entry *entry;
	unsigned int i;

	for (i = 0, entry = list; i < map_size; i++, entry++) {
=======
static void __init xen_ignore_unusable(void)
{
	struct e820entry *entry = xen_e820_map;
	unsigned int i;

	for (i = 0; i < xen_e820_map_entries; i++, entry++) {
>>>>>>> v4.9.227
		if (entry->type == E820_UNUSABLE)
			entry->type = E820_RAM;
	}
}

<<<<<<< HEAD
=======
bool __init xen_is_e820_reserved(phys_addr_t start, phys_addr_t size)
{
	struct e820entry *entry;
	unsigned mapcnt;
	phys_addr_t end;

	if (!size)
		return false;

	end = start + size;
	entry = xen_e820_map;

	for (mapcnt = 0; mapcnt < xen_e820_map_entries; mapcnt++) {
		if (entry->type == E820_RAM && entry->addr <= start &&
		    (entry->addr + entry->size) >= end)
			return false;

		entry++;
	}

	return true;
}

/*
 * Find a free area in physical memory not yet reserved and compliant with
 * E820 map.
 * Used to relocate pre-allocated areas like initrd or p2m list which are in
 * conflict with the to be used E820 map.
 * In case no area is found, return 0. Otherwise return the physical address
 * of the area which is already reserved for convenience.
 */
phys_addr_t __init xen_find_free_area(phys_addr_t size)
{
	unsigned mapcnt;
	phys_addr_t addr, start;
	struct e820entry *entry = xen_e820_map;

	for (mapcnt = 0; mapcnt < xen_e820_map_entries; mapcnt++, entry++) {
		if (entry->type != E820_RAM || entry->size < size)
			continue;
		start = entry->addr;
		for (addr = start; addr < start + size; addr += PAGE_SIZE) {
			if (!memblock_is_reserved(addr))
				continue;
			start = addr + PAGE_SIZE;
			if (start + size > entry->addr + entry->size)
				break;
		}
		if (addr >= start + size) {
			memblock_reserve(start, size);
			return start;
		}
	}

	return 0;
}

/*
 * Like memcpy, but with physical addresses for dest and src.
 */
static void __init xen_phys_memcpy(phys_addr_t dest, phys_addr_t src,
				   phys_addr_t n)
{
	phys_addr_t dest_off, src_off, dest_len, src_len, len;
	void *from, *to;

	while (n) {
		dest_off = dest & ~PAGE_MASK;
		src_off = src & ~PAGE_MASK;
		dest_len = n;
		if (dest_len > (NR_FIX_BTMAPS << PAGE_SHIFT) - dest_off)
			dest_len = (NR_FIX_BTMAPS << PAGE_SHIFT) - dest_off;
		src_len = n;
		if (src_len > (NR_FIX_BTMAPS << PAGE_SHIFT) - src_off)
			src_len = (NR_FIX_BTMAPS << PAGE_SHIFT) - src_off;
		len = min(dest_len, src_len);
		to = early_memremap(dest - dest_off, dest_len + dest_off);
		from = early_memremap(src - src_off, src_len + src_off);
		memcpy(to, from, len);
		early_memunmap(to, dest_len + dest_off);
		early_memunmap(from, src_len + src_off);
		n -= len;
		dest += len;
		src += len;
	}
}

/*
 * Reserve Xen mfn_list.
 */
static void __init xen_reserve_xen_mfnlist(void)
{
	phys_addr_t start, size;

	if (xen_start_info->mfn_list >= __START_KERNEL_map) {
		start = __pa(xen_start_info->mfn_list);
		size = PFN_ALIGN(xen_start_info->nr_pages *
				 sizeof(unsigned long));
	} else {
		start = PFN_PHYS(xen_start_info->first_p2m_pfn);
		size = PFN_PHYS(xen_start_info->nr_p2m_frames);
	}

	memblock_reserve(start, size);
	if (!xen_is_e820_reserved(start, size))
		return;

#ifdef CONFIG_X86_32
	/*
	 * Relocating the p2m on 32 bit system to an arbitrary virtual address
	 * is not supported, so just give up.
	 */
	xen_raw_console_write("Xen hypervisor allocated p2m list conflicts with E820 map\n");
	BUG();
#else
	xen_relocate_p2m();
	memblock_free(start, size);
#endif
}

>>>>>>> v4.9.227
/**
 * machine_specific_memory_setup - Hook for machine specific memory setup.
 **/
char * __init xen_memory_setup(void)
{
<<<<<<< HEAD
	static struct e820entry map[E820MAX] __initdata;

	unsigned long max_pfn = xen_start_info->nr_pages;
	unsigned long long mem_end;
	int rc;
	struct xen_memory_map memmap;
	unsigned long max_pages;
	unsigned long last_pfn = 0;
=======
	unsigned long max_pfn, pfn_s, n_pfns;
	phys_addr_t mem_end, addr, size, chunk_size;
	u32 type;
	int rc;
	struct xen_memory_map memmap;
	unsigned long max_pages;
>>>>>>> v4.9.227
	unsigned long extra_pages = 0;
	int i;
	int op;

<<<<<<< HEAD
	max_pfn = min(MAX_DOMAIN_PAGES, max_pfn);
	mem_end = PFN_PHYS(max_pfn);

	memmap.nr_entries = E820MAX;
	set_xen_guest_handle(memmap.buffer, map);
=======
	xen_parse_512gb();
	max_pfn = xen_get_pages_limit();
	max_pfn = min(max_pfn, xen_start_info->nr_pages);
	mem_end = PFN_PHYS(max_pfn);

	memmap.nr_entries = E820MAX;
	set_xen_guest_handle(memmap.buffer, xen_e820_map);
>>>>>>> v4.9.227

	op = xen_initial_domain() ?
		XENMEM_machine_memory_map :
		XENMEM_memory_map;
	rc = HYPERVISOR_memory_op(op, &memmap);
	if (rc == -ENOSYS) {
		BUG_ON(xen_initial_domain());
		memmap.nr_entries = 1;
<<<<<<< HEAD
		map[0].addr = 0ULL;
		map[0].size = mem_end;
		/* 8MB slack (to balance backend allocations). */
		map[0].size += 8ULL << 20;
		map[0].type = E820_RAM;
=======
		xen_e820_map[0].addr = 0ULL;
		xen_e820_map[0].size = mem_end;
		/* 8MB slack (to balance backend allocations). */
		xen_e820_map[0].size += 8ULL << 20;
		xen_e820_map[0].type = E820_RAM;
>>>>>>> v4.9.227
		rc = 0;
	}
	BUG_ON(rc);
	BUG_ON(memmap.nr_entries == 0);
<<<<<<< HEAD
=======
	xen_e820_map_entries = memmap.nr_entries;
>>>>>>> v4.9.227

	/*
	 * Xen won't allow a 1:1 mapping to be created to UNUSABLE
	 * regions, so if we're using the machine memory map leave the
	 * region as RAM as it is in the pseudo-physical map.
	 *
	 * UNUSABLE regions in domUs are not handled and will need
	 * a patch in the future.
	 */
	if (xen_initial_domain())
<<<<<<< HEAD
		xen_ignore_unusable(map, memmap.nr_entries);

	/* Make sure the Xen-supplied memory map is well-ordered. */
	sanitize_e820_map(map, memmap.nr_entries, &memmap.nr_entries);

	max_pages = xen_get_max_pages();
=======
		xen_ignore_unusable();

	/* Make sure the Xen-supplied memory map is well-ordered. */
	sanitize_e820_map(xen_e820_map, ARRAY_SIZE(xen_e820_map),
			  &xen_e820_map_entries);

	max_pages = xen_get_max_pages();

	/* How many extra pages do we need due to remapping? */
	max_pages += xen_foreach_remap_area(max_pfn, xen_count_remap_pages);

>>>>>>> v4.9.227
	if (max_pages > max_pfn)
		extra_pages += max_pages - max_pfn;

	/*
<<<<<<< HEAD
	 * Set identity map on non-RAM pages and remap the underlying RAM.
	 */
	last_pfn = xen_set_identity_and_remap(map, memmap.nr_entries, max_pfn,
					      &xen_released_pages);

	extra_pages += xen_released_pages;

	if (last_pfn > max_pfn) {
		max_pfn = min(MAX_DOMAIN_PAGES, last_pfn);
		mem_end = PFN_PHYS(max_pfn);
	}
	/*
=======
>>>>>>> v4.9.227
	 * Clamp the amount of extra memory to a EXTRA_MEM_RATIO
	 * factor the base size.  On non-highmem systems, the base
	 * size is the full initial memory allocation; on highmem it
	 * is limited to the max size of lowmem, so that it doesn't
	 * get completely filled.
	 *
<<<<<<< HEAD
=======
	 * Make sure we have no memory above max_pages, as this area
	 * isn't handled by the p2m management.
	 *
>>>>>>> v4.9.227
	 * In principle there could be a problem in lowmem systems if
	 * the initial memory is also very large with respect to
	 * lowmem, but we won't try to deal with that here.
	 */
<<<<<<< HEAD
	extra_pages = min(EXTRA_MEM_RATIO * min(max_pfn, PFN_DOWN(MAXMEM)),
			  extra_pages);
	i = 0;
	while (i < memmap.nr_entries) {
		u64 addr = map[i].addr;
		u64 size = map[i].size;
		u32 type = map[i].type;

		if (type == E820_RAM) {
			if (addr < mem_end) {
				size = min(size, mem_end - addr);
			} else if (extra_pages) {
				size = min(size, (u64)extra_pages * PAGE_SIZE);
				extra_pages -= size / PAGE_SIZE;
				xen_add_extra_mem(addr, size);
			} else
				type = E820_UNUSABLE;
		}

		xen_align_and_add_e820_region(addr, size, type);

		map[i].addr += size;
		map[i].size -= size;
		if (map[i].size == 0)
			i++;
=======
	extra_pages = min3(EXTRA_MEM_RATIO * min(max_pfn, PFN_DOWN(MAXMEM)),
			   extra_pages, max_pages - max_pfn);
	i = 0;
	addr = xen_e820_map[0].addr;
	size = xen_e820_map[0].size;
	while (i < xen_e820_map_entries) {
		bool discard = false;

		chunk_size = size;
		type = xen_e820_map[i].type;

		if (type == E820_RAM) {
			if (addr < mem_end) {
				chunk_size = min(size, mem_end - addr);
			} else if (extra_pages) {
				chunk_size = min(size, PFN_PHYS(extra_pages));
				pfn_s = PFN_UP(addr);
				n_pfns = PFN_DOWN(addr + chunk_size) - pfn_s;
				extra_pages -= n_pfns;
				xen_add_extra_mem(pfn_s, n_pfns);
				xen_max_p2m_pfn = pfn_s + n_pfns;
			} else
				discard = true;
		}

		if (!discard)
			xen_align_and_add_e820_region(addr, chunk_size, type);

		addr += chunk_size;
		size -= chunk_size;
		if (size == 0) {
			i++;
			if (i < xen_e820_map_entries) {
				addr = xen_e820_map[i].addr;
				size = xen_e820_map[i].size;
			}
		}
>>>>>>> v4.9.227
	}

	/*
	 * Set the rest as identity mapped, in case PCI BARs are
	 * located here.
<<<<<<< HEAD
	 *
	 * PFNs above MAX_P2M_PFN are considered identity mapped as
	 * well.
	 */
	set_phys_range_identity(map[i-1].addr / PAGE_SIZE, ~0ul);
=======
	 */
	set_phys_range_identity(addr / PAGE_SIZE, ~0ul);
>>>>>>> v4.9.227

	/*
	 * In domU, the ISA region is normal, usable memory, but we
	 * reserve ISA memory anyway because too many things poke
	 * about in there.
	 */
	e820_add_region(ISA_START_ADDRESS, ISA_END_ADDRESS - ISA_START_ADDRESS,
			E820_RESERVED);

<<<<<<< HEAD
	/*
	 * Reserve Xen bits:
	 *  - mfn_list
	 *  - xen_start_info
	 * See comment above "struct start_info" in <xen/interface/xen.h>
	 * We tried to make the the memblock_reserve more selective so
	 * that it would be clear what region is reserved. Sadly we ran
	 * in the problem wherein on a 64-bit hypervisor with a 32-bit
	 * initial domain, the pt_base has the cr3 value which is not
	 * neccessarily where the pagetable starts! As Jan put it: "
	 * Actually, the adjustment turns out to be correct: The page
	 * tables for a 32-on-64 dom0 get allocated in the order "first L1",
	 * "first L2", "first L3", so the offset to the page table base is
	 * indeed 2. When reading xen/include/public/xen.h's comment
	 * very strictly, this is not a violation (since there nothing is said
	 * that the first thing in the page table space is pointed to by
	 * pt_base; I admit that this seems to be implied though, namely
	 * do I think that it is implied that the page table space is the
	 * range [pt_base, pt_base + nt_pt_frames), whereas that
	 * range here indeed is [pt_base - 2, pt_base - 2 + nt_pt_frames),
	 * which - without a priori knowledge - the kernel would have
	 * difficulty to figure out)." - so lets just fall back to the
	 * easy way and reserve the whole region.
	 */
	memblock_reserve(__pa(xen_start_info->mfn_list),
			 xen_start_info->pt_base - xen_start_info->mfn_list);

	sanitize_e820_map(e820.map, ARRAY_SIZE(e820.map), &e820.nr_map);
=======
	sanitize_e820_map(e820->map, ARRAY_SIZE(e820->map), &e820->nr_map);

	/*
	 * Check whether the kernel itself conflicts with the target E820 map.
	 * Failing now is better than running into weird problems later due
	 * to relocating (and even reusing) pages with kernel text or data.
	 */
	if (xen_is_e820_reserved(__pa_symbol(_text),
			__pa_symbol(__bss_stop) - __pa_symbol(_text))) {
		xen_raw_console_write("Xen hypervisor allocated kernel memory conflicts with E820 map\n");
		BUG();
	}

	/*
	 * Check for a conflict of the hypervisor supplied page tables with
	 * the target E820 map.
	 */
	xen_pt_check_e820();

	xen_reserve_xen_mfnlist();

	/* Check for a conflict of the initrd with the target E820 map. */
	if (xen_is_e820_reserved(boot_params.hdr.ramdisk_image,
				 boot_params.hdr.ramdisk_size)) {
		phys_addr_t new_area, start, size;

		new_area = xen_find_free_area(boot_params.hdr.ramdisk_size);
		if (!new_area) {
			xen_raw_console_write("Can't find new memory area for initrd needed due to E820 map conflict\n");
			BUG();
		}

		start = boot_params.hdr.ramdisk_image;
		size = boot_params.hdr.ramdisk_size;
		xen_phys_memcpy(new_area, start, size);
		pr_info("initrd moved from [mem %#010llx-%#010llx] to [mem %#010llx-%#010llx]\n",
			start, start + size, new_area, new_area + size);
		memblock_free(start, size);
		boot_params.hdr.ramdisk_image = new_area;
		boot_params.ext_ramdisk_image = new_area >> 32;
	}

	/*
	 * Set identity map on non-RAM pages and prepare remapping the
	 * underlying RAM.
	 */
	xen_foreach_remap_area(max_pfn, xen_set_identity_and_remap_chunk);

	pr_info("Released %ld page(s)\n", xen_released_pages);
>>>>>>> v4.9.227

	return "Xen";
}

/*
 * Machine specific memory setup for auto-translated guests.
 */
char * __init xen_auto_xlated_memory_setup(void)
{
<<<<<<< HEAD
	static struct e820entry map[E820MAX] __initdata;

=======
>>>>>>> v4.9.227
	struct xen_memory_map memmap;
	int i;
	int rc;

	memmap.nr_entries = E820MAX;
<<<<<<< HEAD
	set_xen_guest_handle(memmap.buffer, map);
=======
	set_xen_guest_handle(memmap.buffer, xen_e820_map);
>>>>>>> v4.9.227

	rc = HYPERVISOR_memory_op(XENMEM_memory_map, &memmap);
	if (rc < 0)
		panic("No memory map (%d)\n", rc);

<<<<<<< HEAD
	sanitize_e820_map(map, ARRAY_SIZE(map), &memmap.nr_entries);

	for (i = 0; i < memmap.nr_entries; i++)
		e820_add_region(map[i].addr, map[i].size, map[i].type);

	memblock_reserve(__pa(xen_start_info->mfn_list),
			 xen_start_info->pt_base - xen_start_info->mfn_list);
=======
	xen_e820_map_entries = memmap.nr_entries;

	sanitize_e820_map(xen_e820_map, ARRAY_SIZE(xen_e820_map),
			  &xen_e820_map_entries);

	for (i = 0; i < xen_e820_map_entries; i++)
		e820_add_region(xen_e820_map[i].addr, xen_e820_map[i].size,
				xen_e820_map[i].type);

	/* Remove p2m info, it is not needed. */
	xen_start_info->mfn_list = 0;
	xen_start_info->first_p2m_pfn = 0;
	xen_start_info->nr_p2m_frames = 0;
>>>>>>> v4.9.227

	return "Xen";
}

/*
 * Set the bit indicating "nosegneg" library variants should be used.
 * We only need to bother in pure 32-bit mode; compat 32-bit processes
 * can have un-truncated segments, so wrapping around is allowed.
 */
static void __init fiddle_vdso(void)
{
#ifdef CONFIG_X86_32
<<<<<<< HEAD
	/*
	 * This could be called before selected_vdso32 is initialized, so
	 * just fiddle with both possible images.  vdso_image_32_syscall
	 * can't be selected, since it only exists on 64-bit systems.
	 */
	u32 *mask;
	mask = vdso_image_32_int80.data +
		vdso_image_32_int80.sym_VDSO32_NOTE_MASK;
	*mask |= 1 << VDSO_NOTE_NONEGSEG_BIT;
	mask = vdso_image_32_sysenter.data +
		vdso_image_32_sysenter.sym_VDSO32_NOTE_MASK;
=======
	u32 *mask = vdso_image_32.data +
		vdso_image_32.sym_VDSO32_NOTE_MASK;
>>>>>>> v4.9.227
	*mask |= 1 << VDSO_NOTE_NONEGSEG_BIT;
#endif
}

static int register_callback(unsigned type, const void *func)
{
	struct callback_register callback = {
		.type = type,
		.address = XEN_CALLBACK(__KERNEL_CS, func),
		.flags = CALLBACKF_mask_events,
	};

	return HYPERVISOR_callback_op(CALLBACKOP_register, &callback);
}

void xen_enable_sysenter(void)
{
	int ret;
	unsigned sysenter_feature;

#ifdef CONFIG_X86_32
	sysenter_feature = X86_FEATURE_SEP;
#else
	sysenter_feature = X86_FEATURE_SYSENTER32;
#endif

	if (!boot_cpu_has(sysenter_feature))
		return;

	ret = register_callback(CALLBACKTYPE_sysenter, xen_sysenter_target);
	if(ret != 0)
		setup_clear_cpu_cap(sysenter_feature);
}

void xen_enable_syscall(void)
{
#ifdef CONFIG_X86_64
	int ret;

	ret = register_callback(CALLBACKTYPE_syscall, xen_syscall_target);
	if (ret != 0) {
		printk(KERN_ERR "Failed to set syscall callback: %d\n", ret);
		/* Pretty fatal; 64-bit userspace has no other
		   mechanism for syscalls. */
	}

	if (boot_cpu_has(X86_FEATURE_SYSCALL32)) {
		ret = register_callback(CALLBACKTYPE_syscall32,
					xen_syscall32_target);
		if (ret != 0)
			setup_clear_cpu_cap(X86_FEATURE_SYSCALL32);
	}
#endif /* CONFIG_X86_64 */
}

void __init xen_pvmmu_arch_setup(void)
{
	HYPERVISOR_vm_assist(VMASST_CMD_enable, VMASST_TYPE_4gb_segments);
	HYPERVISOR_vm_assist(VMASST_CMD_enable, VMASST_TYPE_writable_pagetables);

	HYPERVISOR_vm_assist(VMASST_CMD_enable,
			     VMASST_TYPE_pae_extended_cr3);

	if (register_callback(CALLBACKTYPE_event, xen_hypervisor_callback) ||
	    register_callback(CALLBACKTYPE_failsafe, xen_failsafe_callback))
		BUG();

	xen_enable_sysenter();
	xen_enable_syscall();
}

/* This function is not called for HVM domains */
void __init xen_arch_setup(void)
{
	xen_panic_handler_init();
	if (!xen_feature(XENFEAT_auto_translated_physmap))
		xen_pvmmu_arch_setup();

#ifdef CONFIG_ACPI
	if (!(xen_start_info->flags & SIF_INITDOMAIN)) {
		printk(KERN_INFO "ACPI in unprivileged domain disabled\n");
		disable_acpi();
	}
#endif

	memcpy(boot_command_line, xen_start_info->cmd_line,
	       MAX_GUEST_CMDLINE > COMMAND_LINE_SIZE ?
	       COMMAND_LINE_SIZE : MAX_GUEST_CMDLINE);

	/* Set up idle, making sure it calls safe_halt() pvop */
	disable_cpuidle();
	disable_cpufreq();
	WARN_ON(xen_set_default_idle());
	fiddle_vdso();
#ifdef CONFIG_NUMA
	numa_off = 1;
#endif
}
