#!/bin/bash
#

##
#	waf_upload.sh
#
# 	make bbb with waf and copy to deploy directory
# 	copy to beaglebone /home/debian/bin
#
##

LOCAL_ARDUPILOT_DIR=~/ardupilot       # where your ardupilot source is
WAF=~/ardupilot/modules/waf/waf-light # waf location

SOURCE_CODE=https://github.com/diydrones/ardupilot # ardupilot source
SOURCE_BRANCH=master            # what branch to pull from
BUILD_TARGET=bbb                # what target to build
UPLOAD_TARGET=192.168.2.7       # beaglebone ip
UPLOAD_USER=debian              # default user is debian, default password is temppwd
UPLOAD_DIR=/home/debian/bin     # /home/debian/bin on beaglebone
DEPLOY_DIR=~/ardupilot/deploy   # compiled files will be copied here for uploading

## update git, create deploy dir, make and upload elfs and scripts

# delete and create files upload dir
echo "Create upload dir: "
rm -rf $DEPLOY_DIR
mkdir $DEPLOY_DIR

# update project
echo "Pull from github.com/diydrones/ardupilot master branch: "
git pull $SOURCE_CODE $SOURCE_BRANCH

echo "Update submodules and init new ones: "
git submodule update --recursive --init

# clean, configure and build project
$WAF distclean
$WAF configure --board=$BUILD_TARGET
$WAF all -j4
#$WAF bin -j4

# copy to deploy dir
echo "Copying files to to deploy dir: "
cp build/$BUILD_TARGET/ArduPlane/#bin/* deploy
cp build/$BUILD_TARGET/ArduCopter/#bin/* deploy
cp build/$BUILD_TARGET/APMrover2/#bin/* deploy

cp -r build/$BUILD_TARGET/examples deploy
cp -r build/$BUILD_TARGET/tools deploy
cp -r build/$BUILD_TARGET/tests deploy

# rebuild dts sourcefile
cd Tools/Linux_HAL_Essentials/devicetree/dronecape/ && make clean && make
cd $LOCAL_ARDUPILOT_DIR

# copy all to deploy dir
cp Tools/Linux_HAL_Essentials/devicetree/dronecape/*.* deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/am335x-boneblack-dronecape.dtb deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/BB-DRONECAPE-00A0.dtbo deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/README.md deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/startup.sh deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/install.sh deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/upload-logs.sh deploy
#cp Tools/Linux_HAL_Essentials/devicetree/dronecape/switch-vehicle.sh deploy

# copy over to beaglebone
# default password is    temppwd
echo "Uploading to beaglebone: "
scp -r $LOCAL_ARDUPILOT_DIR/deploy/* $UPLOAD_USER@$UPLOAD_TARGET:$UPLOAD_DIR
