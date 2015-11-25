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
#include <spl.h>
#include <asm/arch/mx6-ddr.h>
#include "detectboard.h"

const struct mx6sx_iomux_ddr_regs mx6_ddr_ioregs = {
	.dram_dqm0 = 0x00000028,
	.dram_dqm1 = 0x00000028,
	.dram_dqm2 = 0x00000028,
	.dram_dqm3 = 0x00000028,
	.dram_ras = 0x00000020,
	.dram_cas = 0x00000020,
	.dram_odt0 = 0x00000020,
	.dram_odt1 = 0x00000020,
	.dram_sdba2 = 0x00000000,
	.dram_sdcke0 = 0x00003000,
	.dram_sdcke1 = 0x00003000,
	.dram_sdclk_0 = 0x00000030,
	.dram_sdqs0 = 0x00000028,
	.dram_sdqs1 = 0x00000028,
	.dram_sdqs2 = 0x00000028,
	.dram_sdqs3 = 0x00000028,
	.dram_reset = 0x00000020,
};

const struct mx6sx_iomux_grp_regs mx6_grp_ioregs = {
	.grp_addds = 0x00000020,
	.grp_ddrmode_ctl = 0x00020000,
	.grp_ddrpke = 0x00000000,
	.grp_ddrmode = 0x00020000,
	.grp_b0ds = 0x00000028,
	.grp_b1ds = 0x00000028,
	.grp_ctlds = 0x00000020,
	.grp_ddr_type = 0x000c0000,
	.grp_b2ds = 0x00000028,
	.grp_b3ds = 0x00000028,
};

const struct mx6_mmdc_calibration neo_mmcd_calib = {
	.p0_mpwldectrl0 = 0x000E000B,
	.p0_mpwldectrl1 = 0x000E0010,
	.p0_mpdgctrl0 = 0x41600158,
	.p0_mpdgctrl1 = 0x01500140,
	.p0_mprddlctl = 0x3A383E3E,
	.p0_mpwrdlctl = 0x3A383C38,
};

const struct mx6_mmdc_calibration neo_basic_mmcd_calib = {
	.p0_mpwldectrl0 = 0x001E0022,
	.p0_mpwldectrl1 = 0x001C0019,
	.p0_mpdgctrl0 = 0x41540150,
	.p0_mpdgctrl1 = 0x01440138,
	.p0_mprddlctl = 0x403E4644,
	.p0_mpwrdlctl = 0x3C3A4038,
};

/* MT41K256M16 */
static struct mx6_ddr3_cfg neo_mem_ddr = {
	.mem_speed = 1600,
	.density = 4,
	.width = 32,
	.banks = 8,
	.rowaddr = 15,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1375,
	.trcmin = 4875,
	.trasmin = 3500,
};

/* MT41K128M16 */
static struct mx6_ddr3_cfg neo_basic_mem_ddr = {
	.mem_speed = 1600,
	.density = 2,
	.width = 16,
	.banks = 8,
	.rowaddr = 14,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1375,
	.trcmin = 4875,
	.trasmin = 3500,
};

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0xFFFFFFFF, &ccm->CCGR0);
	writel(0xFFFFFFFF, &ccm->CCGR1);
	writel(0xFFFFFFFF, &ccm->CCGR2);
	writel(0xFFFFFFFF, &ccm->CCGR3);
	writel(0xFFFFFFFF, &ccm->CCGR4);
	writel(0xFFFFFFFF, &ccm->CCGR5);
	writel(0xFFFFFFFF, &ccm->CCGR6);
	writel(0xFFFFFFFF, &ccm->CCGR7);
}

static void spl_dram_init(void)
{
	int board = detect_board();
	
	struct mx6_ddr_sysinfo sysinfo = {
		.dsize = neo_mem_ddr.width/32,
		.cs_density = 24,
		.ncs = 1,
		.cs1_mirror = 0,
		.rtt_wr = 2,
		.rtt_nom = 2,		/* RTT_Nom = RZQ/2 */
		.walat = 1,		/* Write additional latency */
		.ralat = 5,		/* Read additional latency */
		.mif3_mode = 3,		/* Command prediction working mode */
		.bi_on = 1,		/* Bank interleaving enabled */
		.sde_to_rst = 0x10,	/* 14 cycles, 200us (JEDEC default) */
		.rst_to_cke = 0x23,	/* 33 cycles, 500us (JEDEC default) */
	};

	if (board == UDOO_NEO_TYPE_BASIC || board == UDOO_NEO_TYPE_BASIC_KS) {
		puts("Seting 512MB RAM calibration data\n");
		mx6sx_dram_iocfg(neo_basic_mem_ddr.width, &mx6_ddr_ioregs, &mx6_grp_ioregs);
		sysinfo.dsize = neo_basic_mem_ddr.width/32;
		mx6_dram_cfg(&sysinfo, &neo_basic_mmcd_calib, &neo_basic_mem_ddr);
		((struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL)->mdctl = 0x83190000;
	} else {
		puts("Setting 1024MB RAM calibration data\n");
		mx6sx_dram_iocfg(neo_mem_ddr.width, &mx6_ddr_ioregs, &mx6_grp_ioregs);
		mx6_dram_cfg(&sysinfo, &neo_mmcd_calib, &neo_mem_ddr);
		((struct mmdc_p_regs *)MX6_MMDC_P0_MDCTL)->mdctl = 0x84190000;
	}
}

void board_init_f(ulong dummy)
{
	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	ccgr_init();

	/* iomux and setup of i2c */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* DDR initialization */
	spl_dram_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}

void reset_cpu(ulong addr)
{
}
