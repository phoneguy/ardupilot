#!/bin/bash
#

##	switch_vehicle.sh
#
#	install or switch vehicle type and update /etc/rc.local
#       to start ardupilot at boot
#
#       by stevenharsanyi@gmail.com
#       2016-01
##

ARDUPILOT_CONFIG=$(grep -A 1 "# B" /etc/rc.local | grep elf)

if [ $# -eq 0 ]
then
    echo "Ardupilot Vehicle Switcher"
    echo ""
    echo "Checking /etc/rc.local..."
    echo " "
    echo "Current setup is: "$ARDUPILOT_CONFIG
    echo " "
    echo "Usage: switch-vehicle.sh <vehicle type>"
    echo "select your vehicle:"
    echo "			copter"
    echo "			plane"
    echo "			rover"
    echo "			antennatracker"
    echo "			none"
    echo ""
    echo "example is: ./switch-vehicle.sh copter"
    echo " "
exit 0
fi

if [ $1 == "copter" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/.elf/d' /etc/rc.local
        sudo sed -i '/# B/a /home/debian/bin/ArduCopter.elf -A /dev/ttyO4 -B /dev/ttyO2 -E /dev/ttyO5 > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "plane" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/.elf/d' /etc/rc.local
        sudo sed -i '/# B/a home/debian/bin/ArduPlane.elf -A /dev/ttyO4 -B /dev/ttyO2 -E /dev/ttyO5 > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "rover" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/.elf/d' /etc/rc.local
        sudo sed -i '/# B/a /home/debian/bin/APMrover2.elf -A /dev/ttyO4 -B /dev/ttyO2 -E /dev/ttyO5 > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "antennatracker" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/.elf/d' /etc/rc.local
        sudo sed -i '/# B/a /home/debian/bin/AntennaTracker.elf -A /dev/ttyO4 -B /dev/ttyO2 -E /dev/ttyO5 > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "none" ]
    then
        echo $1" vehicle type selected - ardupilot removed from rc.local"
        echo " "
        sudo sed -i '/.elf/d' /etc/rc.local

else
echo "!invalid vehicle type! try again"
exit 0
fi
    echo "/etc/rc.local updated"
    echo " "
    echo " "
    echo "sync disk"
    sync
    echo " "
    echo "reboot to start Ardupilot"
    echo " "

