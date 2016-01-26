if [ "`echo $1`" = "load" ]; then
    echo "Loading Capes..."
sudo sh -c "echo 'BB-DRONECAPE' >> /sys/devices/platform/bone_capemgr/slots"
#sudo sh -c "echo 'am33xx_pwm' >> /sys/devices/platform/bone_capemgr/slots"
    dmesg | grep "SPI"
    dmesg | grep "PRU"
    dmesg | grep "tty"
    sudo cat /sys/devices/platform/bone_capemgr/slots | more
    sudo cat /sys/kernel/debug/pinctrl/44e10800.pinmux/pins | more
else
    echo "Usage:"
    echo "      ./startup.sh load  : to load the capes"
fi
