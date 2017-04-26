#!/usr/bin/python 

import struct


fileIN="qspic.imx.txt8"
fileOUT="qspic.imx2"

a=open(fileIN, "ro")
b=open(fileOUT, "wb")

ln=[]
l=a.readlines()

for n in l:
 if n != '':
   b.write(struct.pack("<i", int(n, 16)))

# print bytearray(ln)

a.close()
b.close()
