#ifndef __LINUX_ATA_PLATFORM_H
#define __LINUX_ATA_PLATFORM_H

struct pata_platform_info {
	/*
	 * I/O port shift, for platforms with ports that are
	 * constantly spaced and need larger than the 1-byte
	 * spacing used by ata_std_ports().
	 */
	unsigned int ioport_shift;
};

<<<<<<< HEAD
=======
struct scsi_host_template;

>>>>>>> v4.9.227
extern int __pata_platform_probe(struct device *dev,
				 struct resource *io_res,
				 struct resource *ctl_res,
				 struct resource *irq_res,
				 unsigned int ioport_shift,
<<<<<<< HEAD
				 int __pio_mask);
=======
				 int __pio_mask,
				 struct scsi_host_template *sht);
>>>>>>> v4.9.227

/*
 * Marvell SATA private data
 */
struct mv_sata_platform_data {
	int	n_ports; /* number of sata ports */
};

#endif /* __LINUX_ATA_PLATFORM_H */
