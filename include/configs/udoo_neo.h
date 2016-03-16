/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright Jasbir Matharu
 * Copyright 2015 UDOO Team
 *
 * Configuration settings for the UDOO NEO board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/imx-regs.h>
#include <linux/sizes.h>
#include "mx6_common.h"
#include <asm/imx-common/gpio.h>

#define CONFIG_MX6
#define CONFIG_ROM_UNIFIED_SECTIONS
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_DBG_MONITOR
/* uncomment for PLUGIN mode support */
/* #define CONFIG_USE_PLUGIN */

#ifdef CONFIG_SPL
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_MMC_SUPPORT
#include "imx6_spl.h"
#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SYS_GENERIC_BOARD

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO

#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_BASE

/* MMC Configuration */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_USDHC_NUM	2

#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#undef CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */

#define CONFIG_BAUDRATE			115200

#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS

/* Needed for uEnv.txt support */
/*#undef CONFIG_CMD_EXPORTENV*/
/*#undef CONFIG_CMD_IMPORTENV*/

/* Network */
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
/* #define CONFIG_FEC_MXC */
#define CONFIG_MII
#define CONFIG_FEC_ENET_DEV 0
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_MXC_PHYADDR          0x1
#define CONFIG_FEC_XCV_TYPE             RGMII
#define CONFIG_ETHPRIME                 "FEC"

#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_FEC_DMA_MINALIGN		64

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1

/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		100000

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE3000
#define CONFIG_POWER_PFUZE3000_I2C_ADDR	0x08
#define PFUZE3000_I2C_BUS	0
#define PFUZE3000_SW1AB_SETP(x)    ((x - 7000) / 250)
#define PFUZE3000_SW3_SETP(x)      ((x - 9000) / 500)
#define PFUZE3000_VLDO_SETP(x)     ((x - 8000) / 500)

/* Command definition */
#include <config_cmd_default.h>

#undef CONFIG_CMD_IMLS

#define CONFIG_BOOTDELAY		3

#define CONFIG_LOADADDR			0x80800000
#define CONFIG_SYS_TEXT_BASE		0x87800000

#define CONFIG_SYS_AUXCORE_BOOTDATA 0x78000000 /* Set to QSPI2 B flash at default */
#ifndef CONFIG_SYS_AUXCORE_FASTUP
#define CONFIG_CMD_BOOTAUX /* Boot M4 by command, disable this when M4 fast up */
#define CONFIG_CMD_SETEXPR
#endif

#ifdef CONFIG_CMD_BOOTAUX
#define M4_STARTUP_ENV \
	"m4last=/var/opt/m4/m4last.fw\0" \
	"m4fw=/m4startup.fw\0" \
	"mmcrootpart=2\0" \
	"m4last_cmd=ext2load mmc ${mmcdev}:${mmcrootpart} 0x84000000 ${m4last};\0" \
	"m4fw_cmd=fatload mmc ${mmcdev}:${mmcpart} 0x84000000 ${m4fw}; bootaux 0x84000000\0" \
	"m4boot=if run m4last_cmd ; then bootaux 0x84000000 ; else run m4fw_cmd ; fi\0" \
	"m4mmcargs=uart_from_osc clk_ignore_unused cpuidle.off=1\0"
#else
#define M4_STARTUP_ENV \
	"m4boot=\0" \
	"m4mmcargs=\0"
#endif

#ifdef CONFIG_VIDEO
#define CONFIG_VIDEO_MODE \
	"panel=Hannstar-XGA\0"
#else
#define CONFIG_VIDEO_MODE ""
#endif

/* Linux only */
#define CONFIG_MFG_ENV_SETTINGS \
	"mfgtool_args=setenv bootargs console=${console},${baudrate} " \
		"rdinit=/linuxrc " \
		"g_mass_storage.stall=0 g_mass_storage.removable=1 " \
		"g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF "\
		"g_mass_storage.iSerialNumber=\"\" "\
		"\0" \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffff\0" \
	"bootcmd_mfg=run mfgtool_args;bootz ${loadaddr} ${initrd_addr} ${fdt_addr};\0" \

/* Linux only */
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	M4_STARTUP_ENV \
	CONFIG_VIDEO_MODE \
	"script=uEnv.txt\0" \
	"image=/zImage\0" \
	"console=ttymxc0\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_file=autodetect\0" \
	"fdt_addr=0x83000000\0" \
	"boot_fdt=try\0" \
	"ip_dyn=yes\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=1\0" \
	"mmcrootfstype=ext4\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcautodetect=no\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot} rootfstype=${mmcrootfstype} ${m4mmcargs} consoleblank=0\0" \
	"loadbootscript=" \
		"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"env import -t ${loadaddr} ${filesize};\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run loadfdt; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0" \
	"netargs=setenv bootargs console=${console},${baudrate} " \
		"root=/dev/nfs " \
	"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
		"netboot=echo Booting from net ...; " \
		"run netargs; " \
		"if test ${ip_dyn} = yes; then " \
			"setenv get_cmd dhcp; " \
		"else " \
			"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${image}; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if ${get_cmd} ${fdt_addr} ${fdt_file}; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0"

/* Linux only */
#define CONFIG_BOOTCOMMAND \
	"mmc dev ${mmcdev}; " \
	"if mmc rescan; then " \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"fi; " \
		"udooinit; " \
		"run m4boot; " \
		"if run loadimage; then " \
			"run mmcboot; " \
		"else run netboot; " \
		"fi; " \
	"else run netboot; fi"

#define CONFIG_SYS_BOOTM_LEN	(16 << 20)

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT		"=> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		1024

#define CONFIG_SYS_MAXARGS		256
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x10000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#define CONFIG_CMDLINE_EDITING
#define CONFIG_STACKSIZE		SZ_128K

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR
#define PHYS_SDRAM_SIZE			SZ_1G

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_ENV_SIZE			SZ_8K

#ifdef CONFIG_SYS_AUXCORE_FASTUP
/*#define CONFIG_MXC_RDC*/   /* Disable the RDC temporarily, will enable it in future */
#define CONFIG_ENV_IS_IN_MMC  /* Must disable QSPI driver, because M4 run on QSPI */
#elif defined CONFIG_SYS_BOOT_QSPI
#define CONFIG_FSL_QSPI
#define CONFIG_ENV_IS_IN_SPI_FLASH
#else
#define CONFIG_FSL_QSPI   /* Enable the QSPI flash at default */
#define CONFIG_ENV_IS_IN_MMC
#endif

#ifdef CONFIG_FSL_QSPI
#define CONFIG_QSPI_BASE			QSPI2_BASE_ADDR
#define CONFIG_QSPI_MEMMAP_BASE		QSPI2_ARB_BASE_ADDR

#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_BAR
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_STMICRO
#define	CONFIG_SF_DEFAULT_BUS		0
#define	CONFIG_SF_DEFAULT_CS		0
#define	CONFIG_SF_DEFAULT_SPEED		40000000
#define	CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#endif

#define CONFIG_SYS_MMC_ENV_DEV		0  /*USDHC2*/
#define CONFIG_SYS_MMC_ENV_PART		0	/* user area */
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"  /* USDHC2 */

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_ENV_OFFSET		(8 * SZ_64K)
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
#define CONFIG_ENV_OFFSET		(768 * 1024)
#define CONFIG_ENV_SECT_SIZE		(64 * 1024)
#define CONFIG_ENV_SPI_BUS		CONFIG_SF_DEFAULT_BUS
#define CONFIG_ENV_SPI_CS		CONFIG_SF_DEFAULT_CS
#define CONFIG_ENV_SPI_MODE		CONFIG_SF_DEFAULT_MODE
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#endif

#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ

#define CONFIG_CMD_BMODE

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

#ifdef CONFIG_VIDEO
#define	CONFIG_CFB_CONSOLE
#define	CONFIG_VIDEO_MXS
#define	CONFIG_VIDEO_LOGO
#define	CONFIG_VIDEO_SW_CURSOR
#define	CONFIG_VGA_AS_SINGLE_DEVICE
#define	CONFIG_SYS_CONSOLE_IS_IN_ENV
#define	CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define	CONFIG_CMD_BMP
#define	CONFIG_BMP_16BPP
#define	CONFIG_VIDEO_BMP_RLE8
#define CONFIG_VIDEO_BMP_LOGO
#ifdef CONFIG_VIDEO_GIS
#define CONFIG_VIDEO_CSI
#define CONFIG_VIDEO_PXP
#define CONFIG_VIDEO_VADC
#endif
#endif

#define CONFIG_CMD_USB
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_MXC_USB_PORTSC  (PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS   0
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#endif

#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

#if defined(CONFIG_ANDROID_SUPPORT)
#define CONFIG_OF_LOAD_MANUALLY
#define CONFIG_CMD_BOOTI
#include "mx6sxsabresdandroid.h"
#endif

#if defined(CONFIG_ANDROID_SUPPORT)
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"kernel_loadaddr=0x84808000\0" \
	"fdt_loadaddr=0x85700000\0" \
	"script=uEnv.txt\0" \
	"loadbootscript=" \
		"ext2load mmc 0:5 ${kernel_loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"env import -t ${kernel_loadaddr} ${filesize};\0" \
	"udoo_boot_init=" \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"fi; " \
		"udooinit; " \
		"ext2load mmc 0:5 ${fdt_loadaddr} ${fdt_file}; \0" \
	"splashpos=m,m\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0"
#endif

#define CONFIG_CMD_TIME

#endif				/* __CONFIG_H */
