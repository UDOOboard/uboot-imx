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




#if defined(CONFIG_VIDEO_IPUV3)


static int detect_hdmi (struct display_info_t const *dev) {
	struct hdmi_regs *hdmi  = (struct hdmi_regs *)HDMI_ARB_BASE_ADDR;
	return readb(&hdmi->phy_stat0) & HDMI_DVI_STAT;
}


static void disable_lvds(struct display_info_t const *dev) {
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

	int reg = readl(&iomux->gpr[2]);

	reg &= ~(IOMUXC_GPR2_LVDS_CH0_MODE_MASK |
		 IOMUXC_GPR2_LVDS_CH1_MODE_MASK);

	writel(reg, &iomux->gpr[2]);
}

static void enable_hdmi (struct display_info_t const *dev) {
	disable_lvds(dev);
	imx_enable_hdmi_phy ();
}


static int detect_lvds (struct display_info_t const *dev) {
	return 0;
}


static iomux_v3_cfg_t const backlight_pads[] = {

   MX6_PAD_GPIO_2__GPIO1_IO02 | MUX_PAD_CTRL(NO_PAD_CTRL),
   MX6_PAD_GPIO_4__GPIO1_IO04 | MUX_PAD_CTRL(NO_PAD_CTRL),




//	/* Backlight on RGB connector: J15 */
//	MX6_PAD_SD1_DAT3__GPIO1_IO21 | MUX_PAD_CTRL(NO_PAD_CTRL),
//#define RGB_BACKLIGHT_GP IMX_GPIO_NR(1, 21)
//
//	/* Backlight on LVDS connector: J6 */
//	MX6_PAD_SD1_CMD__GPIO1_IO18 | MUX_PAD_CTRL(NO_PAD_CTRL),
//#define LVDS_BACKLIGHT_GP IMX_GPIO_NR(1, 18)
};




static void setup_iomux_backlight(void)
{
	gpio_direction_output(IMX_GPIO_NR(2, 9), 1);
	imx_iomux_v3_setup_multiple_pads(backlight_pads,
					 ARRAY_SIZE(backlight_pads));
}





 static iomux_v3_cfg_t const lvds_pads[] = {
	           MX6_PAD_GPIO_2__GPIO1_IO02 | MUX_PAD_CTRL(NO_PAD_CTRL),
	            MX6_PAD_GPIO_4__GPIO1_IO04 | MUX_PAD_CTRL(NO_PAD_CTRL),
	    };


static void enable_lvds (struct display_info_t const *dev) {
	struct iomuxc *iomux = (struct iomuxc *)
				IOMUXC_BASE_ADDR;
//	u32 reg = readl(&iomux->gpr[2]);
//	reg |= IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT;
//	writel(reg, &iomux->gpr[2]);
	//gpio_direction_output(LVDS_BACKLIGHT_GP, 1);


	imx_iomux_v3_setup_multiple_pads(lvds_pads, ARRAY_SIZE(lvds_pads));
	         gpio_direction_output(IMX_GPIO_NR(1, 2), 1); /* LVDS power On */
	           gpio_direction_output(IMX_GPIO_NR(1, 4), 1); /* LVDS backlight On */

}



//static void enable_rgb (struct display_info_t const *dev) {
//	imx_iomux_v3_setup_multiple_pads (rgb_pads,
//		 ARRAY_SIZE (rgb_pads));
//	gpio_direction_output (RGB_BACKLIGHT_GP, 1);
//}







static struct display_info_t const displays[] = {
	 {
		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB666,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			// Rif. 800x480 Panel UMSH-8596MD-20T
			.name           = "LDB-WVGA",
			.refresh        = 60,
			.xres           = 800,
			.yres           = 480,
			.pixclock       = 15385,
			.left_margin    = 220,
			.right_margin   = 40,
			.upper_margin   = 21,
			.lower_margin   = 7,
			.hsync_len      = 60,
			.vsync_len      = 10,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		} 
	}, {

		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB666,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			// Rif. Panel 1024x768 Panel UMSH-8596MD-15T
			.name           = "LDB-XGA",
			.refresh        = 60,
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
		} 
	}, {
		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB666,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			// Rif. 1366x768 Panel CHIMEI M156B3-LA1
			.name           = "LDB-WXGA",
			.refresh        = 59,
			.xres           = 1368,
			.yres           = 768,
			.pixclock       = 13890,
			.left_margin    = 93,
			.right_margin   = 33,
			.upper_margin   = 22,
			.lower_margin   = 7,
			.hsync_len      = 40,
			.vsync_len      = 4,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		} 
	},{
		.bus    = -1,
		.addr   = 0,
		.pixfmt = IPU_PIX_FMT_RGB24,
		.detect = detect_hdmi,
		.enable = enable_hdmi,
		.mode   = {


                .name           = "HDMI",
                .refresh        = 60,
                .xres           = 1024,
                .yres           = 768,
                .pixclock       = 15385,
                .left_margin    = 220,
                .right_margin   = 40,
                .upper_margin   = 21,
                .lower_margin   = 7,
                .hsync_len      = 60,
                .vsync_len      = 10,
                .sync           = FB_SYNC_EXT,
                .vmode          = FB_VMODE_NONINTERLACED








				} 
	},
};


int board_video_skip (void) {
	int i;
	int ret;
	char const *panel = getenv("panel");

	if ( !panel ) {
		for ( i = 0 ; i < ARRAY_SIZE(displays) ; i++ ) {
			struct display_info_t const *dev = displays+i;
			if ( dev->detect && dev->detect(dev) ) {
				panel = dev->mode.name;
				printf ("auto-detected panel %s\n", panel);
				break;
			}
		}
		if ( !panel ) {
			panel = displays[0].mode.name;
			printf ("No panel detected: default to %s\n", panel);
			i = 0;
		}
	} else {
		for ( i = 0 ; i < ARRAY_SIZE(displays) ; i++ ) {
			if ( !strcmp(panel, displays[i].mode.name) )
				break;
		}
	}

	if ( i < ARRAY_SIZE(displays) ) {
		ret = ipuv3_fb_init(&displays[i].mode, 0,
				    displays[i].pixfmt);
		if ( !ret ) {
			if ( displays[i].enable )
				displays[i].enable (displays+i);
			printf ("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf ("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		printf ("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	return 0;
}







iomux_v3_cfg_t const di0_pads[] = {
	MX6_PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK,	/* DISP0_CLK */
	MX6_PAD_DI0_PIN2__IPU1_DI0_PIN02,		/* DISP0_HSYNC */
	MX6_PAD_DI0_PIN3__IPU1_DI0_PIN03,		/* DISP0_VSYNC */
};






void setup_display (void) {

	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
	int reg;

	/* Setup HSYNC, VSYNC, DISP_CLK for debugging purposes */
	imx_iomux_v3_setup_multiple_pads(di0_pads, ARRAY_SIZE(di0_pads));

	enable_ipu_clock();
	imx_setup_hdmi();

	/* Turn on LDB0, LDB1, IPU,IPU DI0 clocks */
	reg = readl(&mxc_ccm->CCGR3);
	reg |=  MXC_CCM_CCGR3_LDB_DI0_MASK | MXC_CCM_CCGR3_LDB_DI1_MASK;
	writel(reg, &mxc_ccm->CCGR3);

	/* set LDB0, LDB1 clk select to 011/011 */
	reg = readl(&mxc_ccm->cs2cdr);
	reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK
		 | MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
	reg |= (3 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET)
	      | (3 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->cs2cdr);

	reg = readl(&mxc_ccm->cscmr2);
	reg |= MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV | MXC_CCM_CSCMR2_LDB_DI1_IPU_DIV;
	writel(reg, &mxc_ccm->cscmr2);

	reg = readl(&mxc_ccm->chsccdr);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI0_CLK_SEL_OFFSET);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->chsccdr);

	reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
	     | IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH1_18BIT
	     | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH0_18BIT
	     | IOMUXC_GPR2_LVDS_CH0_MODE_DISABLED
	     | IOMUXC_GPR2_LVDS_CH1_MODE_ENABLED_DI0;
	writel(reg, &iomux->gpr[2]);

	reg = readl(&iomux->gpr[3]);
	reg = (reg & ~(IOMUXC_GPR3_LVDS1_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
	    | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
	       << IOMUXC_GPR3_LVDS1_MUX_CTL_OFFSET);
	writel(reg, &iomux->gpr[3]);
}



#endif /* CONFIG_VIDEO_IPUV3 */


