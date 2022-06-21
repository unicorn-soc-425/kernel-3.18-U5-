#ifndef __BEN_VLAN_PROC_INC__
#define __BEN_VLAN_PROC_INC__

#ifdef CONFIG_PROC_FS
struct net;

int vlan_proc_init(struct net *net);
<<<<<<< HEAD
int vlan_proc_rem_dev(struct net_device *vlandev);
=======
void vlan_proc_rem_dev(struct net_device *vlandev);
>>>>>>> v4.9.227
int vlan_proc_add_dev(struct net_device *vlandev);
void vlan_proc_cleanup(struct net *net);

#else /* No CONFIG_PROC_FS */

#define vlan_proc_init(net)	(0)
#define vlan_proc_cleanup(net)	do {} while (0)
#define vlan_proc_add_dev(dev)	({(void)(dev), 0; })
<<<<<<< HEAD
#define vlan_proc_rem_dev(dev)	({(void)(dev), 0; })
=======
#define vlan_proc_rem_dev(dev)	do {} while (0)
>>>>>>> v4.9.227
#endif

#endif /* !(__BEN_VLAN_PROC_INC__) */
