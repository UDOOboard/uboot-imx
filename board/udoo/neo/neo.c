/*
 * Copyright (C) 2014-2015 Freescale Semiconductor, Inc.
 * Copyright (C) Jasbir Matharu
 * Copyright (C) UDOO Team
 *
 * Author: Francesco Montefoschi <francesco.monte@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/io.h>
#include <asm/imx-common/mxc_i2c.h>
#include <linux/sizes.h>
#include <common.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <i2c.h>
#include <miiphy.h>
#include <netdev.h>
#include <power/pmic.h>
#include <power/pfuze3000_pmic.h>
#include <usb.h>
#include <usb/ehci-fsl.h>
#include "detectboard.h"

#ifdef CONFIG_MXC_RDC
#include <asm/imx-common/rdc-sema.h>
#include <asm/arch/imx-rdc.h>
#endif

#ifdef CONFIG_VIDEO_MXS
#include <linux/fb.h>
#include <mxsfb.h>
#endif

#ifdef CONFIG_FSL_FASTBOOT
#include <fsl_fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FSL_FASTBOOT*/

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_22K_UP  | PAD_CTL_SPEED_LOW |		\
	PAD_CTL_DSE_80ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define I2C_PAD_CTRL    (PAD_CTL_PKE | PAD_CTL_PUE |            \
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |               \
	PAD_CTL_DSE_40ohm | PAD_CTL_HYS |			\
	PAD_CTL_ODE)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP | PAD_CTL_PUE |     \
	PAD_CTL_SPEED_HIGH   |                                   \
	PAD_CTL_DSE_48ohm   | PAD_CTL_SRE_FAST)

#define ENET_CLK_PAD_CTRL  (PAD_CTL_SPEED_MED | \
	PAD_CTL_DSE_120ohm   | PAD_CTL_SRE_FAST)

#define ENET_RX_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |          \
	PAD_CTL_SPEED_HIGH   | PAD_CTL_SRE_FAST)

#define I2C_PAD_CTRL    (PAD_CTL_PKE | PAD_CTL_PUE |            \
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |               \
	PAD_CTL_DSE_40ohm | PAD_CTL_HYS |			\
	PAD_CTL_ODE)

#define LCD_PAD_CTRL    (PAD_CTL_HYS | PAD_CTL_PUS_100K_UP | PAD_CTL_PUE | \
	PAD_CTL_PKE | PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm)

#define BUTTON_PAD_CTRL    (PAD_CTL_PKE | PAD_CTL_PUE | \
	PAD_CTL_PUS_22K_UP | PAD_CTL_DSE_40ohm)

#define WDOG_PAD_CTRL (PAD_CTL_PUE | PAD_CTL_PKE | PAD_CTL_SPEED_MED |	\
	PAD_CTL_DSE_40ohm)

#define OTG_ID_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_47K_UP  | PAD_CTL_SPEED_LOW |		\
	PAD_CTL_DSE_80ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)


#define DIO_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_34ohm | PAD_CTL_HYS | PAD_CTL_SRE_FAST)
#define DIO_PAD_CFG   (MUX_PAD_CTRL(DIO_PAD_CTRL) | MUX_MODE_SION)

int dram_init(void)
{
	int ddr_mb = 1024;
	int board_variant = detect_board();
	if (board_variant == UDOO_NEO_TYPE_BASIC || board_variant == UDOO_NEO_TYPE_BASIC_KS) {
		ddr_mb = 512;
	}
	
	gd->ram_size = ((ulong)ddr_mb * 1024 * 1024);

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_GPIO1_IO04__UART1_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_GPIO1_IO05__UART1_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DATA0__USDHC2_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DATA1__USDHC2_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DATA2__USDHC2_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DATA3__USDHC2_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	
	/* CD pin */    
	MX6_PAD_SD1_DATA0__GPIO6_IO_2 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* Power */
	MX6_PAD_SD1_CMD__GPIO6_IO_1 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc3_pads[] = {
    /* Configured for WLAN */
	MX6_PAD_SD3_CLK__USDHC3_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CMD__USDHC3_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DATA0__USDHC3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DATA1__USDHC3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DATA2__USDHC3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DATA3__USDHC3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

static iomux_v3_cfg_t const gpio_pads[] = {

    MX6_PAD_NAND_DATA02__GPIO4_IO_6 | MUX_PAD_CTRL(NO_PAD_CTRL), // LED
    MX6_PAD_SD1_CLK__GPIO6_IO_0   | MUX_PAD_CTRL(NO_PAD_CTRL),   // LED

    MX6_PAD_KEY_COL3__GPIO2_IO_13 | MUX_PAD_CTRL(NO_PAD_CTRL),     // Gyro interrupt
    MX6_PAD_NAND_WE_B__GPIO4_IO_14 | MUX_PAD_CTRL(NO_PAD_CTRL),    // Mag interrupt
    MX6_PAD_QSPI1B_DATA2__GPIO4_IO_26 | MUX_PAD_CTRL(NO_PAD_CTRL), // Accel interrupt

    MX6_PAD_SD1_DATA2__GPIO6_IO_4 | MUX_PAD_CTRL(NO_PAD_CTRL),  // Touch interrupt
    MX6_PAD_SD1_DATA3__GPIO6_IO_5 | MUX_PAD_CTRL(NO_PAD_CTRL),  // Touch reset

    MX6_PAD_NAND_READY_B__GPIO4_IO_13 | MUX_PAD_CTRL(NO_PAD_CTRL),  // recognition GPIOs - Connected to R184
    MX6_PAD_NAND_ALE__GPIO4_IO_0 | MUX_PAD_CTRL(NO_PAD_CTRL),       // recognition GPIOs - Connected to R185

    // Multiplexer pins for GPIO/ADC (J5)
    MX6_PAD_RGMII2_TXC__GPIO5_IO_23 | MUX_PAD_CTRL(NO_PAD_CTRL),    // MUX_A
    MX6_PAD_RGMII2_RX_CTL__GPIO5_IO_16 | MUX_PAD_CTRL(NO_PAD_CTRL), // MUX_B
    MX6_PAD_QSPI1A_SCLK__GPIO4_IO_21 | MUX_PAD_CTRL(NO_PAD_CTRL),   // MUX_C
    MX6_PAD_QSPI1A_SS0_B__GPIO4_IO_22 | MUX_PAD_CTRL(NO_PAD_CTRL),  // MUX_D
    MX6_PAD_QSPI1A_DATA2__GPIO4_IO_18 | MUX_PAD_CTRL(NO_PAD_CTRL),  // MUX_E
    MX6_PAD_QSPI1A_DATA3__GPIO4_IO_19 | MUX_PAD_CTRL(NO_PAD_CTRL),  // MUX_F
};

static iomux_v3_cfg_t const wdog_b_pad = {
	MX6_PAD_GPIO1_IO13__GPIO1_IO_13 | MUX_PAD_CTRL(WDOG_PAD_CTRL),
};

static iomux_v3_cfg_t const peri_3v3_pads[] = {
	MX6_PAD_QSPI1A_DATA0__GPIO4_IO_16 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

#ifdef CONFIG_FEC_MXC
static iomux_v3_cfg_t const fec1_pads[] = {
	MX6_PAD_ENET1_MDC__ENET1_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_MDIO__ENET1_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII1_RX_CTL__ENET1_RX_EN | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX6_PAD_RGMII1_RD0__ENET1_RX_DATA_0 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX6_PAD_RGMII1_RD1__ENET1_RX_DATA_1 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX6_PAD_RGMII1_RD2__ENET1_RX_DATA_2 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX6_PAD_RGMII1_RD3__ENET1_RX_DATA_3 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX6_PAD_RGMII1_RXC__ENET1_RX_CLK | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX6_PAD_RGMII1_TX_CTL__ENET1_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII1_TD0__ENET1_TX_DATA_0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII1_TD1__ENET1_TX_DATA_1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII1_TD2__ENET1_TX_DATA_2 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII1_TD3__ENET1_TX_DATA_3 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII1_TXC__ENET1_RGMII_TXC | MUX_PAD_CTRL(ENET_PAD_CTRL),
};

static iomux_v3_cfg_t const phy_control_pads[] = {
	/* 25MHz Ethernet PHY Clock */
	MX6_PAD_ENET2_RX_CLK__ENET2_REF_CLK_25M | MUX_PAD_CTRL(ENET_CLK_PAD_CTRL),

	/* ENET PHY Power */
	MX6_PAD_ENET2_COL__GPIO2_IO_6 | MUX_PAD_CTRL(NO_PAD_CTRL),

	/* AR8031 PHY Reset */
	MX6_PAD_ENET2_CRS__GPIO2_IO_7 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static int setup_fec(int fec_id)
{
	struct anatop_regs *anatop = (struct anatop_regs *)ANATOP_BASE_ADDR;
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *) IOMUXC_GPR_BASE_ADDR;
	int reg;

	/* Use 125M anatop loopback REF_CLK1 for ENET1, clear gpr1[13], gpr1[17]*/
	clrsetbits_le32(&iomuxc_gpr_regs->gpr[1], IOMUX_GPR1_FEC1_MASK, 0);

	imx_iomux_v3_setup_multiple_pads(phy_control_pads,
					 ARRAY_SIZE(phy_control_pads));

	/* Enable the ENET power, active low */
	gpio_direction_output(IMX_GPIO_NR(2, 6) , 0);

	/* Reset AR8031 PHY */
	gpio_direction_output(IMX_GPIO_NR(2, 7) , 0);
	udelay(500);
	gpio_set_value(IMX_GPIO_NR(2, 7), 1);

	reg = readl(&anatop->pll_enet);
	reg |= BM_ANADIG_PLL_ENET_REF_25M_ENABLE;
	writel(reg, &anatop->pll_enet);

	return enable_fec_anatop_clock(fec_id, ENET_125MHZ);
}

int board_eth_init(bd_t *bis)
{
	imx_iomux_v3_setup_multiple_pads(fec1_pads, ARRAY_SIZE(fec1_pads));

	setup_fec(CONFIG_FEC_ENET_DEV);

	return cpu_eth_init(bis);
}

int board_phy_config(struct phy_device *phydev)
{
	/*
	 * Enable 1.8V(SEL_1P5_1P8_POS_REG) on
	 * Phy control debug reg 0
	 */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x1f);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x8);

	/* rgmii tx clock delay enable */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x05);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x100);

	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}
#endif

static void setup_iomux_gpio(void)
{
    imx_iomux_v3_setup_multiple_pads(gpio_pads, ARRAY_SIZE(gpio_pads));

    gpio_direction_input(IMX_GPIO_NR(2,13)); // Gyro interrupt
    gpio_direction_input(IMX_GPIO_NR(4,14)); // Mag interrupt
    gpio_direction_input(IMX_GPIO_NR(4,26)); // Accel interupt
    gpio_direction_input(IMX_GPIO_NR(2,4));  // Touch interrupt

    gpio_direction_input(IMX_GPIO_NR(4,13));  // recognition GPIOs - Connected to R184
    gpio_direction_input(IMX_GPIO_NR(4,0));   // recognition GPIOs - Connected to R185

    gpio_direction_output(IMX_GPIO_NR(4,6), 0);	 // LED
    gpio_direction_output(IMX_GPIO_NR(6,0), 1);  // LED

    gpio_direction_output(IMX_GPIO_NR(5,23), 0); // MUX_A
    gpio_direction_output(IMX_GPIO_NR(5,16), 0); // MUX_B
    gpio_direction_output(IMX_GPIO_NR(4,21), 0); // MUX_C
    gpio_direction_output(IMX_GPIO_NR(4,22), 0); // MUX_D
    gpio_direction_output(IMX_GPIO_NR(4,18), 0); // MUX_E
    gpio_direction_output(IMX_GPIO_NR(4,19), 0); // MUX_F
};

static iomux_v3_cfg_t const wl1831_control_pads[] = {
	/* WL1831 - WiFi enable */
	MX6_PAD_KEY_COL2__GPIO2_IO_12 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* WL1831 - Bt enable */
	MX6_PAD_KEY_ROW2__GPIO2_IO_17 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_wl1831(void)
{
	imx_iomux_v3_setup_multiple_pads(wl1831_control_pads, ARRAY_SIZE(wl1831_control_pads));

	gpio_direction_output(IMX_GPIO_NR(2,12), 0); // WL1831 - WiFi enable
	gpio_direction_output(IMX_GPIO_NR(2,17), 0); // WL1831 - Bt enable
}

#ifdef CONFIG_VIDEO_MXS
static iomux_v3_cfg_t const lvds_ctrl_pads[] = {
	/* CABC enable */
	MX6_PAD_QSPI1B_DATA3__GPIO4_IO_27 | MUX_PAD_CTRL(NO_PAD_CTRL),

	/* Use GPIO for Brightness adjustment, duty cycle = period */
	MX6_PAD_SD1_DATA1__GPIO6_IO_3 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const lcd_pads[] = {
	MX6_PAD_LCD1_CLK__LCDIF1_CLK | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_ENABLE__LCDIF1_ENABLE | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_HSYNC__LCDIF1_HSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_VSYNC__LCDIF1_VSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA00__LCDIF1_DATA_0 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA01__LCDIF1_DATA_1 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA02__LCDIF1_DATA_2 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA03__LCDIF1_DATA_3 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA04__LCDIF1_DATA_4 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA05__LCDIF1_DATA_5 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA06__LCDIF1_DATA_6 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA07__LCDIF1_DATA_7 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA08__LCDIF1_DATA_8 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA09__LCDIF1_DATA_9 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA10__LCDIF1_DATA_10 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA11__LCDIF1_DATA_11 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA12__LCDIF1_DATA_12 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA13__LCDIF1_DATA_13 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA14__LCDIF1_DATA_14 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA15__LCDIF1_DATA_15 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA16__LCDIF1_DATA_16 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA17__LCDIF1_DATA_17 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA18__LCDIF1_DATA_18 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA19__LCDIF1_DATA_19 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA20__LCDIF1_DATA_20 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA21__LCDIF1_DATA_21 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA22__LCDIF1_DATA_22 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_DATA23__LCDIF1_DATA_23 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD1_RESET__GPIO3_IO_27 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* CEC Clock */
	MX6_PAD_GPIO1_IO12__CCM_CLKO2 | MUX_PAD_CTRL(NO_PAD_CTRL),
};


struct lcd_panel_info_t {
	unsigned int lcdif_base_addr;
	int depth;
	void	(*enable)(struct lcd_panel_info_t const *dev);
	struct fb_videomode mode;
};

void do_enable_lvds(struct lcd_panel_info_t const *dev)
{
	enable_lcdif_clock(dev->lcdif_base_addr);
	enable_lvds(dev->lcdif_base_addr);

	imx_iomux_v3_setup_multiple_pads(lvds_ctrl_pads,
							ARRAY_SIZE(lvds_ctrl_pads));

	/* Enable CABC */
	gpio_direction_output(IMX_GPIO_NR(4, 27) , 1);

	/* Set Brightness to high */
	gpio_direction_output(IMX_GPIO_NR(6, 3) , 1);
}

void do_enable_parallel_lcd(struct lcd_panel_info_t const *dev)
{
	// enable_lcdif_clock(dev->lcdif_base_addr);
	// 
	// imx_iomux_v3_setup_multiple_pads(lcd_pads, ARRAY_SIZE(lcd_pads));
	// 
	// /* HDMI interrupt */
	// gpio_direction_input(IMX_GPIO_NR(3, 27));
	// 
	// /* Set clock CCM_CLKO2 to 12MHz */
	// struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	// int reg;
	// reg = readl(&mxc_ccm->ccosr);
	// reg |= (MXC_CCM_CCOSR_CKO2_EN_OFFSET  |
	// 	1 << MXC_CCM_CCOSR_CKO2_DIV_OFFSET |
	// 	14 << MXC_CCM_CCOSR_CKO2_SEL_OFFSET );
	// 
	// /* Initialise HDMI controller */
	// tda19988_init();
	// tda19988_reset();
	// tda19988_fb_mode(dev->mode);
}

static struct lcd_panel_info_t const displays[] = {{
	.lcdif_base_addr = LCDIF2_BASE_ADDR,
	.depth = 18,
	.enable	= do_enable_lvds,
	.mode	= {
		.name			= "Hannstar-XGA",
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.lcdif_base_addr = LCDIF1_BASE_ADDR,
	.depth = 24,
	.enable	= do_enable_parallel_lcd,
	.mode	= {
        /*
         * Attached to TDA19988. For now set default resolution to
         * 1280x720@60 this should work for both DVI and HDMI sources.
         */
		.name			= "MCIMX28LCD",
		.xres           = 1280,
		.yres           = 720,
		.pixclock       = 13468, // 74.250 MHz
		.left_margin    = 220,
		.right_margin   = 110,
		.upper_margin   = 20,
		.lower_margin   = 5,
		.hsync_len      = 40,
		.vsync_len      = 5,
		.sync           = FB_SYNC_VERT_HIGH_ACT,
		.vmode          = FB_VMODE_NONINTERLACED
} } };

int board_video_skip(void)
{
	int i;
	int ret;
	char const *panel = getenv("panel");
	if (!panel) {
		panel = displays[0].mode.name;
		printf("No panel detected: default to %s\n", panel);
		i = 0;
	} else {
		for (i = 0; i < ARRAY_SIZE(displays); i++) {
			if (!strcmp(panel, displays[i].mode.name))
				break;
		}
	}
	if (i < ARRAY_SIZE(displays)) {
		ret = mxs_lcd_panel_setup(displays[i].mode, displays[i].depth,
				    displays[i].lcdif_base_addr);
		if (!ret) {
			if (displays[i].enable)
				displays[i].enable(displays+i);
			printf("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		printf("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	return 0;
}
#endif

#ifdef CONFIG_SYS_I2C_MXC
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)
/* I2C1 for PMIC */
static struct i2c_pads_info i2c_pad_info1 = {
	.scl = {
		.i2c_mode = MX6_PAD_GPIO1_IO00__I2C1_SCL | PC,
		.gpio_mode = MX6_PAD_GPIO1_IO00__GPIO1_IO_0 | PC,
		.gp = IMX_GPIO_NR(1, 0),
	},
	.sda = {
		.i2c_mode = MX6_PAD_GPIO1_IO01__I2C1_SDA | PC,
		.gpio_mode = MX6_PAD_GPIO1_IO01__GPIO1_IO_1 | PC,
		.gp = IMX_GPIO_NR(1, 1),
	},
};

/* I2C3 */
struct i2c_pads_info i2c_pad_info3 = {
	.scl = {
		.i2c_mode = MX6_PAD_KEY_COL4__I2C3_SCL | PC,
		.gpio_mode = MX6_PAD_KEY_COL4__GPIO2_IO_14 | PC,
		.gp = IMX_GPIO_NR(2, 14),
	},
	.sda = {
		.i2c_mode = MX6_PAD_KEY_ROW4__I2C3_SDA | PC,
		.gpio_mode = MX6_PAD_KEY_ROW4__GPIO2_IO_19 | PC,
		.gp = IMX_GPIO_NR(2, 19),
	},
};

int power_init_board(void)
{
	struct pmic *p;
	int ret;
	unsigned int reg, rev_id;

	ret = power_pfuze3000_init(PFUZE3000_I2C_BUS);
	if (ret)
		return ret;

	p = pmic_get("PFUZE3000");
	ret = pmic_probe(p);
	if (ret)
		return ret;

	pmic_reg_read(p, PFUZE3000_DEVICEID, &reg);
	pmic_reg_read(p, PFUZE3000_REVID, &rev_id);
	printf("PMIC:  PFUZE3000 DEV_ID=0x%x REV_ID=0x%x\n", reg, rev_id);

	/* disable Low Power Mode during standby mode */
	pmic_reg_read(p, PFUZE3000_LDOGCTL, &reg);
	reg |= 0x1;
	pmic_reg_write(p, PFUZE3000_LDOGCTL, reg);

	if (pmic_reg_write(p, PFUZE3000_SW1AMODE, 0xc)) {
		printf("PMIC: Set SW1AMODE error!\n");
		return -1;
	}
	
	if (pmic_reg_write(p, PFUZE3000_SW1BMODE, 0xc)) {
		printf("PMIC: Set SW1BMODE error!\n");
		return -1;
	}
	
	if (pmic_reg_write(p, PFUZE3000_SW2MODE, 0xc)) {
		printf("PMIC: Set SW2MODE error!\n");
		return -1;
	}
	
	if (pmic_reg_write(p, PFUZE3000_SW3MODE, 0xc)) {
		printf("PMIC: Set SW3MODE error!\n");
		return -1;
	}
	
	/* set SW1A standby volatage 0.975V */
	pmic_reg_read(p, PFUZE3000_SW1ASTBY, &reg);
	reg &= ~0x3f;
	reg |= PFUZE3000_SW1AB_SETP(9750);
	if (pmic_reg_write(p, PFUZE3000_SW1ASTBY, reg)) {
		printf("PMIC: Set SW1ASTBY error!\n");
		return -1;
	}
	
	/* set SW1B standby volatage 0.975V */
	pmic_reg_read(p, PFUZE3000_SW1BSTBY, &reg);
	reg &= ~0x3f;
	reg |= PFUZE3000_SW1AB_SETP(9750);
	if (pmic_reg_write(p, PFUZE3000_SW1BSTBY, reg)) {
		printf("PMIC: Set SW1BSTBY error!\n");
		return -1;
	}
	
	/* set SW1A/VDD_ARM_IN step ramp up time from 16us to 4us/25mV */
	pmic_reg_read(p, PFUZE3000_SW1ACONF, &reg);
	reg &= ~0xc0;
	reg |= 0x40;
	if (pmic_reg_write(p, PFUZE3000_SW1ACONF, reg)) {
		printf("PMIC: Set SW1ACONF error!\n");
		return -1;
	}
	
	/* set SW1B/VDD_SOC_IN step ramp up time from 16us to 4us/25mV */
	pmic_reg_read(p, PFUZE3000_SW1BCONF, &reg);
	reg &= ~0xc0;
	reg |= 0x40;
	if (pmic_reg_write(p, PFUZE3000_SW1BCONF, reg)) {
		printf("PMIC: Set SW1BCONF error!\n");
		return -1;
	}
	
	/* set VDD_ARM_IN to 1.350V */
	pmic_reg_read(p, PFUZE3000_SW1AVOLT, &reg);
	reg &= ~0x3f;
	reg |= PFUZE3000_SW1AB_SETP(13500);
	if (pmic_reg_write(p, PFUZE3000_SW1AVOLT, reg)) {
		printf("PMIC: Set SW1AVOLT error!\n");
		return -1;
	}
	
	/* set VDD_SOC_IN to 1.350V */
	pmic_reg_read(p, PFUZE3000_SW1BVOLT, &reg);
	reg &= ~0x3f;
	reg |= PFUZE3000_SW1AB_SETP(13500);
	if (pmic_reg_write(p, PFUZE3000_SW1BVOLT, reg)) {
		printf("PMIC: Set SW1BVOLT error!\n");
		return -1;
	}
	
	/* set DDR_1_5V to 1.350V */
	pmic_reg_read(p, PFUZE3000_SW3VOLT, &reg);
	reg &= ~0x0f;
	reg |= PFUZE3000_SW3_SETP(13500);
	if (pmic_reg_write(p, PFUZE3000_SW3VOLT, reg)) {
		printf("PMIC: Set SW3VOLT error!\n");
		return -1;
	}

	/* set VGEN2_1V5 to 1.5V */
	pmic_reg_read(p, PFUZE3000_VLDO2CTL, &reg);
	reg &= ~0x0f;
	reg |= PFUZE3000_VLDO_SETP(15000);
	/*  enable  */
	reg |= 0x10;
	if (pmic_reg_write(p, PFUZE3000_VLDO2CTL, reg)) {
		printf("PMIC: Set VLDO2CTL error!\n");
		return -1;
	}

	return 0;
}

#ifdef CONFIG_LDO_BYPASS_CHECK
void ldo_mode_set(int ldo_bypass)
{
	unsigned int reg;
	int is_400M;
	u32 vddarm;
	struct pmic *p;
	
	/* switch to ldo_bypass mode */
	if (ldo_bypass) {
		prep_anatop_bypass();
		
		p = pmic_get("PFUZE3000");
		
		/* set VDD_ARM_IN to 1.350V */
		pmic_reg_read(p, PFUZE3000_SW1AVOLT, &reg);
		reg &= ~0x3f;
		reg |= PFUZE3000_SW1AB_SETP(13500);
		if (pmic_reg_write(p, PFUZE3000_SW1AVOLT, reg)) {
			printf("PMIC: Set SW1AVOLT error!\n");
			return;
		}
		
		/* set VDD_SOC_IN to 1.350V */
		pmic_reg_read(p, PFUZE3000_SW1BVOLT, &reg);
		reg &= ~0x3f;
		reg |= PFUZE3000_SW1AB_SETP(13500);
		if (pmic_reg_write(p, PFUZE3000_SW1BVOLT, reg)) {
			printf("PMIC: Set SW1BVOLT error!\n");
			return;
		}

		is_400M = set_anatop_bypass(1);
		if (is_400M)
			vddarm = PFUZE3000_SW1AB_SETP(10750);
		else
			vddarm = PFUZE3000_SW1AB_SETP(11750);
		
		pmic_reg_read(p, PFUZE3000_SW1AVOLT, &reg);
		reg &= ~0x3f;
		reg |= vddarm;
		if (pmic_reg_write(p, PFUZE3000_SW1AVOLT, reg)) {
			printf("PMIC: Set SW1AVOLT error!\n");
			return;
		}
		
		pmic_reg_read(p, PFUZE3000_SW1BVOLT, &reg);
		reg &= ~0x3f;
		reg |= PFUZE3000_SW1AB_SETP(11750);
		if (pmic_reg_write(p, PFUZE3000_SW1BVOLT, reg)) {
			printf("PMIC: Set SW1BVOLT error!\n");
			return;
		}
		
		finish_anatop_bypass();
		printf("Switched to ldo_bypass mode!\n");
	}

}
#endif
#endif

#ifdef CONFIG_MXC_RDC
static rdc_peri_cfg_t const shared_resources[] = {
	(RDC_PER_GPIO1 | RDC_DOMAIN(0) | RDC_DOMAIN(1)),
};
#endif

#ifdef CONFIG_USB_EHCI_MX6
#define USB_OTHERREGS_OFFSET	0x800
#define UCTRL_PWR_POL		(1 << 9)

static iomux_v3_cfg_t const usb_otg_pads[] = {
	/* OGT1 */
	MX6_PAD_GPIO1_IO09__USB_OTG1_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_GPIO1_IO10__ANATOP_OTG1_ID | MUX_PAD_CTRL(OTG_ID_PAD_CTRL),
	/* OTG2 */
	MX6_PAD_GPIO1_IO12__USB_OTG2_PWR | MUX_PAD_CTRL(NO_PAD_CTRL)
};

static void setup_usb(void)
{
	imx_iomux_v3_setup_multiple_pads(usb_otg_pads,
					 ARRAY_SIZE(usb_otg_pads));
}

int board_usb_phy_mode(int port)
{
	if (port == 1)
		return USB_INIT_HOST;
	else
		return usb_phy_mode(port);
}

int board_ehci_hcd_init(int port)
{
	u32 *usbnc_usb_ctrl;

	if (port > 1)
		return -EINVAL;

	usbnc_usb_ctrl = (u32 *)(USB_BASE_ADDR + USB_OTHERREGS_OFFSET +
				 port * 4);

	/* Set Power polarity */
	setbits_le32(usbnc_usb_ctrl, UCTRL_PWR_POL);

	return 0;
}
#endif

int board_early_init_f(void)
{
#ifdef CONFIG_MXC_RDC
	imx_rdc_setup_peripherals(shared_resources, ARRAY_SIZE(shared_resources));
#endif

#ifdef CONFIG_SYS_AUXCORE_FASTUP
	arch_auxiliary_core_up(0, CONFIG_SYS_AUXCORE_BOOTDATA);
#endif

	setup_iomux_uart();

	return 0;
}

static struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC2_BASE_ADDR, 0, 4},
	{USDHC3_BASE_ADDR, 0, 4},
};

#define USDHC2_PWR_GPIO IMX_GPIO_NR(6, 1)
#define USDHC2_CD_GPIO	IMX_GPIO_NR(6, 2)

int mmc_get_env_devno(void)
{
	u32 soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	int dev_no;
	u32 bootsel;

	bootsel = (soc_sbmr & 0x000000FF) >> 6 ;

	/* If not boot from sd/mmc, use default value */
	if (bootsel != 1)
		return CONFIG_SYS_MMC_ENV_DEV;

	/* BOOT_CFG2[3] and BOOT_CFG2[4] */
	dev_no = (soc_sbmr & 0x00001800) >> 11;

	/* need ubstract 1 to map to the mmc device id
	 * see the comments in board_mmc_init function
	 */

	dev_no--;

	return dev_no;
}

int mmc_map_to_kernel_blk(int dev_no)
{
	return dev_no + 1;
}

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC2_BASE_ADDR:
		ret = !gpio_get_value(USDHC2_CD_GPIO);
		break;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
#ifndef CONFIG_SPL_BUILD
	int i, ret;

	/*
	 * According to the board_mmc_init() the following map is done:
	 * (U-boot device node)    (Physical Port)
	 * mmc0                    USDHC2
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
			gpio_direction_input(USDHC2_CD_GPIO);
			gpio_direction_output(USDHC2_PWR_GPIO, 1);
			break;
		case 1:
			imx_iomux_v3_setup_multiple_pads(
				usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) than supported by the board\n", i + 1);
			return -EINVAL;
			}

			ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
			if (ret) {
				printf("Warning: failed to initialize mmc dev %d\n", i);
				return ret;
			}
	}

	return 0;
#else
	struct src *src_regs = (struct src *)SRC_BASE_ADDR;
	u32 val;
	u32 port;

	val = readl(&src_regs->sbmr1);

	if ((val & 0xc0) != 0x40) {
		printf("Not boot from USDHC!\n");
		return -EINVAL;
	}

	port = (val >> 11) & 0x3;
	printf("port %d\n", port);
	switch (port) {
	case 1:
		imx_iomux_v3_setup_multiple_pads(
			usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
		usdhc_cfg[0].esdhc_base = USDHC2_BASE_ADDR;
		gpio_direction_input(USDHC2_CD_GPIO);
		gpio_direction_output(USDHC2_PWR_GPIO, 1);
		break;
	case 2:
		imx_iomux_v3_setup_multiple_pads(
			usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
		usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
		usdhc_cfg[1].esdhc_base = USDHC3_BASE_ADDR;
		break;
	}

	gd->arch.sdhc_clk = usdhc_cfg[0].sdhc_clk;
	return fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
#endif
}

int check_mmc_autodetect(void)
{
	char *autodetect_str = getenv("mmcautodetect");

	if ((autodetect_str != NULL) &&
		(strcmp(autodetect_str, "yes") == 0)) {
		return 1;
	}

	return 0;
}

void board_late_mmc_init(void)
{
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

#ifdef CONFIG_FSL_QSPI

#define QSPI_PAD_CTRL1	\
	(PAD_CTL_SRE_FAST | PAD_CTL_SPEED_HIGH | \
	 PAD_CTL_PKE | PAD_CTL_PUE | PAD_CTL_PUS_47K_UP | PAD_CTL_DSE_40ohm)

static iomux_v3_cfg_t const quadspi_pads[] = {
	MX6_PAD_NAND_WP_B__QSPI2_A_DATA_0	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_READY_B__QSPI2_A_DATA_1	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_CE0_B__QSPI2_A_DATA_2	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_CE1_B__QSPI2_A_DATA_3	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_ALE__QSPI2_A_SS0_B		| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_CLE__QSPI2_A_SCLK		| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
};

int board_qspi_init(void)
{
	/* Set the iomux */
	imx_iomux_v3_setup_multiple_pads(quadspi_pads,
					 ARRAY_SIZE(quadspi_pads));

	/* Set the clock */
	enable_qspi_clk(1);

	return 0;
}
#endif

int board_init(void)
{
	/* Address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	/*
	 * Because kernel set WDOG_B mux before pad with the commone pinctrl
	 * framwork now and wdog reset will be triggered once set WDOG_B mux
	 * with default pad setting, we set pad setting here to workaround this.
	 * Since imx_iomux_v3_setup_pad also set mux before pad setting, we set
	 * as GPIO mux firstly here to workaround it.
	 */
	imx_iomux_v3_setup_pad(wdog_b_pad);

	/* Enable PERI_3V3, which is used by SD2, ENET, LVDS, BT */
	imx_iomux_v3_setup_multiple_pads(peri_3v3_pads,
					 ARRAY_SIZE(peri_3v3_pads));

	/* Active high for ncp692 */
	gpio_direction_output(IMX_GPIO_NR(4, 16) , 1);

	setup_iomux_gpio();

#ifdef CONFIG_SYS_I2C_MXC
	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info1);
	setup_i2c(2, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info3);
#endif

#ifdef CONFIG_USB_EHCI_MX6
	setup_usb();
#endif

#ifdef CONFIG_FSL_QSPI
	board_qspi_init();
#endif

	setup_wl1831();

	return 0;
}

int board_late_init(void)
{
#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_init();
#endif
	
	return 0;
}

u32 get_board_rev(void)
{
	return get_cpu_rev();
}

int checkboard(void)
{
	int board_variant = detect_board();
	
	if (board_variant == UDOO_NEO_TYPE_BASIC) {
		puts("Board: UDOO Neo Basic\n");
	}
	if (board_variant == UDOO_NEO_TYPE_BASIC_KS) {
		puts("Board: UDOO Neo Basic (Kickstarter Edition)\n");
	}
	if (board_variant == UDOO_NEO_TYPE_EXTENDED) {
		puts("Board: UDOO Neo Extended\n");
	}
	if (board_variant == UDOO_NEO_TYPE_FULL) {
		puts("Board: UDOO Neo Full\n");
	}
		
	return 0;
}

#ifdef CONFIG_FSL_FASTBOOT

void board_fastboot_setup(void)
{
	switch (get_boot_device()) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc0");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc1");
		break;
	case SD4_BOOT:
	case MMC4_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc2");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc2");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("unsupported boot devices\n");
		break;
	}
}

#ifdef CONFIG_ANDROID_RECOVERY

#define GPIO_VOL_DN_KEY IMX_GPIO_NR(1, 19)
iomux_v3_cfg_t const recovery_key_pads[] = {
	(MX6_PAD_CSI_DATA05__GPIO1_IO_19 | MUX_PAD_CTRL(BUTTON_PAD_CTRL)),
};

int check_recovery_cmd_file(void)
{
	int button_pressed = 0;
	int recovery_mode = 0;

	recovery_mode = recovery_check_and_clean_flag();

	/* Check Recovery Combo Button press or not. */
	imx_iomux_v3_setup_multiple_pads(recovery_key_pads,
		ARRAY_SIZE(recovery_key_pads));

	gpio_direction_input(GPIO_VOL_DN_KEY);

	if (gpio_get_value(GPIO_VOL_DN_KEY) == 0) { /* VOL_DN key is low assert */
		button_pressed = 1;
		printf("Recovery key pressed\n");
	}

	return recovery_mode || button_pressed;
}

void board_recovery_setup(void)
{
	int bootdev = get_boot_device();

	switch (bootdev) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "boota mmc0 recovery");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "boota mmc1 recovery");
		break;
	case SD4_BOOT:
	case MMC4_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "boota mmc2 recovery");
		break;
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

int isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}

char *trim(char *s) {
	char *ptr;
	if (!s)
		return NULL;   // handle NULL string
	if (!*s)
		return s;      // handle empty string
	for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
	ptr[1] = '\0';
	
	return s;
}

/**
 * After loading uEnv.txt, we autodetect which fdt file we need to load and if we should start M4.
 * uEnv.txt can contain:
 *  - video_output=hdmi|lvds7|lvds15|disabled
 *    any other value (or if the variable is not specified) will default to "hdmi"
 *  - m4_enabled=true|false
 *    any other value (or if the variable is not specified) will default to "true"
 *  - use_custom_dtb=true|false
 *    any other value (or if the variable is not specified) will default to "false"
 * 
 * Despite the signature, this command does not accept any argument.
 */
int do_udooinit(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int board_variant = detect_board();
	char* modelfdt;

	switch (board_variant) {
		case UDOO_NEO_TYPE_BASIC:
			modelfdt = "imx6sx-udoo-neo-basic";
			break;
		case UDOO_NEO_TYPE_BASIC_KS:
			modelfdt = "imx6sx-udoo-neo-basicks";
			break;
		case UDOO_NEO_TYPE_EXTENDED:
			modelfdt = "imx6sx-udoo-neo-extended";
			break;
		case UDOO_NEO_TYPE_FULL:
			modelfdt = "imx6sx-udoo-neo";
			break;
		default:
			return 0;
	}
	
	char* video_part = "-hdmi";
	char* video = trim(getenv("video_output"));
	if (video) {
		if (strcmp(video, "lvds7") == 0) {
			video_part = "-lvds7";
		} else if (strcmp(video, "lvds15") == 0) {
			video_part = "-lvds15";
		} else if (strcmp(video, "disabled") == 0) {
			video_part = "";
		}
	}
	
	char* m4_part = "-m4";
	char* m4 = trim(getenv("m4_enabled"));
	if (m4) {
		if (strcmp(m4, "false") == 0 || strcmp(m4, "no") == 0 || strcmp(m4, "disabled") == 0) {
			m4_part = "";
			setenv("m4mmcargs", "");
			setenv("m4boot", "");
			printf("M4: disabled via environment variables.\n");

		}
	}
	
	char* dir_part = "dts";
	char* customdtb = trim(getenv("use_custom_dtb"));
	if (customdtb) {
		if (strcmp(m4, "true") == 0 || strcmp(m4, "yes") == 0 || strcmp(m4, "enabled") == 0) {
			dir_part = "dts-overlay";
		}
	}
	
	char fdt_file[100];
	sprintf(fdt_file, "%s/%s%s%s.dtb", dir_part, modelfdt, video_part, m4_part);
	
	printf("Device Tree: %s\n", fdt_file);
	setenv("fdt_file", fdt_file);
	
	return 0;
}

U_BOOT_CMD(
	udooinit,	1,	1,	do_udooinit,
	"(UDOO Neo) initialize M4 core and determine the device tree to load", ""
);
