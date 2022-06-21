/*
 * ld script to make ARM Linux kernel
 * taken from the i386 version by Russell King
 * Written by Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 */

#include <asm-generic/vmlinux.lds.h>
<<<<<<< HEAD
=======
#include <asm/cache.h>
>>>>>>> v4.9.227
#include <asm/kernel-pgtable.h>
#include <asm/thread_info.h>
#include <asm/memory.h>
#include <asm/page.h>
#include <asm/pgtable.h>
<<<<<<< HEAD
#include <asm/cache.h>

#include "image.h"

#if (!defined(CONFIG_DEBUG_SPINLOCK)) || defined(CONFIG_GENERIC_BUG)
#define ARM_EXIT_KEEP(x)	x
#define ARM_EXIT_DISCARD(x)
#else
#define ARM_EXIT_KEEP(x)
#define ARM_EXIT_DISCARD(x)	x
#endif
=======

#include "image.h"

/* .exit.text needed in case of alternative patching */
#define ARM_EXIT_KEEP(x)	x
#define ARM_EXIT_DISCARD(x)
>>>>>>> v4.9.227

OUTPUT_ARCH(aarch64)
ENTRY(_text)

jiffies = jiffies_64;

#define HYPERVISOR_TEXT					\
	/*						\
<<<<<<< HEAD
	 * Force the alignment to be compatible with	\
	 * the vectors requirements			\
	 */						\
	. = ALIGN(2048);				\
=======
	 * Align to 4 KB so that			\
	 * a) the HYP vector table is at its minimum	\
	 *    alignment of 2048 bytes			\
	 * b) the HYP init code will not cross a page	\
	 *    boundary if its size does not exceed	\
	 *    4 KB (see related ASSERT() below)		\
	 */						\
	. = ALIGN(SZ_4K);				\
>>>>>>> v4.9.227
	VMLINUX_SYMBOL(__hyp_idmap_text_start) = .;	\
	*(.hyp.idmap.text)				\
	VMLINUX_SYMBOL(__hyp_idmap_text_end) = .;	\
	VMLINUX_SYMBOL(__hyp_text_start) = .;		\
	*(.hyp.text)					\
	VMLINUX_SYMBOL(__hyp_text_end) = .;

<<<<<<< HEAD
=======
#define IDMAP_TEXT					\
	. = ALIGN(SZ_4K);				\
	VMLINUX_SYMBOL(__idmap_text_start) = .;		\
	*(.idmap.text)					\
	VMLINUX_SYMBOL(__idmap_text_end) = .;

#ifdef CONFIG_HIBERNATION
#define HIBERNATE_TEXT					\
	. = ALIGN(SZ_4K);				\
	VMLINUX_SYMBOL(__hibernate_exit_text_start) = .;\
	*(.hibernate_exit.text)				\
	VMLINUX_SYMBOL(__hibernate_exit_text_end) = .;
#else
#define HIBERNATE_TEXT
#endif

>>>>>>> v4.9.227
#ifdef CONFIG_UNMAP_KERNEL_AT_EL0
#define TRAMP_TEXT					\
	. = ALIGN(PAGE_SIZE);				\
	VMLINUX_SYMBOL(__entry_tramp_text_start) = .;	\
	*(.entry.tramp.text)				\
	. = ALIGN(PAGE_SIZE);				\
	VMLINUX_SYMBOL(__entry_tramp_text_end) = .;
#else
#define TRAMP_TEXT
#endif

/*
 * The size of the PE/COFF section that covers the kernel image, which
 * runs from stext to _edata, must be a round multiple of the PE/COFF
 * FileAlignment, which we set to its minimum value of 0x200. 'stext'
 * itself is 4 KB aligned, so padding out _edata to a 0x200 aligned
 * boundary should be sufficient.
 */
PECOFF_FILE_ALIGNMENT = 0x200;

#ifdef CONFIG_EFI
#define PECOFF_EDATA_PADDING	\
	.pecoff_edata_padding : { BYTE(0); . = ALIGN(PECOFF_FILE_ALIGNMENT); }
#else
#define PECOFF_EDATA_PADDING
#endif

<<<<<<< HEAD
#ifdef CONFIG_DEBUG_ALIGN_RODATA
#define ALIGN_DEBUG_RO			. = ALIGN(1<<SECTION_SHIFT);
#define ALIGN_DEBUG_RO_MIN(min)		ALIGN_DEBUG_RO
#else
#define ALIGN_DEBUG_RO
#define ALIGN_DEBUG_RO_MIN(min)		. = ALIGN(min);
=======
#if defined(CONFIG_DEBUG_ALIGN_RODATA)
/*
 *  4 KB granule:   1 level 2 entry
 * 16 KB granule: 128 level 3 entries, with contiguous bit
 * 64 KB granule:  32 level 3 entries, with contiguous bit
 */
#define SEGMENT_ALIGN			SZ_2M
#else
/*
 *  4 KB granule:  16 level 3 entries, with contiguous bit
 * 16 KB granule:   4 level 3 entries, without contiguous bit
 * 64 KB granule:   1 level 3 entry
 */
#define SEGMENT_ALIGN			SZ_64K
>>>>>>> v4.9.227
#endif

SECTIONS
{
	/*
	 * XXX: The linker does not define how output sections are
	 * assigned to input sections when there are multiple statements
	 * matching the same input section name.  There is no documented
	 * order of matching.
	 */
	/DISCARD/ : {
		ARM_EXIT_DISCARD(EXIT_TEXT)
		ARM_EXIT_DISCARD(EXIT_DATA)
		EXIT_CALL
		*(.discard)
		*(.discard.*)
<<<<<<< HEAD
	}

	. = PAGE_OFFSET + TEXT_OFFSET;
=======
		*(.interp .dynamic)
		*(.dynsym .dynstr .hash)
	}

	. = KIMAGE_VADDR + TEXT_OFFSET;
>>>>>>> v4.9.227

	.head.text : {
		_text = .;
		HEAD_TEXT
	}
<<<<<<< HEAD
	ALIGN_DEBUG_RO
=======
>>>>>>> v4.9.227
	.text : {			/* Real text segment		*/
		_stext = .;		/* Text and read-only data	*/
			__exception_text_start = .;
			*(.exception.text)
			__exception_text_end = .;
			IRQENTRY_TEXT
<<<<<<< HEAD
			TEXT_TEXT
			SCHED_TEXT
			LOCK_TEXT
			HYPERVISOR_TEXT
=======
			SOFTIRQENTRY_TEXT
			ENTRY_TEXT
			TEXT_TEXT
			SCHED_TEXT
			CPUIDLE_TEXT
			LOCK_TEXT
			KPROBES_TEXT
			HYPERVISOR_TEXT
			IDMAP_TEXT
			HIBERNATE_TEXT
>>>>>>> v4.9.227
			TRAMP_TEXT
			*(.fixup)
			*(.gnu.warning)
		. = ALIGN(16);
		*(.got)			/* Global offset table		*/
	}

<<<<<<< HEAD
	ALIGN_DEBUG_RO
	_etext = .;			/* End of text section */

	RO_DATA(PAGE_SIZE)
	EXCEPTION_TABLE(8)
	NOTES
	ALIGN_DEBUG_RO

	ALIGN_DEBUG_RO_MIN(PAGE_SIZE)
=======
	. = ALIGN(SEGMENT_ALIGN);
	_etext = .;			/* End of text section */

	RO_DATA(PAGE_SIZE)		/* everything from this point to     */
	EXCEPTION_TABLE(8)		/* __init_begin will be marked RO NX */
	NOTES

	. = ALIGN(SEGMENT_ALIGN);
>>>>>>> v4.9.227
	__init_begin = .;

	INIT_TEXT_SECTION(8)
	.exit.text : {
		ARM_EXIT_KEEP(EXIT_TEXT)
	}

<<<<<<< HEAD
	ALIGN_DEBUG_RO_MIN(16)
=======
>>>>>>> v4.9.227
	.init.data : {
		INIT_DATA
		INIT_SETUP(16)
		INIT_CALLS
		CON_INITCALL
		SECURITY_INITCALL
		INIT_RAM_FS
<<<<<<< HEAD
=======
		*(.init.rodata.* .init.bss)	/* from the EFI stub */
>>>>>>> v4.9.227
	}
	.exit.data : {
		ARM_EXIT_KEEP(EXIT_DATA)
	}

	PERCPU_SECTION(L1_CACHE_BYTES)

<<<<<<< HEAD
#ifdef CONFIG_RELOCATABLE_KERNEL
	.rela : {
		. = ALIGN(8);
		__reloc_start = .;
		*(.rela .rela*)
		__reloc_end = .;
	}
	.dynsym : {
		. = ALIGN(8);
		__dynsym_start = .;
		*(.dynsym)
	}
#endif 

	. = ALIGN(PAGE_SIZE);
	__init_end = .;

=======
>>>>>>> v4.9.227
	. = ALIGN(4);
	.altinstructions : {
		__alt_instructions = .;
		*(.altinstructions)
		__alt_instructions_end = .;
	}
	.altinstr_replacement : {
		*(.altinstr_replacement)
	}
<<<<<<< HEAD

	. = ALIGN(PAGE_SIZE);
	_data = .;
	_sdata = .;
	RW_DATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE, THREAD_SIZE)
=======
	.rela : ALIGN(8) {
		*(.rela .rela*)
	}

	__rela_offset	= ABSOLUTE(ADDR(.rela) - KIMAGE_VADDR);
	__rela_size	= SIZEOF(.rela);

	. = ALIGN(SEGMENT_ALIGN);
	__init_end = .;

	_data = .;
	_sdata = .;
	RW_DATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE, THREAD_SIZE)

	/*
	 * Data written with the MMU off but read with the MMU on requires
	 * cache lines to be invalidated, discarding up to a Cache Writeback
	 * Granule (CWG) of data from the cache. Keep the section that
	 * requires this type of maintenance to be in its own Cache Writeback
	 * Granule (CWG) area so the cache maintenance operations don't
	 * interfere with adjacent data.
	 */
	.mmuoff.data.write : ALIGN(SZ_2K) {
		__mmuoff_data_start = .;
		*(.mmuoff.data.write)
	}
	. = ALIGN(SZ_2K);
	.mmuoff.data.read : {
		*(.mmuoff.data.read)
		__mmuoff_data_end = .;
	}

>>>>>>> v4.9.227
	PECOFF_EDATA_PADDING
	_edata = .;

	BSS_SECTION(0, 0, 0)

	. = ALIGN(PAGE_SIZE);
	idmap_pg_dir = .;
	. += IDMAP_DIR_SIZE;
	swapper_pg_dir = .;
	. += SWAPPER_DIR_SIZE;

<<<<<<< HEAD
#ifdef CONFIG_ARM64_SW_TTBR0_PAN
	reserved_ttbr0 = .;
	. += RESERVED_TTBR0_SIZE;
#endif

=======
>>>>>>> v4.9.227
#ifdef CONFIG_UNMAP_KERNEL_AT_EL0
	tramp_pg_dir = .;
	. += PAGE_SIZE;
#endif

	_end = .;

	STABS_DEBUG

	HEAD_SYMBOLS
}

/*
<<<<<<< HEAD
 * The HYP init code can't be more than a page long.
 */
ASSERT(((__hyp_idmap_text_start + PAGE_SIZE) > __hyp_idmap_text_end),
       "HYP init code too big")

=======
 * The HYP init code and ID map text can't be longer than a page each,
 * and should not cross a page boundary.
 */
ASSERT(__hyp_idmap_text_end - (__hyp_idmap_text_start & ~(SZ_4K - 1)) <= SZ_4K,
	"HYP init code too big or misaligned")
ASSERT(__idmap_text_end - (__idmap_text_start & ~(SZ_4K - 1)) <= SZ_4K,
	"ID map text too big or misaligned")
#ifdef CONFIG_HIBERNATION
ASSERT(__hibernate_exit_text_end - (__hibernate_exit_text_start & ~(SZ_4K - 1))
	<= SZ_4K, "Hibernate exit text too big or misaligned")
#endif
>>>>>>> v4.9.227
#ifdef CONFIG_UNMAP_KERNEL_AT_EL0
ASSERT((__entry_tramp_text_end - __entry_tramp_text_start) == PAGE_SIZE,
	"Entry trampoline text too big")
#endif
/*
 * If padding is applied before .head.text, virt<->phys conversions will fail.
 */
<<<<<<< HEAD
ASSERT(_text == (PAGE_OFFSET + TEXT_OFFSET), "HEAD is misaligned")
=======
ASSERT(_text == (KIMAGE_VADDR + TEXT_OFFSET), "HEAD is misaligned")
>>>>>>> v4.9.227
