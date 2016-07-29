/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 * Author: Giuseppe Pagano <giuseppe.pagano@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include "pin_muxing_A62.h"
#include "../common/proto_seco.h"
#include "detectboard.h"


#define MX6_GPIO_USB_RESET    IMX_GPIO_NR(7, 12)


/*  I2C2 - EEPROM, HDMI  */
 static struct i2c_pads_info mx6q_i2c_pad_info1 = {
	.scl = {
		.i2c_mode  = MX6Q_PAD_KEY_COL3__I2C2_SCL   | PC,
		.gpio_mode = MX6Q_PAD_KEY_COL3__GPIO4_IO12 | PC,
		.gp        = IMX_GPIO_NR(4, 12),
	},
	.sda = {
		.i2c_mode  = MX6Q_PAD_KEY_ROW3__I2C2_SDA   | PC,
		.gpio_mode = MX6Q_PAD_KEY_ROW3__GPIO4_IO13 | PC,
		.gp        = IMX_GPIO_NR(4, 13),
	},
};

static struct i2c_pads_info mx6dl_i2c_pad_info1 = {
	.scl = {
		.i2c_mode  = MX6DL_PAD_KEY_COL3__I2C2_SCL   | PC,
		.gpio_mode = MX6DL_PAD_KEY_COL3__GPIO4_IO12 | PC,
		.gp        = IMX_GPIO_NR(4, 12),
	},
	.sda = {
		.i2c_mode  = MX6DL_PAD_KEY_ROW3__I2C2_SDA   | PC,
		.gpio_mode = MX6DL_PAD_KEY_ROW3__GPIO4_IO13 | PC,
		.gp        = IMX_GPIO_NR(4, 13),
	},
};

/*  I2C3 - H29, CN11  */
static struct i2c_pads_info mx6q_i2c_pad_info2 = {
	.scl = {
		.i2c_mode  = MX6Q_PAD_GPIO_5__I2C3_SCL   | PC,
		.gpio_mode = MX6Q_PAD_GPIO_5__GPIO1_IO05 | PC,
		.gp        = IMX_GPIO_NR(1, 5),
	},
	.sda = {
		.i2c_mode  = MX6Q_PAD_GPIO_6__I2C3_SDA   | PC,
		.gpio_mode = MX6Q_PAD_GPIO_6__GPIO1_IO06 | PC,
		.gp        = IMX_GPIO_NR(1, 6),
	},
};

static struct i2c_pads_info mx6dl_i2c_pad_info2 = {
	.scl = {
		.i2c_mode  = MX6DL_PAD_GPIO_5__I2C3_SCL   | PC,
		.gpio_mode = MX6DL_PAD_GPIO_5__GPIO1_IO05 | PC,
		.gp        = IMX_GPIO_NR(1, 5),
	},
	.sda = {
		.i2c_mode  = MX6DL_PAD_GPIO_6__I2C3_SDA   | PC,
		.gpio_mode = MX6DL_PAD_GPIO_6__GPIO1_IO06 | PC,
		.gp        = IMX_GPIO_NR(1, 6),
	},
};

void ldo_mode_set(int ldo_bypass) {} 

int dram_init (void) {

	gd->ram_size = imx_ddr_size();

	return 0;
}

/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   UART                                   |
 * |__________________________________________________________________________|
 */

static void setup_iomux_uart (void) {
	SETUP_IOMUX_PADS(uart_debug_pads);
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
	SETUP_IOMUX_PADS(ecspi1_pads);
	gpio_direction_output(IMX_GPIO_NR(2, 30), 0);
}

int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return (bus == 0 && cs == 0) ? (IMX_GPIO_NR(2, 30)) : cs >> 8;
}

#endif   /*  CONFIG_SYS_USE_SPINOR  */
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */

#ifdef CONFIG_FSL_FASTBOOT

void board_fastboot_setup(void)
{
	switch (get_boot_device()) {
#if defined(CONFIG_FASTBOOT_STORAGE_SATA)
	case SATA_BOOT:
		if (!getenv("root_device"))
			setenv("root_device", "sata");
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "sata");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota sata");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_SATA*/
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD2_BOOT:
	case MMC1_BOOT:
	case MMC2_BOOT:
	case MMC4_BOOT:
	    if (!getenv("mmc_cur"))
			setenv("mmc_cur", "0");
	    if (!getenv("root_device"))
			setenv("root_device", "emmc");
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "run a62_boot_init; boota mmc0");
	    break;
	case SD3_BOOT:
	case MMC3_BOOT:
	    if (!getenv("mmc_cur"))
			setenv("mmc_cur", "1");
	    if (!getenv("root_device"))
			setenv("root_device", "sd");
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "run a62_boot_init; boota mmc1");
	    break;
//	case MMC4_BOOT:
//	    if (!getenv("fastboot_dev"))
//			setenv("fastboot_dev", "mmc2");
//	    if (!getenv("bootcmd"))
//			setenv("bootcmd", "run a62_boot_init; boota mmc2");
//	    break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("unsupported boot devices\n");
		break;
	}
}

#ifdef CONFIG_ANDROID_RECOVERY
int check_recovery_cmd_file(void)
{
	return recovery_check_and_clean_flag();
}

void board_recovery_setup(void)
{
	int bootdev = get_boot_device();

	switch (bootdev) {
#if defined(CONFIG_FASTBOOT_STORAGE_SATA)
	case SATA_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota sata recovery");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_SATA*/
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD2_BOOT:
	case MMC1_BOOT:
	case MMC2_BOOT:
	case MMC4_BOOT:
	    	if (!getenv("mmc_cur"))
			setenv("mmc_cur", "0");
	    	if (!getenv("root_device"))
			setenv("root_device", "emmc");
	    	if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc0 recovery");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
	    	if (!getenv("mmc_cur"))
			setenv("mmc_cur", "1");
	    	if (!getenv("root_device"))
			setenv("root_device", "sd");
	    	if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc1 recovery");
		break;
//	case MMC4_BOOT:
//		if (!getenv("bootcmd_android_recovery"))
//			setenv("bootcmd_android_recovery",
//				"boota mmc2 recovery");
//		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("Unsupported bootup device for recovery: dev: %d\n",
			bootdev);
		return;
	}

	printf("setup env for recovery..\n");
	setenv("bootcmd", "run bootcmd_android_recovery");
}

#endif /*CONFIG_ANDROID_RECOVERY*/

#endif /*CONFIG_FSL_FASTBOOT*/

#ifdef CONFIG_IMX_UDC
iomux_v3_cfg_t const otg_udc_pads[] = {
	IOMUX_PADS(PAD_ENET_RX_ER__USB_OTG_ID | MUX_PAD_CTRL(NO_PAD_CTRL)),
};
void udc_pins_setting(void)
{
	SETUP_IOMUX_PADS(otg_udc_pads);

	/*set daisy chain for otg_pin_id on 6q. for 6dl, this bit is reserved*/
    imx_iomux_set_gpr_register(1, 13, 1, 0);
}
#endif /*CONFIG_IMX_UDC*/


/*  __________________________________________________________________________
 * |                                                                          |
 * |                               WATCHDOG APX                               |
 * |__________________________________________________________________________|
 */
static inline void setup_iomux_apx_watchdog (void) {
	SETUP_IOMUX_PADS(wdt_trigger_pads);
}
/*  __________________________________________________________________________
 * |__________________________________________________________________________|
 */


/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   ETHERNET                               |
 * |__________________________________________________________________________|
 */

static iomux_v3_cfg_t const enet_pads[] = {
	IOMUX_PADS(PAD_KEY_COL2__GPIO4_IO10 | MUX_PAD_CTRL(ENET_PAD_CTRL)),
};
inline void enable_ethernet_rail (void) {
	// Power on Ethernet 
	SETUP_IOMUX_PADS(enet_pads);
	gpio_direction_output (IMX_GPIO_NR(4, 10), 1);
	gpio_set_value (IMX_GPIO_NR(4, 10), 1);
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

#define USDHC3_CD_GPIO	IMX_GPIO_NR(7, 0)
#define USDHC3_PWR_GPIO IMX_GPIO_NR(2, 5)

/* USDHC map:
 * 	USDHC4  -->  eMMC  -->  FSL_SDHC: 0
 * 	USDHC3  -->  uSD   -->  FSL_SDHC: 1
 */

boot_mem_dev_t boot_mem_dev_list[SECO_NUM_BOOT_DEV] = {
	{ 0x4, 0x5, -1,   0, -1, "on board uSD" },
	{ 0x6, 0x7, -1,  -1, -1, "eMMC" },
};

struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM] = {
	{USDHC4_BASE_ADDR, 0, 8},
	{USDHC3_BASE_ADDR, 0, 4},
};

struct usdhc_l usdhc_list[CONFIG_SYS_FSL_USDHC_NUM] = {
	{usdhc4_pads, ARRAY_SIZE(usdhc4_pads), -1},
	{usdhc3_pads, ARRAY_SIZE(usdhc3_pads), USDHC3_CD_GPIO},
};

enum mxc_clock usdhc_clk[CONFIG_SYS_FSL_USDHC_NUM] = {
	MXC_ESDHC4_CLK,
	MXC_ESDHC3_CLK,
};

/* map the usdhc controller id to the devno given to the board device */
int usdhc_devno[4] = { -1, -1, 1, 0};

int board_mmc_getcd (struct mmc *mmc) {
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
		case USDHC3_BASE_ADDR:
			ret = !gpio_get_value(USDHC3_CD_GPIO);
			break;
		case USDHC4_BASE_ADDR:
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

int board_mmc_init(bd_t *bis){
#ifndef CONFIG_SPL_BUILD
	int ret;
	int i;
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			SETUP_IOMUX_PADS(usdhc4_pads);
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);
			break;			
		case 1:
			SETUP_IOMUX_PADS(usdhc3_pads);
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);		
			gpio_direction_input(USDHC3_CD_GPIO);
			gpio_direction_output(USDHC3_PWR_GPIO, 1);
			break;
			
		default:
			printf("Warning: you configured more USDHC controllers"
			       "(%d) then supported by the board (%d)\n",
			       i + 1, CONFIG_SYS_FSL_USDHC_NUM);
			return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
		if (ret)
			return ret;
	}

	return 0;
#else
	struct src *psrc = (struct src *)SRC_BASE_ADDR;
	unsigned reg_smbr1 = readl(&psrc->sbmr1);
	unsigned reg = readl(&psrc->sbmr1) >> 11;	

	printf("mmc port %d\n", reg & 0x3);

	if ((reg_smbr1 & 0x0820) == 0x0820) {
	   /* Setup eMMC */
	   SETUP_IOMUX_PADS(usdhc4_pads);
	   usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);
	   return fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
	} else {
	   /* Setup uSD */
	   SETUP_IOMUX_PADS(usdhc3_pads);
	   usdhc_cfg[1].esdhc_base = USDHC3_BASE_ADDR;
	   usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
	   gpio_direction_input(USDHC3_CD_GPIO);
	   gpio_direction_output(USDHC3_PWR_GPIO, 1);
	   gd->arch.sdhc_clk = usdhc_cfg[1].sdhc_clk;
	   return fsl_esdhc_initialize(bis, &usdhc_cfg[1]);
	}
#endif
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
		imx_iomux_set_gpr_register(1, 13, 1, 0);
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
char *board_name = "Seco SBC-i.MX6 (SBC) - A62";
#endif


int board_init (void) {
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

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

#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_env_init();
#endif

#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	if (is_cpu_type(MXC_CPU_MX6Q))
		setenv("board_rev", "MX6Q");
	else
		setenv("board_rev", "MX6DL");
#endif
	return ret;
}


int isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\12');
}
char *trim(char *str)
{
	char *end;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
	return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}

/**
 * After loading uEnv.txt, we autodetect which fdt file we need to load.
 * uEnv.txt can contain:
 *  - video_output=hdmi|lvds7|lvds15
 *    any other value (or if the variable is not specified) will default to "hdmi"
 *  - use_custom_dtb=true|false
 *    any other value (or if the variable is not specified) will default to "false"
 * 
 * Despite the signature, this command does not accept any argument.
 */
int do_a62init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char* modelfdt;

	if (is_cpu_type(MXC_CPU_MX6Q)) {
		modelfdt = "imx6q-a62";
	} else {
		modelfdt = "imx6dl-a62";
	}
	
	char* video_part = "-lvds7hdmi";
	char* video = getenv("video_output");
	
	if (video) {
		video = trim(video);
		if (strcmp(video, "lvds7") == 0) {
			video_part = "-lvds7";
#ifdef CONFIG_ANDROID_SUPPORT
			setenv("lcd_density", "128");
#endif
		} else if (strcmp(video, "lvds15") == 0) {
			video_part = "-lvds15";
		} else if (strcmp(video, "lvds7hdmi") == 0) {
			video_part = "-lvds7hdmi";
#ifdef CONFIG_ANDROID_SUPPORT
			setenv("lcd_density", "128");
#endif
		} else if (strcmp(video, "hdmi") == 0) {
			video_part = "-hdmi";
		}
	}
	
	char* dir_part = "dts";
	char* customdtb = getenv("use_custom_dtb");
	if (customdtb) {
		customdtb = trim(customdtb);
		if (strcmp(customdtb, "true") == 0 || strcmp(customdtb, "yes") == 0 || strcmp(customdtb, "enabled") == 0) {
			dir_part = "dts-overlay";
		}
	}
	
	char fdt_file[100];
	sprintf(fdt_file, "%s/%s%s.dtb", dir_part, modelfdt, video_part);
	
	printf("Device Tree: %s\n", fdt_file);
	setenv("fdt_file", fdt_file);
	
	return 0;
}


U_BOOT_CMD(
	a62init,	1,	1,	do_a62init,
	"(A62) determine the device tree to load", ""
);
