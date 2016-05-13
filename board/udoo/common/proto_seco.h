/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


/*
 * proto_seco.h - definitions an prototypes for seco platform device
 */

#ifndef _PROTO_SECO_H_
#define _PROTO_SECO_H_



typedef struct boot_mem_dev {
	uint mem_ctl_min;
	uint mem_ctl_max;
	uint mem_type;
	uint bt_usdhc_num;
	uint bt_port_select;
	char *label; 
} boot_mem_dev_t;

extern boot_mem_dev_t boot_mem_dev_list[SECO_NUM_BOOT_DEV];



/*  __________________________________________________________________________
 * |                                                                          |
 * |                                   USDHC                                  |
 * |__________________________________________________________________________|
 */

struct usdhc_l {
	iomux_v3_cfg_t const *pad;
	int num;
	int gpio_cd;
};

extern struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM];
extern struct usdhc_l usdhc_list[CONFIG_SYS_FSL_USDHC_NUM];
extern enum mxc_clock usdhc_clk[CONFIG_SYS_FSL_USDHC_NUM];

extern int usdhc_devno[4];
int check_mmc_autodetect (void);
void board_late_mmc_env_init (void);

/*  __________________________________________________________________________
 * |                                                                          |
 * |                               BOOT VALIDATE                              |
 * |__________________________________________________________________________|
 */
extern void boot_validate (int gpio, const iomux_v3_cfg_t *gpio_pad);



/*  __________________________________________________________________________
 * |                                                                          |
 * |                               BOARD REVISION                             |
 * |__________________________________________________________________________|
 */
extern int get_seco_board_revision (struct i2c_pads_info *i2c_pad, unsigned char *boardrev);



/*  __________________________________________________________________________
 * |                                                                          |
 * |                                 DISPLAY INFO                             |
 * |__________________________________________________________________________|
 */
extern char *board_name;
extern void print_boot_device (void);


/*  __________________________________________________________________________
 * |                                                                          |
 * |                                  ETHERNET                                |
 * |__________________________________________________________________________|
 */
extern void enable_ethernet_rail (void);


/*  __________________________________________________________________________
 * |                                                                          |
 * |                                 DISPLAY INFO                             |
 * |__________________________________________________________________________|
 */
struct display_info_t {
	int	bus;
	int	addr;
	int	pixfmt;
	int	(*detect)(struct display_info_t const *dev);
	void	(*enable)(struct display_info_t const *dev);
	struct	fb_videomode mode;
};

extern void setup_display (void); 


/*  __________________________________________________________________________
 * |                                                                          |
 * |                                    PFUZE                                 |
 * |__________________________________________________________________________|
 */
extern int setup_pmic_voltages(struct i2c_pads_info *i2c_pad);

#endif   /*  _PROTO_SECO_H_  */
