#
# (C) Copyright 2015 Seco
#
# Author: Davide Cardillo <davide.cardillo@seco.com>
#
# SPDX-License-Identifier:     GPL-2.0+
#

obj-y                      := mx6seco_928.o
obj-y                      += ../common/board.o
obj-${CONFIG_FSL_ESDHC}    += ../common/usdhc.o
obj-y                      += ../common/net_rgmii.o
obj-${CONFIG_VIDEO_IPUV3}  += ../common/display.o

extra-$(CONFIG_USE_PLUGIN) :=  plugin.bin
$(obj)/plugin.bin: $(obj)/plugin.o
	$(OBJCOPY) -O binary --gap-fill 0xff $< $@
