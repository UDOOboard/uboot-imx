/*
 * Copyright (C) UDOO Team
 *
 * Author: Francesco Montefoschi <francesco.monte@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/mx6-pins.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include "detectboard.h"

#define DIO_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_34ohm | PAD_CTL_HYS | PAD_CTL_SRE_FAST)
#define DIO_PAD_CFG   (MUX_PAD_CTRL(DIO_PAD_CTRL) | MUX_MODE_SION)


#define GPIO_R184 IMX_GPIO_NR(4, 13)
#define GPIO_R185 IMX_GPIO_NR(4, 0)
// iomux_v3_cfg_t const board_recognition_pads[] = {
//     MX6_PAD_NAND_READY_B__GPIO4_IO_13 | DIO_PAD_CFG,  // Connected to R184
//     MX6_PAD_NAND_ALE__GPIO4_IO_0 | DIO_PAD_CFG,       // Connected to R185
// };

/**
 * Detects the board model by checking the R184 and R185 resistors.
 * A mounted resistor (0Ohm) connects the GPIO to ground, so the
 * GPIO value will be 0.
 * 
 * FULL     - Eth, WiFi, motion sensors, 1GB RAM         -> R184 not mounted - R185 mounted
 * EXTENDED - NO Eth, WiFi, motion sensors, 1GB RAM      -> R184 not mounted - R185 not mounted
 * BASE     - Eth, NO WiFi, NO motion sensors, 512MB RAM -> R184 mounted     - R185 mounted
 * BASE KS  - NO Eth, WiFi, NO motion sensors, 512MB RAM -> R184 mounted     - R185 not mounted
 */
int detect_board(void)
{
    return A62_MX6QD_4x256MB;
/*
    imx_iomux_v3_setup_multiple_pads(board_recognition_pads,
        ARRAY_SIZE(board_recognition_pads));
    
    gpio_direction_input(GPIO_R184);
    gpio_direction_input(GPIO_R185);
    
    int r184 = gpio_get_value(GPIO_R184);
    int r185 = gpio_get_value(GPIO_R185);
    
    if (r184 == 0 && r185 == 0) {
        return UDOO_NEO_TYPE_BASIC;
    }
    if (r184 == 0 && r185 == 1) {
        return UDOO_NEO_TYPE_BASIC_KS;
    }
    if (r184 == 1 && r185 == 0) {
        return UDOO_NEO_TYPE_FULL;
    }
    if (r184 == 1 && r185 == 1) {
        return UDOO_NEO_TYPE_EXTENDED;
    }
    
    return UDOO_NEO_TYPE_FULL;
*/
}
