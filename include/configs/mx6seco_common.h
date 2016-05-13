/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#ifndef __MX6SECO_COMMON_CONFIG_H
#define __MX6SECO_COMMON_CONFIG_H

#define CONFIG_MX6

#ifdef CONFIG_MX6SOLO
#define CONFIG_MX6DL
#endif

/* uncomment for PLUGIN mode support */
/* #define CONFIG_USE_PLUGIN */

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

#include "mx6_common.h"
#include <linux/sizes.h>


#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#ifdef CONFIG_SPL
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SPL_WATCHDOG_SUPPORT
#include "imx6_spl.h"
#endif

/* Common Seco system environment */
#define  DDR_SIZE_2x256   0
#define  DDR_SIZE_2x512   1
#define  DDR_SIZE_4x256   2
#define  DDR_SIZE_4x512   3
#define  DDR_SIZE_8x512   4

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(10 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO


#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* Command definition */
#include <config_cmd_default.h>

#define CONFIG_CMD_BMODE
#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_SETEXPR
#undef CONFIG_CMD_IMLS

#define CONFIG_BOOTDELAY               3

#define CONFIG_LOADADDR                0x12000000
#define CONFIG_FDT_LOADADDR            0x18000000
#define CONFIG_SYS_TEXT_BASE           0x17800000

#define CONFIG_SYS_LOAD_ADDR	       CONFIG_LOADADDR

#define CONFIG_SPI_KERNEL_LOADADDR     0x080000
#define CONFIG_SPI_KERNEL_LEN          0x380000

#define CONFIG_ARP_TIMEOUT     200UL

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2     "> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE              1024

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START       0x10000000
#define CONFIG_SYS_MEMTEST_END         0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH     0x10800000

#define CONFIG_SYS_LOAD_ADDR           CONFIG_LOADADDR

#define CONFIG_STACKSIZE               (128 * 1024)


#ifndef CONFIG_SYS_NOSMP
#define CONFIG_SYS_NOSMP
#endif

#if defined CONFIG_SYS_BOOT_SPINOR
#define CONFIG_SYS_USE_SPINOR
#define CONFIG_ENV_IS_IN_SPI_FLASH
#elif defined CONFIG_SYS_BOOT_EIMNOR
#define CONFIG_SYS_USE_EIMNOR
#define CONFIG_ENV_IS_IN_FLASH
#elif defined CONFIG_SYS_BOOT_NAND
#define CONFIG_SYS_USE_NAND
#define CONFIG_ENV_IS_IN_NAND
#elif defined CONFIG_SYS_BOOT_SATA
#define CONFIG_ENV_IS_IN_SATA
#define CONFIG_CMD_SATA
#else
#define CONFIG_ENV_IS_IN_MMC
#endif


#define CONFIG_OF_LIBFDT

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

/* ____________________________________________________________________________
  |                                                                            |
  |                            PHYSICAL MEMORY MAP                             |
  |____________________________________________________________________________|
*/
#define CONFIG_NR_DRAM_BANKS           1
#define PHYS_SDRAM                     MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE          PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR       IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE       IRAM_SIZE

#if (CONFIG_DDR_SIZE == 0)
	#define CONFIG_DDR_32BIT
#elif (CONFIG_DDR_SIZE > 0)
	#define CONFIG_DDR_64BIT
#endif

#if (CONFIG_DDR_SIZE == 0)
	#define PHYS_SDRAM_SIZE        (512u * 1024 * 1024)
#endif

#if (CONFIG_DDR_SIZE == 1 || CONFIG_DDR_SIZE == 2)
	#define PHYS_SDRAM_SIZE        (1u * 1024 * 1024 * 1024)
#endif

#if (CONFIG_DDR_SIZE == 3)
	#define PHYS_SDRAM_SIZE        (2u * 1024 * 1024 * 1024)
#endif

#if (CONFIG_DDR_SIZE == 4)
	#define PHYS_SDRAM_SIZE        (4u * 1024 * 1024 * 1024 - 256u * 1024 * 1024)
#endif

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* ____________________________________________________________________________
  |                                                                            |
  |                                     TAGs                                   |
  |____________________________________________________________________________|
*/
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

/* ____________________________________________________________________________
  |                                                                            |
  |                                   DEBUG UART                               |
  |____________________________________________________________________________|
*/
#define CONFIG_MXC_UART
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

/* ____________________________________________________________________________
  |                                                                            |
  |                                DISPLAY INFO                                |
  |____________________________________________________________________________|
*/
#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* ____________________________________________________________________________
  |                                                                            |
  |                                 MMC / SD                                   |
  |____________________________________________________________________________|
*/
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR      0

#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */

/* ____________________________________________________________________________
  |                                                                            |
  |                                       SPI                                  |
  |____________________________________________________________________________|
*/
#ifdef CONFIG_SYS_USE_SPI_BUS

#ifdef CONFIG_SYS_USE_SPINOR
	#define CONFIG_CMD_SF
	#define CONFIG_SPI_FLASH
	#define CONFIG_MXC_SPI
	#define CONFIG_SF_DEFAULT_SPEED        20 * 1000 * 1000
	#define CONFIG_SF_DEFAULT_MODE         (SPI_MODE_0)
	#define CONFIG_SYS_MAX_FLASH_BANKS 1    /* max number of memory banks */
#endif

#ifdef CONFIG_RTC_LOW_POWER 
	#define CONFIG_CMD_RTC
	#define CONFIG_RTC_PCF2123
	#define CONFIG_RTC_SNVS
#endif

#endif

/* ____________________________________________________________________________
  |                                                                            |
  |                                       I2C                                  |
  |____________________________________________________________________________|
*/
#ifdef CONFIG_SYS_USE_I2C_BUS

	#define CONFIG_SYS_I2C
	#define CONFIG_SYS_I2C_MXC
	#define CONFIG_CMD_I2C
	#define CONFIG_HARD_I2C                1
	#define CONFIG_I2C_MXC                 1

#endif

/* ____________________________________________________________________________
  |                                                                            |
  |                                     SATA                                   |
  |____________________________________________________________________________|
*/
#ifdef CONFIG_SYS_USE_SATA

	#define  CONFIG_CMD_SATA
	#define CONFIG_DWC_AHSATA
	#define CONFIG_DWC_AHSATA_BASE_ADDR           SATA_ARB_BASE_ADDR
	#define CONFIG_LBA48
	#define CONFIG_LIBATA

#endif

/* ____________________________________________________________________________
  |                                                                            |
  |                                    ETHERNET                                |
  |____________________________________________________________________________|
*/
#ifdef CONFIG_SYS_USE_ETHERNET

	#define CONFIG_CMD_PING
	#define CONFIG_CMD_DHCP
	#define CONFIG_CMD_MII
	#define CONFIG_CMD_NET
	#define CONFIG_FEC_MXC
	#define CONFIG_MII
	#define IMX_FEC_BASE			ENET_BASE_ADDR
	#define CONFIG_ETHPRIME			"FEC"
	#define CONFIG_PHYLIB

#endif

/* ____________________________________________________________________________
  |                                                                            |
  |                                ENVIRONMENT                                 |
  |____________________________________________________________________________|
*/
#define CONFIG_ENV_SIZE			(8 * 1024)

#if defined(CONFIG_ENV_IS_IN_MMC)

	#define CONFIG_ENV_OFFSET              (8 * 64 * 1024)

#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)

	#define CONFIG_SYS_MAX_FLASH_SECT	256
	#define CONFIG_ENV_OFFSET              (768 * 1024)
	#define CONFIG_ENV_SECT_SIZE           (64 * 1024)
	#define CONFIG_ENV_SPI_BUS             CONFIG_SF_DEFAULT_BUS
	#define CONFIG_ENV_SPI_CS              CONFIG_SF_DEFAULT_CS
	#define CONFIG_ENV_SPI_MODE            CONFIG_SF_DEFAULT_MODE
	#define CONFIG_ENV_SPI_MAX_HZ          CONFIG_SF_DEFAULT_SPEED

#elif defined(CONFIG_ENV_IS_IN_FLASH)

	#undef CONFIG_ENV_SIZE 
	#define CONFIG_ENV_SIZE                CONFIG_SYS_FLASH_SECT_SIZE
	#define CONFIG_ENV_SECT_SIZE           CONFIG_SYS_FLASH_SECT_SIZE
	#define CONFIG_ENV_OFFSET              (4 * CONFIG_SYS_FLASH_SECT_SIZE)

#elif defined(CONFIG_ENV_IS_IN_NAND)

	#undef CONFIG_ENV_SIZE
	#define CONFIG_ENV_OFFSET              (8 << 20)
	#define CONFIG_ENV_SECT_SIZE           (128 << 10)
	#define CONFIG_ENV_SIZE                CONFIG_ENV_SECT_SIZE

#elif defined(CONFIG_ENV_IS_IN_SATA)

	#define CONFIG_ENV_OFFSET              (768 * 1024)
	#define CONFIG_SATA_ENV_DEV            0
	#define CONFIG_SYS_DCACHE_OFF /* remove when sata driver support cache */

#endif

/* ____________________________________________________________________________
  |                                                                            |
  |                                    PROMPT                                  |
  |____________________________________________________________________________|
*/
#ifndef CONFIG_MX6QDL
#define CONFIG_SYS_PROMPT              CONFIG_SECO_PROMPT
#endif
#define CONFIG_AUTO_COMPLETE

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE              (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	       256
#define CONFIG_SYS_BARGSIZE            CONFIG_SYS_CBSIZE

#define CONFIG_SYS_HZ                  1000

#define CONFIG_CMDLINE_EDITING

/* ____________________________________________________________________________
  |                                                                            |
  |                              DISPLAY (FRAMEBUFFER)                         |
  |____________________________________________________________________________|
*/
#ifdef CONFIG_USE_DISPLAY
	#define CONFIG_VIDEO
	#define CONFIG_VIDEO_IPUV3
	#define CONFIG_CFB_CONSOLE
	#define CONFIG_VGA_AS_SINGLE_DEVICE
	#define CONFIG_SYS_CONSOLE_IS_IN_ENV
	#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
	#define CONFIG_VIDEO_BMP_RLE8
	#define CONFIG_SPLASH_SCREEN
	#define CONFIG_SPLASH_SCREEN_ALIGN
	#define CONFIG_BMP_16BPP
	#define CONFIG_VIDEO_LOGO
	#define CONFIG_VIDEO_BMP_LOGO
	#ifdef CONFIG_MX6DL
	#define CONFIG_IPUV3_CLK 198000000
	#else
	#define CONFIG_IPUV3_CLK 264000000
	#endif
	#define CONFIG_IMX_HDMI
#endif  /*  CONFIG_USE_DISPLAY  */

/* ____________________________________________________________________________
  |                                                                            |
  |                                   ENVIRONMENT                              |
  |____________________________________________________________________________|
*/

#define CONFIG_SYS_MMC_IMG_LOAD_PART   1

#define CONFIG_ROOT_DEV_ID       1
#define CONFIG_ROOT_PARTITION    1

#define CONFIG_BOOTARGS_BASE  "setenv bootargs ${console_interface} ${memory} ${cpu_freq} ${videomode} ${root_dev}"


#define CONFIG_ENV_COMMON                                        \
	"bootdelay="__stringify(CONFIG_BOOTDELAY)"\0"            \
	"recoveryboot=0\0"                                         \
	"stdin=serial\0"                                         \
	"stdout=serial\0"                                        \
	"stderr=serial\0"


#define CONFIG_ENV_BOOTARG_BASE                                                                               \
	"console_interface='console=" CONFIG_CONSOLE_DEV "," __stringify(CONFIG_BAUDRATE)"'\0"                \
	"videomode=video=mxcfb0:dev=hdmi,1920x1080M@60\0"                                                     \
	"bootargs_base="CONFIG_BOOTARGS_BASE"\0"   

#define CONFIG_FASTBOOT_DEVICE			\
	"fastboot_dev=mmc1\0"

#define CONFIG_ENV_KERNEL                                                                                                                   \
	"kernel_device_id="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0"                                                                          \
        "kernel_partition="__stringify(CONFIG_SYS_MMC_IMG_LOAD_PART)"\0"                                                                    \
        "kernel_loadaddr="__stringify(CONFIG_LOADADDR)"\0"                                                                                  \
	"kernel_file=zImage\0"                                                                                                              \
	"kernel_load=mmc dev ${kernel_device_id}; fatload mmc ${kernel_device_id}:${kernel_partition} ${kernel_loadaddr} ${kernel_file}\0" 


#define CONFIG_ENV_FDT                                                                                                      \
	"fdt_device_id="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0"                                                             \
        "fdt_partition="__stringify(CONFIG_SYS_MMC_IMG_LOAD_PART)"\0"                                                       \
	"fdt_loadaddr="__stringify(CONFIG_FDT_LOADADDR)"\0"                                                                 \
	"fdt_file=" CONFIG_DEFAULT_FDT_FILE "\0"                                                                            \
	"fdt_high=0xffffffff\0"	                                                                                            \
	"fdt_load=mmc dev ${fdt_device_id}; fatload mmc ${fdt_device_id}:${fdt_partition} ${fdt_loadaddr} ${fdt_file}\0"  


#define CONFIG_ENV_ROOT                                                                                   \
	"root_device_id="__stringify(CONFIG_ROOT_DEV_ID)"\0"                                              \
	"root_partition="__stringify(CONFIG_ROOT_PARTITION)"\0"                                           \
	"root_set_dev=setenv root_dev root=/dev/mmcblk${root_device_id}p${root_partition} rootwait rw\0"  \
	"root_add_env=setenv bootargs ${bootargs} ${root_dev}\0"
	
#define CONFIG_MMC_TEST                                                                                   \
	"mmc_test=if ext2load mmc 1 10600000 /boot/uEnv.txt; then env import -t 10600000 1600; fi;\0"

#define CONFIG_USB_TEST                                                                                   \
	"usb_test=usb start; if ext2load usb 0 10600000 /boot/uEnv.txt; then env import -t 10600000 1600; fi;\0"

#if defined(CONFIG_CMD_SET_BOOT_ANDROID)
#define CONFIG_CMD_SET_BOOT  "run mmc_test; run usb_test; if test ${recoveryboot} = 1 ; then boot; else boota mmc1; fi"
#else
#define CONFIG_CMD_SET_BOOT                                             \
	"run bootargs_base; "                                           \
	"if test \"${run_from_nfs}\" = \"0\"; then "                    \
		"echo Running in Normal Mode... ; "                     \
	"else "                                                         \
		"echo Running in Remote... ; "                          \
		"run set_ip; "                                          \
	"fi;"                                                           \
	"run root_set_dev; "                                            \
	"run root_add_env; "                                            \
	"run kernel_load; "                                             \
	"run fdt_load; "                                                \
	"bootz ${kernel_loadaddr} - ${fdt_loadaddr}"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS               \
	CONFIG_ENV_COMMON			\
	CONFIG_ENV_BOOTARG_BASE                 \
	CONFIG_FASTBOOT_DEVICE                  \
	CONFIG_ENV_KERNEL                       \
	CONFIG_ENV_FDT                          \
	CONFIG_ENV_ROOT                        	\
	CONFIG_MMC_TEST                        	\
	CONFIG_USB_TEST                        	\
	"bootcmd=" CONFIG_CMD_SET_BOOT "\0"	

#ifdef CONFIG_ANDROID_SUPPORT
#include "mx6seco_android_common.h"
#endif

#endif     /* __MX6SECO_COMMON_CONFIG_H */
