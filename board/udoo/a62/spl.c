/*
 * Copyright (C) UDOO Team
 *
 * Author: Francesco Montefoschi <francesco.monte@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <common.h>
#include <libfdt.h>
#include <asm/arch/iomux.h>
#include "detectboard.h"

#include <spl.h>
#include <asm/arch/mx6-ddr.h>

#ifdef CONFIG_APX_WATCHDOG
#include <watchdog.h>
#endif

/* configure MX6Q/DUAL mmdc DDR io registers */
const struct mx6dq_iomux_ddr_regs mx6_ddr_ioregs = {
        .dram_sdclk_0 =  0x00000028,
        .dram_sdclk_1 =  0x00000028,
        .dram_cas =  0x00000028,
        .dram_ras =  0x00000028,
        .dram_reset =  0x00000028,
        .dram_sdcke0 =  0x00003000,
        .dram_sdcke1 =  0x00003000,
        .dram_sdba2 =  0x00000000,
        .dram_sdodt0 =  0x00000028,
        .dram_sdodt1 =  0x00000028,
        .dram_sdqs0 =  0x00000028,
        .dram_sdqs1 =  0x00000028,
        .dram_sdqs2 =  0x00000028,
        .dram_sdqs3 =  0x00000028,
        .dram_sdqs4 =  0x00000028,
        .dram_sdqs5 =  0x00000028,
        .dram_sdqs6 =  0x00000028,
        .dram_sdqs7 =  0x00000028,
        .dram_dqm0 =  0x00000028,
        .dram_dqm1 =  0x00000028,
        .dram_dqm2 =  0x00000028,
        .dram_dqm3 =  0x00000028,
        .dram_dqm4 =  0x00000028,
        .dram_dqm5 =  0x00000028,
        .dram_dqm6 =  0x00000028,
        .dram_dqm7 =  0x00000028,
};

/* configure MX6Q/DUAL mmdc GRP io registers */
const struct mx6dq_iomux_grp_regs mx6_grp_ioregs = {
        .grp_ddr_type =  0x000C0000,
        .grp_ddrmode_ctl =  0x00020000,
        .grp_ddrpke =  0x00000000,
        .grp_addds =  0x00000028,
        .grp_ctlds =  0x00000030,
        .grp_ddrmode =  0x00020000,
        .grp_b0ds =  0x00000028,
        .grp_b1ds =  0x00000028,
        .grp_b2ds =  0x00000028,
        .grp_b3ds =  0x00000028,
        .grp_b4ds =  0x00000028,
        .grp_b5ds =  0x00000028,
        .grp_b6ds =  0x00000028,
        .grp_b7ds =  0x00000028,
};

/* configure MX6SOLO/DUALLITE mmdc DDR io registers */
struct mx6sdl_iomux_ddr_regs mx6sdl_ddr_ioregs = {
	.dram_sdclk_0 = 0x00000028,
	.dram_sdclk_1 = 0x00000028,
	.dram_cas = 0x00000028,
	.dram_ras = 0x00000028,
	.dram_reset = 0x00000028,
	.dram_sdcke0 = 0x00000028,
	.dram_sdcke1 = 0x00000028,
	.dram_sdba2 = 0x00000000,
	.dram_sdodt0 = 0x00000028,
	.dram_sdodt1 = 0x00000028,
	.dram_sdqs0 = 0x00000028,
	.dram_sdqs1 = 0x00000028,
	.dram_sdqs2 = 0x00000028,
	.dram_sdqs3 = 0x00000028,
	.dram_sdqs4 = 0x00000028,
	.dram_sdqs5 = 0x00000028,
	.dram_sdqs6 = 0x00000028,
	.dram_sdqs7 = 0x00000028,
	.dram_dqm0 = 0x00000028,
	.dram_dqm1 = 0x00000028,
	.dram_dqm2 = 0x00000028,
	.dram_dqm3 = 0x00000028,
	.dram_dqm4 = 0x00000028,
	.dram_dqm5 = 0x00000028,
	.dram_dqm6 = 0x00000028,
	.dram_dqm7 = 0x00000028,
};

/* configure MX6SOLO/DUALLITE mmdc GRP io registers */
struct mx6sdl_iomux_grp_regs mx6sdl_grp_ioregs = {
	.grp_ddr_type = 0x000c0000,
	.grp_ddrmode_ctl = 0x00020000,
	.grp_ddrpke = 0x00000000,
	.grp_addds = 0x00000028,
	.grp_ctlds = 0x00000028,
	.grp_ddrmode = 0x00020000,
	.grp_b0ds = 0x00000028,
	.grp_b1ds = 0x00000028,
	.grp_b2ds = 0x00000028,
	.grp_b3ds = 0x00000028,
	.grp_b4ds = 0x00000028,
	.grp_b5ds = 0x00000028,
	.grp_b6ds = 0x00000028,
	.grp_b7ds = 0x00000028,
};


const struct mx6_mmdc_calibration mx6qd_mmcd_4x256_calib = {
        .p0_mpwldectrl0 =  0x00710076,
        .p0_mpwldectrl1 =  0x00630059,
        .p1_mpwldectrl0 =  0x0030004E,
        .p1_mpwldectrl1 =  0x000D001A,

        .p0_mpdgctrl0 =  0x44280438,
        .p0_mpdgctrl1 =  0x04080400,
        .p1_mpdgctrl0 =  0x437C037C,
        .p1_mpdgctrl1 =  0x035C0338,

        .p0_mprddlctl =  0x423A3C3E,
        .p1_mprddlctl =  0x3C3C3646,
        .p0_mpwrdlctl =  0x383E403E,
        .p1_mpwrdlctl =  0x48364840,
};

const struct mx6_mmdc_calibration mx6qd_mmcd_4x512_calib = {
        .p0_mpwldectrl0 =  0x001F001F,
        .p0_mpwldectrl1 =  0x001F001F,
        .p1_mpwldectrl0 =  0x001F0001,
        .p1_mpwldectrl1 =  0x001F001F,

        .p0_mpdgctrl0 =  0x431A0326,
        .p0_mpdgctrl1 =  0x0323031B,
        .p1_mpdgctrl0 =  0x433F0340,
        .p1_mpdgctrl1 =  0x0345031C,

        .p0_mprddlctl =  0x40343137,
        .p1_mprddlctl =  0x40372F45,
        .p0_mpwrdlctl =  0x32414741,
        .p1_mpwrdlctl =  0x4731473C,
};

static struct mx6_mmdc_calibration mx6dl_1g_mmdc_calib = {
	.p0_mpwldectrl0 = 0x002F0038,
	.p0_mpwldectrl1 = 0x001F001F,
	.p1_mpwldectrl0 = 0x001F001F,
	.p1_mpwldectrl1 = 0x001F001F,
	.p0_mpdgctrl0 = 0x425C0251,
	.p0_mpdgctrl1 = 0x021B021E,
	.p1_mpdgctrl0 = 0x021B021E,
	.p1_mpdgctrl1 = 0x01730200,
	.p0_mprddlctl = 0x45474C45,
	.p1_mprddlctl = 0x44464744,
	.p0_mpwrdlctl = 0x3F3F3336,
	.p1_mpwrdlctl = 0x32383630,
};

/* Micron MT41J256M16RE-15E */
static struct mx6_ddr3_cfg mt41j256m16re_15e = {
	.mem_speed = 1333,
	.density = 4,
	.width = 16,
	.banks = 8,
	.rowaddr = 15,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1350,
	.trcmin = 4950,
	.trasmin = 3600,
};

/* Micron MT41J512M16RE-15E */ /* not confirmed yet*/
static struct mx6_ddr3_cfg mt41j512m16re_15e = {
	.mem_speed = 1333,
	.density = 4,
	.width = 16,
	.banks = 8,
	.rowaddr = 16,
	.coladdr = 10,
	.pagesz = 1,
	.trcd = 1350,
	.trcmin = 4950,
	.trasmin = 3600,
};

/* Micron MT41K128M16JT-125 */
static struct mx6_ddr3_cfg mt41k128m16jt_125 = {
	/* duallite */
	.mem_speed = 800,
	.density = 2,
	.width = 16,
	.banks = 8,
	.rowaddr = 14,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1375,
	.trcmin = 4875,
	.trasmin = 3500,
	.SRT = 0,
};

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0x00C03F3F, &ccm->CCGR0);
	writel(0x0030FC03, &ccm->CCGR1);
	writel(0x0FFFC000, &ccm->CCGR2);
	writel(0x3FF00000, &ccm->CCGR3);
	writel(0x00FFF300, &ccm->CCGR4);
	writel(0x0F0000C3, &ccm->CCGR5);
	writel(0x000003FF, &ccm->CCGR6);
	/* Enable USB HUB Clock on A62 Board */
	writel(0x010E0000, &ccm->ccosr);

}

static void gpr_init(void)
{
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

	/* enable AXI cache for VDOA/VPU/IPU */
	writel(0xF00000FF, &iomux->gpr[4]);
	/* set IPU AXI-id0 Qos=0xf(bypass) AXI-id1 Qos=0x7 */
	writel(0x007F007F, &iomux->gpr[6]);
	writel(0x007F007F, &iomux->gpr[7]);
}

static void spl_dram_init(void)
{
	
        struct mx6_ddr_sysinfo sysinfo = {
                .dsize = 2,             /* total bus width/32, i.e 4x16/32 */
                .cs_density = 32,
                .ncs = 1,
                .cs1_mirror = 0,
                .rtt_wr = 1,
                .rtt_nom = 2,           /* RTT_Nom = RZQ/2 */
                .walat = 1,             /* Write additional latency */
                .ralat = 5,             /* Read additional latency */
                .mif3_mode = 3,         /* Command prediction working mode */
                .bi_on = 1,             /* Bank interleaving enabled */
                .sde_to_rst = 0x10,     /* 14 cycles, 200us (JEDEC default) */
                .rst_to_cke = 0x23,     /* 33 cycles, 500us (JEDEC default) */
        };

	if (is_cpu_type(MXC_CPU_MX6Q)) {
#if (CONFIG_DDR_SIZE == SYS_DDR_SIZE_4x512)
		puts("Setting 2048MB RAM calibration data\n");
		mx6dq_dram_iocfg(64, &mx6_ddr_ioregs, &mx6_grp_ioregs);
	 	mx6_dram_cfg(&sysinfo, &mx6qd_mmcd_4x512_calib, &mt41j512m16re_15e);
		((struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL)->mdctl = 0x841A0000;
#elif (CONFIG_DDR_SIZE == SYS_DDR_SIZE_4x256)
		puts("Setting 1024MB RAM calibration data\n");
		mx6dq_dram_iocfg(64, &mx6_ddr_ioregs, &mx6_grp_ioregs);
		mx6_dram_cfg(&sysinfo, &mx6qd_mmcd_4x256_calib, &mt41j256m16re_15e);
		((struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL)->mdctl = 0x831A0000;
#else /*(CONFIG_DDR_SIZE == SYS_DDR_SIZE_2x256)*/
	/*TODO*/
#endif
	} else if (is_cpu_type(MXC_CPU_MX6DL)) {

#if (CONFIG_DDR_SIZE == SYS_DDR_SIZE_4x512)
	/*TODO*/
#elif (CONFIG_DDR_SIZE == SYS_DDR_SIZE_4x256)
		sysinfo.rtt_nom = 1;
		mx6sdl_dram_iocfg(64, &mx6sdl_ddr_ioregs, &mx6sdl_grp_ioregs);
		mx6_dram_cfg(&sysinfo, &mx6dl_1g_mmdc_calib, &mt41k128m16jt_125);
#else /* (CONFIG_DDR_SIZE == SYS_DDR_SIZE_2x256) */
	/*TODO*/		
#endif	
	}else{ /*MXC_CPU_MX6SOLO*/
		/* TODO */
	}	
}

#ifdef CONFIG_APX_WATCHDOG
static void init_func_watchdog_init(void)
{
	hw_watchdog_init();
	WATCHDOG_RESET();
}
#endif /* CONFIG_APX_WATCHDOG */

void board_init_f(ulong dummy)
{
	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	ccgr_init();
	gpr_init();

	/* iomux and setup of i2c */
	board_early_init_f();

#ifdef CONFIG_APX_WATCHDOG
	init_func_watchdog_init();
#endif

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* DDR initialization */
	spl_dram_init();


	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

#ifdef CONFIG_APX_WATCHDOG
	WATCHDOG_RESET();
#endif

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}
