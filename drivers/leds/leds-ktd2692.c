/*
<<<<<<< HEAD
 * LED driver - leds-ktd2692.c
 *
 * Copyright (C) 2013 Sunggeun Yim <sunggeun.yim@samsung.com>
=======
 * LED driver : leds-ktd2692.c
 *
 * Copyright (C) 2015 Samsung Electronics
 * Ingi Kim <ingi2.kim@samsung.com>
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
<<<<<<< HEAD
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/pwm.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <linux/of_gpio.h>
#include <linux/leds-ktd2692.h>
#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_gpio.h>
#endif

extern struct class *camera_class; /*sys/class/camera*/
#if defined(CONFIG_LEDS_KTD2692_FOR_FRONT)
extern struct device *flash_dev;
#else
struct device *flash_dev;
#endif
struct ktd2692_platform_data *global_ktd2692data;
struct device *ktd2692_dev;

#if defined(CONFIG_ACTIVE_FLASH)
// Tuning factor
#if defined(CONFIG_SEC_GTA2SLTE_PROJECT)||defined(CONFIG_SEC_GTA2SWIFI_PROJECT)
static int torchlevel[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
#else
static int torchlevel[] = {0x00,0x01,0x01,0x02,0x02,0x03,0x04,0x05,0x06,0x08};
#endif
#endif
#if !(defined CONFIG_SEC_J3Y17QLTE_PROJECT || defined  CONFIG_SEC_J2Y18LTE_PROJECT)
int assistive_light = 0;
#endif
void ktd2692_setGpio(int onoff)
{
	if (onoff) {
		__gpio_set_value(global_ktd2692data->flash_control, 1);
	} else {
		__gpio_set_value(global_ktd2692data->flash_control, 0);
	}
}
void ktd2692_set_low_bit(void)
{
	__gpio_set_value(global_ktd2692data->flash_control, 0);
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
	ndelay(T_L_LB*1000);	/* 12ms */
#else
	udelay(T_L_LB);	/* 80ms */
#endif
	__gpio_set_value(global_ktd2692data->flash_control, 1);
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
	ndelay(T_H_LB*1000);	/* 4ms */
#else
	udelay(T_H_LB);	/* 5ms */
#endif
}
void ktd2692_set_high_bit(void)
{
	__gpio_set_value(global_ktd2692data->flash_control, 0);
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
	ndelay(T_L_HB*1000);	/* 4ms */
#else
	udelay(T_L_HB);	/* 5ms */
#endif
	__gpio_set_value(global_ktd2692data->flash_control, 1);
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
	ndelay(T_H_HB*1000);	/* 12ms */
#else
	udelay(T_H_HB);	/* 80ms */
#endif
}
static int ktd2692_set_bit(unsigned int bit)
{
	if (bit) {
		ktd2692_set_high_bit();
	} else {
		ktd2692_set_low_bit();
	}
	return 0;
}
int ktd2692_write_data(unsigned data)
{
	int err = 0;
	unsigned int bit = 0;
	/* Data Start Condition */
	__gpio_set_value(global_ktd2692data->flash_control, 1);
	ndelay(T_SOD*1000); //15us
	/* BIT 7*/
	bit = ((data>> 7) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 6 */
	bit = ((data>> 6) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 5*/
	bit = ((data>> 5) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 4 */
	bit = ((data>> 4) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 3*/
	bit = ((data>> 3) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 2 */
	bit = ((data>> 2) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 1*/
	bit = ((data>> 1) & 0x01);
	ktd2692_set_bit(bit);
	/* BIT 0 */
	bit = ((data>> 0) & 0x01);
	ktd2692_set_bit(bit);
	 __gpio_set_value(global_ktd2692data->flash_control, 0);
	ndelay(T_EOD_L*1000); //4us
	/* Data End Condition */
	__gpio_set_value(global_ktd2692data->flash_control, 1);
	udelay(T_EOD_H);

	return err;
}

EXPORT_SYMBOL_GPL(ktd2692_write_data);
void ktd2692_flash_on(unsigned data)
{
	int ret;
	unsigned long flags = 0;
	struct pinctrl *pinctrl;
	if(data == 0){
		ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
		if (ret) {
			printk("Failed to requeset ktd2692_led_control\n");
		} else {
			printk("<ktd2692_flash_on> KTD2692-TORCH OFF. : E(%d)\n", data);
			global_ktd2692data->mode_status = KTD2692_DISABLES_MOVIE_FLASH_MODE;
			spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
			ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
			spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
			ktd2692_setGpio(0);
			gpio_free(global_ktd2692data->flash_control);
			printk("<ktd2692_flash_on> KTD2692-TORCH OFF. : X(%d)\n", data);
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
			pinctrl = devm_pinctrl_get_select(ktd2692_dev, "front_fled_sleep");
			if (IS_ERR(pinctrl))
				pr_err("%s: flash %s pins are not configured\n", __func__, "front_fled_sleep");
#else
			pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_sleep");
			if (IS_ERR(pinctrl))
				pr_err("%s: flash %s pins are not configured\n", __func__, "fled_sleep");
#endif
		}
   }else{
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "front_fled_default");
		if (IS_ERR(pinctrl))
			pr_err("%s: flash %s pins are not configured\n", __func__, "front_fled_default");
#else
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_default");
		if (IS_ERR(pinctrl))
			pr_err("%s: flash %s pins are not configured\n", __func__, "fled_default");
#endif
		ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
		if (ret) {
			printk("Failed to requeset ktd2692_led_control\n");
		} else {
			printk("<ktd2692_flash_on> KTD2692-TORCH ON. : E(%d)\n", data);
			global_ktd2692data->mode_status = KTD2692_ENABLE_MOVIE_MODE;
			spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
			ktd2692_write_data(global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING);
			#if 0	/* use the internel defualt setting */
				ktd2692_write_data(global_ktd2692data->flash_timeout|KTD2692_ADDR_FLASH_TIMEOUT_SETTING);
			#endif
			ktd2692_write_data(global_ktd2692data->movie_current_value|KTD2692_ADDR_MOVIE_CURRENT_SETTING);
			ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
			spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
			gpio_free(global_ktd2692data->flash_control);
			printk("<ktd2692_flash_on> KTD2692-TORCH ON. : X(%d)\n", data);
		}
	}
}
#if !(defined CONFIG_SEC_J3Y17QLTE_PROJECT)
int ktd2692_fled_led_off(unsigned char index)
{
	int ret;
	unsigned long flags = 0;
	struct pinctrl *pinctrl;
	if(index == 0){
		ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
		if (ret) {
			printk("Failed to requeset ktd2692_led_control\n");
		} else {
			printk("<ktd2692_fled_led_off> KTD2692-FLASH OFF: E(%d)\n", index);
			global_ktd2692data->mode_status = KTD2692_DISABLES_MOVIE_FLASH_MODE;
			spin_lock_irqsave(&global_ktd2692data->int_lock, flags);

			ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
			spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
			ktd2692_setGpio(0);
			gpio_free(global_ktd2692data->flash_control);
			printk("<ktd2692_fled_led_off>KTD2692-FLASH OFF OFF: X(%d)\n", index);
			pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_sleep");
			if (IS_ERR(pinctrl))
				pr_err("%s: flash %s pins are not configured\n", __func__, "fled_sleep");
		}
	}
	return 0;
}
EXPORT_SYMBOL(ktd2692_fled_led_off);

int msm_fled_led_off_ktd2692(ext_pmic_flash_ctrl_t *flash_ctrl)
{
    int ret = 0;
#if !(defined  CONFIG_SEC_J2Y18LTE_PROJECT)
    if(!assistive_light)
      ret = ktd2692_fled_led_off(flash_ctrl->index);
#endif
    return ret;
}
EXPORT_SYMBOL(msm_fled_led_off_ktd2692);

int ktd2692_fled_torch_on(unsigned char index)
{
	int ret;
	unsigned long flags = 0;
	struct pinctrl *pinctrl;	
	pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_default");
	if (IS_ERR(pinctrl))
		pr_err("%s: flash %s pins are not configured\n", __func__, "fled_default");

	ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
	if (ret) {
		pr_err("Failed to requeset ktd2692_led_control\n");
	} else {
		pr_err("<ktd2692_flash_on> KTD2692-TORCH ON. : E(%d)\n", index);
		global_ktd2692data->mode_status = KTD2692_ENABLE_MOVIE_MODE;
		spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
		ktd2692_write_data(global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING);
		ktd2692_write_data(global_ktd2692data->movie_current_value|KTD2692_ADDR_MOVIE_CURRENT_SETTING);
		ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
		spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
		gpio_free(global_ktd2692data->flash_control);
                /*pr_err("LVP: %x, current %x, mode %x",global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING, 
                   global_ktd2692data->movie_current_value|KTD2692_ADDR_FLASH_CURRENT_SETTING,
                   global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL); */
		pr_err("<ktd2692_flash_on> KTD2692-TORCH ON. : X(%d)\n", index);
	}

	return 0;	
}

EXPORT_SYMBOL(ktd2692_fled_torch_on);

int msm_fled_torch_on_ktd2692(ext_pmic_flash_ctrl_t *flash_ctrl)
{
    int ret = 0;
#if !(defined  CONFIG_SEC_J2Y18LTE_PROJECT)
    if(!assistive_light)
       ret = ktd2692_fled_torch_on(flash_ctrl->index);
#endif
    return ret;
}
EXPORT_SYMBOL(msm_fled_torch_on_ktd2692);

int ktd2692_fled_flash_on(unsigned char index)
{
	int ret;
	unsigned long flags = 0;
	struct pinctrl *pinctrl;
	pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_default");
	if (IS_ERR(pinctrl))
		pr_err("%s: flash %s pins are not configured\n", __func__, "fled_default");

	ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
	if (ret) {
		pr_err("Failed to requeset ktd2692_led_control\n");
	} else {
		pr_err("<ktd2692_flash_on> KTD2692-FLASH ON. : E(%d)\n", index);
		global_ktd2692data->mode_status = KTD2692_ENABLE_FLASH_MODE;
		spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
		ktd2692_write_data(global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING);
		#if 0	/* use the internel defualt setting */
			ktd2692_write_data(global_ktd2692data->flash_timeout|KTD2692_ADDR_FLASH_TIMEOUT_SETTING);
		#endif
		ktd2692_write_data(global_ktd2692data->flash_current_value|KTD2692_ADDR_FLASH_CURRENT_SETTING);
		ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
		spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
		gpio_free(global_ktd2692data->flash_control);
                /*pr_err("LVP: %x, current %x, mode %x",global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING, 
                  global_ktd2692data->flash_current_value|KTD2692_ADDR_FLASH_CURRENT_SETTING,
                  global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL); */
		pr_err("<ktd2692_flash_on> KTD2692-FLASH ON. : X(%d)\n", index);
	}
	return 0;
}
EXPORT_SYMBOL(ktd2692_fled_flash_on);

int ktd2692_fled_pre_flash_on(unsigned char index, int32_t pre_flash_current_mA)
{
	int ret;
	unsigned long flags = 0;
	struct pinctrl *pinctrl;
	pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_default");
	if (IS_ERR(pinctrl))
		pr_err("%s: flash %s pins are not configured\n", __func__, "fled_default");

	ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
	if (ret) {
		pr_err("Failed to requeset ktd2692_led_control\n");
	} else {
		pr_err("<ktd2692_flash_on> KTD2692-PREFLASH ON. : E(%d)\n", index);
		global_ktd2692data->mode_status = KTD2692_ENABLE_FLASH_MODE;
		spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
		ktd2692_write_data(global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING);

		global_ktd2692data->pre_flash_current_value = pre_flash_current_mA;

		ktd2692_write_data(global_ktd2692data->pre_flash_current_value|KTD2692_ADDR_FLASH_CURRENT_SETTING);
		ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
		spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
                gpio_free(global_ktd2692data->flash_control);
       		pr_err("<ktd2692_flash_on> KTD2692-PREFLASH ON. : X(%d)\n", index);
	}
	return 0;
}

int msm_fled_pre_flash_on_ktd2692(ext_pmic_flash_ctrl_t *flash_ctrl)
{
    int ret = 0;
#if !(defined  CONFIG_SEC_J2Y18LTE_PROJECT)
    if(!assistive_light)
      ret = ktd2692_fled_pre_flash_on(flash_ctrl->index, flash_ctrl->flash_current_mA);
#endif
    return ret;
}
EXPORT_SYMBOL(msm_fled_pre_flash_on_ktd2692);

int msm_fled_flash_on_ktd2692(ext_pmic_flash_ctrl_t *flash_ctrl)
{
    int ret = 0;
#if !(defined  CONFIG_SEC_J2Y18LTE_PROJECT)
    if(!assistive_light)
      ret = ktd2692_fled_flash_on(flash_ctrl->index);
#endif
    return ret;
}
EXPORT_SYMBOL(msm_fled_flash_on_ktd2692);

int ktd2692_fled_flash_on_set_current(unsigned char index, int32_t flash_current_mA)
{
	return 0;
}
EXPORT_SYMBOL(ktd2692_fled_flash_on_set_current);

int msm_fled_flash_on_set_current_ktd2692(ext_pmic_flash_ctrl_t *flash_ctrl)
{
	return ktd2692_fled_flash_on_set_current(flash_ctrl->index, flash_ctrl->flash_current_mA);
}
EXPORT_SYMBOL(msm_fled_flash_on_set_current_ktd2692);
#endif

ssize_t ktd2692_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
#if defined(CONFIG_ACTIVE_FLASH)
	int sel = 0;
#endif
	int value = 0;
	int ret = 0;
	unsigned long flags = 0;
	struct pinctrl *pinctrl;
	if ((buf == NULL) || kstrtouint(buf, 10, &value)) {
		return -1;
	}
    
	global_ktd2692data->sysfs_input_data = value;
	if (value <= 0) {
#if !(defined CONFIG_SEC_J3Y17QLTE_PROJECT || defined  CONFIG_SEC_J2Y18LTE_PROJECT)
		assistive_light = 0;
#endif
		ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
		if (ret) {
			printk("Failed to requeset ktd2692_led_control\n");
		} else {
			pr_err("sysfs KTD2692-TORCH OFF. : E(%d)\n", value);
			global_ktd2692data->mode_status = KTD2692_DISABLES_MOVIE_FLASH_MODE;
			spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
			ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
			spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
			ktd2692_setGpio(0);
			gpio_free(global_ktd2692data->flash_control);
			printk("KTD2692-TORCH OFF. : X(%d)\n", value);
		}
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "front_fled_sleep");
#else
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_sleep");
#endif
		if (IS_ERR(pinctrl))
			pr_err("%s: flash %s pins are not configured\n", __func__, "is");
	} else if((value == 1) || (value == 100) || (value == 200)){
#if !(defined CONFIG_SEC_J3Y17QLTE_PROJECT || defined  CONFIG_SEC_J2Y18LTE_PROJECT)
		if(value == 1)
		   assistive_light = 1;
#endif
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "front_fled_default");
#else
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_default");
#endif
		if (IS_ERR(pinctrl))
			pr_err("%s: flash %s pins are not configured\n", __func__, "host");
		ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
		if (ret) {
			printk("Failed to requeset ktd2692_led_control\n");
		} else {
			printk("KTD2692-TORCH ON. : E(%d)\n", value);
			global_ktd2692data->mode_status = KTD2692_ENABLE_MOVIE_MODE;
			spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
			ktd2692_write_data(global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING);
#if 0	/* use the internel defualt setting */
			ktd2692_write_data(global_ktd2692data->flash_timeout|KTD2692_ADDR_FLASH_TIMEOUT_SETTING);
#endif
			if(value == 1)
				ktd2692_write_data(global_ktd2692data->movie_current_value|KTD2692_ADDR_MOVIE_CURRENT_SETTING);
			else
				ktd2692_write_data(global_ktd2692data->factory_movie_current_value|KTD2692_ADDR_MOVIE_CURRENT_SETTING);
			ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
			spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
			gpio_free(global_ktd2692data->flash_control);
			printk("KTD2692-TORCH FACTORY ON. : X(%d)\n", value);
		}
	}
#if defined(CONFIG_ACTIVE_FLASH)
	else if (value>1000 && value<=1010) {
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "front_fled_default");
#else
		pinctrl = devm_pinctrl_get_select(ktd2692_dev, "fled_default");
#endif
		if (IS_ERR(pinctrl))
			pr_err("%s: flash %s pins are not configured\n", __func__, "host");
		ret = gpio_request(global_ktd2692data->flash_control, "ktd2692_led_control");
		if (ret) {
			printk("Failed to requeset ktd2692_led_control\n");
		} else {
			pr_err("Torch ON-F active\n");
			printk("KTD2692-TORCH ON. : E(%d)\n", value);
			global_ktd2692data->mode_status = KTD2692_ENABLE_MOVIE_MODE;
			spin_lock_irqsave(&global_ktd2692data->int_lock, flags);
			ktd2692_write_data(global_ktd2692data->LVP_Voltage|KTD2692_ADDR_LVP_SETTING);
			sel = torchlevel[value - 1001];
			//global_ktd2692data->factory_movie_current_value = sel;
			ktd2692_write_data(sel|KTD2692_ADDR_MOVIE_CURRENT_SETTING);
			ktd2692_write_data(global_ktd2692data->mode_status|KTD2692_ADDR_MOVIE_FLASHMODE_CONTROL);
			spin_unlock_irqrestore(&global_ktd2692data->int_lock, flags);
			gpio_free(global_ktd2692data->flash_control);
			//assistive_light = true;
			printk("KTD2692-TORCH ON. : X(%d)\n", value);
		}
	}
#endif	
	else{
		printk("KTD2692-TORCH ON. : X(%d)\n", value);
	}
	if ((value <= 0 || value == 1 || value == 100) && !IS_ERR(pinctrl))
		devm_pinctrl_put(pinctrl);

	return count;
}
ssize_t ktd2692_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", global_ktd2692data->sysfs_input_data);
}
#if defined(CONFIG_LEDS_KTD2692_FOR_FRONT)
static DEVICE_ATTR(front_flash, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH,	ktd2692_show, ktd2692_store);
#else
static DEVICE_ATTR(rear_flash, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH, ktd2692_show, ktd2692_store);
#endif

static int ktd2692_parse_dt(struct device *dev,
                                struct ktd2692_platform_data *pdata)
{
	struct device_node *dnode = dev->of_node;
	int ret = 0;
	/* Defulat Value */
	pdata->LVP_Voltage = KTD2692_DISABLE_LVP;
	pdata->flash_timeout = KTD2692_TIMER_1049ms;	/* default */
	pdata->min_current_value = KTD2692_MIN_CURRENT_240mA;
#if defined(CONFIG_SEC_J2Y18LTE_PROJECT) || defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
	pdata->movie_current_value = KTD2692_MOVIE_CURRENT9;
#else
	pdata->movie_current_value = KTD2692_MOVIE_CURRENT6;
#endif
#if defined(CONFIG_SEC_J3Y17QLTE_PROJECT)
	pdata->factory_movie_current_value = KTD2692_MOVIE_CURRENT10;
	pdata->flash_current_value = KTD2692_FLASH_CURRENT16;
#else
	pdata->factory_movie_current_value = KTD2692_MOVIE_CURRENT6;
	pdata->flash_current_value = KTD2692_FLASH_CURRENT15;
	pdata->pre_flash_current_value = KTD2692_FLASH_CURRENT2;
#endif		
	pdata->mode_status = KTD2692_DISABLES_MOVIE_FLASH_MODE;

	/* get gpio */
	pdata->flash_control = of_get_named_gpio(dnode, "flash-en-gpio", 0);
	if (!gpio_is_valid(pdata->flash_control)) {
		dev_err(dev, "failed to get flash_control\n");
		return -1;
	}
	return ret;
}
static int ktd2692_probe(struct platform_device *pdev)
{
	struct ktd2692_platform_data *pdata;
	int ret = 0;
	if (pdev->dev.of_node) {
		pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
		if (!pdata) {
			dev_err(&pdev->dev, "Failed to allocate memory\n");
			return -ENOMEM;
		}
		ret = ktd2692_parse_dt(&pdev->dev, pdata);
		if (ret < 0) {
			return -EFAULT;
		}
	} else {
	pdata = pdev->dev.platform_data;
		if (pdata == NULL) {
			return -EFAULT;
		}
	}
	global_ktd2692data = pdata;
	ktd2692_dev = &pdev->dev;
	printk("KTD2692_LED Probe\n");
#if defined(CONFIG_LEDS_KTD2692_FOR_FRONT)
/*	if (IS_ERR(flash_dev)) {
		printk("Failed to access device(flash)!\n");
	}
*/
	if (IS_ERR_OR_NULL(camera_class)) {
		pr_err("flash_sysfs: error, camera class not exist");
		return -ENODEV;
	}
	
	flash_dev = device_create(camera_class, NULL, 0, NULL, "flash");
	if (IS_ERR(flash_dev)) {
		pr_err("flash_sysfs: failed to create device(flash)\n");
		return -ENODEV;
	}
	if (device_create_file(flash_dev, &dev_attr_front_flash) < 0) {
		printk("failed to create device file, %s\n",
				dev_attr_front_flash.attr.name);
	}
#else
	if (IS_ERR_OR_NULL(camera_class)) {
		pr_err("flash_sysfs: error, camera class not exist");
		return -ENODEV;
	}

	flash_dev = device_create(camera_class, NULL, 0, NULL, "flash");
	if (IS_ERR(flash_dev)) {
		printk("Failed to create device(flash)!\n");
	}
	if (device_create_file(flash_dev, &dev_attr_rear_flash) < 0) {
		printk("failed to create device file, %s\n",
				dev_attr_rear_flash.attr.name);
	}
#endif
	spin_lock_init(&pdata->int_lock);
	return 0;
}
static int ktd2692_remove(struct platform_device *pdev)
{
#if defined(CONFIG_LEDS_KTD2692_FOR_FRONT)
	device_remove_file(flash_dev, &dev_attr_front_flash);
#else
	device_remove_file(flash_dev, &dev_attr_rear_flash);
#endif
	device_destroy(camera_class, 0);
	class_destroy(camera_class);

	return 0;
}
#ifdef CONFIG_OF
static struct of_device_id ktd2692_flash_of_match[] = {
	{ .compatible = "ktd2692",},
	{},
};
MODULE_DEVICE_TABLE(of, ktd2692_flash_of_match);
#endif
static struct platform_driver ktd2692_flash_driver = {
	.probe = ktd2692_probe,
	.remove = ktd2692_remove,
	.driver = {
		.name = "ktd2692-flash",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ktd2692_flash_of_match),
	},
};
module_platform_driver(ktd2692_flash_driver);
/*
static int __init ktd2692_init(void)
{
	return platform_driver_register(&ktd2692_driver);
}
static void __exit ktd2692_exit(void)
{
	platform_driver_unregister(&ktd2692_driver);
}
*/
MODULE_DESCRIPTION("KTD2692 Flash Driver");
MODULE_AUTHOR("Daniel Jeong");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:lm3632-flash");
=======

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/led-class-flash.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

/* Value related the movie mode */
#define KTD2692_MOVIE_MODE_CURRENT_LEVELS	16
#define KTD2692_MM_TO_FL_RATIO(x)		((x) / 3)
#define KTD2962_MM_MIN_CURR_THRESHOLD_SCALE	8

/* Value related the flash mode */
#define KTD2692_FLASH_MODE_TIMEOUT_LEVELS	8
#define KTD2692_FLASH_MODE_TIMEOUT_DISABLE	0
#define KTD2692_FLASH_MODE_CURR_PERCENT(x)	(((x) * 16) / 100)

/* Macro for getting offset of flash timeout */
#define GET_TIMEOUT_OFFSET(timeout, step)	((timeout) / (step))

/* Base register address */
#define KTD2692_REG_LVP_BASE			0x00
#define KTD2692_REG_FLASH_TIMEOUT_BASE		0x20
#define KTD2692_REG_MM_MIN_CURR_THRESHOLD_BASE	0x40
#define KTD2692_REG_MOVIE_CURRENT_BASE		0x60
#define KTD2692_REG_FLASH_CURRENT_BASE		0x80
#define KTD2692_REG_MODE_BASE			0xA0

/* Set bit coding time for expresswire interface */
#define KTD2692_TIME_RESET_US			700
#define KTD2692_TIME_DATA_START_TIME_US		10
#define KTD2692_TIME_HIGH_END_OF_DATA_US	350
#define KTD2692_TIME_LOW_END_OF_DATA_US		10
#define KTD2692_TIME_SHORT_BITSET_US		4
#define KTD2692_TIME_LONG_BITSET_US		12

/* KTD2692 default length of name */
#define KTD2692_NAME_LENGTH			20

enum ktd2692_bitset {
	KTD2692_LOW = 0,
	KTD2692_HIGH,
};

/* Movie / Flash Mode Control */
enum ktd2692_led_mode {
	KTD2692_MODE_DISABLE = 0,	/* default */
	KTD2692_MODE_MOVIE,
	KTD2692_MODE_FLASH,
};

struct ktd2692_led_config_data {
	/* maximum LED current in movie mode */
	u32 movie_max_microamp;
	/* maximum LED current in flash mode */
	u32 flash_max_microamp;
	/* maximum flash timeout */
	u32 flash_max_timeout;
	/* max LED brightness level */
	enum led_brightness max_brightness;
};

struct ktd2692_context {
	/* Related LED Flash class device */
	struct led_classdev_flash fled_cdev;

	/* secures access to the device */
	struct mutex lock;
	struct regulator *regulator;

	struct gpio_desc *aux_gpio;
	struct gpio_desc *ctrl_gpio;

	enum ktd2692_led_mode mode;
	enum led_brightness torch_brightness;
};

static struct ktd2692_context *fled_cdev_to_led(
				struct led_classdev_flash *fled_cdev)
{
	return container_of(fled_cdev, struct ktd2692_context, fled_cdev);
}

static void ktd2692_expresswire_start(struct ktd2692_context *led)
{
	gpiod_direction_output(led->ctrl_gpio, KTD2692_HIGH);
	udelay(KTD2692_TIME_DATA_START_TIME_US);
}

static void ktd2692_expresswire_reset(struct ktd2692_context *led)
{
	gpiod_direction_output(led->ctrl_gpio, KTD2692_LOW);
	udelay(KTD2692_TIME_RESET_US);
}

static void ktd2692_expresswire_end(struct ktd2692_context *led)
{
	gpiod_direction_output(led->ctrl_gpio, KTD2692_LOW);
	udelay(KTD2692_TIME_LOW_END_OF_DATA_US);
	gpiod_direction_output(led->ctrl_gpio, KTD2692_HIGH);
	udelay(KTD2692_TIME_HIGH_END_OF_DATA_US);
}

static void ktd2692_expresswire_set_bit(struct ktd2692_context *led, bool bit)
{
	/*
	 * The Low Bit(0) and High Bit(1) is based on a time detection
	 * algorithm between time low and time high
	 * Time_(L_LB) : Low time of the Low Bit(0)
	 * Time_(H_LB) : High time of the LOW Bit(0)
	 * Time_(L_HB) : Low time of the High Bit(1)
	 * Time_(H_HB) : High time of the High Bit(1)
	 *
	 * It can be simplified to:
	 * Low Bit(0) : 2 * Time_(H_LB) < Time_(L_LB)
	 * High Bit(1) : 2 * Time_(L_HB) < Time_(H_HB)
	 * HIGH  ___           ____    _..     _________    ___
	 *          |_________|    |_..  |____|         |__|
	 * LOW        <L_LB>  <H_LB>     <L_HB>  <H_HB>
	 *          [  Low Bit (0) ]     [  High Bit(1) ]
	 */
	if (bit) {
		gpiod_direction_output(led->ctrl_gpio, KTD2692_LOW);
		udelay(KTD2692_TIME_SHORT_BITSET_US);
		gpiod_direction_output(led->ctrl_gpio, KTD2692_HIGH);
		udelay(KTD2692_TIME_LONG_BITSET_US);
	} else {
		gpiod_direction_output(led->ctrl_gpio, KTD2692_LOW);
		udelay(KTD2692_TIME_LONG_BITSET_US);
		gpiod_direction_output(led->ctrl_gpio, KTD2692_HIGH);
		udelay(KTD2692_TIME_SHORT_BITSET_US);
	}
}

static void ktd2692_expresswire_write(struct ktd2692_context *led, u8 value)
{
	int i;

	ktd2692_expresswire_start(led);
	for (i = 7; i >= 0; i--)
		ktd2692_expresswire_set_bit(led, value & BIT(i));
	ktd2692_expresswire_end(led);
}

static int ktd2692_led_brightness_set(struct led_classdev *led_cdev,
				       enum led_brightness brightness)
{
	struct led_classdev_flash *fled_cdev = lcdev_to_flcdev(led_cdev);
	struct ktd2692_context *led = fled_cdev_to_led(fled_cdev);

	mutex_lock(&led->lock);

	if (brightness == LED_OFF) {
		led->mode = KTD2692_MODE_DISABLE;
		gpiod_direction_output(led->aux_gpio, KTD2692_LOW);
	} else {
		ktd2692_expresswire_write(led, brightness |
					KTD2692_REG_MOVIE_CURRENT_BASE);
		led->mode = KTD2692_MODE_MOVIE;
	}

	ktd2692_expresswire_write(led, led->mode | KTD2692_REG_MODE_BASE);
	mutex_unlock(&led->lock);

	return 0;
}

static int ktd2692_led_flash_strobe_set(struct led_classdev_flash *fled_cdev,
					bool state)
{
	struct ktd2692_context *led = fled_cdev_to_led(fled_cdev);
	struct led_flash_setting *timeout = &fled_cdev->timeout;
	u32 flash_tm_reg;

	mutex_lock(&led->lock);

	if (state) {
		flash_tm_reg = GET_TIMEOUT_OFFSET(timeout->val, timeout->step);
		ktd2692_expresswire_write(led, flash_tm_reg
				| KTD2692_REG_FLASH_TIMEOUT_BASE);

		led->mode = KTD2692_MODE_FLASH;
		gpiod_direction_output(led->aux_gpio, KTD2692_HIGH);
	} else {
		led->mode = KTD2692_MODE_DISABLE;
		gpiod_direction_output(led->aux_gpio, KTD2692_LOW);
	}

	ktd2692_expresswire_write(led, led->mode | KTD2692_REG_MODE_BASE);

	fled_cdev->led_cdev.brightness = LED_OFF;
	led->mode = KTD2692_MODE_DISABLE;

	mutex_unlock(&led->lock);

	return 0;
}

static int ktd2692_led_flash_timeout_set(struct led_classdev_flash *fled_cdev,
					 u32 timeout)
{
	return 0;
}

static void ktd2692_init_movie_current_max(struct ktd2692_led_config_data *cfg)
{
	u32 offset, step;
	u32 movie_current_microamp;

	offset = KTD2692_MOVIE_MODE_CURRENT_LEVELS;
	step = KTD2692_MM_TO_FL_RATIO(cfg->flash_max_microamp)
		/ KTD2692_MOVIE_MODE_CURRENT_LEVELS;

	do {
		movie_current_microamp = step * offset;
		offset--;
	} while ((movie_current_microamp > cfg->movie_max_microamp) &&
		(offset > 0));

	cfg->max_brightness = offset;
}

static void ktd2692_init_flash_timeout(struct led_classdev_flash *fled_cdev,
				       struct ktd2692_led_config_data *cfg)
{
	struct led_flash_setting *setting;

	setting = &fled_cdev->timeout;
	setting->min = KTD2692_FLASH_MODE_TIMEOUT_DISABLE;
	setting->max = cfg->flash_max_timeout;
	setting->step = cfg->flash_max_timeout
			/ (KTD2692_FLASH_MODE_TIMEOUT_LEVELS - 1);
	setting->val = cfg->flash_max_timeout;
}

static void ktd2692_setup(struct ktd2692_context *led)
{
	led->mode = KTD2692_MODE_DISABLE;
	ktd2692_expresswire_reset(led);
	gpiod_direction_output(led->aux_gpio, KTD2692_LOW);

	ktd2692_expresswire_write(led, (KTD2962_MM_MIN_CURR_THRESHOLD_SCALE - 1)
				 | KTD2692_REG_MM_MIN_CURR_THRESHOLD_BASE);
	ktd2692_expresswire_write(led, KTD2692_FLASH_MODE_CURR_PERCENT(45)
				 | KTD2692_REG_FLASH_CURRENT_BASE);
}

static int ktd2692_parse_dt(struct ktd2692_context *led, struct device *dev,
			    struct ktd2692_led_config_data *cfg)
{
	struct device_node *np = dev->of_node;
	struct device_node *child_node;
	int ret;

	if (!dev->of_node)
		return -ENXIO;

	led->ctrl_gpio = devm_gpiod_get(dev, "ctrl", GPIOD_ASIS);
	ret = PTR_ERR_OR_ZERO(led->ctrl_gpio);
	if (ret) {
		dev_err(dev, "cannot get ctrl-gpios %d\n", ret);
		return ret;
	}

	led->aux_gpio = devm_gpiod_get(dev, "aux", GPIOD_ASIS);
	ret = PTR_ERR_OR_ZERO(led->aux_gpio);
	if (ret) {
		dev_err(dev, "cannot get aux-gpios %d\n", ret);
		return ret;
	}

	led->regulator = devm_regulator_get(dev, "vin");
	if (IS_ERR(led->regulator))
		led->regulator = NULL;

	if (led->regulator) {
		ret = regulator_enable(led->regulator);
		if (ret)
			dev_err(dev, "Failed to enable supply: %d\n", ret);
	}

	child_node = of_get_next_available_child(np, NULL);
	if (!child_node) {
		dev_err(dev, "No DT child node found for connected LED.\n");
		return -EINVAL;
	}

	led->fled_cdev.led_cdev.name =
		of_get_property(child_node, "label", NULL) ? : child_node->name;

	ret = of_property_read_u32(child_node, "led-max-microamp",
				   &cfg->movie_max_microamp);
	if (ret) {
		dev_err(dev, "failed to parse led-max-microamp\n");
		goto err_parse_dt;
	}

	ret = of_property_read_u32(child_node, "flash-max-microamp",
				   &cfg->flash_max_microamp);
	if (ret) {
		dev_err(dev, "failed to parse flash-max-microamp\n");
		goto err_parse_dt;
	}

	ret = of_property_read_u32(child_node, "flash-max-timeout-us",
				   &cfg->flash_max_timeout);
	if (ret) {
		dev_err(dev, "failed to parse flash-max-timeout-us\n");
		goto err_parse_dt;
	}

err_parse_dt:
	of_node_put(child_node);
	return ret;
}

static const struct led_flash_ops flash_ops = {
	.strobe_set = ktd2692_led_flash_strobe_set,
	.timeout_set = ktd2692_led_flash_timeout_set,
};

static int ktd2692_probe(struct platform_device *pdev)
{
	struct ktd2692_context *led;
	struct led_classdev *led_cdev;
	struct led_classdev_flash *fled_cdev;
	struct ktd2692_led_config_data led_cfg;
	int ret;

	led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	fled_cdev = &led->fled_cdev;
	led_cdev = &fled_cdev->led_cdev;

	ret = ktd2692_parse_dt(led, &pdev->dev, &led_cfg);
	if (ret)
		return ret;

	ktd2692_init_flash_timeout(fled_cdev, &led_cfg);
	ktd2692_init_movie_current_max(&led_cfg);

	fled_cdev->ops = &flash_ops;

	led_cdev->max_brightness = led_cfg.max_brightness;
	led_cdev->brightness_set_blocking = ktd2692_led_brightness_set;
	led_cdev->flags |= LED_CORE_SUSPENDRESUME | LED_DEV_CAP_FLASH;

	mutex_init(&led->lock);

	platform_set_drvdata(pdev, led);

	ret = led_classdev_flash_register(&pdev->dev, fled_cdev);
	if (ret) {
		dev_err(&pdev->dev, "can't register LED %s\n", led_cdev->name);
		mutex_destroy(&led->lock);
		return ret;
	}

	ktd2692_setup(led);

	return 0;
}

static int ktd2692_remove(struct platform_device *pdev)
{
	struct ktd2692_context *led = platform_get_drvdata(pdev);
	int ret;

	led_classdev_flash_unregister(&led->fled_cdev);

	if (led->regulator) {
		ret = regulator_disable(led->regulator);
		if (ret)
			dev_err(&pdev->dev,
				"Failed to disable supply: %d\n", ret);
	}

	mutex_destroy(&led->lock);

	return 0;
}

static const struct of_device_id ktd2692_match[] = {
	{ .compatible = "kinetic,ktd2692", },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, ktd2692_match);

static struct platform_driver ktd2692_driver = {
	.driver = {
		.name  = "ktd2692",
		.of_match_table = ktd2692_match,
	},
	.probe  = ktd2692_probe,
	.remove = ktd2692_remove,
};

module_platform_driver(ktd2692_driver);

MODULE_AUTHOR("Ingi Kim <ingi2.kim@samsung.com>");
MODULE_DESCRIPTION("Kinetic KTD2692 LED driver");
MODULE_LICENSE("GPL v2");
>>>>>>> v4.9.227
