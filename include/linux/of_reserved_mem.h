#ifndef __OF_RESERVED_MEM_H
#define __OF_RESERVED_MEM_H

<<<<<<< HEAD
struct device;
=======
#include <linux/device.h>

>>>>>>> v4.9.227
struct of_phandle_args;
struct reserved_mem_ops;

struct reserved_mem {
	const char			*name;
	unsigned long			fdt_node;
	unsigned long			phandle;
	const struct reserved_mem_ops	*ops;
	phys_addr_t			base;
	phys_addr_t			size;
	void				*priv;
<<<<<<< HEAD
	bool				reusable;
	int				fixup;
=======
>>>>>>> v4.9.227
};

struct reserved_mem_ops {
	int	(*device_init)(struct reserved_mem *rmem,
			       struct device *dev);
	void	(*device_release)(struct reserved_mem *rmem,
				  struct device *dev);
};

typedef int (*reservedmem_of_init_fn)(struct reserved_mem *rmem);

#define RESERVEDMEM_OF_DECLARE(name, compat, init)			\
	_OF_DECLARE(reservedmem, name, compat, init, reservedmem_of_init_fn)

#ifdef CONFIG_OF_RESERVED_MEM
<<<<<<< HEAD
int of_reserved_mem_device_init(struct device *dev);
void of_reserved_mem_device_release(struct device *dev);

=======

int of_reserved_mem_device_init_by_idx(struct device *dev,
				       struct device_node *np, int idx);
void of_reserved_mem_device_release(struct device *dev);

int early_init_dt_alloc_reserved_memory_arch(phys_addr_t size,
					     phys_addr_t align,
					     phys_addr_t start,
					     phys_addr_t end,
					     bool nomap,
					     phys_addr_t *res_base);

>>>>>>> v4.9.227
void fdt_init_reserved_mem(void);
void fdt_reserved_mem_save_node(unsigned long node, const char *uname,
			       phys_addr_t base, phys_addr_t size);
#else
<<<<<<< HEAD
static inline int of_reserved_mem_device_init(struct device *dev)
=======
static inline int of_reserved_mem_device_init_by_idx(struct device *dev,
					struct device_node *np, int idx)
>>>>>>> v4.9.227
{
	return -ENOSYS;
}
static inline void of_reserved_mem_device_release(struct device *pdev) { }

static inline void fdt_init_reserved_mem(void) { }
static inline void fdt_reserved_mem_save_node(unsigned long node,
		const char *uname, phys_addr_t base, phys_addr_t size) { }
#endif

<<<<<<< HEAD
=======
/**
 * of_reserved_mem_device_init() - assign reserved memory region to given device
 * @dev:	Pointer to the device to configure
 *
 * This function assigns respective DMA-mapping operations based on the first
 * reserved memory region specified by 'memory-region' property in device tree
 * node of the given device.
 *
 * Returns error code or zero on success.
 */
static inline int of_reserved_mem_device_init(struct device *dev)
{
	return of_reserved_mem_device_init_by_idx(dev, dev->of_node, 0);
}

>>>>>>> v4.9.227
#endif /* __OF_RESERVED_MEM_H */
