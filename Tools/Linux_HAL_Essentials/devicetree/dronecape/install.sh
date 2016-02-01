#!/bin/bash
##

#    copy devicetree overlay file to /boot/dtbs/x.x.x
#    copy dronecape eeprom devicetree to lib/firmware
#
#
#
##

echo "copying file to /boot/dtbs/"$(uname -r)
#sudo cp am335x-boneblack-dronecape.dtb /boot/dtbs/$(uname -r)
echo "copying file to /lib/firmware"
#sudo cp BB-DRONECAPE-00A0.dtbo /lib/firmware

