/**
  * Copyright (C) Jasbir Matharu
  * Copyright (C) Texas Instruments
  * 
  * A large portion of this code is taken from the tda998x_drv.c kernel driver
  * by Rob Clark <robdclark@gmail.com>.
  * 
  * This program is free software; you can redistribute it and/or modify it
  * under the terms of the GNU General Public License version 2 as published by
  * the Free Software Foundation.
  *
  * This program is distributed in the hope that it will be useful, but WITHOUT
  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  * more details.
  *
  **/
 
#include <linux/sizes.h>
#include <common.h>

#include <i2c.h>
#include <asm/imx-common/mxc_i2c.h>

#include <linux/fb.h>
#include <mxsfb.h>


#define REG(page, addr) (((page) << 8) | (addr))
#define REG2ADDR(reg)   ((reg) & 0xff)
#define REG2PAGE(reg)   (((reg) >> 8) & 0xff)

#define REG_CURPAGE               0xff                /* write */


/* Page 00h: General Control */
#define REG_VERSION_LSB           REG(0x00, 0x00)     /* read */
#define REG_MAIN_CNTRL0           REG(0x00, 0x01)     /* read/write */
# define MAIN_CNTRL0_SR           (1 << 0)
# define MAIN_CNTRL0_DECS         (1 << 1)
# define MAIN_CNTRL0_DEHS         (1 << 2)
# define MAIN_CNTRL0_CECS         (1 << 3)
# define MAIN_CNTRL0_CEHS         (1 << 4)
# define MAIN_CNTRL0_SCALER       (1 << 7)
#define REG_VERSION_MSB           REG(0x00, 0x02)     /* read */
#define REG_SOFTRESET             REG(0x00, 0x0a)     /* write */
# define SOFTRESET_AUDIO          (1 << 0)
# define SOFTRESET_I2C_MASTER     (1 << 1)
#define REG_DDC_DISABLE           REG(0x00, 0x0b)     /* read/write */
#define REG_CCLK_ON               REG(0x00, 0x0c)     /* read/write */
#define REG_I2C_MASTER            REG(0x00, 0x0d)     /* read/write */
# define I2C_MASTER_DIS_MM        (1 << 0)
# define I2C_MASTER_DIS_FILT      (1 << 1)
# define I2C_MASTER_APP_STRT_LAT  (1 << 2)
#define REG_FEAT_POWERDOWN        REG(0x00, 0x0e)     /* read/write */
# define FEAT_POWERDOWN_SPDIF     (1 << 3)
#define REG_INT_FLAGS_0           REG(0x00, 0x0f)     /* read/write */
#define REG_INT_FLAGS_1           REG(0x00, 0x10)     /* read/write */
#define REG_INT_FLAGS_2           REG(0x00, 0x11)     /* read/write */
# define INT_FLAGS_2_EDID_BLK_RD  (1 << 1)
#define REG_ENA_ACLK              REG(0x00, 0x16)     /* read/write */
#define REG_ENA_VP_0              REG(0x00, 0x18)     /* read/write */
#define REG_ENA_VP_1              REG(0x00, 0x19)     /* read/write */
#define REG_ENA_VP_2              REG(0x00, 0x1a)     /* read/write */
#define REG_ENA_AP                REG(0x00, 0x1e)     /* read/write */
#define REG_VIP_CNTRL_0           REG(0x00, 0x20)     /* write */
# define VIP_CNTRL_0_MIRR_A       (1 << 7)
# define VIP_CNTRL_0_SWAP_A(x)    (((x) & 7) << 4)
# define VIP_CNTRL_0_MIRR_B       (1 << 3)
# define VIP_CNTRL_0_SWAP_B(x)    (((x) & 7) << 0)
#define REG_VIP_CNTRL_1           REG(0x00, 0x21)     /* write */
# define VIP_CNTRL_1_MIRR_C       (1 << 7)
# define VIP_CNTRL_1_SWAP_C(x)    (((x) & 7) << 4)
# define VIP_CNTRL_1_MIRR_D       (1 << 3)
# define VIP_CNTRL_1_SWAP_D(x)    (((x) & 7) << 0)
#define REG_VIP_CNTRL_2           REG(0x00, 0x22)     /* write */
# define VIP_CNTRL_2_MIRR_E       (1 << 7)
# define VIP_CNTRL_2_SWAP_E(x)    (((x) & 7) << 4)
# define VIP_CNTRL_2_MIRR_F       (1 << 3)
# define VIP_CNTRL_2_SWAP_F(x)    (((x) & 7) << 0)
#define REG_VIP_CNTRL_3           REG(0x00, 0x23)     /* write */
# define VIP_CNTRL_3_X_TGL        (1 << 0)
# define VIP_CNTRL_3_H_TGL        (1 << 1)
# define VIP_CNTRL_3_V_TGL        (1 << 2)
# define VIP_CNTRL_3_EMB          (1 << 3)
# define VIP_CNTRL_3_SYNC_DE      (1 << 4)
# define VIP_CNTRL_3_SYNC_HS      (1 << 5)
# define VIP_CNTRL_3_DE_INT       (1 << 6)
# define VIP_CNTRL_3_EDGE         (1 << 7)
#define REG_VIP_CNTRL_4           REG(0x00, 0x24)     /* write */
# define VIP_CNTRL_4_BLC(x)       (((x) & 3) << 0)
# define VIP_CNTRL_4_BLANKIT(x)   (((x) & 3) << 2)
# define VIP_CNTRL_4_CCIR656      (1 << 4)
# define VIP_CNTRL_4_656_ALT      (1 << 5)
# define VIP_CNTRL_4_TST_656      (1 << 6)
# define VIP_CNTRL_4_TST_PAT      (1 << 7)
#define REG_VIP_CNTRL_5           REG(0x00, 0x25)     /* write */
# define VIP_CNTRL_5_CKCASE       (1 << 0)
# define VIP_CNTRL_5_SP_CNT(x)    (((x) & 3) << 1)
#define REG_MUX_AP                REG(0x00, 0x26)     /* read/write */
# define MUX_AP_SELECT_I2S	  0x64
# define MUX_AP_SELECT_SPDIF	  0x40
#define REG_MUX_VP_VIP_OUT        REG(0x00, 0x27)     /* read/write */
#define REG_MAT_CONTRL            REG(0x00, 0x80)     /* write */
# define MAT_CONTRL_MAT_SC(x)     (((x) & 3) << 0)
# define MAT_CONTRL_MAT_BP        (1 << 2)
#define REG_VIDFORMAT             REG(0x00, 0xa0)     /* write */
#define REG_REFPIX_MSB            REG(0x00, 0xa1)     /* write */
#define REG_REFPIX_LSB            REG(0x00, 0xa2)     /* write */
#define REG_REFLINE_MSB           REG(0x00, 0xa3)     /* write */
#define REG_REFLINE_LSB           REG(0x00, 0xa4)     /* write */
#define REG_NPIX_MSB              REG(0x00, 0xa5)     /* write */
#define REG_NPIX_LSB              REG(0x00, 0xa6)     /* write */
#define REG_NLINE_MSB             REG(0x00, 0xa7)     /* write */
#define REG_NLINE_LSB             REG(0x00, 0xa8)     /* write */
#define REG_VS_LINE_STRT_1_MSB    REG(0x00, 0xa9)     /* write */
#define REG_VS_LINE_STRT_1_LSB    REG(0x00, 0xaa)     /* write */
#define REG_VS_PIX_STRT_1_MSB     REG(0x00, 0xab)     /* write */
#define REG_VS_PIX_STRT_1_LSB     REG(0x00, 0xac)     /* write */
#define REG_VS_LINE_END_1_MSB     REG(0x00, 0xad)     /* write */
#define REG_VS_LINE_END_1_LSB     REG(0x00, 0xae)     /* write */
#define REG_VS_PIX_END_1_MSB      REG(0x00, 0xaf)     /* write */
#define REG_VS_PIX_END_1_LSB      REG(0x00, 0xb0)     /* write */
#define REG_VS_LINE_STRT_2_MSB    REG(0x00, 0xb1)     /* write */
#define REG_VS_LINE_STRT_2_LSB    REG(0x00, 0xb2)     /* write */
#define REG_VS_PIX_STRT_2_MSB     REG(0x00, 0xb3)     /* write */
#define REG_VS_PIX_STRT_2_LSB     REG(0x00, 0xb4)     /* write */
#define REG_VS_LINE_END_2_MSB     REG(0x00, 0xb5)     /* write */
#define REG_VS_LINE_END_2_LSB     REG(0x00, 0xb6)     /* write */
#define REG_VS_PIX_END_2_MSB      REG(0x00, 0xb7)     /* write */
#define REG_VS_PIX_END_2_LSB      REG(0x00, 0xb8)     /* write */
#define REG_HS_PIX_START_MSB      REG(0x00, 0xb9)     /* write */
#define REG_HS_PIX_START_LSB      REG(0x00, 0xba)     /* write */
#define REG_HS_PIX_STOP_MSB       REG(0x00, 0xbb)     /* write */
#define REG_HS_PIX_STOP_LSB       REG(0x00, 0xbc)     /* write */
#define REG_VWIN_START_1_MSB      REG(0x00, 0xbd)     /* write */
#define REG_VWIN_START_1_LSB      REG(0x00, 0xbe)     /* write */
#define REG_VWIN_END_1_MSB        REG(0x00, 0xbf)     /* write */
#define REG_VWIN_END_1_LSB        REG(0x00, 0xc0)     /* write */
#define REG_VWIN_START_2_MSB      REG(0x00, 0xc1)     /* write */
#define REG_VWIN_START_2_LSB      REG(0x00, 0xc2)     /* write */
#define REG_VWIN_END_2_MSB        REG(0x00, 0xc3)     /* write */
#define REG_VWIN_END_2_LSB        REG(0x00, 0xc4)     /* write */
#define REG_DE_START_MSB          REG(0x00, 0xc5)     /* write */
#define REG_DE_START_LSB          REG(0x00, 0xc6)     /* write */
#define REG_DE_STOP_MSB           REG(0x00, 0xc7)     /* write */
#define REG_DE_STOP_LSB           REG(0x00, 0xc8)     /* write */
#define REG_TBG_CNTRL_0           REG(0x00, 0xca)     /* write */
# define TBG_CNTRL_0_TOP_TGL      (1 << 0)
# define TBG_CNTRL_0_TOP_SEL      (1 << 1)
# define TBG_CNTRL_0_DE_EXT       (1 << 2)
# define TBG_CNTRL_0_TOP_EXT      (1 << 3)
# define TBG_CNTRL_0_FRAME_DIS    (1 << 5)
# define TBG_CNTRL_0_SYNC_MTHD    (1 << 6)
# define TBG_CNTRL_0_SYNC_ONCE    (1 << 7)
#define REG_TBG_CNTRL_1           REG(0x00, 0xcb)     /* write */
# define TBG_CNTRL_1_H_TGL        (1 << 0)
# define TBG_CNTRL_1_V_TGL        (1 << 1)
# define TBG_CNTRL_1_TGL_EN       (1 << 2)
# define TBG_CNTRL_1_X_EXT        (1 << 3)
# define TBG_CNTRL_1_H_EXT        (1 << 4)
# define TBG_CNTRL_1_V_EXT        (1 << 5)
# define TBG_CNTRL_1_DWIN_DIS     (1 << 6)
#define REG_ENABLE_SPACE          REG(0x00, 0xd6)     /* write */
#define REG_HVF_CNTRL_0           REG(0x00, 0xe4)     /* write */
# define HVF_CNTRL_0_SM           (1 << 7)
# define HVF_CNTRL_0_RWB          (1 << 6)
# define HVF_CNTRL_0_PREFIL(x)    (((x) & 3) << 2)
# define HVF_CNTRL_0_INTPOL(x)    (((x) & 3) << 0)
#define REG_HVF_CNTRL_1           REG(0x00, 0xe5)     /* write */
# define HVF_CNTRL_1_FOR          (1 << 0)
# define HVF_CNTRL_1_YUVBLK       (1 << 1)
# define HVF_CNTRL_1_VQR(x)       (((x) & 3) << 2)
# define HVF_CNTRL_1_PAD(x)       (((x) & 3) << 4)
# define HVF_CNTRL_1_SEMI_PLANAR  (1 << 6)
#define REG_RPT_CNTRL             REG(0x00, 0xf0)     /* write */
#define REG_I2S_FORMAT            REG(0x00, 0xfc)     /* read/write */
# define I2S_FORMAT(x)            (((x) & 3) << 0)
#define REG_AIP_CLKSEL            REG(0x00, 0xfd)     /* write */
# define AIP_CLKSEL_AIP_SPDIF	  (0 << 3)
# define AIP_CLKSEL_AIP_I2S	  (1 << 3)
# define AIP_CLKSEL_FS_ACLK	  (0 << 0)
# define AIP_CLKSEL_FS_MCLK	  (1 << 0)
# define AIP_CLKSEL_FS_FS64SPDIF  (2 << 0)

#define CONFIG_TDA19988_I2C_BUS		2
#define CONFIG_TDA19988_I2C_CEC	    0x34
#define CONFIG_TDA19988_I2C_HDMI	0x70

/* Page 02h: PLL settings */
#define REG_PLL_SERIAL_1          REG(0x02, 0x00)     /* read/write */
# define PLL_SERIAL_1_SRL_FDN     (1 << 0)
# define PLL_SERIAL_1_SRL_IZ(x)   (((x) & 3) << 1)
# define PLL_SERIAL_1_SRL_MAN_IZ  (1 << 6)
#define REG_PLL_SERIAL_2          REG(0x02, 0x01)     /* read/write */
# define PLL_SERIAL_2_SRL_NOSC(x) ((x) << 0)
# define PLL_SERIAL_2_SRL_PR(x)   (((x) & 0xf) << 4)
#define REG_PLL_SERIAL_3          REG(0x02, 0x02)     /* read/write */
# define PLL_SERIAL_3_SRL_CCIR    (1 << 0)
# define PLL_SERIAL_3_SRL_DE      (1 << 2)
# define PLL_SERIAL_3_SRL_PXIN_SEL (1 << 4)
#define REG_SERIALIZER            REG(0x02, 0x03)     /* read/write */
#define REG_BUFFER_OUT            REG(0x02, 0x04)     /* read/write */
#define REG_PLL_SCG1              REG(0x02, 0x05)     /* read/write */
#define REG_PLL_SCG2              REG(0x02, 0x06)     /* read/write */
#define REG_PLL_SCGN1             REG(0x02, 0x07)     /* read/write */
#define REG_PLL_SCGN2             REG(0x02, 0x08)     /* read/write */
#define REG_PLL_SCGR1             REG(0x02, 0x09)     /* read/write */
#define REG_PLL_SCGR2             REG(0x02, 0x0a)     /* read/write */
#define REG_AUDIO_DIV             REG(0x02, 0x0e)     /* read/write */
# define AUDIO_DIV_SERCLK_1       0
# define AUDIO_DIV_SERCLK_2       1
# define AUDIO_DIV_SERCLK_4       2
# define AUDIO_DIV_SERCLK_8       3
# define AUDIO_DIV_SERCLK_16      4
# define AUDIO_DIV_SERCLK_32      5
#define REG_SEL_CLK               REG(0x02, 0x11)     /* read/write */
# define SEL_CLK_SEL_CLK1         (1 << 0)
# define SEL_CLK_SEL_VRF_CLK(x)   (((x) & 3) << 1)
# define SEL_CLK_ENA_SC_CLK       (1 << 3)
#define REG_ANA_GENERAL           REG(0x02, 0x12)     /* read/write */

/* Page 09h: EDID Control */
#define REG_EDID_DATA_0           REG(0x09, 0x00)     /* read */
/* next 127 successive registers are the EDID block */
#define REG_EDID_CTRL             REG(0x09, 0xfa)     /* read/write */
#define REG_DDC_ADDR              REG(0x09, 0xfb)     /* read/write */
#define REG_DDC_OFFS              REG(0x09, 0xfc)     /* read/write */
#define REG_DDC_SEGM_ADDR         REG(0x09, 0xfd)     /* read/write */
#define REG_DDC_SEGM              REG(0x09, 0xfe)     /* read/write */

/* Page 10h: information frames and packets */
#define REG_IF1_HB0               REG(0x10, 0x20)     /* read/write */
#define REG_IF2_HB0               REG(0x10, 0x40)     /* read/write */
#define REG_IF3_HB0               REG(0x10, 0x60)     /* read/write */
#define REG_IF4_HB0               REG(0x10, 0x80)     /* read/write */
#define REG_IF5_HB0               REG(0x10, 0xa0)     /* read/write */

/* Page 11h: audio settings and content info packets */
#define REG_AIP_CNTRL_0           REG(0x11, 0x00)     /* read/write */
# define AIP_CNTRL_0_RST_FIFO     (1 << 0)
# define AIP_CNTRL_0_SWAP         (1 << 1)
# define AIP_CNTRL_0_LAYOUT       (1 << 2)
# define AIP_CNTRL_0_ACR_MAN      (1 << 5)
# define AIP_CNTRL_0_RST_CTS      (1 << 6)
#define REG_CA_I2S                REG(0x11, 0x01)     /* read/write */
# define CA_I2S_CA_I2S(x)         (((x) & 31) << 0)
# define CA_I2S_HBR_CHSTAT        (1 << 6)
#define REG_LATENCY_RD            REG(0x11, 0x04)     /* read/write */
#define REG_ACR_CTS_0             REG(0x11, 0x05)     /* read/write */
#define REG_ACR_CTS_1             REG(0x11, 0x06)     /* read/write */
#define REG_ACR_CTS_2             REG(0x11, 0x07)     /* read/write */
#define REG_ACR_N_0               REG(0x11, 0x08)     /* read/write */
#define REG_ACR_N_1               REG(0x11, 0x09)     /* read/write */
#define REG_ACR_N_2               REG(0x11, 0x0a)     /* read/write */
#define REG_CTS_N                 REG(0x11, 0x0c)     /* read/write */
# define CTS_N_K(x)               (((x) & 7) << 0)
# define CTS_N_M(x)               (((x) & 3) << 4)
#define REG_ENC_CNTRL             REG(0x11, 0x0d)     /* read/write */
# define ENC_CNTRL_RST_ENC        (1 << 0)
# define ENC_CNTRL_RST_SEL        (1 << 1)
# define ENC_CNTRL_CTL_CODE(x)    (((x) & 3) << 2)
#define REG_DIP_FLAGS             REG(0x11, 0x0e)     /* read/write */
# define DIP_FLAGS_ACR            (1 << 0)
# define DIP_FLAGS_GC             (1 << 1)
#define REG_DIP_IF_FLAGS          REG(0x11, 0x0f)     /* read/write */
# define DIP_IF_FLAGS_IF1         (1 << 1)
# define DIP_IF_FLAGS_IF2         (1 << 2)
# define DIP_IF_FLAGS_IF3         (1 << 3)
# define DIP_IF_FLAGS_IF4         (1 << 4)
# define DIP_IF_FLAGS_IF5         (1 << 5)
#define REG_CH_STAT_B(x)          REG(0x11, 0x14 + (x)) /* read/write */

/* Page 12h: HDCP and OTP */
#define REG_TX3                   REG(0x12, 0x9a)     /* read/write */
#define REG_TX4                   REG(0x12, 0x9b)     /* read/write */
# define TX4_PD_RAM               (1 << 1)
#define REG_TX33                  REG(0x12, 0xb8)     /* read/write */
# define TX33_HDMI                (1 << 1)

void hdmi_reg_write(uint page_reg, char unsigned value) {
    
     char unsigned page = REG2PAGE(page_reg);
     i2c_write(CONFIG_TDA19988_I2C_HDMI, 0xFF, 1, &page , 1);
     
     i2c_write(CONFIG_TDA19988_I2C_HDMI, (uint)REG2ADDR(page_reg), 1, &value , 1);
}

void hdmi_reg_write16(uint page_reg, uint value16) {

     char unsigned page = REG2PAGE(page_reg);
     i2c_write(CONFIG_TDA19988_I2C_HDMI, 0xFF, 1, &page , 1);
     uint addr = REG2ADDR(page_reg);
     char unsigned value = value16 >> 8;
     i2c_write(CONFIG_TDA19988_I2C_HDMI, addr, 1, &value , 1);
     addr++;
     value = value16 & 0xFF;
     i2c_write(CONFIG_TDA19988_I2C_HDMI, addr, 1, &value , 1);

}

void hdmi_reg_set(uint page_reg, char unsigned bit) {
    
    char unsigned value;
    
    i2c_read(CONFIG_TDA19988_I2C_HDMI, (uint)REG2ADDR(page_reg), 1,&value, 1);
    value = value | bit;
    i2c_write(CONFIG_TDA19988_I2C_HDMI, (uint)REG2ADDR(page_reg), 1, &value , 1);    
}

void hdmi_reg_clear(uint page_reg, char unsigned bit) {
    
    char unsigned value;
    
    i2c_read(CONFIG_TDA19988_I2C_HDMI, (uint)REG2ADDR(page_reg), 1, &value, 1);
    value = value & ~bit;
    i2c_write(CONFIG_TDA19988_I2C_HDMI, (uint)REG2ADDR(page_reg), 1, &value , 1);    
}

/**
 * Probe for TDA19988 and wakeup HDMI i2c
 **/ 
int tda19988_init(void) {

    char unsigned value;
    i2c_set_bus_num(CONFIG_TDA19988_I2C_BUS);

	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_TDA19988_I2C_CEC);
	if (!i2c_probe(CONFIG_TDA19988_I2C_CEC)) {
        
        /*  Initialise HDMI I2C interface */
        value = 0x87;
        if (i2c_write(CONFIG_TDA19988_I2C_CEC, 0xFF, 1, &value, 1)) {
            printf("Failed to turn on TDA19988 HDMI from CEC \n");
            return -1;
        }
    
        if (!i2c_probe(CONFIG_TDA19988_I2C_HDMI)) {
            return 0;
        } else {
            printf("Failed to probe TDA19988 HDMI i2c interface \n");
            return -1;
        }
    } else {
        printf("Failed to probe TDA19988 CEC i2c\n");
        return -1;
    }
}

void tda19988_reset(void) {
    /* reset audio and i2c master: */
    hdmi_reg_write(REG_SOFTRESET, SOFTRESET_AUDIO | SOFTRESET_I2C_MASTER);
    udelay(50000);
    hdmi_reg_write(REG_SOFTRESET, 0);
    udelay(50000);
 
    /* reset transmitter: */
    hdmi_reg_set(REG_MAIN_CNTRL0, MAIN_CNTRL0_SR);
    hdmi_reg_clear(REG_MAIN_CNTRL0, MAIN_CNTRL0_SR);
 
    /* PLL registers common configuration */
    hdmi_reg_write(REG_PLL_SERIAL_1, 0x00);
    hdmi_reg_write(REG_PLL_SERIAL_2, PLL_SERIAL_2_SRL_NOSC(1));
    hdmi_reg_write(REG_PLL_SERIAL_3, 0x00);
    hdmi_reg_write(REG_SERIALIZER,   0x00);
    hdmi_reg_write(REG_BUFFER_OUT,   0x00);
    hdmi_reg_write(REG_PLL_SCG1,     0x00);
    hdmi_reg_write(REG_AUDIO_DIV,    AUDIO_DIV_SERCLK_8);
    hdmi_reg_write(REG_SEL_CLK,      SEL_CLK_SEL_CLK1 | SEL_CLK_ENA_SC_CLK);
    hdmi_reg_write(REG_PLL_SCGN1,    0xfa);
    hdmi_reg_write(REG_PLL_SCGN2,    0x00);
    hdmi_reg_write(REG_PLL_SCGR1,    0x5b);
    hdmi_reg_write(REG_PLL_SCGR2,    0x00);
    hdmi_reg_write(REG_PLL_SCG2,     0x10);
 
    /* Write the default value MUX register */
    hdmi_reg_write(REG_MUX_VP_VIP_OUT, 0x24);

    /* Enable video ports */
    hdmi_reg_write(REG_ENA_VP_0, 0xff);
    hdmi_reg_write(REG_ENA_VP_1, 0xff);
    hdmi_reg_write(REG_ENA_VP_2, 0xff);

    /* Setup RGB Muxing for MCIMX28LCD */
    hdmi_reg_write(REG_VIP_CNTRL_0,0x20);
    hdmi_reg_write(REG_VIP_CNTRL_1,0x01);
    hdmi_reg_write(REG_VIP_CNTRL_2,0x45);
    hdmi_reg_write(REG_VIP_CNTRL_3,0x23);

}

/** 
 * Configure TDA19988 with fb resolution 
 **/ 
void tda19988_fb_mode(struct fb_videomode mode) {

    char unsigned value; 
    uint hdisplay, hsync_start, hsync_end, htotal;
    uint vdisplay, vsync_start, vsync_end, vtotal;

    hdmi_reg_write(REG_DDC_DISABLE, 0x00);
     /* set clock on DDC channel: */
    hdmi_reg_write(REG_TX3, 39);

    hdisplay = mode.xres;
    hsync_start = hdisplay + mode.right_margin;
    hsync_end = hsync_start + mode.hsync_len;
    htotal = hsync_end + mode.left_margin;
    vdisplay = mode.yres;
    vsync_start = vdisplay + mode.lower_margin;
    vsync_end = vsync_start + mode.vsync_len;
    vtotal = vsync_end + mode.upper_margin;

    uint n_pix        = htotal;
    uint n_line       = vtotal;

    uint hs_pix_s     = hsync_start - hdisplay;
    uint hs_pix_e     = hsync_end - hdisplay;
    uint de_pix_s     = htotal - hdisplay;
    uint de_pix_e     = htotal;

    uint ref_pix      = 3 + hs_pix_s;

    uint ref_line     = 1 + vsync_start - vdisplay;
    uint vwin1_line_s = vtotal - vdisplay - 1;
    uint vwin1_line_e = vwin1_line_s + vdisplay;
    uint vs1_pix_s    = hs_pix_s;
    uint vs1_pix_e    = hs_pix_s;
    uint vs1_line_s   = vsync_start - vdisplay;
    uint vs1_line_e   = vs1_line_s + vsync_end - vsync_start;
    uint vwin2_line_s = 0;
    uint vwin2_line_e = 0;
    uint vs2_pix_s    = 0;
    uint vs2_pix_e    = 0;
    uint vs2_line_s   = 0;
    uint vs2_line_e   = 0; 

    /* mute the audio FIFO: */
    hdmi_reg_set(REG_AIP_CNTRL_0, AIP_CNTRL_0_RST_FIFO);

    /* set HDMI HDCP mode off: */
    hdmi_reg_write(REG_TBG_CNTRL_1, TBG_CNTRL_1_DWIN_DIS);
    hdmi_reg_clear(REG_TX33, TX33_HDMI);
    hdmi_reg_write(REG_ENC_CNTRL, ENC_CNTRL_CTL_CODE(0));

    uint div = 0;
    uint clk = (PICOS2KHZ(mode.pixclock)/10)*10;
    div = 148500 / clk;
    if (div != 0) {
        div--;
        if (div > 3)
            div = 3;
    }

    hdmi_reg_write(REG_HVF_CNTRL_0, HVF_CNTRL_0_PREFIL(0) |
                   HVF_CNTRL_0_INTPOL(0));
    hdmi_reg_write(REG_VIP_CNTRL_5, VIP_CNTRL_5_SP_CNT(0));
    hdmi_reg_write(REG_VIP_CNTRL_4, VIP_CNTRL_4_BLANKIT(0) |
                   VIP_CNTRL_4_BLC(0));

    hdmi_reg_clear(REG_PLL_SERIAL_1, PLL_SERIAL_1_SRL_MAN_IZ);
    hdmi_reg_clear(REG_PLL_SERIAL_3, PLL_SERIAL_3_SRL_CCIR |
                                             PLL_SERIAL_3_SRL_DE);
    hdmi_reg_write(REG_SERIALIZER, 0);
    hdmi_reg_write(REG_HVF_CNTRL_1, HVF_CNTRL_1_VQR(0));

    /* TODO enable pixel repeat for pixel rates less than 25Msamp/s */
    uint rep = 0;
    hdmi_reg_write(REG_RPT_CNTRL, 0);
    hdmi_reg_write(REG_SEL_CLK, SEL_CLK_SEL_VRF_CLK(0) |
                   SEL_CLK_SEL_CLK1 | SEL_CLK_ENA_SC_CLK);

    hdmi_reg_write(REG_PLL_SERIAL_2, PLL_SERIAL_2_SRL_NOSC(div) |
                   PLL_SERIAL_2_SRL_PR(rep));

    /* set color matrix bypass flag: */
    hdmi_reg_write(REG_MAT_CONTRL, MAT_CONTRL_MAT_BP |
                                   MAT_CONTRL_MAT_SC(1));

    /* set BIAS tmds value: */
    hdmi_reg_write(REG_ANA_GENERAL, 0x09);

    /* For MX28 LCD controller invert Hsync */
    value = VIP_CNTRL_3_SYNC_HS;
    value = value | VIP_CNTRL_3_H_TGL;
    hdmi_reg_write(REG_VIP_CNTRL_3, value);

    hdmi_reg_write(REG_VIDFORMAT, 0x00);
    hdmi_reg_write16(REG_REFPIX_MSB, ref_pix);
    hdmi_reg_write16(REG_REFLINE_MSB, ref_line);
    hdmi_reg_write16(REG_NPIX_MSB, n_pix);
    hdmi_reg_write16(REG_NLINE_MSB, n_line);
    hdmi_reg_write16(REG_VS_LINE_STRT_1_MSB, vs1_line_s);
    hdmi_reg_write16(REG_VS_PIX_STRT_1_MSB, vs1_pix_s);
    hdmi_reg_write16(REG_VS_LINE_END_1_MSB, vs1_line_e);
    hdmi_reg_write16(REG_VS_PIX_END_1_MSB, vs1_pix_e);
    hdmi_reg_write16(REG_VS_LINE_STRT_2_MSB, vs2_line_s);
    hdmi_reg_write16(REG_VS_PIX_STRT_2_MSB, vs2_pix_s);
    hdmi_reg_write16(REG_VS_LINE_END_2_MSB, vs2_line_e);
    hdmi_reg_write16(REG_VS_PIX_END_2_MSB, vs2_pix_e);
    hdmi_reg_write16(REG_HS_PIX_START_MSB, hs_pix_s);
    hdmi_reg_write16(REG_HS_PIX_STOP_MSB, hs_pix_e);
    hdmi_reg_write16(REG_VWIN_START_1_MSB, vwin1_line_s);
    hdmi_reg_write16(REG_VWIN_END_1_MSB, vwin1_line_e);
    hdmi_reg_write16(REG_VWIN_START_2_MSB, vwin2_line_s);
    hdmi_reg_write16(REG_VWIN_END_2_MSB, vwin2_line_e);
    hdmi_reg_write16(REG_DE_START_MSB, de_pix_s);
    hdmi_reg_write16(REG_DE_STOP_MSB, de_pix_e);

    hdmi_reg_write(REG_ENABLE_SPACE, 0x01);

    /* For MX28 LCD controller invert Hsync */
    value =  TBG_CNTRL_1_DWIN_DIS | TBG_CNTRL_1_TGL_EN;
    value = value | TBG_CNTRL_1_H_TGL;
    hdmi_reg_write(REG_TBG_CNTRL_1, value);

    /* must be last register set: */
    hdmi_reg_write(REG_TBG_CNTRL_0, 0);

}
