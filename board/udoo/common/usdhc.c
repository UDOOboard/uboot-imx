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

/*TODO:introduce multi SOC support for this function below to use it as common one. */

/*int board_mmc_init (bd_t *bis) {
	int index, count, status = 0;

	count = ARRAY_SIZE (usdhc_list);

	for ( index = 0 ; index < CONFIG_SYS_FSL_USDHC_NUM ; index++ ) {
	
		if ( index < count ) {
			
			imx_iomux_v3_setup_multiple_pads (usdhc_list[index].pad,
					usdhc_list[index].num);

			usdhc_cfg[index].sdhc_clk = mxc_get_clock (usdhc_clk[index]);

			if ( usdhc_list[index].gpio_cd > 0 ) {
				gpio_direction_input (usdhc_list[index].gpio_cd);
			}

			status != fsl_esdhc_initialize (bis, &usdhc_cfg[index]);

			if ( status )
				printf ("Warning: failed to initialize mmc dev %d\n", index);

		} else {
			
			printf("Warning: you configured more USDHC controllers"
					"(%d) than supported by the board\n", index + 1);

		}

	}

	return status;
}*/


/* FuseMap 
 * See The register BOOT_CFG1[7:3]
 * BootDev      BOOT_CFG1[7]  BOOT_CFG1[6]  BOOT_CFG1[5]  BOOT_CFG1[4]  BOOT_CFG1[3]
 * EIM               0             0             0             0             0  for NOR Flash
 *                                                                           1  for OneNAND
 * SATA              0             0             1             0             X
 * SerialROM         0             0             1             1             X
 * SD/eSD            0             1             0             X             X
 * MMC/eMMC          0             1             1             X             X
 * NAND Flash        1             X             X             X             X
 */
int mmc_get_env_devno (void) {
	u32 soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	u32 dev_no = -1;
	u32 bootsel;

	bootsel = (soc_sbmr & 0x000000FF) >> 6 ;
	if ( bootsel == 1 ) {	// the boot device is an SD/MMC
		/* so we can obtain the usdhc controller */
		/* BOOT_CFG2[3] and BOOT_CFG2[4] */
		dev_no = (soc_sbmr & 0x00001800) >> 11;	
	}

	/* If not boot from sd/mmc, use default value */
	if (bootsel != 1)
		return CONFIG_SYS_MMC_ENV_DEV;
	
	return usdhc_devno[dev_no];
}


int mmc_map_to_kernel_blk (int dev_no) {
	return dev_no + 1;
}


void board_late_mmc_env_init (void) {
	char cmd[32];
	char mmcblk[32];
	u32 dev_no = mmc_get_env_devno();

	if (!check_mmc_autodetect())
		return;

	setenv_ulong("mmcdev", dev_no);

	/* Set mmcblk env */
	sprintf(mmcblk, "/dev/mmcblk%dp2 rootwait rw",
		mmc_map_to_kernel_blk(dev_no));
	setenv("mmcroot", mmcblk);

	sprintf(cmd, "mmc dev %d", dev_no);
	run_command(cmd, 0);
}

