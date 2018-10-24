/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/crm_regs.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/boot_mode.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mxc_hdmi.h>
#include <linux/fb.h>
#include <micrel.h>
#include <i2c.h>


#include "proto_seco.h"






/*  __________________________________________________________________________
 * |                                                                          |
 * |                               BOOT VALIDATE                              |
 * |__________________________________________________________________________|
 */
void boot_validate (int gpio, const iomux_v3_cfg_t *gpio_pad) {

	imx_iomux_v3_setup_multiple_pads (gpio_pad, ARRAY_SIZE(gpio_pad));

	gpio_direction_output (gpio, 0);

        /* Set Low */
	gpio_set_value (gpio, 0);
    	udelay(1000);

        /* Set High */
	gpio_set_value (gpio, 1);
}



/*  __________________________________________________________________________
 * |                                                                          |
 * |                               BOARD REVISION                             |
 * |__________________________________________________________________________|
 */
int scode_value (int seco_code) {
	gpio_direction_input(seco_code);
	return gpio_get_value(seco_code);
}


int get_seco_board_revision (struct i2c_pads_info *i2c_pad, unsigned char *boardrev) {

	boardrev = malloc (4 * sizeof(unsigned char));
	if ( !boardrev )
		return 0;

	setup_i2c(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, i2c_pad);
	/* SECO BOARD REV */
	i2c_probe (0x40);
	if (i2c_read (0x40, 0x0E, 1, boardrev, 2)) {
		printf ("Read Board Revision Failed\n");
	}
	if (i2c_read (0x40, 0x0C, 1, boardrev, 2)) {
		printf ("Read Board Revision Failed\n");
	}	

	return 4;
}



/*  __________________________________________________________________________
 * |                                                                          |
 * |                                 DISPLAY INFO                             |
 * |__________________________________________________________________________|
 */
int checkboard (void) {

	if (is_cpu_type(MXC_CPU_MX6Q))
	   printf("Board: %s Quad/Dual \n", board_name);
	else
	   printf("Board: %s DualLite/Solo \n", board_name);

	return 0;
}



void print_boot_device (void) {

	uint soc_sbmr       = readl(SRC_BASE_ADDR + 0x4);
	uint bt_mem_ctl     = (soc_sbmr & 0x000000F0) >> 4 ;
	uint bt_mem_type    = (soc_sbmr & 0x00000008) >> 3;

	uint bt_usdhc_num   = (soc_sbmr & 0x00001800) >> 11;
	uint bt_port_select = (soc_sbmr & 0x05000000) >> 24;

	int index;

	for ( index = 0 ; index < SECO_NUM_BOOT_DEV ; index++ ) {
	
		if ( bt_mem_ctl >= boot_mem_dev_list[index].mem_ctl_min && 
				bt_mem_ctl <= boot_mem_dev_list[index].mem_ctl_max ) {

			if ( boot_mem_dev_list[index].mem_type != -1 ) 
				if ( boot_mem_dev_list[index].mem_type != bt_mem_type )
					continue;

			if ( boot_mem_dev_list[index].bt_usdhc_num != -1 )
				if ( boot_mem_dev_list[index].bt_usdhc_num != bt_usdhc_num )
					continue;

			if ( boot_mem_dev_list[index].bt_port_select != -1 )
				if ( boot_mem_dev_list[index].bt_port_select != bt_port_select )
					continue;
			break;

		}
	}

	if ( index < SECO_NUM_BOOT_DEV )
		printf ("Boot: %s\n", boot_mem_dev_list[index].label);
	else
		printf ("Boot: Unknow\n"); 


}


