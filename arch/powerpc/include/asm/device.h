/*
 * Arch specific extensions to struct device
 *
 * This file is released under the GPLv2
 */
#ifndef _ASM_POWERPC_DEVICE_H
#define _ASM_POWERPC_DEVICE_H

struct dma_map_ops;
struct device_node;
<<<<<<< HEAD
=======
#ifdef CONFIG_PPC64
struct pci_dn;
struct iommu_table;
#endif
>>>>>>> v4.9.227

/*
 * Arch extensions to struct device.
 *
 * When adding fields, consider macio_add_one_device in
 * drivers/macintosh/macio_asic.c
 */
struct dev_archdata {
	/* DMA operations on that device */
	struct dma_map_ops	*dma_ops;

	/*
<<<<<<< HEAD
	 * When an iommu is in use, dma_data is used as a ptr to the base of the
	 * iommu_table.  Otherwise, it is a simple numerical offset.
	 */
	union {
		dma_addr_t	dma_offset;
		void		*iommu_table_base;
	} dma_data;
=======
	 * These two used to be a union. However, with the hybrid ops we need
	 * both so here we store both a DMA offset for direct mappings and
	 * an iommu_table for remapped DMA.
	 */
	dma_addr_t		dma_offset;

#ifdef CONFIG_PPC64
	struct iommu_table	*iommu_table_base;
#endif
>>>>>>> v4.9.227

#ifdef CONFIG_IOMMU_API
	void			*iommu_domain;
#endif
#ifdef CONFIG_SWIOTLB
	dma_addr_t		max_direct_dma_addr;
#endif
<<<<<<< HEAD
=======
#ifdef CONFIG_PPC64
	struct pci_dn		*pci_data;
#endif
>>>>>>> v4.9.227
#ifdef CONFIG_EEH
	struct eeh_dev		*edev;
#endif
#ifdef CONFIG_FAIL_IOMMU
	int fail_iommu;
#endif
<<<<<<< HEAD
=======
#ifdef CONFIG_CXL_BASE
	struct cxl_context	*cxl_ctx;
#endif
>>>>>>> v4.9.227
};

struct pdev_archdata {
	u64 dma_mask;
};

#define ARCH_HAS_DMA_GET_REQUIRED_MASK

#endif /* _ASM_POWERPC_DEVICE_H */
