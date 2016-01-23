if [ "`echo $1`" = "load" ]; then
    echo "Loading Capes..."
sudo sh -c "echo 'BB-DRONECAPE' >> /sys/devices/platform/bone_capemgr/slots"
#sudo sh -c "echo 'am33xx_pwm' >> /sys/devices/platform/bone_capemgr/slots"
    dmesg | grep "SPI"
    dmesg | grep "PRU"
    cat /sys/devices/platform/bone_capemgr/slots
else
    echo "Usage:"
    echo "      ./startup.sh load  : to load the capes"
fi
