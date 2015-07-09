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
#include <ipu_pixfmt.h>
#include <micrel.h>
#include <i2c.h>

#include "proto_seco.h"
#include "muxing.h"




#ifdef CONFIG_SYS_I2C_MXC
#ifdef CONFIG_PFUZE100_PMIC_I2C
#define PFUZE100_DEVICEID	0x0
#define PFUZE100_REVID		0x3
#define PFUZE100_FABID		0x4

#define PFUZE100_SW1ABVOL	0x20
#define PFUZE100_SW1ABSTBY	0x21
#define PFUZE100_SW1ABCONF	0x24
#define PFUZE100_SW1CVOL	0x2e
#define PFUZE100_SW1CSTBY	0x2f
#define PFUZE100_SW1CCONF	0x32
#define PFUZE100_SW1ABC_SETP(x)	((x - 3000) / 250)
#define PFUZE100_VGEN5CTL	0x70

/* set all switches APS in normal and PFM mode in standby */
static int setup_pmic_mode(int chip)
{
	unsigned char offset, i, switch_num, value;

	if (!chip) {
		/* pfuze100 */
		switch_num = 6;
		offset = 0x31;
	} else {
		/* pfuze200 */
		switch_num = 4;
		offset = 0x38;
	}

	value = 0xc;
	if (i2c_write(0x8, 0x23, 1, &value, 1)) {
		printf("Set SW1AB mode error!\n");
		return -1;
	}

	for (i = 0; i < switch_num - 1; i++) {
		if (i2c_write(0x8, offset + i * 7, 1, &value, 1)) {
			printf("Set switch%x mode error!\n", offset);
			return -1;
		}
	}

	return 0;
}


int setup_pmic_voltages (struct i2c_pads_info *i2c_pad)
{
	unsigned char value, rev_id = 0;

	setup_i2c (0, CONFIG_SYS_I2C_SPEED, CONFIG_PMIC_I2C_SLAVE, i2c_pad);

	if (!i2c_probe(CONFIG_PMIC_I2C_SLAVE)) {
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_DEVICEID, 1, &value, 1)) {
			printf("Read device ID error!\n");
			return -1;
		}
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_REVID, 1, &rev_id, 1)) {
			printf("Read Rev ID error!\n");
			return -1;
		}
		/*
		 * PFUZE200: Die version 0001 = PF0200
		 * PFUZE100: Die version 0000 = PF0100
		 */
		printf("Found %s! deviceid 0x%x, revid 0x%x\n", (value & 0xf) ?
		       "PFUZE200" : "PFUZE100", value & 0xf, rev_id);

		if (setup_pmic_mode(value & 0xf)) {
			printf("setup pmic mode error!\n");
			return -1;
		}
		/* set SW1AB standby volatage 0.975V */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABSTBY, 1, &value, 1)) {
			printf("Read SW1ABSTBY error!\n");
			return -1;
		}
		value &= ~0x3f;
		value |= PFUZE100_SW1ABC_SETP(9750);
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABSTBY, 1, &value, 1)) {
			printf("Set SW1ABSTBY error!\n");
			return -1;
		}

		/* set SW1AB/VDDARM step ramp up time from 16us to 4us/25mV */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABCONF, 1, &value, 1)) {
			printf("Read SW1ABCONFIG error!\n");
			return -1;
		}
		value &= ~0xc0;
		value |= 0x40;
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABCONF, 1, &value, 1)) {
			printf("Set SW1ABCONFIG error!\n");
			return -1;
		}

		/* set SW1C standby volatage 0.975V */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CSTBY, 1, &value, 1)) {
			printf("Read SW1CSTBY error!\n");
			return -1;
		}
		value &= ~0x3f;
		value |= PFUZE100_SW1ABC_SETP(9750);
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CSTBY, 1, &value, 1)) {
			printf("Set SW1CSTBY error!\n");
			return -1;
		}

		/* set SW1C/VDDSOC step ramp up time to from 16us to 4us/25mV */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CCONF, 1, &value, 1)) {
			printf("Read SW1CCONFIG error!\n");
			return -1;
		}
		value &= ~0xc0;
		value |= 0x40;
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CCONF, 1, &value, 1)) {
			printf("Set SW1CCONFIG error!\n");
			return -1;
		}

		/* Enable power of VGEN5 3V3, needed for SD3 */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_VGEN5CTL, 1, &value, 1)) {
			printf("Read VGEN5CTL error!\n");
			return -1;
		}
		value &= ~0x1F;
		value |= 0x1F;
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_VGEN5CTL, 1, &value, 1)) {
			printf("Set VGEN5CTL error!\n");
			return -1;
		}
	} else {
		printf("PMIC not Found\n");
	}

	return 0;
}



#ifdef CONFIG_LDO_BYPASS_CHECK
void ldo_mode_set(int ldo_bypass)
{
	unsigned char value;
	int is_400M;
	u32 vddarm;
	/* switch to ldo_bypass mode */
	if (ldo_bypass) {
		prep_anatop_bypass();
		/* decrease VDDARM to 1.275V */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABVOL, 1, &value, 1)) {
			printf("Read SW1AB error!\n");
			return;
		}
		value &= ~0x3f;
		value |= PFUZE100_SW1ABC_SETP(12750);
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABVOL, 1, &value, 1)) {
			printf("Set SW1AB error!\n");
			return;
		}
		/* decrease VDDSOC to 1.3V */
		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CVOL, 1, &value, 1)) {
			printf("Read SW1C error!\n");
			return;
		}
		value &= ~0x3f;
		value |= PFUZE100_SW1ABC_SETP(13000);
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CVOL, 1, &value, 1)) {
			printf("Set SW1C error!\n");
			return;
		}

		is_400M = set_anatop_bypass(1);
		if (is_400M)
			vddarm = PFUZE100_SW1ABC_SETP(10750);
		else
			vddarm = PFUZE100_SW1ABC_SETP(11750);

		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABVOL, 1, &value, 1)) {
			printf("Read SW1AB error!\n");
			return;
		}
		value &= ~0x3f;
		value |= vddarm;
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1ABVOL, 1, &value, 1)) {
			printf("Set SW1AB error!\n");
			return;
		}

		if (i2c_read(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CVOL, 1, &value, 1)) {
			printf("Read SW1C error!\n");
			return;
		}
		value &= ~0x3f;
		value |= PFUZE100_SW1ABC_SETP(11750);
		if (i2c_write(CONFIG_PMIC_I2C_SLAVE, PFUZE100_SW1CVOL, 1, &value, 1)) {
			printf("Set SW1C error!\n");
			return;
		}

		finish_anatop_bypass();
		printf("switch to ldo_bypass mode!\n");
	}

}
#endif   /*  CONFIG_LDO_BYPASS_CHECK  */
#endif   /*  CONFIG_PFUZE100_PMIC_I2C  */
#endif   /*  CONFIG_SYS_I2C_MXC  */

