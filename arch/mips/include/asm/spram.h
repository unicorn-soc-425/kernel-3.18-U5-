#ifndef _MIPS_SPRAM_H
#define _MIPS_SPRAM_H

<<<<<<< HEAD
#ifdef CONFIG_CPU_MIPSR2
extern __init void spram_config(void);
#else
static inline void spram_config(void) { };
#endif /* CONFIG_CPU_MIPSR2 */
=======
#if defined(CONFIG_MIPS_SPRAM)
extern __init void spram_config(void);
#else
static inline void spram_config(void) { };
#endif /* CONFIG_MIPS_SPRAM */
>>>>>>> v4.9.227

#endif /* _MIPS_SPRAM_H */
