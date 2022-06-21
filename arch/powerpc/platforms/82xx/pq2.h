#ifndef _PQ2_H
#define _PQ2_H

<<<<<<< HEAD
void pq2_restart(char *cmd);
=======
void __noreturn pq2_restart(char *cmd);
>>>>>>> v4.9.227

#ifdef CONFIG_PCI
int pq2ads_pci_init_irq(void);
void pq2_init_pci(void);
#else
static inline int pq2ads_pci_init_irq(void)
{
	return 0;
}

static inline void pq2_init_pci(void)
{
}
#endif

#endif
