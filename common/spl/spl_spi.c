/*
 * Copyright (C) 2011 OMICRON electronics GmbH
 *
 * based on drivers/mtd/nand/nand_spl_load.c
 *
 * Copyright (C) 2011
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <errno.h>
#include <common.h>
#include <spi.h>
#include <spi_flash.h>
#include <spl.h>
#include "sf_internal.h"


DECLARE_GLOBAL_DATA_PTR;

/* Read commands array */
static u8 spi_read_cmds_array[] = {
	CMD_READ_ARRAY_SLOW,
	CMD_READ_ARRAY_FAST,
	CMD_READ_DUAL_OUTPUT_FAST,
	CMD_READ_DUAL_IO_FAST,
	CMD_READ_QUAD_OUTPUT_FAST,
	CMD_READ_QUAD_IO_FAST,
};

#ifdef CONFIG_SPL_OS_BOOT
/*
 * Load the kernel, check for a valid header we can parse, and if found load
 * the kernel and then device tree.
 */
static int spi_load_image_os(struct spi_flash *flash,
			     struct image_header *header)
{
	/* Read for a header, parse or error out. */
	spi_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS, 0x40,
		       (void *)header);

	if (image_get_magic(header) != IH_MAGIC)
		return -1;

	spl_parse_image_header(header);

	spi_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS,
		       spl_image.size, (void *)spl_image.load_addr);

	/* Read device tree. */
	spi_flash_read(flash, CONFIG_SYS_SPI_ARGS_OFFS,
		       CONFIG_SYS_SPI_ARGS_SIZE,
		       (void *)CONFIG_SYS_SPL_ARGS_ADDR);

	return 0;
}
#endif


#ifdef CONFIG_SPI_FLASH_MACRONIX
static int spi_flash_set_qeb_mxic(struct spi_flash *flash)
{
	u8 qeb_status;
	int ret;

	ret = spi_flash_cmd_read_status(flash, &qeb_status);
	if (ret < 0)
		return ret;

	if (qeb_status & STATUS_QEB_MXIC) {
		debug("SF: mxic: QEB is already set\n");
	} else {
		ret = spi_flash_cmd_write_status(flash, STATUS_QEB_MXIC);
		if (ret < 0)
			return ret;
	}

	return ret;
}
#endif

#if defined(CONFIG_SPI_FLASH_SPANSION) || defined(CONFIG_SPI_FLASH_WINBOND)
static int spi_flash_set_qeb_winspan(struct spi_flash *flash)
{
	u8 qeb_status;
	int ret;

	ret = spi_flash_cmd_read_config(flash, &qeb_status);
	if (ret < 0)
		return ret;

	if (qeb_status & STATUS_QEB_WINSPAN) {
		debug("SF: winspan: QEB is already set\n");
	} else {
		ret = spi_flash_cmd_write_config(flash, STATUS_QEB_WINSPAN);
		if (ret < 0)
			return ret;
	}

	return ret;
}
#endif

static int spi_flash_set_qeb(struct spi_flash *flash, u8 idcode0)
{
	switch (idcode0) {
#ifdef CONFIG_SPI_FLASH_MACRONIX
	case SPI_FLASH_CFI_MFR_MACRONIX:
		return spi_flash_set_qeb_mxic(flash);
#endif
#if defined(CONFIG_SPI_FLASH_SPANSION) || defined(CONFIG_SPI_FLASH_WINBOND)
	case SPI_FLASH_CFI_MFR_SPANSION:
	case SPI_FLASH_CFI_MFR_WINBOND:
		return spi_flash_set_qeb_winspan(flash);
#endif
#ifdef CONFIG_SPI_FLASH_STMICRO
	case SPI_FLASH_CFI_MFR_STMICRO:
		debug("SF: QEB is volatile for %02x flash\n", idcode0);
		return 0;
#endif
	default:
		printf("SF: Need set QEB func for %02x flash\n", idcode0);
		return -1;
	}
}


#ifdef CONFIG_SYS_SPI_ST_ENABLE_WP_PIN
/* enable the W#/Vpp signal to disable writing to the status register */
static int spi_enable_wp_pin(struct spi_flash *flash)
{
	u8 status;
	int ret;

	ret = spi_flash_cmd_read_status(flash, &status);
	if (ret < 0)
		return ret;

	ret = spi_flash_cmd_write_status(flash, STATUS_SRWD);
	if (ret < 0)
		return ret;

	ret = spi_flash_cmd_write_disable(flash);
	if (ret < 0)
		return ret;

	return 0;
}
#else
static int spi_enable_wp_pin(struct spi_flash *flash)
{
	return 0;
}
#endif


static int spi_flash_validate_params(struct spi_slave *spi, u8 *idcode,
				     struct spi_flash *flash)
{
	const struct spi_flash_params *params;
	u8 cmd;
	u16 jedec = idcode[1] << 8 | idcode[2];
	u16 ext_jedec = idcode[3] << 8 | idcode[4];

	/* Validate params from spi_flash_params table */
	params = spi_flash_params_table;
	for (; params->name != NULL; params++) {
		if ((params->jedec >> 16) == idcode[0]) {
			if ((params->jedec & 0xFFFF) == jedec) {
				if (params->ext_jedec == 0)
					break;
				else if (params->ext_jedec == ext_jedec)
					break;
			}
		}
	}

	if (!params->name) {
		printf("SF: Unsupported flash IDs: ");
		printf("manuf %02x, jedec %04x, ext_jedec %04x\n",
		       idcode[0], jedec, ext_jedec);
		return -EPROTONOSUPPORT;
	}

	/* Assign spi data */
	flash->spi = spi;
	flash->name = params->name;
	flash->memory_map = spi->memory_map;
	flash->dual_flash = flash->spi->option;

	/* Assign spi_flash ops */
#ifndef CONFIG_DM_SPI_FLASH
	flash->write = spi_flash_cmd_write_ops;
#if defined(CONFIG_SPI_FLASH_SST)
	if (params->flags & SST_WR) {
		if (flash->spi->op_mode_tx & SPI_OPM_TX_BP)
			flash->write = sst_write_bp;
		else
			flash->write = sst_write_wp;
	}
#endif
	flash->erase = spi_flash_cmd_erase_ops;
	flash->read = spi_flash_cmd_read_ops;
#endif

	/* Compute the flash size */
	flash->shift = (flash->dual_flash & SF_DUAL_PARALLEL_FLASH) ? 1 : 0;
	/*
	 * The Spansion S25FL032P and S25FL064P have 256b pages, yet use the
	 * 0x4d00 Extended JEDEC code. The rest of the Spansion flashes with
	 * the 0x4d00 Extended JEDEC code have 512b pages. All of the others
	 * have 256b pages.
	 */
	if (ext_jedec == 0x4d00) {
		if ((jedec == 0x0215) || (jedec == 0x216))
			flash->page_size = 256;
		else
			flash->page_size = 512;
	} else {
		flash->page_size = 256;
	}
	flash->page_size <<= flash->shift;
	flash->sector_size = params->sector_size << flash->shift;
	flash->size = flash->sector_size * params->nr_sectors << flash->shift;
#ifdef CONFIG_SF_DUAL_FLASH
	if (flash->dual_flash & SF_DUAL_STACKED_FLASH)
		flash->size <<= 1;
#endif

	/* Compute erase sector and command */
	if (params->flags & SECT_2K) {
		flash->erase_cmd = CMD_ERASE_2K;
		flash->erase_size = 2048 << flash->shift;
	} else if (params->flags & SECT_4K) {
		flash->erase_cmd = CMD_ERASE_4K;
		flash->erase_size = 4096 << flash->shift;
	} else if (params->flags & SECT_32K) {
		flash->erase_cmd = CMD_ERASE_32K;
		flash->erase_size = 32768 << flash->shift;
	} else {
		flash->erase_cmd = CMD_ERASE_64K;
		flash->erase_size = flash->sector_size;
	}

	/* Look for the fastest read cmd */
	cmd = fls(params->e_rd_cmd & flash->spi->op_mode_rx);
	if (cmd) {
		cmd = spi_read_cmds_array[cmd - 1];
		flash->read_cmd = cmd;
	} else {
		/* Go for default supported read cmd */
		flash->read_cmd = CMD_READ_ARRAY_FAST;
	}

	/* Not require to look for fastest only two write cmds yet */
	if (params->flags & WR_QPP && flash->spi->op_mode_tx & SPI_OPM_TX_QPP)
		flash->write_cmd = CMD_QUAD_PAGE_PROGRAM;
	else
		/* Go for default supported write cmd */
		flash->write_cmd = CMD_PAGE_PROGRAM;

	/* Read dummy_byte: dummy byte is determined based on the
	 * dummy cycles of a particular command.
	 * Fast commands - dummy_byte = dummy_cycles/8
	 * I/O commands- dummy_byte = (dummy_cycles * no.of lines)/8
	 * For I/O commands except cmd[0] everything goes on no.of lines
	 * based on particular command but incase of fast commands except
	 * data all go on single line irrespective of command.
	 */
	switch (flash->read_cmd) {
	case CMD_READ_QUAD_IO_FAST:
		flash->dummy_byte = 2;
		break;
	case CMD_READ_ARRAY_SLOW:
		flash->dummy_byte = 0;
		break;
	default:
		flash->dummy_byte = 1;
	}

	/* Poll cmd selection */
	flash->poll_cmd = CMD_READ_STATUS;
#ifdef CONFIG_SPI_FLASH_STMICRO
	if (params->flags & E_FSR)
		flash->poll_cmd = CMD_FLAG_STATUS;
#endif

	/* Configure the BAR - discover bank cmds and read current bank */
#ifdef CONFIG_SPI_FLASH_BAR
	u8 curr_bank = 0;
	if (flash->size > SPI_FLASH_16MB_BOUN) {
		int ret;

		flash->bank_read_cmd = (idcode[0] == 0x01) ?
					CMD_BANKADDR_BRRD : CMD_EXTNADDR_RDEAR;
		flash->bank_write_cmd = (idcode[0] == 0x01) ?
					CMD_BANKADDR_BRWR : CMD_EXTNADDR_WREAR;

		ret = spi_flash_read_common(flash, &flash->bank_read_cmd, 1,
					    &curr_bank, 1);
		if (ret) {
			debug("SF: fail to read bank addr register\n");
			return ret;
		}
		flash->bank_curr = curr_bank;
	} else {
		flash->bank_curr = curr_bank;
	}
#endif

	/* Flash powers up read-only, so clear BP# bits */
#if defined(CONFIG_SPI_FLASH_ATMEL) || \
	defined(CONFIG_SPI_FLASH_MACRONIX) || \
	defined(CONFIG_SPI_FLASH_SST)
		spi_flash_cmd_write_status(flash, 0);
#endif

	return 0;
}


static int spi_flash_read_write(struct spi_slave *spi,
				const u8 *cmd, size_t cmd_len,
				const u8 *data_out, u8 *data_in,
				size_t data_len)
{
	unsigned long flags = SPI_XFER_BEGIN;
	int ret;

#ifdef CONFIG_SF_DUAL_FLASH
	if (spi->flags & SPI_XFER_U_PAGE)
		flags |= SPI_XFER_U_PAGE;
#endif
	if (data_len == 0)
		flags |= SPI_XFER_END;

	ret = spi_xfer(spi, cmd_len * 8, cmd, NULL, flags);
	if (ret) {
		debug("SF: Failed to send command (%zu bytes): %d\n",
		      cmd_len, ret);
	} else if (data_len != 0) {
		ret = spi_xfer(spi, data_len * 8, data_out, data_in,
					SPI_XFER_END);
		if (ret)
			debug("SF: Failed to transfer %zu bytes of data: %d\n",
			      data_len, ret);
	}

	return ret;
}

int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len)
{
	return spi_flash_read_write(spi, cmd, cmd_len, NULL, data, data_len);
}

int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len)
{
	return spi_flash_cmd_read(spi, &cmd, 1, response, len);
}

int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len,
		const void *data, size_t data_len)
{
	return spi_flash_read_write(spi, cmd, cmd_len, data, NULL, data_len);
}
/**
 * spi_flash_probe_slave() - Probe for a SPI flash device on a bus
 *
 * @spi: Bus to probe
 * @flashp: Pointer to place to put flash info, which may be NULL if the
 * space should be allocated
 */
int spi_flash_probe_slave(struct spi_slave *spi, struct spi_flash *flash)
{
	u8 idcode[5];
	int ret;

	/* Setup spi_slave */
	if (!spi) {
		printf("SF: Failed to set up slave\n");
		return -ENODEV;
	}

	/* Claim spi bus */
	ret = spi_claim_bus(spi);
	if (ret) {
		debug("SF: Failed to claim SPI bus: %d\n", ret);
		return ret;
	}

	/* Read the ID codes */
	ret = spi_flash_cmd(spi, CMD_READ_ID, idcode, sizeof(idcode));
	if (ret) {
		printf("SF: Failed to get idcodes\n");
		goto err_read_id;
	}

#ifdef DEBUG
	printf("SF: Got idcodes\n");
	print_buffer(0, idcode, 1, sizeof(idcode), 0);
#endif

	if (spi_flash_validate_params(spi, idcode, flash)) {
		ret = -EINVAL;
		goto err_read_id;
	}

	/* Set the quad enable bit - only for quad commands */
	if ((flash->read_cmd == CMD_READ_QUAD_OUTPUT_FAST) ||
	    (flash->read_cmd == CMD_READ_QUAD_IO_FAST) ||
	    (flash->write_cmd == CMD_QUAD_PAGE_PROGRAM)) {
		if (spi_flash_set_qeb(flash, idcode[0])) {
			debug("SF: Fail to set QEB for %02x\n", idcode[0]);
			ret = -EINVAL;
			goto err_read_id;
		}
	}

#ifdef CONFIG_OF_CONTROL
	if (spi_flash_decode_fdt(gd->fdt_blob, flash)) {
		debug("SF: FDT decode error\n");
		ret = -EINVAL;
		goto err_read_id;
	}
#endif
#ifndef CONFIG_SPL_BUILD
	printf("SF: Detected %s with page size ", flash->name);
	print_size(flash->page_size, ", erase size ");
	print_size(flash->erase_size, ", total ");
	print_size(flash->size, "");
	if (flash->memory_map)
		printf(", mapped at %p", flash->memory_map);
	puts("\n");
#endif
#ifndef CONFIG_SPI_FLASH_BAR
	if (((flash->dual_flash == SF_SINGLE_FLASH) &&
	     (flash->size > SPI_FLASH_16MB_BOUN)) ||
	     ((flash->dual_flash > SF_SINGLE_FLASH) &&
	     (flash->size > SPI_FLASH_16MB_BOUN << 1))) {
		puts("SF: Warning - Only lower 16MiB accessible,");
		puts(" Full access #define CONFIG_SPI_FLASH_BAR\n");
	}
#endif
	if (spi_enable_wp_pin(flash))
		puts("Enable WP pin failed\n");


#if defined(CONFIG_SPI_FLASH_ATMEL)
	/*
	 * Default configure the page size to 256bytes to
	 * be compatible with the mtd/spi framework
	 */
	spi_flash_cmd_write_config(flash, SPI_FLASH_PAGE_256);
#endif

	/* Release spi bus */
	spi_release_bus(spi);

	return 0;

err_read_id:
	spi_release_bus(spi);
	return ret;
}



struct spi_flash *spi_flash_probe_tail(struct spi_slave *bus)
{
        struct spi_flash *flash;

        /* Allocate space if needed (not used by sf-uclass */
        flash = calloc(1, sizeof(*flash));
        if (!flash) {
                debug("SF: Failed to allocate spi_flash\n");
                return NULL;
        }

        if (spi_flash_probe_slave(bus, flash)) {
                spi_free_slave(bus);
                free(flash);
                return NULL;
        }

        return flash;
}

struct spi_flash *spi_flash_probe(unsigned int busnum, unsigned int cs,
                unsigned int max_hz, unsigned int spi_mode)
{
        struct spi_slave *bus;

        bus = spi_setup_slave(busnum, cs, max_hz, spi_mode);
        if (!bus)
                return NULL;
        return spi_flash_probe_tail(bus);
}



/*
 * The main entry for SPI booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from SPI into SDRAM and starts it from there.
 */
void spl_spi_load_image(void)
{
	struct spi_flash *flash;
	struct image_header *header;

        board_qspi_init();
        printf("SPL: u-boot second stage will be loaded from SPI FLASH\n");

	/*
	 * Load U-Boot image from SPI flash into RAM
	 */

	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
				CONFIG_SF_DEFAULT_CS,
				CONFIG_ENV_SPI_MAX_HZ,
				CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		puts("SPI probe failed.\n");
		hang();
	}

	/* use CONFIG_SYS_TEXT_BASE as temporary storage area */
	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE);

#ifdef CONFIG_SPL_OS_BOOT
	if (spl_start_uboot() || spi_load_image_os(flash, header))
#endif
	{
		/* Load u-boot, mkimage header is 64 bytes. */
		spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS, 0x40,
			       (void *)header);
		spl_parse_image_header(header);
		spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS,
			       spl_image.size, (void *)spl_image.load_addr);
	}
}
