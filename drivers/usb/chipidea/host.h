#ifndef __DRIVERS_USB_CHIPIDEA_HOST_H
#define __DRIVERS_USB_CHIPIDEA_HOST_H

#ifdef CONFIG_USB_CHIPIDEA_HOST

int ci_hdrc_host_init(struct ci_hdrc *ci);
void ci_hdrc_host_destroy(struct ci_hdrc *ci);
<<<<<<< HEAD
=======
void ci_hdrc_host_driver_init(void);
>>>>>>> v4.9.227

#else

static inline int ci_hdrc_host_init(struct ci_hdrc *ci)
{
	return -ENXIO;
}

static inline void ci_hdrc_host_destroy(struct ci_hdrc *ci)
{

}

<<<<<<< HEAD
=======
static void ci_hdrc_host_driver_init(void)
{

}

>>>>>>> v4.9.227
#endif

#endif /* __DRIVERS_USB_CHIPIDEA_HOST_H */
