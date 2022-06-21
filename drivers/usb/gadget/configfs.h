#ifndef USB__GADGET__CONFIGFS__H
#define USB__GADGET__CONFIGFS__H

#include <linux/configfs.h>

void unregister_gadget_item(struct config_item *item);

<<<<<<< HEAD
int usb_os_desc_prepare_interf_dir(struct config_group *parent,
				   int n_interf,
				   struct usb_os_desc **desc,
				   char **names,
				   struct module *owner);
=======
struct config_group *usb_os_desc_prepare_interf_dir(
		struct config_group *parent,
		int n_interf,
		struct usb_os_desc **desc,
		char **names,
		struct module *owner);
>>>>>>> v4.9.227

static inline struct usb_os_desc *to_usb_os_desc(struct config_item *item)
{
	return container_of(to_config_group(item), struct usb_os_desc, group);
}

#endif /*  USB__GADGET__CONFIGFS__H */
