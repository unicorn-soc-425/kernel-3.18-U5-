#ifndef __ASM_RCAR_GEN2_H__
#define __ASM_RCAR_GEN2_H__

void rcar_gen2_timer_init(void);
#define MD(nr) BIT(nr)
u32 rcar_gen2_read_mode_pins(void);
void rcar_gen2_reserve(void);
<<<<<<< HEAD
=======
void rcar_gen2_pm_init(void);
>>>>>>> v4.9.227

#endif /* __ASM_RCAR_GEN2_H__ */
