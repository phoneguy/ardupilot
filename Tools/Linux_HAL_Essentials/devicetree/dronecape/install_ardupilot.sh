#!/bin/bash
#

#
#	install vehicle type and update /etc/rc.local to start ardupilot at boot
#       by stevenharsanyi@gmail.com
#

## sudo sed -i 's/#dtb=$/dtb=am335x-boneblack-dronecape.dtb/' /boot/uEnv.txt
##
#

ARDUPILOT_CONFIG=$(grep -B 1 "exit 0" /etc/rc.local | grep elf)

if [ $# -eq 0 ]
then
    echo "Ardupilot Installer"
    echo ""
    echo "Checking /etc/rc.local..."
    echo " "
    echo "Current setup is: "$ARDUPILOT_CONFIG
    echo " "
    echo "Usage: install_ardupilot.sh <vehicle type>"
    echo "select your vehicle:"
    echo "			copter"
    echo "			plane"
    echo "			rover"
    echo "			heli"
    echo "			antennatracker"
    echo ""
    echo "example is: ./install.sh copter"
    echo ""
exit 0
fi

if [ $1 == "copter" ]
then
    echo $1" vehicle type selected"
    echo " "
##sudo sed -i 's/*.elf$/ArduCopter.elf -A udp:192.168.2.91:14551 -B /dev/ttyO5 -E /dev/ttyO2/ > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "plane" ]
then
    echo $1" vehicle type selected"
    echo " "
##sudo sed -i 's/*.elf$/ArduPlane.elf -A /dev/ttyO4 -B /dev/ttyO5 -E /dev/ttyO2/ > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "rover" ]
then
    echo $1" vehicle type selected"
    echo " "
##sudo sed -i 's/*.elf$/APMrover2.elf -A /dev/ttyO4 -B /dev/ttyO5 -E /dev/ttyO2/ > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "antennatracker" ]
then
    echo $1" vehicle type selected"
    echo " "
##sudo sed -i 's/*.elf$/AntennaTracker.elf -A /dev/ttyO4 -B /dev/ttyO5 -E /dev/ttyO2/ > /home/debian/startup.log &' /etc/rc.local

else
echo "!invalid vehicle type! try again"
exit 0
fi
    echo "/etc/rc.local updated"
    echo " "
    echo "copying BB-DRONECAPE-00A0.dtbo to /lib/firmware"
#    cp BB-DRONECAPE-OOAO-dtbo /lib/firmware
    echo " "
    echo "updating initramfs"
#    sudo update-initramfs -u -k $(uname -r)
    echo "sync disk"
    sync
    echo " "
    echo "please restart beaglebone to start ardupilot"
    echo " "

