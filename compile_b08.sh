#!/bin/bash


make distclean
make seco_b08_defconfig
# make udoo_neo_android_defconfig
make -j 4
dd if=SPL of=u-boot.imx bs=1K seek=0 conv=notrunc;
dd if=u-boot.img of=u-boot.imx bs=1K seek=68; 
sleep 1

dd if=qspic_header.imx of=u-boot.spi seek=0 conv=notrunc
./crea_qspi_image.py
dd if=footer.spi of=u-boot.spi bs=1k seek=3


echo ""
echo ""
echo ""
echo -e "\tProduced \"u-boot.imx\" for uSD card,\n\tand \"u-boot.spi\" for spi flash." 
echo ""
echo ""
