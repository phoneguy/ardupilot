#!/bin/bash

##
#	beaglebone ardupilot install.sh script
#	this script updates the system, installs firmware files
#       jan-2016 stevenharsanyi@gmail.com
##

echo "Update software: "
sudo apt-get update && sudo apt-get upgrade -y

echo " Install software: "
sudo apt-get install -y python python-dev
sudo apt-get install -y cpufrequtils g++ gawk git make ti-pru-cgt-installer device-tree-compiler screen

echo "Install Mavlink and Droneapi:"
sudo pip install mavproxy
sudo pip install droneapi

echo "Update Timezone:"
sudo dpkg-reconfigure tzdata

echo "Update /opt/scripts: "
cd /opt/scripts && sudo git pull

#echo "Install RT Kernel: "
#sudo /opt/scripts/tools/update_kernel.sh --bone-rt-kernel --lts-4_1

echo "Change to /home/debian/bin: "
cd /home/debian/bin

echo "copying file to /boot/dtbs/"$(uname -r)
sudo cp am335x-boneblack-dronecape.dtb /boot/dtbs/$(uname -r)

echo "Add DRONECAPE DTB to uEnv.txt: "
sudo sed -i 's/#dtb=$/dtb=am335x-boneblack-dronecape.dtb/' /boot/uEnv.txt

echo "Copying file to /lib/firmware"
sudo cp BB-DRONECAPE-00A0.dtbo /lib/firmware

echo "Add DRONECAPE DTBO to uEnv.txt: "
sudo sed -i 's/#cape_enable=bone_capemgr.enable_partno=/cape_enable=bone_capemgr.enable_partno=BB-DRONECAPE/g' /boot/uEnv.txt 

echo "Adjusting the BBB clock: "
sudo sed -i 's/GOVERNOR="ondemand"/GOVERNOR="performance"/g' /etc/init.d/cpufrequtils

#// Clone overlays:
#//cd /home/debian
#//git clone https://github.com/beagleboard/bb.org-overlays

#// Change dir:
#//cd bb.org-overlays

#// Update DTC:
#./dtc-overlay.sh

#//Build and install overlays:
#//./install.sh

echo "Clone ArduPilot code:"
cd /home/debian
git clone https://github.com/phoneguy/ardupilot ~/ardupilot

echo "Change to ardupilot dir: "
cd ardupilot

echo "Checkout bbb branch: "
git checkout bbb

echo "Init and update submodules: "
git submodule update --init

echo "Change dir to rangefinderpru: "
cd /home/debian/ardupilot/Tools/Linux_HAL_Essentials/pru/rangefinderpru

echo "Build Rangefinder firmware: "
make

echo "Install Rangefinder firmware: "
sudo make install

cd /home/debian

echo "Your BeagleBone is now ready to use. "
echo " "
echo " You must sudo reboot to complete install"
echo ""

