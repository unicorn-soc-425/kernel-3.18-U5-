#ifndef __LOCAL_MEI_PHY_H_
#define __LOCAL_MEI_PHY_H_

#include <linux/mei_cl_bus.h>
#include <net/nfc/hci.h>
<<<<<<< HEAD

#define MEI_NFC_HEADER_SIZE 10
#define MEI_NFC_MAX_HCI_PAYLOAD 300

struct nfc_mei_phy {
	struct mei_cl_device *device;
	struct nfc_hci_dev *hdev;

	int powered;

	int hard_fault;		/*
				 * < 0 if hardware error occured
				 * and prevents normal operation.
				 */
=======
#include <linux/uuid.h>

#define MEI_NFC_UUID UUID_LE(0x0bb17a78, 0x2a8e, 0x4c50, \
		0x94, 0xd4, 0x50, 0x26, 0x67, 0x23, 0x77, 0x5c)
#define MEI_NFC_HEADER_SIZE 10
#define MEI_NFC_MAX_HCI_PAYLOAD 300

/**
 * struct nfc_mei_phy
 *
 * @cldev: mei client device
 * @hdev:   nfc hci device

 * @send_wq: send completion wait queue
 * @fw_ivn: NFC Interface Version Number
 * @vendor_id: NFC manufacturer ID
 * @radio_type: NFC radio type
 * @reserved: reserved for alignment
 * @req_id:  message counter
 * @recv_req_id: reception message counter
 * @powered: the device is in powered state
 * @hard_fault: < 0 if hardware error occurred
 *    and prevents normal operation.
 */
struct nfc_mei_phy {
	struct mei_cl_device *cldev;
	struct nfc_hci_dev *hdev;

	wait_queue_head_t send_wq;
	u8 fw_ivn;
	u8 vendor_id;
	u8 radio_type;
	u8 reserved;

	u16 req_id;
	u16 recv_req_id;

	int powered;
	int hard_fault;
>>>>>>> v4.9.227
};

extern struct nfc_phy_ops mei_phy_ops;

<<<<<<< HEAD
int nfc_mei_phy_enable(void *phy_id);
void nfc_mei_phy_disable(void *phy_id);
void nfc_mei_event_cb(struct mei_cl_device *device, u32 events, void *context);
=======
>>>>>>> v4.9.227
struct nfc_mei_phy *nfc_mei_phy_alloc(struct mei_cl_device *device);
void nfc_mei_phy_free(struct nfc_mei_phy *phy);

#endif /* __LOCAL_MEI_PHY_H_ */
