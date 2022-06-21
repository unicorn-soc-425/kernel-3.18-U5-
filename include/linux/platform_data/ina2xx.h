/*
 * Driver for Texas Instruments INA219, INA226 power monitor chips
 *
<<<<<<< HEAD
 * Copyright (C) 2012 Lothar Felten <l-felten@ti.com>
=======
 * Copyright (C) 2012 Lothar Felten <lothar.felten@gmail.com>
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * For further information, see the Documentation/hwmon/ina2xx file.
 */

/**
 * struct ina2xx_platform_data - ina2xx info
 * @shunt_uohms		shunt resistance in microohms
 */
struct ina2xx_platform_data {
	long shunt_uohms;
};
