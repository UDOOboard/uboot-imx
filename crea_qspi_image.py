#!/usr/bin/python 

import struct

fileIN="u-boot.imx"
fileOUT="footer.spi"

a=open(fileIN, "rob")
b=open(fileOUT, "wb")

fdata=a.read()
xdata=bytearray(fdata)
a.close()

count=0

while count < len(fdata):
  num=xdata[count + 3] << 24 | xdata[count + 2] << 16 | xdata[count + 1] << 8 | xdata[count + 0]
  if count == 0x20:
    print "0x%08x" % num
    num = num - 0xc00
    print "0x%08x" % num
  if count == 0x24:
    print "0x%08x" % num
    num = num + 0xc00
    print "0x%08x" % num
  count = count + 4
  b.write(struct.pack("<I", num))

b.close()
