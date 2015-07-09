/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#include "pin_muxing_962.h"
#include "../common/proto_seco.h"

#define MX6_GPIO_BOOT_VALIDATE    IMX_GPIO_NR(2, 4)




/* I2C1 - Embedded Controller */
struct i2c_pads_info i2c_pad_info0 = {
	.scl = {
		.i2c_mode  = MX6_PAD_EIM_D21__I2C1_SCL   | PC,
		.gpio_mode = MX6_PAD_EIM_D21__GPIO3_IO21 | PC,
		.gp        = IMX_GPIO_NR(3, 21),
	},
	.sda = {
		.i2c_mode  = MX6_PAD_EIM_D28__I2C1_SDA   | PC,
		.gpio_mode = MX6_PAD_EIM_D28__GPIO3_IO28 | PC,
		.gp        = IMX_GPIO_NR(3, 28),
	},
};


/* I2C2 - PMIC */
struct i2c_pads_info i2c_pad_info1 = {
	.scl = {
		.i2c_mode  = MX6_PAD_KEY_COL3__I2C2_SCL   | PC,
		.gpio_mode = MX6_PAD_KEY_COL3__GPIO4_IO12 | PC,
		.gp        = IMX_GPIO_NR(4, 12),
	},
	.sda = {
		.i2c_mode  = MX6_PAD_KEY_ROW3__I2C2_SDA   | PC,
		.gpio_mode = MX6_PAD_KEY_ROW3__GPIO4_IO13 | PC,
		.gp        = IMX_GPIO_NR(4, 13),
	},
};


/* I2C2 - Q7 connector */
struct i2c_pads_info i2c_pad_info2 = {
	.scl = {
		.i2c_mode  = MX6_PAD_GPIO_5__I2C3_SCL   | PC,
		.gpio_mode = MX6_PAD_GPIO_5__GPIO1_IO05 | PC,
		.gp        = IMX_GPIO_NR(1, 5),
	},
	.sda = {
		.i2c_mode  = MX6_PAD_GPIO_16__I2C3_SDA   | PC,
		.gpio_mode = MX6_PAD_GPIO_16__GPIO7_IO11 | PC,
		.gp        = IMX_GPIO_NR(7, 11),
	},
};




//void ldo_mode_set(int ldo_bypass) {} 




int dram_init (void) {
	gd->ram_size = ((ulong)CONFIG_DDR_MB * 1024 * 1024);

	return 0;
}





/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   UART                                   |
 * |__________________________________________________________________________|
 */

static void setup_iomux_uart (void) {
	imx_iomux_v3_setup_multiple_pads(uart_debug_pads, ARRAY_SIZE(uart_debug_pads));
}
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */


/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   SPI                                    |
 * |__________________________________________________________________________|
 */

#ifdef CONFIG_SYS_USE_SPINOR

static void setup_spinor(void) {
	imx_iomux_v3_setup_multiple_pads(ecspi1_pads, ARRAY_SIZE(ecspi1_pads));
	gpio_direction_output(IMX_GPIO_NR(3, 19), 0);
}

#endif   /*  CONFIG_SYS_USE_SPINOR  */
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */






/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   ETHERNET                               |
 * |__________________________________________________________________________|
 */
inline void enable_ethernet_rail (void) {
}
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */



/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   USDHC                                  |
 * |__________________________________________________________________________|
 */
#ifdef CONFIG_FSL_ESDHC


#define USDHC4_CD_GPIO	IMX_GPIO_NR(2, 6)

boot_mem_dev_t boot_mem_dev_list[SECO_NUM_BOOT_DEV] = {
	{ 0x3, 0x3, -1,  -1,  0, "SPI Nor" },
	{ 0x4, 0x5, -1,   0, -1, "External SD" },
	{ 0x6, 0x7, -1,  -1, -1, "eMMC" },
};


struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM] = {
	{USDHC3_BASE_ADDR, 0, 8},
	{USDHC4_BASE_ADDR, 0, 4},
};

struct usdhc_l usdhc_list[CONFIG_SYS_FSL_USDHC_NUM] = {
	{usdhc3_pads, ARRAY_SIZE(usdhc3_pads), -1},
	{usdhc4_pads, ARRAY_SIZE(usdhc4_pads), USDHC4_CD_GPIO},
};


enum mxc_clock usdhc_clk[CONFIG_SYS_FSL_USDHC_NUM] = {
	MXC_ESDHC3_CLK,
	MXC_ESDHC4_CLK,
};

/* map the usdhc controller id to the devno given to the board device */
int usdhc_devno[4] = { -1, -1, 0, 1};



int board_mmc_getcd (struct mmc *mmc) {
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
		case USDHC4_BASE_ADDR:
			ret = !gpio_get_value(USDHC4_CD_GPIO);
			break;
		case USDHC3_BASE_ADDR:
			ret = 1; /* eMMC/uSDHC4 is always present */
			break;
	}

	return ret;
}

int check_mmc_autodetect (void) {
	char *autodetect_str = getenv ("mmcautodetect");

	if ( (autodetect_str != NULL) &&
		(strcmp(autodetect_str, "yes") == 0) ) {
		return 1;
	}

	return 0;
}
#endif  /*  CONFIG_FSL_ESDHC  */

/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */



/*  __________________________________________________________________________
 * |                                                                          |
 * |                                    USB                                   |
 * |__________________________________________________________________________|
 */
#ifdef CONFIG_USB_EHCI_MX6

#define USB_HOST1_PWR     IMX_GPIO_NR(7, 12)
#define USB_OTG_PWR       IMX_GPIO_NR(3, 22)


int board_ehci_hcd_init (int port) {
	switch (port) {
	case 0:
		imx_iomux_v3_setup_multiple_pads(usb_otg_pads,
			ARRAY_SIZE(usb_otg_pads));

		/*set daisy chain for otg_pin_id on 6q. for 6dl, this bit is reserved*/
		mxc_iomux_set_gpr_register(1, 13, 1, 0);
		break;
	case 1:
		imx_iomux_v3_setup_multiple_pads(usb_hc1_pads,
			ARRAY_SIZE(usb_hc1_pads));
		break;
	default:
		printf("MXC USB port %d not yet supported\n", port);
		return 1;
	}
	return 0;
}


int board_ehci_power (int port, int on) {
	switch (port) {
	case 0:
		if (on)
			gpio_direction_output(USB_OTG_PWR, 1);
		else
			gpio_direction_output(USB_OTG_PWR, 0);
		break;
	case 1:
		if (on)
			gpio_direction_output(USB_HOST1_PWR, 1);
		else
			gpio_direction_output(USB_HOST1_PWR, 0);
		break;
	default:
		printf("MXC USB port %d not yet supported\n", port);
		return 1;
	}
	return 0;
}

#endif  /*  CONFIG_USB_EHCI_MX6  */
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */


/*  __________________________________________________________________________
 * |                                                                          |
 * |                                 DISPLAY INFO                             |
 * |__________________________________________________________________________|
 */

#ifdef CONFIG_SECO_BOARD_NAME
char *board_name = CONFIG_SECO_BOARD_NAME;
#else
char *board_name = "Seco uQ7-i.MX6 (uQ7) - 962";
#endif


int board_init (void) {
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	boot_validate (MX6_GPIO_BOOT_VALIDATE, gpio_boot_validate_pad);

	print_boot_device ();

	return 0;
}
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */



/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console (void) {
	return 1;
}



int board_early_init_f (void) {
	setup_iomux_uart();
#if defined(CONFIG_VIDEO_IPUV3)
	setup_display();
#endif

#ifdef CONFIG_SYS_USE_SPINOR
	setup_spinor();
#endif

#ifdef CONFIG_CMD_SATA
	setup_sata();
#endif

	return 0;
}




#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"sd3",	 MAKE_CFGVAL(0x40, 0x30, 0x00, 0x00)},
	/* 8 bit bus width */
	{"emmc", MAKE_CFGVAL(0x60, 0x58, 0x00, 0x00)},
	{NULL,	 0},
};
#endif



int board_late_init (void) {
	int ret = 0;
#ifdef CONFIG_SYS_I2C_MXC
	int s, i;
	unsigned char *boardrev = NULL;
#endif

#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

#ifdef CONFIG_SYS_I2C_MXC
	s = get_seco_board_revision(&i2c_pad_info0, boardrev);
	for ( i = 0 ; i < s ; i++ )
		gd->bd->board_revision[i] = boardrev[i];
	printf ("dav 0-1  %#X   %#X\n", gd->bd->board_revision[0], gd->bd->board_revision[1]);
	printf ("dav 2-3  %#X   %#X\n", gd->bd->board_revision[2], gd->bd->board_revision[3]);
#endif

#ifdef CONFIG_PFUZE100_PMIC_I2C
	ret = setup_pmic_voltages(&i2c_pad_info1);
	if (ret)
		return -1;
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_env_init();
#endif

	return ret;
}







iomux_v3_cfg_t const pcie_pads[] = {
	MX6_PAD_EIM_D19__GPIO3_IO19 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* POWER */
	MX6_PAD_GPIO_17__GPIO7_IO12 | MUX_PAD_CTRL(NO_PAD_CTRL),	/* RESET */
};

static void setup_pcie(void)
{
	imx_iomux_v3_setup_multiple_pads(pcie_pads, ARRAY_SIZE(pcie_pads));
}


