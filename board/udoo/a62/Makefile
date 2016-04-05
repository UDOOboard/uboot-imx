#
# (C) Copyright 2015 Seco
#
# Author: Davide Cardillo <davide.cardillo@seco.com>
#
# SPDX-License-Identifier:     GPL-2.0+
#

obj-y                            := mx6qd_a62.o
obj-y                            += ../common/board.o
obj-${CONFIG_FSL_ESDHC}          += ../common/usdhc.o
obj-${CONFIG_PFUZE100_PMIC_I2C}  += ../common/pmic100.o
obj-${CONFIG_SYS_USE_ETHERNET}   += ../common/net_rgmii.o
obj-${CONFIG_VIDEO_IPUV3}        += ../common/display.o
obj-$(CONFIG_SPL_BUILD)  	 += spl.o detectboard.o
