#ifndef __PHY_FIXED_H
#define __PHY_FIXED_H

struct fixed_phy_status {
	int link;
	int speed;
	int duplex;
	int pause;
	int asym_pause;
};

struct device_node;

<<<<<<< HEAD
#ifdef CONFIG_FIXED_PHY
extern int fixed_phy_add(unsigned int irq, int phy_id,
			 struct fixed_phy_status *status);
extern struct phy_device *fixed_phy_register(unsigned int irq,
					     struct fixed_phy_status *status,
					     struct device_node *np);
extern void fixed_phy_del(int phy_addr);
extern int fixed_phy_set_link_update(struct phy_device *phydev,
			int (*link_update)(struct net_device *,
					   struct fixed_phy_status *));
#else
static inline int fixed_phy_add(unsigned int irq, int phy_id,
				struct fixed_phy_status *status)
=======
#if IS_ENABLED(CONFIG_FIXED_PHY)
extern int fixed_phy_add(unsigned int irq, int phy_id,
			 struct fixed_phy_status *status,
			 int link_gpio);
extern struct phy_device *fixed_phy_register(unsigned int irq,
					     struct fixed_phy_status *status,
					     int link_gpio,
					     struct device_node *np);
extern void fixed_phy_unregister(struct phy_device *phydev);
extern int fixed_phy_set_link_update(struct phy_device *phydev,
			int (*link_update)(struct net_device *,
					   struct fixed_phy_status *));
extern int fixed_phy_update_state(struct phy_device *phydev,
			   const struct fixed_phy_status *status,
			   const struct fixed_phy_status *changed);
#else
static inline int fixed_phy_add(unsigned int irq, int phy_id,
				struct fixed_phy_status *status,
				int link_gpio)
>>>>>>> v4.9.227
{
	return -ENODEV;
}
static inline struct phy_device *fixed_phy_register(unsigned int irq,
						struct fixed_phy_status *status,
<<<<<<< HEAD
=======
						int gpio_link,
>>>>>>> v4.9.227
						struct device_node *np)
{
	return ERR_PTR(-ENODEV);
}
<<<<<<< HEAD
static inline int fixed_phy_del(int phy_addr)
{
	return -ENODEV;
=======
static inline void fixed_phy_unregister(struct phy_device *phydev)
{
>>>>>>> v4.9.227
}
static inline int fixed_phy_set_link_update(struct phy_device *phydev,
			int (*link_update)(struct net_device *,
					   struct fixed_phy_status *))
{
	return -ENODEV;
}
<<<<<<< HEAD
=======
static inline int fixed_phy_update_state(struct phy_device *phydev,
			   const struct fixed_phy_status *status,
			   const struct fixed_phy_status *changed)
{
	return -ENODEV;
}
>>>>>>> v4.9.227
#endif /* CONFIG_FIXED_PHY */

#endif /* __PHY_FIXED_H */
