/*
 * Arch specific extensions to struct device
 *
 * This file is released under the GPLv2
 */
<<<<<<< HEAD
#include <asm-generic/device.h>

=======
struct dev_archdata {
	struct dma_map_ops *dma_ops;
};

struct pdev_archdata {
};
>>>>>>> v4.9.227
