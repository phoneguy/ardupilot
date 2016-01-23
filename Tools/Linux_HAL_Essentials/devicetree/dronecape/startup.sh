if [ "`echo $1`" = "load" ]; then
    echo "Loading Capes..."
sudo sh -c "echo 'BB-DRONECAPE' >> /sys/devices/platform/bone_capemgr/slots"
#    echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots
#    echo bone_pwm_P8_36 > /sys/devices/bone_capemgr.9/slots
    dmesg | grep "SPI"
    dmesg | grep "PRU"
    cat /sys/devices/platform/bone_capemgr/slots
else
    echo "Usage:"
    echo "      ./startup.sh load  : to load the capes"
fi
