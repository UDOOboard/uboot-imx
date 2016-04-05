/*
 * apx_watchdog.c - driver for i.MX6 a66 Apx Watchdog
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <asm/io.h>
#include <watchdog.h>
#include <asm/arch/imx-regs.h>

#ifdef CONFIG_APX_WATCHDOG

#define WDT_IOMUX_REG(x)        ((x))
#define WDT_PADCTRL_REG(x)      ((x))
#define WDT_GDIR_REG(x)         ((x) + 0x4) 
#define WDT_DR_REG(x)           ((x))

 
#define WDT_PAD_MUX_GPIO(x)     writel(readl(WDT_IOMUX_REG((x))) | 0x5, WDT_IOMUX_REG((x)))

#define WDT_PAD_CTRL_GPIO(x)    writel(0x06028, WDT_PADCTRL_REG((x)))

#define WDT_DIR_OUT_GPIO(x,n)   writel(readl(WDT_GDIR_REG((x))) | ( 1 << (n)), WDT_GDIR_REG((x)))

#define WDT_SET_H_GPIO(x,n)     writel(readl(WDT_DR_REG((x))) | (1 << (n)) , WDT_DR_REG((x)))

#define WDT_SET_L_GPIO(x,n)     writel(readl(WDT_DR_REG((x))) & ~(1 << (n)) , WDT_DR_REG((x)))


void apx_watchdog_reset(void)
{
	//APX Watchdog - Refresh watchdog 

   	WDT_DIR_OUT_GPIO(APX_WDT_TRIGGER_BASE, APX_WDT_TRIGGER_NUM);
	WDT_SET_H_GPIO(APX_WDT_TRIGGER_BASE, APX_WDT_TRIGGER_NUM);
	WDT_SET_L_GPIO(APX_WDT_TRIGGER_BASE, APX_WDT_TRIGGER_NUM);
}

void apx_watchdog_init(void)
{
   	WDT_DIR_OUT_GPIO(APX_WDT_TRIGGER_BASE, APX_WDT_TRIGGER_NUM);
	WDT_DIR_OUT_GPIO(APX_WDT_ENABLE_BASE, APX_WDT_ENABLE_NUM);

	WDT_SET_L_GPIO(APX_WDT_TRIGGER_BASE, APX_WDT_TRIGGER_NUM);
}
#endif

