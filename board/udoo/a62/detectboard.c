/*
 * Copyright (C) UDOO Team
 *
 * Author: Giuseppe Pagano <giuseppe.pagano@secom.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/mx6-pins.h>
#include <i2c.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include "detectboard.h"

/**
 * Autodetect video output searching for connected I2C touchsreen address
 * 0x55 = LVDS 7''
 * 0x5D = LVDS 10''
 */

enum touchAddr { GOODIX = 0x14, ST1232 = 0x55 };

int addr[2] = { GOODIX, ST1232};

int detect_video(void)
{
        int ret=0;
        struct udevice *bus;

        I2C_SET_BUS(TOUCH_I2C_BUS);

	for (int j = 0; j < (sizeof(addr)/sizeof(addr[0])); j++) {
           ret = i2c_probe(addr[j]);
           if (ret == 0) {
                switch(addr[j]) {
                   case GOODIX:
                        printf("Auto-select LVDS 10'' video output.\n");
                        return GOODIX;
                   case ST1232:
                        printf("Auto-select LVDS 7'' video output.\n");
                        return ST1232;
                }
           }
        }
        return 0;
}

