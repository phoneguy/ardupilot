#!/bin/bash
#

# make bbb and copy to deploy directory
# copy over to beaglebone to /home/debian/bin

LOCAL_ARDUPILOT_DIR=~/ardupilot
UPLOAD_TARGET=192.168.2.30
UPLOAD_USER=debian
UPLOAD_DIR=~/bin

# update git
#rm -rf deploy
mkdir deploy
git checkout bbb-mpu9250
git pull diydrones master
git submodule update --init
make clean
make bbb -j4

# copy to deploy dir
cp ArduCopter/ArduCopter.elf deploy
cp ArduPlane/ArduPlane.elf deploy
cp APMrover2/APMrover2.elf deploy
cp AntennaTracker/AntennaTracker.elf deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/am335x-boneblack-dronecape.dtb deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/BB-DRONECAPE-00A0.dtbo deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/README.md deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/startup.sh deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/install.sh deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/upload-logs.sh deploy
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/switch-vehicle.sh deploy

# copy over to beaglebone
# default password is    temppwd
scp ~/ardupilot/deploy/*.* debian@192.168.2.30:~/bin