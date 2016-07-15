/*
 * Copyright (C) 2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2015 UDOO Team
 *
 * Configuration settings for UDOO Quad/Dual board's Android Support.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef UDOO_QD_ANDROID_H
#define UDOO_QD_ANDROID_H

#define CONFIG_CI_UDC
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_DUALSPEED

#define CONFIG_USB_GADGET
#define CONFIG_CMD_USB_MASS_STORAGE
#define CONFIG_USB_GADGET_MASS_STORAGE
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_USB_GADGET_VBUS_DRAW	2

#define CONFIG_G_DNL_VENDOR_NUM		0x18d1
#define CONFIG_G_DNL_PRODUCT_NUM	0x0d02
#define CONFIG_G_DNL_MANUFACTURER	"FSL"

#define CONFIG_FASTBOOT_SATA_NO		 0

#define CONFIG_CMD_FASTBOOT
#define CONFIG_ANDROID_BOOT_IMAGE
#define CONFIG_FASTBOOT_FLASH

#define CONFIG_FSL_FASTBOOT
#define CONFIG_ANDROID_RECOVERY

#if defined CONFIG_SYS_BOOT_NAND
#define CONFIG_FASTBOOT_STORAGE_NAND
#elif defined CONFIG_SYS_BOOT_SATA
#define CONFIG_FASTBOOT_STORAGE_SATA
#else
#define CONFIG_FASTBOOT_STORAGE_MMC
#endif

#define CONFIG_ANDROID_MAIN_MMC_BUS 0
#define CONFIG_ANDROID_BOOT_PARTITION_MMC 1
#define CONFIG_ANDROID_SYSTEM_PARTITION_MMC 5
#define CONFIG_ANDROID_RECOVERY_PARTITION_MMC 2
#define CONFIG_ANDROID_CACHE_PARTITION_MMC 6
#define CONFIG_ANDROID_DATA_PARTITION_MMC 4

#if defined(CONFIG_FASTBOOT_STORAGE_NAND)
#define ANDROID_FASTBOOT_NAND_PARTS "16m@64m(boot) 16m@80m(recovery) 810m@96m(android_root)ubifs"
#endif

#define CONFIG_CMD_BOOTA
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_SERIAL_TAG

#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_BOOTCOMMAND

#define CONFIG_EXTRA_ENV_SETTINGS					\
	"kernel_loadaddr=0x14008000\0" \
	"fdt_loadaddr=0x14f00000\0" \
	"script=uEnv.txt\0" \
	"loadbootscript=" \
		"ext2load mmc 0:4 ${kernel_loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"env import -t ${kernel_loadaddr} ${filesize};\0" \
	"udoo_boot_init=" \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"fi; " \
		"udooinit; " \
		"ext2load mmc 0:5 ${fdt_loadaddr} ${fdt_file}; \0" \
	"splashpos=m,m\0"	  \
	"fdt_high=0xffffffff\0"	  \
	"initrd_high=0xffffffff\0" \
	"recoverycmd=run udoo_boot_init; boota mmc0 recovery\0" \

#define CONFIG_USB_FASTBOOT_BUF_ADDR   CONFIG_SYS_LOAD_ADDR
#define CONFIG_USB_FASTBOOT_BUF_SIZE   0x19000000

#endif /* UDOO_QD_ANDROID_H */
