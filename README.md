UDOO U-Boot
===========

UDOO Neo
--------
To build the U-Boot for [UDOO Neo](http://www.udoo.org/udoo-neo/), use the [2015.04.imx](https://github.com/UDOOboard/uboot-imx/tree/2015.04.imx) branch.
This branch is based on Freescale's U-Boot (uboot-imx) version 2015.04.

The build can be started with:

    ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make udoo_neo_defconfig
    ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make

The produced files, `SPL` and `u-boot.img`, can be used to boot all Neo variants (*basic*, *basic kickstarter*, *extended*, *full*).

UDOO Quad/Dual
--------------

To build the U-Boot for [UDOO Quad/Dual](http://www.udoo.org/udoo-dual-and-quad/), use the [2015.10.fslc-qdl](https://github.com/UDOOboard/uboot-imx/tree/2015.10.fslc-qdl) branch.
This branch is based on Freescale's U-Boot (uboot-imx) version 2015.10.

The build can be started with:

    ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make udoo_qd_config
    ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make

The produced files, `SPL` and `u-boot.img`, can be used to boot both Quad and Dual boards.

B08 board
--------------
To build the U-Boot for [Seco B08](http://www.seco.com/prods/it/sbc-b08.html), use the [2015.04.imx](https://github.com/UDOOboard/uboot-imx/tree/2015.04.imx) branch.
This branch is based on Freescale's U-Boot (uboot-imx) version 2015.04.

The build can be started with:

    ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- ./compile_b08.sh

The produced files: `SPL`, `u-boot.imx` and `u-boot.spi` can be used to boot from uSD ard and SPI flash.
To write SPI flash format the memory (flash_erase) and after execute: 

     dd if=u-boot.spi of=/dev/mtd0 bs=1k seek=1 

SD card flashing
----------------
Flash your `/dev/mmcblk0` SD card with:

    dd if=SPL of=/dev/mmcblk0 bs=1K seek=1
    dd if=u-boot.img of=/dev/mmcblk0 bs=1K seek=69
