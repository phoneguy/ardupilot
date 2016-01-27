#!/bin/bash
#

# make bbb and copy to deploy directory
# copy over to beaglebone to /home/debian/bin

#rm -rf deploy
mkdir deploy
git checkout bbb-mpu9250
git pull diydrones master
git submodule update --init
make clean
make bbb -j4
cp ArduCopter/ArduCopter.elf deploy
cp ArduPlane/ArduPlane.elf deploy
cp APMrover2/APMrover2.elf deploy
cp AntennaTracker/AntennaTracker.elf deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/switch-vehicle.sh deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/BB-DRONECAPE-00A0.dtbo deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/README.md deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/startup.sh deploy

scp ~/ardupilot/deploy/*.* debian@192.168.2.30:~/bin
