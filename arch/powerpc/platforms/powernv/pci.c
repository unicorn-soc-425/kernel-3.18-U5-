/*
 * Support PCI/PCIe on PowerNV platforms
 *
<<<<<<< HEAD
 * Currently supports only P5IOC2
 *
=======
>>>>>>> v4.9.227
 * Copyright 2011 Benjamin Herrenschmidt, IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/init.h>
<<<<<<< HEAD
#include <linux/bootmem.h>
=======
>>>>>>> v4.9.227
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/msi.h>
#include <linux/iommu.h>

#include <asm/sections.h>
#include <asm/io.h>
#include <asm/prom.h>
#include <asm/pci-bridge.h>
#include <asm/machdep.h>
#include <asm/msi_bitmap.h>
#include <asm/ppc-pci.h>
<<<<<<< HEAD
=======
#include <asm/pnv-pci.h>
>>>>>>> v4.9.227
#include <asm/opal.h>
#include <asm/iommu.h>
#include <asm/tce.h>
#include <asm/firmware.h>
#include <asm/eeh_event.h>
#include <asm/eeh.h>

#include "powernv.h"
#include "pci.h"

<<<<<<< HEAD
/* Delay in usec */
#define PCI_RESET_DELAY_US	3000000

#define cfg_dbg(fmt...)	do { } while(0)
//#define cfg_dbg(fmt...)	printk(fmt)

#ifdef CONFIG_PCI_MSI
static int pnv_setup_msi_irqs(struct pci_dev *pdev, int nvec, int type)
=======
int pnv_pci_get_slot_id(struct device_node *np, uint64_t *id)
{
	struct device_node *parent = np;
	u32 bdfn;
	u64 phbid;
	int ret;

	ret = of_property_read_u32(np, "reg", &bdfn);
	if (ret)
		return -ENXIO;

	bdfn = ((bdfn & 0x00ffff00) >> 8);
	while ((parent = of_get_parent(parent))) {
		if (!PCI_DN(parent)) {
			of_node_put(parent);
			break;
		}

		if (!of_device_is_compatible(parent, "ibm,ioda2-phb")) {
			of_node_put(parent);
			continue;
		}

		ret = of_property_read_u64(parent, "ibm,opal-phbid", &phbid);
		if (ret) {
			of_node_put(parent);
			return -ENXIO;
		}

		*id = PCI_SLOT_ID(phbid, bdfn);
		return 0;
	}

	return -ENODEV;
}
EXPORT_SYMBOL_GPL(pnv_pci_get_slot_id);

int pnv_pci_get_device_tree(uint32_t phandle, void *buf, uint64_t len)
{
	int64_t rc;

	if (!opal_check_token(OPAL_GET_DEVICE_TREE))
		return -ENXIO;

	rc = opal_get_device_tree(phandle, (uint64_t)buf, len);
	if (rc < OPAL_SUCCESS)
		return -EIO;

	return rc;
}
EXPORT_SYMBOL_GPL(pnv_pci_get_device_tree);

int pnv_pci_get_presence_state(uint64_t id, uint8_t *state)
{
	int64_t rc;

	if (!opal_check_token(OPAL_PCI_GET_PRESENCE_STATE))
		return -ENXIO;

	rc = opal_pci_get_presence_state(id, (uint64_t)state);
	if (rc != OPAL_SUCCESS)
		return -EIO;

	return 0;
}
EXPORT_SYMBOL_GPL(pnv_pci_get_presence_state);

int pnv_pci_get_power_state(uint64_t id, uint8_t *state)
{
	int64_t rc;

	if (!opal_check_token(OPAL_PCI_GET_POWER_STATE))
		return -ENXIO;

	rc = opal_pci_get_power_state(id, (uint64_t)state);
	if (rc != OPAL_SUCCESS)
		return -EIO;

	return 0;
}
EXPORT_SYMBOL_GPL(pnv_pci_get_power_state);

int pnv_pci_set_power_state(uint64_t id, uint8_t state, struct opal_msg *msg)
{
	struct opal_msg m;
	int token, ret;
	int64_t rc;

	if (!opal_check_token(OPAL_PCI_SET_POWER_STATE))
		return -ENXIO;

	token = opal_async_get_token_interruptible();
	if (unlikely(token < 0))
		return token;

	rc = opal_pci_set_power_state(token, id, (uint64_t)&state);
	if (rc == OPAL_SUCCESS) {
		ret = 0;
		goto exit;
	} else if (rc != OPAL_ASYNC_COMPLETION) {
		ret = -EIO;
		goto exit;
	}

	ret = opal_async_wait_response(token, &m);
	if (ret < 0)
		goto exit;

	if (msg) {
		ret = 1;
		memcpy(msg, &m, sizeof(m));
	}

exit:
	opal_async_release_token(token);
	return ret;
}
EXPORT_SYMBOL_GPL(pnv_pci_set_power_state);

#ifdef CONFIG_PCI_MSI
int pnv_setup_msi_irqs(struct pci_dev *pdev, int nvec, int type)
>>>>>>> v4.9.227
{
	struct pci_controller *hose = pci_bus_to_host(pdev->bus);
	struct pnv_phb *phb = hose->private_data;
	struct msi_desc *entry;
	struct msi_msg msg;
	int hwirq;
	unsigned int virq;
	int rc;

	if (WARN_ON(!phb) || !phb->msi_bmp.bitmap)
		return -ENODEV;

	if (pdev->no_64bit_msi && !phb->msi32_support)
		return -ENODEV;

<<<<<<< HEAD
	list_for_each_entry(entry, &pdev->msi_list, list) {
=======
	for_each_pci_msi_entry(entry, pdev) {
>>>>>>> v4.9.227
		if (!entry->msi_attrib.is_64 && !phb->msi32_support) {
			pr_warn("%s: Supports only 64-bit MSIs\n",
				pci_name(pdev));
			return -ENXIO;
		}
		hwirq = msi_bitmap_alloc_hwirqs(&phb->msi_bmp, 1);
		if (hwirq < 0) {
			pr_warn("%s: Failed to find a free MSI\n",
				pci_name(pdev));
			return -ENOSPC;
		}
		virq = irq_create_mapping(NULL, phb->msi_base + hwirq);
<<<<<<< HEAD
		if (virq == NO_IRQ) {
=======
		if (!virq) {
>>>>>>> v4.9.227
			pr_warn("%s: Failed to map MSI to linux irq\n",
				pci_name(pdev));
			msi_bitmap_free_hwirqs(&phb->msi_bmp, hwirq, 1);
			return -ENOMEM;
		}
		rc = phb->msi_setup(phb, pdev, phb->msi_base + hwirq,
				    virq, entry->msi_attrib.is_64, &msg);
		if (rc) {
			pr_warn("%s: Failed to setup MSI\n", pci_name(pdev));
			irq_dispose_mapping(virq);
			msi_bitmap_free_hwirqs(&phb->msi_bmp, hwirq, 1);
			return rc;
		}
		irq_set_msi_desc(virq, entry);
		pci_write_msi_msg(virq, &msg);
	}
	return 0;
}

<<<<<<< HEAD
static void pnv_teardown_msi_irqs(struct pci_dev *pdev)
=======
void pnv_teardown_msi_irqs(struct pci_dev *pdev)
>>>>>>> v4.9.227
{
	struct pci_controller *hose = pci_bus_to_host(pdev->bus);
	struct pnv_phb *phb = hose->private_data;
	struct msi_desc *entry;
	irq_hw_number_t hwirq;

	if (WARN_ON(!phb))
		return;

<<<<<<< HEAD
	list_for_each_entry(entry, &pdev->msi_list, list) {
		if (entry->irq == NO_IRQ)
=======
	for_each_pci_msi_entry(entry, pdev) {
		if (!entry->irq)
>>>>>>> v4.9.227
			continue;
		hwirq = virq_to_hw(entry->irq);
		irq_set_msi_desc(entry->irq, NULL);
		irq_dispose_mapping(entry->irq);
		msi_bitmap_free_hwirqs(&phb->msi_bmp, hwirq - phb->msi_base, 1);
	}
}
#endif /* CONFIG_PCI_MSI */

static void pnv_pci_dump_p7ioc_diag_data(struct pci_controller *hose,
					 struct OpalIoPhbErrorCommon *common)
{
	struct OpalIoP7IOCPhbErrorData *data;
	int i;

	data = (struct OpalIoP7IOCPhbErrorData *)common;
	pr_info("P7IOC PHB#%d Diag-data (Version: %d)\n",
		hose->global_number, be32_to_cpu(common->version));

	if (data->brdgCtl)
		pr_info("brdgCtl:     %08x\n",
			be32_to_cpu(data->brdgCtl));
	if (data->portStatusReg || data->rootCmplxStatus ||
	    data->busAgentStatus)
		pr_info("UtlSts:      %08x %08x %08x\n",
			be32_to_cpu(data->portStatusReg),
			be32_to_cpu(data->rootCmplxStatus),
			be32_to_cpu(data->busAgentStatus));
	if (data->deviceStatus || data->slotStatus   ||
	    data->linkStatus   || data->devCmdStatus ||
	    data->devSecStatus)
		pr_info("RootSts:     %08x %08x %08x %08x %08x\n",
			be32_to_cpu(data->deviceStatus),
			be32_to_cpu(data->slotStatus),
			be32_to_cpu(data->linkStatus),
			be32_to_cpu(data->devCmdStatus),
			be32_to_cpu(data->devSecStatus));
	if (data->rootErrorStatus   || data->uncorrErrorStatus ||
	    data->corrErrorStatus)
		pr_info("RootErrSts:  %08x %08x %08x\n",
			be32_to_cpu(data->rootErrorStatus),
			be32_to_cpu(data->uncorrErrorStatus),
			be32_to_cpu(data->corrErrorStatus));
	if (data->tlpHdr1 || data->tlpHdr2 ||
	    data->tlpHdr3 || data->tlpHdr4)
		pr_info("RootErrLog:  %08x %08x %08x %08x\n",
			be32_to_cpu(data->tlpHdr1),
			be32_to_cpu(data->tlpHdr2),
			be32_to_cpu(data->tlpHdr3),
			be32_to_cpu(data->tlpHdr4));
	if (data->sourceId || data->errorClass ||
	    data->correlator)
		pr_info("RootErrLog1: %08x %016llx %016llx\n",
			be32_to_cpu(data->sourceId),
			be64_to_cpu(data->errorClass),
			be64_to_cpu(data->correlator));
	if (data->p7iocPlssr || data->p7iocCsr)
		pr_info("PhbSts:      %016llx %016llx\n",
			be64_to_cpu(data->p7iocPlssr),
			be64_to_cpu(data->p7iocCsr));
	if (data->lemFir)
		pr_info("Lem:         %016llx %016llx %016llx\n",
			be64_to_cpu(data->lemFir),
			be64_to_cpu(data->lemErrorMask),
			be64_to_cpu(data->lemWOF));
	if (data->phbErrorStatus)
		pr_info("PhbErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->phbErrorStatus),
			be64_to_cpu(data->phbFirstErrorStatus),
			be64_to_cpu(data->phbErrorLog0),
			be64_to_cpu(data->phbErrorLog1));
	if (data->mmioErrorStatus)
		pr_info("OutErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->mmioErrorStatus),
			be64_to_cpu(data->mmioFirstErrorStatus),
			be64_to_cpu(data->mmioErrorLog0),
			be64_to_cpu(data->mmioErrorLog1));
	if (data->dma0ErrorStatus)
		pr_info("InAErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->dma0ErrorStatus),
			be64_to_cpu(data->dma0FirstErrorStatus),
			be64_to_cpu(data->dma0ErrorLog0),
			be64_to_cpu(data->dma0ErrorLog1));
	if (data->dma1ErrorStatus)
		pr_info("InBErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->dma1ErrorStatus),
			be64_to_cpu(data->dma1FirstErrorStatus),
			be64_to_cpu(data->dma1ErrorLog0),
			be64_to_cpu(data->dma1ErrorLog1));

	for (i = 0; i < OPAL_P7IOC_NUM_PEST_REGS; i++) {
<<<<<<< HEAD
		if ((data->pestA[i] >> 63) == 0 &&
		    (data->pestB[i] >> 63) == 0)
=======
		if ((be64_to_cpu(data->pestA[i]) >> 63) == 0 &&
		    (be64_to_cpu(data->pestB[i]) >> 63) == 0)
>>>>>>> v4.9.227
			continue;

		pr_info("PE[%3d] A/B: %016llx %016llx\n",
			i, be64_to_cpu(data->pestA[i]),
			be64_to_cpu(data->pestB[i]));
	}
}

static void pnv_pci_dump_phb3_diag_data(struct pci_controller *hose,
					struct OpalIoPhbErrorCommon *common)
{
	struct OpalIoPhb3ErrorData *data;
	int i;

	data = (struct OpalIoPhb3ErrorData*)common;
	pr_info("PHB3 PHB#%d Diag-data (Version: %d)\n",
		hose->global_number, be32_to_cpu(common->version));
	if (data->brdgCtl)
		pr_info("brdgCtl:     %08x\n",
			be32_to_cpu(data->brdgCtl));
	if (data->portStatusReg || data->rootCmplxStatus ||
	    data->busAgentStatus)
		pr_info("UtlSts:      %08x %08x %08x\n",
			be32_to_cpu(data->portStatusReg),
			be32_to_cpu(data->rootCmplxStatus),
			be32_to_cpu(data->busAgentStatus));
	if (data->deviceStatus || data->slotStatus   ||
	    data->linkStatus   || data->devCmdStatus ||
	    data->devSecStatus)
		pr_info("RootSts:     %08x %08x %08x %08x %08x\n",
			be32_to_cpu(data->deviceStatus),
			be32_to_cpu(data->slotStatus),
			be32_to_cpu(data->linkStatus),
			be32_to_cpu(data->devCmdStatus),
			be32_to_cpu(data->devSecStatus));
	if (data->rootErrorStatus || data->uncorrErrorStatus ||
	    data->corrErrorStatus)
		pr_info("RootErrSts:  %08x %08x %08x\n",
			be32_to_cpu(data->rootErrorStatus),
			be32_to_cpu(data->uncorrErrorStatus),
			be32_to_cpu(data->corrErrorStatus));
	if (data->tlpHdr1 || data->tlpHdr2 ||
	    data->tlpHdr3 || data->tlpHdr4)
		pr_info("RootErrLog:  %08x %08x %08x %08x\n",
			be32_to_cpu(data->tlpHdr1),
			be32_to_cpu(data->tlpHdr2),
			be32_to_cpu(data->tlpHdr3),
			be32_to_cpu(data->tlpHdr4));
	if (data->sourceId || data->errorClass ||
	    data->correlator)
		pr_info("RootErrLog1: %08x %016llx %016llx\n",
			be32_to_cpu(data->sourceId),
			be64_to_cpu(data->errorClass),
			be64_to_cpu(data->correlator));
	if (data->nFir)
		pr_info("nFir:        %016llx %016llx %016llx\n",
			be64_to_cpu(data->nFir),
			be64_to_cpu(data->nFirMask),
			be64_to_cpu(data->nFirWOF));
	if (data->phbPlssr || data->phbCsr)
		pr_info("PhbSts:      %016llx %016llx\n",
			be64_to_cpu(data->phbPlssr),
			be64_to_cpu(data->phbCsr));
	if (data->lemFir)
		pr_info("Lem:         %016llx %016llx %016llx\n",
			be64_to_cpu(data->lemFir),
			be64_to_cpu(data->lemErrorMask),
			be64_to_cpu(data->lemWOF));
	if (data->phbErrorStatus)
		pr_info("PhbErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->phbErrorStatus),
			be64_to_cpu(data->phbFirstErrorStatus),
			be64_to_cpu(data->phbErrorLog0),
			be64_to_cpu(data->phbErrorLog1));
	if (data->mmioErrorStatus)
		pr_info("OutErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->mmioErrorStatus),
			be64_to_cpu(data->mmioFirstErrorStatus),
			be64_to_cpu(data->mmioErrorLog0),
			be64_to_cpu(data->mmioErrorLog1));
	if (data->dma0ErrorStatus)
		pr_info("InAErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->dma0ErrorStatus),
			be64_to_cpu(data->dma0FirstErrorStatus),
			be64_to_cpu(data->dma0ErrorLog0),
			be64_to_cpu(data->dma0ErrorLog1));
	if (data->dma1ErrorStatus)
		pr_info("InBErr:      %016llx %016llx %016llx %016llx\n",
			be64_to_cpu(data->dma1ErrorStatus),
			be64_to_cpu(data->dma1FirstErrorStatus),
			be64_to_cpu(data->dma1ErrorLog0),
			be64_to_cpu(data->dma1ErrorLog1));

	for (i = 0; i < OPAL_PHB3_NUM_PEST_REGS; i++) {
		if ((be64_to_cpu(data->pestA[i]) >> 63) == 0 &&
		    (be64_to_cpu(data->pestB[i]) >> 63) == 0)
			continue;

		pr_info("PE[%3d] A/B: %016llx %016llx\n",
				i, be64_to_cpu(data->pestA[i]),
				be64_to_cpu(data->pestB[i]));
	}
}

void pnv_pci_dump_phb_diag_data(struct pci_controller *hose,
				unsigned char *log_buff)
{
	struct OpalIoPhbErrorCommon *common;

	if (!hose || !log_buff)
		return;

	common = (struct OpalIoPhbErrorCommon *)log_buff;
	switch (be32_to_cpu(common->ioType)) {
	case OPAL_PHB_ERROR_DATA_TYPE_P7IOC:
		pnv_pci_dump_p7ioc_diag_data(hose, common);
		break;
	case OPAL_PHB_ERROR_DATA_TYPE_PHB3:
		pnv_pci_dump_phb3_diag_data(hose, common);
		break;
	default:
		pr_warn("%s: Unrecognized ioType %d\n",
			__func__, be32_to_cpu(common->ioType));
	}
}

static void pnv_pci_handle_eeh_config(struct pnv_phb *phb, u32 pe_no)
{
	unsigned long flags, rc;
	int has_diag, ret = 0;

	spin_lock_irqsave(&phb->lock, flags);

	/* Fetch PHB diag-data */
	rc = opal_pci_get_phb_diag_data2(phb->opal_id, phb->diag.blob,
					 PNV_PCI_DIAG_BUF_SIZE);
	has_diag = (rc == OPAL_SUCCESS);

	/* If PHB supports compound PE, to handle it */
	if (phb->unfreeze_pe) {
		ret = phb->unfreeze_pe(phb,
				       pe_no,
				       OPAL_EEH_ACTION_CLEAR_FREEZE_ALL);
	} else {
		rc = opal_pci_eeh_freeze_clear(phb->opal_id,
					     pe_no,
					     OPAL_EEH_ACTION_CLEAR_FREEZE_ALL);
		if (rc) {
			pr_warn("%s: Failure %ld clearing frozen "
				"PHB#%x-PE#%x\n",
				__func__, rc, phb->hose->global_number,
				pe_no);
			ret = -EIO;
		}
	}

	/*
	 * For now, let's only display the diag buffer when we fail to clear
	 * the EEH status. We'll do more sensible things later when we have
	 * proper EEH support. We need to make sure we don't pollute ourselves
	 * with the normal errors generated when probing empty slots
	 */
	if (has_diag && ret)
		pnv_pci_dump_phb_diag_data(phb->hose, phb->diag.blob);

	spin_unlock_irqrestore(&phb->lock, flags);
}

<<<<<<< HEAD
static void pnv_pci_config_check_eeh(struct pnv_phb *phb,
				     struct device_node *dn)
{
	u8	fstate;
	__be16	pcierr;
	int	pe_no;
=======
static void pnv_pci_config_check_eeh(struct pci_dn *pdn)
{
	struct pnv_phb *phb = pdn->phb->private_data;
	u8	fstate = 0;
	__be16	pcierr = 0;
	unsigned int pe_no;
>>>>>>> v4.9.227
	s64	rc;

	/*
	 * Get the PE#. During the PCI probe stage, we might not
	 * setup that yet. So all ER errors should be mapped to
	 * reserved PE.
	 */
<<<<<<< HEAD
	pe_no = PCI_DN(dn)->pe_number;
	if (pe_no == IODA_INVALID_PE) {
		if (phb->type == PNV_PHB_P5IOC2)
			pe_no = 0;
		else
			pe_no = phb->ioda.reserved_pe;
=======
	pe_no = pdn->pe_number;
	if (pe_no == IODA_INVALID_PE) {
		pe_no = phb->ioda.reserved_pe_idx;
>>>>>>> v4.9.227
	}

	/*
	 * Fetch frozen state. If the PHB support compound PE,
	 * we need handle that case.
	 */
	if (phb->get_pe_state) {
		fstate = phb->get_pe_state(phb, pe_no);
	} else {
		rc = opal_pci_eeh_freeze_status(phb->opal_id,
						pe_no,
						&fstate,
						&pcierr,
						NULL);
		if (rc) {
			pr_warn("%s: Failure %lld getting PHB#%x-PE#%x state\n",
				__func__, rc, phb->hose->global_number, pe_no);
			return;
		}
	}

<<<<<<< HEAD
	cfg_dbg(" -> EEH check, bdfn=%04x PE#%d fstate=%x\n",
		(PCI_DN(dn)->busno << 8) | (PCI_DN(dn)->devfn),
		pe_no, fstate);
=======
	pr_devel(" -> EEH check, bdfn=%04x PE#%d fstate=%x\n",
		 (pdn->busno << 8) | (pdn->devfn), pe_no, fstate);
>>>>>>> v4.9.227

	/* Clear the frozen state if applicable */
	if (fstate == OPAL_EEH_STOPPED_MMIO_FREEZE ||
	    fstate == OPAL_EEH_STOPPED_DMA_FREEZE  ||
	    fstate == OPAL_EEH_STOPPED_MMIO_DMA_FREEZE) {
		/*
		 * If PHB supports compound PE, freeze it for
		 * consistency.
		 */
		if (phb->freeze_pe)
			phb->freeze_pe(phb, pe_no);

		pnv_pci_handle_eeh_config(phb, pe_no);
	}
}

<<<<<<< HEAD
int pnv_pci_cfg_read(struct device_node *dn,
		     int where, int size, u32 *val)
{
	struct pci_dn *pdn = PCI_DN(dn);
=======
int pnv_pci_cfg_read(struct pci_dn *pdn,
		     int where, int size, u32 *val)
{
>>>>>>> v4.9.227
	struct pnv_phb *phb = pdn->phb->private_data;
	u32 bdfn = (pdn->busno << 8) | pdn->devfn;
	s64 rc;

	switch (size) {
	case 1: {
		u8 v8;
		rc = opal_pci_config_read_byte(phb->opal_id, bdfn, where, &v8);
		*val = (rc == OPAL_SUCCESS) ? v8 : 0xff;
		break;
	}
	case 2: {
		__be16 v16;
		rc = opal_pci_config_read_half_word(phb->opal_id, bdfn, where,
						   &v16);
		*val = (rc == OPAL_SUCCESS) ? be16_to_cpu(v16) : 0xffff;
		break;
	}
	case 4: {
		__be32 v32;
		rc = opal_pci_config_read_word(phb->opal_id, bdfn, where, &v32);
		*val = (rc == OPAL_SUCCESS) ? be32_to_cpu(v32) : 0xffffffff;
		break;
	}
	default:
		return PCIBIOS_FUNC_NOT_SUPPORTED;
	}

<<<<<<< HEAD
	cfg_dbg("%s: bus: %x devfn: %x +%x/%x -> %08x\n",
		__func__, pdn->busno, pdn->devfn, where, size, *val);
	return PCIBIOS_SUCCESSFUL;
}

int pnv_pci_cfg_write(struct device_node *dn,
		      int where, int size, u32 val)
{
	struct pci_dn *pdn = PCI_DN(dn);
	struct pnv_phb *phb = pdn->phb->private_data;
	u32 bdfn = (pdn->busno << 8) | pdn->devfn;

	cfg_dbg("%s: bus: %x devfn: %x +%x/%x -> %08x\n",
		pdn->busno, pdn->devfn, where, size, val);
=======
	pr_devel("%s: bus: %x devfn: %x +%x/%x -> %08x\n",
		 __func__, pdn->busno, pdn->devfn, where, size, *val);
	return PCIBIOS_SUCCESSFUL;
}

int pnv_pci_cfg_write(struct pci_dn *pdn,
		      int where, int size, u32 val)
{
	struct pnv_phb *phb = pdn->phb->private_data;
	u32 bdfn = (pdn->busno << 8) | pdn->devfn;

	pr_devel("%s: bus: %x devfn: %x +%x/%x -> %08x\n",
		 __func__, pdn->busno, pdn->devfn, where, size, val);
>>>>>>> v4.9.227
	switch (size) {
	case 1:
		opal_pci_config_write_byte(phb->opal_id, bdfn, where, val);
		break;
	case 2:
		opal_pci_config_write_half_word(phb->opal_id, bdfn, where, val);
		break;
	case 4:
		opal_pci_config_write_word(phb->opal_id, bdfn, where, val);
		break;
	default:
		return PCIBIOS_FUNC_NOT_SUPPORTED;
	}

	return PCIBIOS_SUCCESSFUL;
}

#if CONFIG_EEH
<<<<<<< HEAD
static bool pnv_pci_cfg_check(struct pci_controller *hose,
			      struct device_node *dn)
{
	struct eeh_dev *edev = NULL;
	struct pnv_phb *phb = hose->private_data;
=======
static bool pnv_pci_cfg_check(struct pci_dn *pdn)
{
	struct eeh_dev *edev = NULL;
	struct pnv_phb *phb = pdn->phb->private_data;
>>>>>>> v4.9.227

	/* EEH not enabled ? */
	if (!(phb->flags & PNV_PHB_FLAG_EEH))
		return true;

	/* PE reset or device removed ? */
<<<<<<< HEAD
	edev = of_node_to_eeh_dev(dn);
=======
	edev = pdn->edev;
>>>>>>> v4.9.227
	if (edev) {
		if (edev->pe &&
		    (edev->pe->state & EEH_PE_CFG_BLOCKED))
			return false;

		if (edev->mode & EEH_DEV_REMOVED)
			return false;
	}

	return true;
}
#else
<<<<<<< HEAD
static inline pnv_pci_cfg_check(struct pci_controller *hose,
				struct device_node *dn)
=======
static inline pnv_pci_cfg_check(struct pci_dn *pdn)
>>>>>>> v4.9.227
{
	return true;
}
#endif /* CONFIG_EEH */

static int pnv_pci_read_config(struct pci_bus *bus,
			       unsigned int devfn,
			       int where, int size, u32 *val)
{
<<<<<<< HEAD
	struct device_node *dn, *busdn = pci_bus_to_OF_node(bus);
	struct pci_dn *pdn;
	struct pnv_phb *phb;
	bool found = false;
	int ret;

	*val = 0xFFFFFFFF;
	for (dn = busdn->child; dn; dn = dn->sibling) {
		pdn = PCI_DN(dn);
		if (pdn && pdn->devfn == devfn) {
			phb = pdn->phb->private_data;
			found = true;
			break;
		}
	}

	if (!found || !pnv_pci_cfg_check(pdn->phb, dn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	ret = pnv_pci_cfg_read(dn, where, size, val);
	if (phb->flags & PNV_PHB_FLAG_EEH) {
		if (*val == EEH_IO_ERROR_VALUE(size) &&
		    eeh_dev_check_failure(of_node_to_eeh_dev(dn)))
                        return PCIBIOS_DEVICE_NOT_FOUND;
	} else {
		pnv_pci_config_check_eeh(phb, dn);
=======
	struct pci_dn *pdn;
	struct pnv_phb *phb;
	int ret;

	*val = 0xFFFFFFFF;
	pdn = pci_get_pdn_by_devfn(bus, devfn);
	if (!pdn)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (!pnv_pci_cfg_check(pdn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	ret = pnv_pci_cfg_read(pdn, where, size, val);
	phb = pdn->phb->private_data;
	if (phb->flags & PNV_PHB_FLAG_EEH && pdn->edev) {
		if (*val == EEH_IO_ERROR_VALUE(size) &&
		    eeh_dev_check_failure(pdn->edev))
                        return PCIBIOS_DEVICE_NOT_FOUND;
	} else {
		pnv_pci_config_check_eeh(pdn);
>>>>>>> v4.9.227
	}

	return ret;
}

static int pnv_pci_write_config(struct pci_bus *bus,
				unsigned int devfn,
				int where, int size, u32 val)
{
<<<<<<< HEAD
	struct device_node *dn, *busdn = pci_bus_to_OF_node(bus);
	struct pci_dn *pdn;
	struct pnv_phb *phb;
	bool found = false;
	int ret;

	for (dn = busdn->child; dn; dn = dn->sibling) {
		pdn = PCI_DN(dn);
		if (pdn && pdn->devfn == devfn) {
			phb = pdn->phb->private_data;
			found = true;
			break;
		}
	}

	if (!found || !pnv_pci_cfg_check(pdn->phb, dn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	ret = pnv_pci_cfg_write(dn, where, size, val);
	if (!(phb->flags & PNV_PHB_FLAG_EEH))
		pnv_pci_config_check_eeh(phb, dn);
=======
	struct pci_dn *pdn;
	struct pnv_phb *phb;
	int ret;

	pdn = pci_get_pdn_by_devfn(bus, devfn);
	if (!pdn)
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (!pnv_pci_cfg_check(pdn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	ret = pnv_pci_cfg_write(pdn, where, size, val);
	phb = pdn->phb->private_data;
	if (!(phb->flags & PNV_PHB_FLAG_EEH))
		pnv_pci_config_check_eeh(pdn);
>>>>>>> v4.9.227

	return ret;
}

struct pci_ops pnv_pci_ops = {
	.read  = pnv_pci_read_config,
	.write = pnv_pci_write_config,
};

<<<<<<< HEAD
static int pnv_tce_build(struct iommu_table *tbl, long index, long npages,
			 unsigned long uaddr, enum dma_data_direction direction,
			 struct dma_attrs *attrs, bool rm)
{
	u64 proto_tce;
	__be64 *tcep, *tces;
	u64 rpn;

	proto_tce = TCE_PCI_READ; // Read allowed

	if (direction != DMA_TO_DEVICE)
		proto_tce |= TCE_PCI_WRITE;

	tces = tcep = ((__be64 *)tbl->it_base) + index - tbl->it_offset;
	rpn = __pa(uaddr) >> tbl->it_page_shift;

	while (npages--)
		*(tcep++) = cpu_to_be64(proto_tce |
				(rpn++ << tbl->it_page_shift));

	/* Some implementations won't cache invalid TCEs and thus may not
	 * need that flush. We'll probably turn it_type into a bit mask
	 * of flags if that becomes the case
	 */
	if (tbl->it_type & TCE_PCI_SWINV_CREATE)
		pnv_pci_ioda_tce_invalidate(tbl, tces, tcep - 1, rm);
=======
static __be64 *pnv_tce(struct iommu_table *tbl, long idx)
{
	__be64 *tmp = ((__be64 *)tbl->it_base);
	int  level = tbl->it_indirect_levels;
	const long shift = ilog2(tbl->it_level_size);
	unsigned long mask = (tbl->it_level_size - 1) << (level * shift);

	while (level) {
		int n = (idx & mask) >> (level * shift);
		unsigned long tce = be64_to_cpu(tmp[n]);

		tmp = __va(tce & ~(TCE_PCI_READ | TCE_PCI_WRITE));
		idx &= ~mask;
		mask >>= shift;
		--level;
	}

	return tmp + idx;
}

int pnv_tce_build(struct iommu_table *tbl, long index, long npages,
		unsigned long uaddr, enum dma_data_direction direction,
		unsigned long attrs)
{
	u64 proto_tce = iommu_direction_to_tce_perm(direction);
	u64 rpn = __pa(uaddr) >> tbl->it_page_shift;
	long i;

	if (proto_tce & TCE_PCI_WRITE)
		proto_tce |= TCE_PCI_READ;

	for (i = 0; i < npages; i++) {
		unsigned long newtce = proto_tce |
			((rpn + i) << tbl->it_page_shift);
		unsigned long idx = index - tbl->it_offset + i;

		*(pnv_tce(tbl, idx)) = cpu_to_be64(newtce);
	}
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int pnv_tce_build_vm(struct iommu_table *tbl, long index, long npages,
			    unsigned long uaddr,
			    enum dma_data_direction direction,
			    struct dma_attrs *attrs)
{
	return pnv_tce_build(tbl, index, npages, uaddr, direction, attrs,
			false);
}

static void pnv_tce_free(struct iommu_table *tbl, long index, long npages,
		bool rm)
{
	__be64 *tcep, *tces;

	tces = tcep = ((__be64 *)tbl->it_base) + index - tbl->it_offset;

	while (npages--)
		*(tcep++) = cpu_to_be64(0);

	if (tbl->it_type & TCE_PCI_SWINV_FREE)
		pnv_pci_ioda_tce_invalidate(tbl, tces, tcep - 1, rm);
}

static void pnv_tce_free_vm(struct iommu_table *tbl, long index, long npages)
{
	pnv_tce_free(tbl, index, npages, false);
}

static unsigned long pnv_tce_get(struct iommu_table *tbl, long index)
{
	return ((u64 *)tbl->it_base)[index - tbl->it_offset];
}

static int pnv_tce_build_rm(struct iommu_table *tbl, long index, long npages,
			    unsigned long uaddr,
			    enum dma_data_direction direction,
			    struct dma_attrs *attrs)
{
	return pnv_tce_build(tbl, index, npages, uaddr, direction, attrs, true);
}

static void pnv_tce_free_rm(struct iommu_table *tbl, long index, long npages)
{
	pnv_tce_free(tbl, index, npages, true);
=======
#ifdef CONFIG_IOMMU_API
int pnv_tce_xchg(struct iommu_table *tbl, long index,
		unsigned long *hpa, enum dma_data_direction *direction)
{
	u64 proto_tce = iommu_direction_to_tce_perm(*direction);
	unsigned long newtce = *hpa | proto_tce, oldtce;
	unsigned long idx = index - tbl->it_offset;

	BUG_ON(*hpa & ~IOMMU_PAGE_MASK(tbl));

	if (newtce & TCE_PCI_WRITE)
		newtce |= TCE_PCI_READ;

	oldtce = be64_to_cpu(xchg(pnv_tce(tbl, idx), cpu_to_be64(newtce)));
	*hpa = oldtce & ~(TCE_PCI_READ | TCE_PCI_WRITE);
	*direction = iommu_tce_direction(oldtce);

	return 0;
}
#endif

void pnv_tce_free(struct iommu_table *tbl, long index, long npages)
{
	long i;

	for (i = 0; i < npages; i++) {
		unsigned long idx = index - tbl->it_offset + i;

		*(pnv_tce(tbl, idx)) = cpu_to_be64(0);
	}
}

unsigned long pnv_tce_get(struct iommu_table *tbl, long index)
{
	return *(pnv_tce(tbl, index - tbl->it_offset));
}

struct iommu_table *pnv_pci_table_alloc(int nid)
{
	struct iommu_table *tbl;

	tbl = kzalloc_node(sizeof(struct iommu_table), GFP_KERNEL, nid);
	INIT_LIST_HEAD_RCU(&tbl->it_group_list);

	return tbl;
}

long pnv_pci_link_table_and_group(int node, int num,
		struct iommu_table *tbl,
		struct iommu_table_group *table_group)
{
	struct iommu_table_group_link *tgl = NULL;

	if (WARN_ON(!tbl || !table_group))
		return -EINVAL;

	tgl = kzalloc_node(sizeof(struct iommu_table_group_link), GFP_KERNEL,
			node);
	if (!tgl)
		return -ENOMEM;

	tgl->table_group = table_group;
	list_add_rcu(&tgl->next, &tbl->it_group_list);

	table_group->tables[num] = tbl;

	return 0;
}

static void pnv_iommu_table_group_link_free(struct rcu_head *head)
{
	struct iommu_table_group_link *tgl = container_of(head,
			struct iommu_table_group_link, rcu);

	kfree(tgl);
}

void pnv_pci_unlink_table_and_group(struct iommu_table *tbl,
		struct iommu_table_group *table_group)
{
	long i;
	bool found;
	struct iommu_table_group_link *tgl;

	if (!tbl || !table_group)
		return;

	/* Remove link to a group from table's list of attached groups */
	found = false;
	list_for_each_entry_rcu(tgl, &tbl->it_group_list, next) {
		if (tgl->table_group == table_group) {
			list_del_rcu(&tgl->next);
			call_rcu(&tgl->rcu, pnv_iommu_table_group_link_free);
			found = true;
			break;
		}
	}
	if (WARN_ON(!found))
		return;

	/* Clean a pointer to iommu_table in iommu_table_group::tables[] */
	found = false;
	for (i = 0; i < IOMMU_TABLE_GROUP_MAX_TABLES; ++i) {
		if (table_group->tables[i] == tbl) {
			table_group->tables[i] = NULL;
			found = true;
			break;
		}
	}
	WARN_ON(!found);
>>>>>>> v4.9.227
}

void pnv_pci_setup_iommu_table(struct iommu_table *tbl,
			       void *tce_mem, u64 tce_size,
			       u64 dma_offset, unsigned page_shift)
{
	tbl->it_blocksize = 16;
	tbl->it_base = (unsigned long)tce_mem;
	tbl->it_page_shift = page_shift;
	tbl->it_offset = dma_offset >> tbl->it_page_shift;
	tbl->it_index = 0;
	tbl->it_size = tce_size >> 3;
	tbl->it_busno = 0;
	tbl->it_type = TCE_PCI;
}

<<<<<<< HEAD
static struct iommu_table *pnv_pci_setup_bml_iommu(struct pci_controller *hose)
{
	struct iommu_table *tbl;
	const __be64 *basep, *swinvp;
	const __be32 *sizep;

	basep = of_get_property(hose->dn, "linux,tce-base", NULL);
	sizep = of_get_property(hose->dn, "linux,tce-size", NULL);
	if (basep == NULL || sizep == NULL) {
		pr_err("PCI: %s has missing tce entries !\n",
		       hose->dn->full_name);
		return NULL;
	}
	tbl = kzalloc_node(sizeof(struct iommu_table), GFP_KERNEL, hose->node);
	if (WARN_ON(!tbl))
		return NULL;
	pnv_pci_setup_iommu_table(tbl, __va(be64_to_cpup(basep)),
				  be32_to_cpup(sizep), 0, IOMMU_PAGE_SHIFT_4K);
	iommu_init_table(tbl, hose->node);
	iommu_register_group(tbl, pci_domain_nr(hose->bus), 0);

	/* Deal with SW invalidated TCEs when needed (BML way) */
	swinvp = of_get_property(hose->dn, "linux,tce-sw-invalidate-info",
				 NULL);
	if (swinvp) {
		tbl->it_busno = be64_to_cpu(swinvp[1]);
		tbl->it_index = (unsigned long)ioremap(be64_to_cpup(swinvp), 8);
		tbl->it_type = TCE_PCI_SWINV_CREATE | TCE_PCI_SWINV_FREE;
	}
	return tbl;
}

static void pnv_pci_dma_fallback_setup(struct pci_controller *hose,
				       struct pci_dev *pdev)
{
	struct device_node *np = pci_bus_to_OF_node(hose->bus);
	struct pci_dn *pdn;

	if (np == NULL)
		return;
	pdn = PCI_DN(np);
	if (!pdn->iommu_table)
		pdn->iommu_table = pnv_pci_setup_bml_iommu(hose);
	if (!pdn->iommu_table)
		return;
	set_iommu_table_base_and_group(&pdev->dev, pdn->iommu_table);
}

static void pnv_pci_dma_dev_setup(struct pci_dev *pdev)
{
	struct pci_controller *hose = pci_bus_to_host(pdev->bus);
	struct pnv_phb *phb = hose->private_data;

	/* If we have no phb structure, try to setup a fallback based on
	 * the device-tree (RTAS PCI for example)
	 */
	if (phb && phb->dma_dev_setup)
		phb->dma_dev_setup(phb, pdev);
	else
		pnv_pci_dma_fallback_setup(hose, pdev);
}

int pnv_pci_dma_set_mask(struct pci_dev *pdev, u64 dma_mask)
{
	struct pci_controller *hose = pci_bus_to_host(pdev->bus);
	struct pnv_phb *phb = hose->private_data;

	if (phb && phb->dma_set_mask)
		return phb->dma_set_mask(phb, pdev, dma_mask);
	return __dma_set_mask(&pdev->dev, dma_mask);
}

u64 pnv_pci_dma_get_required_mask(struct pci_dev *pdev)
{
	struct pci_controller *hose = pci_bus_to_host(pdev->bus);
	struct pnv_phb *phb = hose->private_data;

	if (phb && phb->dma_get_required_mask)
		return phb->dma_get_required_mask(phb, pdev);

	return __dma_get_required_mask(&pdev->dev);
=======
void pnv_pci_dma_dev_setup(struct pci_dev *pdev)
{
	struct pci_controller *hose = pci_bus_to_host(pdev->bus);
	struct pnv_phb *phb = hose->private_data;
#ifdef CONFIG_PCI_IOV
	struct pnv_ioda_pe *pe;

	/* Fix the VF pdn PE number */
	if (pdev->is_virtfn) {
		list_for_each_entry(pe, &phb->ioda.pe_list, list) {
			if (pe->rid == ((pdev->bus->number << 8) |
			    (pdev->devfn & 0xff))) {
				pe->pdev = pdev;
				break;
			}
		}
	}
#endif /* CONFIG_PCI_IOV */

	if (phb && phb->dma_dev_setup)
		phb->dma_dev_setup(phb, pdev);
}

void pnv_pci_dma_bus_setup(struct pci_bus *bus)
{
	struct pci_controller *hose = bus->sysdata;
	struct pnv_phb *phb = hose->private_data;
	struct pnv_ioda_pe *pe;

	list_for_each_entry(pe, &phb->ioda.pe_list, list) {
		if (!(pe->flags & (PNV_IODA_PE_BUS | PNV_IODA_PE_BUS_ALL)))
			continue;

		if (!pe->pbus)
			continue;

		if (bus->number == ((pe->rid >> 8) & 0xFF)) {
			pe->pbus = bus;
			break;
		}
	}
>>>>>>> v4.9.227
}

void pnv_pci_shutdown(void)
{
	struct pci_controller *hose;

<<<<<<< HEAD
	list_for_each_entry(hose, &hose_list, list_node) {
		struct pnv_phb *phb = hose->private_data;

		if (phb && phb->shutdown)
			phb->shutdown(phb);
	}
=======
	list_for_each_entry(hose, &hose_list, list_node)
		if (hose->controller_ops.shutdown)
			hose->controller_ops.shutdown(hose);
>>>>>>> v4.9.227
}

/* Fixup wrong class code in p7ioc and p8 root complex */
static void pnv_p7ioc_rc_quirk(struct pci_dev *dev)
{
	dev->class = PCI_CLASS_BRIDGE_PCI << 8;
}
DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_IBM, 0x3b9, pnv_p7ioc_rc_quirk);

<<<<<<< HEAD
static int pnv_pci_probe_mode(struct pci_bus *bus)
{
	struct pci_controller *hose = pci_bus_to_host(bus);
	const __be64 *tstamp;
	u64 now, target;


	/* We hijack this as a way to ensure we have waited long
	 * enough since the reset was lifted on the PCI bus
	 */
	if (bus != hose->bus)
		return PCI_PROBE_NORMAL;
	tstamp = of_get_property(hose->dn, "reset-clear-timestamp", NULL);
	if (!tstamp || !*tstamp)
		return PCI_PROBE_NORMAL;

	now = mftb() / tb_ticks_per_usec;
	target = (be64_to_cpup(tstamp) / tb_ticks_per_usec)
		+ PCI_RESET_DELAY_US;

	pr_devel("pci %04d: Reset target: 0x%llx now: 0x%llx\n",
		 hose->global_number, target, now);

	if (now < target)
		msleep((target - now + 999) / 1000);

	return PCI_PROBE_NORMAL;
}

=======
>>>>>>> v4.9.227
void __init pnv_pci_init(void)
{
	struct device_node *np;

	pci_add_flags(PCI_CAN_SKIP_ISA_ALIGN);

<<<<<<< HEAD
	/* OPAL absent, try POPAL first then RTAS detection of PHBs */
	if (!firmware_has_feature(FW_FEATURE_OPAL)) {
#ifdef CONFIG_PPC_POWERNV_RTAS
		init_pci_config_tokens();
		find_and_init_phbs();
#endif /* CONFIG_PPC_POWERNV_RTAS */
	}
	/* OPAL is here, do our normal stuff */
	else {
		int found_ioda = 0;

		/* Look for IODA IO-Hubs. We don't support mixing IODA
		 * and p5ioc2 due to the need to change some global
		 * probing flags
		 */
		for_each_compatible_node(np, NULL, "ibm,ioda-hub") {
			pnv_pci_init_ioda_hub(np);
			found_ioda = 1;
		}

		/* Look for p5ioc2 IO-Hubs */
		if (!found_ioda)
			for_each_compatible_node(np, NULL, "ibm,p5ioc2")
				pnv_pci_init_p5ioc2_hub(np);

		/* Look for ioda2 built-in PHB3's */
		for_each_compatible_node(np, NULL, "ibm,ioda2-phb")
			pnv_pci_init_ioda2_phb(np);
	}

	/* Setup the linkage between OF nodes and PHBs */
	pci_devs_phb_init();

	/* Configure IOMMU DMA hooks */
	ppc_md.pci_dma_dev_setup = pnv_pci_dma_dev_setup;
	ppc_md.tce_build = pnv_tce_build_vm;
	ppc_md.tce_free = pnv_tce_free_vm;
	ppc_md.tce_build_rm = pnv_tce_build_rm;
	ppc_md.tce_free_rm = pnv_tce_free_rm;
	ppc_md.tce_get = pnv_tce_get;
	ppc_md.pci_probe_mode = pnv_pci_probe_mode;
	set_pci_dma_ops(&dma_iommu_ops);

	/* Configure MSIs */
#ifdef CONFIG_PCI_MSI
	ppc_md.setup_msi_irqs = pnv_setup_msi_irqs;
	ppc_md.teardown_msi_irqs = pnv_teardown_msi_irqs;
#endif
}

static int tce_iommu_bus_notifier(struct notifier_block *nb,
		unsigned long action, void *data)
{
	struct device *dev = data;

	switch (action) {
	case BUS_NOTIFY_ADD_DEVICE:
		return iommu_add_device(dev);
	case BUS_NOTIFY_DEL_DEVICE:
		if (dev->iommu_group)
			iommu_del_device(dev);
		return 0;
	default:
		return 0;
	}
}

static struct notifier_block tce_iommu_bus_nb = {
	.notifier_call = tce_iommu_bus_notifier,
};

static int __init tce_iommu_bus_notifier_init(void)
{
	bus_register_notifier(&pci_bus_type, &tce_iommu_bus_nb);
	return 0;
}
=======
	/* If we don't have OPAL, eg. in sim, just skip PCI probe */
	if (!firmware_has_feature(FW_FEATURE_OPAL))
		return;

#ifdef CONFIG_PCIEPORTBUS
	/*
	 * On PowerNV PCIe devices are (currently) managed in cooperation
	 * with firmware. This isn't *strictly* required, but there's enough
	 * assumptions baked into both firmware and the platform code that
	 * it's unwise to allow the portbus services to be used.
	 *
	 * We need to fix this eventually, but for now set this flag to disable
	 * the portbus driver. The AER service isn't required since that AER
	 * events are handled via EEH. The pciehp hotplug driver can't work
	 * without kernel changes (and portbus binding breaks pnv_php). The
	 * other services also require some thinking about how we're going
	 * to integrate them.
	 */
	pcie_ports_disabled = true;
#endif

	/* Look for IODA IO-Hubs. */
	for_each_compatible_node(np, NULL, "ibm,ioda-hub") {
		pnv_pci_init_ioda_hub(np);
	}

	/* Look for ioda2 built-in PHB3's */
	for_each_compatible_node(np, NULL, "ibm,ioda2-phb")
		pnv_pci_init_ioda2_phb(np);

	/* Look for ioda3 built-in PHB4's, we treat them as IODA2 */
	for_each_compatible_node(np, NULL, "ibm,ioda3-phb")
		pnv_pci_init_ioda2_phb(np);

	/* Look for NPU PHBs */
	for_each_compatible_node(np, NULL, "ibm,ioda2-npu-phb")
		pnv_pci_init_npu_phb(np);

	/* Configure IOMMU DMA hooks */
	set_pci_dma_ops(&dma_iommu_ops);
}

>>>>>>> v4.9.227
machine_subsys_initcall_sync(powernv, tce_iommu_bus_notifier_init);
