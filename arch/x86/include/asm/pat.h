#ifndef _ASM_X86_PAT_H
#define _ASM_X86_PAT_H

#include <linux/types.h>
#include <asm/pgtable_types.h>

<<<<<<< HEAD
#ifdef CONFIG_X86_PAT
extern int pat_enabled;
#else
static const int pat_enabled;
#endif

extern void pat_init(void);

extern int reserve_memtype(u64 start, u64 end,
		unsigned long req_type, unsigned long *ret_type);
extern int free_memtype(u64 start, u64 end);

extern int kernel_map_sync_memtype(u64 base, unsigned long size,
		unsigned long flag);

int io_reserve_memtype(resource_size_t start, resource_size_t end,
			unsigned long *type);
=======
bool pat_enabled(void);
void pat_disable(const char *reason);
extern void pat_init(void);
extern void init_cache_modes(void);

extern int reserve_memtype(u64 start, u64 end,
		enum page_cache_mode req_pcm, enum page_cache_mode *ret_pcm);
extern int free_memtype(u64 start, u64 end);

extern int kernel_map_sync_memtype(u64 base, unsigned long size,
		enum page_cache_mode pcm);

int io_reserve_memtype(resource_size_t start, resource_size_t end,
			enum page_cache_mode *pcm);
>>>>>>> v4.9.227

void io_free_memtype(resource_size_t start, resource_size_t end);

#endif /* _ASM_X86_PAT_H */
