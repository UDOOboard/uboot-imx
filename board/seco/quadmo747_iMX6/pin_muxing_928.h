/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _BOARD_MX6_SECO_928_H__
#define _BOARD_MX6_SECO_928_H__

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <miiphy.h>
#include <netdev.h>
#if defined(CONFIG_MX6DL) && defined(CONFIG_MXC_EPDC)
#include <lcd.h>
#include <mxc_epdc_fb.h>
#endif
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/crm_regs.h>
#include <linux/fb.h>
#include <ipu_pixfmt.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#ifdef CONFIG_SYS_I2C_MXC
#include <i2c.h>
#include <asm/imx-common/mxc_i2c.h>
#endif
#ifdef CONFIG_CMD_SATA
#include <asm/imx-common/sata.h>
#endif
#ifdef CONFIG_FASTBOOT
#include <fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FASTBOOT*/




DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |                   \
		PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |                 \
		PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |                    \
		PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |                 \
		PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP |                   \
		PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED | \
		PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

#define I2C_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
			PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
			PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)

#define EPDC_PAD_CTRL    (PAD_CTL_PKE | PAD_CTL_SPEED_MED |     \
		PAD_CTL_DSE_40ohm | PAD_CTL_HYS)


/*  __________________________________________________________________________
 * |                                                                          |
 * |		                      UART		                      |
 * |__________________________________________________________________________|
 */

iomux_v3_cfg_t const uart_debug_pads[] = {
	MX6_PAD_EIM_D26__UART2_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_EIM_D27__UART2_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};


/*  __________________________________________________________________________
 * |                                                                          |
 * |		                  BOOT VALIDATE		                      |
 * |__________________________________________________________________________|
 */
static iomux_v3_cfg_t const gpio_boot_validate_pad[] = {
	MX6_PAD_SD4_DAT5__GPIO2_IO13 | MUX_PAD_CTRL(NO_PAD_CTRL),
};


/*  __________________________________________________________________________
 * |                                                                          |
 * |		                      MMC		                      |
 * |__________________________________________________________________________|
 */
iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CLK__SD3_CLK    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CMD__SD3_CMD    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT4__SD3_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT5__SD3_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT6__SD3_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT7__SD3_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

/*  __________________________________________________________________________
 * |                                                                          |
 * |	                         On Board uSD		                      |
 * |__________________________________________________________________________|
 */
iomux_v3_cfg_t const usdhc4_pads[] = {
	MX6_PAD_SD4_CLK__SD4_CLK     | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_CMD__SD4_CMD     | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT0__SD4_DATA0  | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT1__SD4_DATA1  | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT2__SD4_DATA2  | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT3__SD4_DATA3  | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D6__GPIO2_IO06 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

/*  __________________________________________________________________________
 * |                                                                          |
 * |	                         External SD		                      |
 * |__________________________________________________________________________|
 */
iomux_v3_cfg_t const usdhc1_pads[] = {
	MX6_PAD_SD1_CLK__SD1_CLK      | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_CMD__SD1_CMD      | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT0__SD1_DATA0   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT1__SD1_DATA1   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT2__SD1_DATA2   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT3__SD1_DATA3   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_CS2__GPIO6_IO15 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

/*  __________________________________________________________________________
 * |                                                                          |
 * |	                              SPI		                      |
 * |__________________________________________________________________________|
 */

#ifdef CONFIG_SYS_USE_SPINOR
iomux_v3_cfg_t const ecspi1_pads[] = {
	MX6_PAD_KEY_COL0__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_KEY_COL1__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_KEY_ROW0__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_KEY_ROW1__GPIO4_IO09 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
#endif  /*  CONFIG_SYS_USE_SPINOR  */



/*  __________________________________________________________________________
 * |                                                                          |
 * |	                               USB		                      |
 * |__________________________________________________________________________|
 */
#ifdef CONFIG_USB_EHCI_MX6

iomux_v3_cfg_t const usb_otg_pads[] = {
	MX6_PAD_EIM_D22__USB_OTG_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_GPIO_1__USB_OTG_ID | MUX_PAD_CTRL(NO_PAD_CTRL),
};


iomux_v3_cfg_t const usb_hc1_pads[] = {
	 MX6_PAD_GPIO_17__GPIO7_IO12  | MUX_PAD_CTRL(NO_PAD_CTRL),
};

#endif  /*  CONFIG_USB_EHCI_MX6  */



#endif   /* _BOARD_MX6_SECO_928_H__ */
