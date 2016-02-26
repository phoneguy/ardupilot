#!/bin/bash
#

##	switch_vehicle.sh
#
#	install or switch vehicle type and update /etc/rc.local
#       to start ardupilot at boot
#
#       by stevenharsanyi@gmail.com
#       jan-2016
##

ARDUPILOT_CONFIG=$(grep -A 1 "# B" /etc/rc.local | grep ardu)

clear

if [ $# -eq 0 ]
then
    echo "Ardupilot Vehicle Switcher"
    echo " "
    echo "Checking /etc/rc.local..."
    echo " "
    echo "Current setup is: "$ARDUPILOT_CONFIG
    echo "                                           TELEM1     GPS1     TELEM2     GPS2  "
    echo "Usage: switch-vehicle.sh <vehicle type> -A <port> -B <port> -C <port> -E <port>"
    echo "select your vehicle:"
    echo "			plane"
    echo "			copter"
    echo "			rover"
    echo "			none"
    echo " "
    echo "example 1 is: ./switch-vehicle.sh copter -A /dev/ttyS4 -B /dev/ttyS5"
    echo " "
    echo "example 2 is: ./switch-vehicle.sh copter -A udp:192.168.2.254:14550 -B /dev/ttyS5 -C /dev/ttyS4"

exit 0
fi

if [ $1 == "copter" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/ardu/d' /etc/rc.local
        sudo sed -i '/# B/a /home/debian/bin/arducopter '$2' '$3' '$4' '$5' '$6' '$7' '$8' '$9' > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "plane" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/ardu/d' /etc/rc.local
        sudo sed -i '/# B/a /home/debian/bin/arduplane '$2' '$3' '$4' '$5' '$6' '$7' '$8' '$9' > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "rover" ]
    then
        echo $1" vehicle type selected"
        echo " "
        sudo sed -i '/ardu/d' /etc/rc.local
        sudo sed -i '/# B/a /home/debian/bin/ardurover '$2' '$3' '$4' '$5' '$6' '$7' '$8' '$9' > /home/debian/startup.log &' /etc/rc.local

elif [ $1 == "none" ]
    then
        echo $1" vehicle type selected - Ardupilot removed from rc.local"
        echo " "
        sudo sed -i '/ardu/d' /etc/rc.local
    else
        echo "!invalid vehicle type! try again"
        exit 0
fi

    echo "/etc/rc.local updated:"
    cat /etc/rc.local
    echo " "
    echo " "
    echo "sudo reboot to start Ardupilot"
    echo " "
