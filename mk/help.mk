help:
	@echo ""
	@echo " ArduPilot Building"
	@echo " =================="
	@echo ""
	@echo " The following web page has detailed information on building the code"
	@echo "     http://dev.ardupilot.com/wiki/building-the-code/"
	@echo ""
	@echo " Before building a target you need to be in the target vehicle type directory"
	@echo " e.g. ArduPlane, ArduCopter, APMrover2, AntennaTracker"
	@echo ""
	@echo " Most targets support a \"-upload\" extension to upload the firmware"
	@echo " to a connected board.  e.g. \"make px4-v2-upload\""
	@echo ""
	@echo " Some targets support a \"-debug\" extension to enable a debug build"
	@echo " (with debug symbols, and without optimisation)"
	@echo ""
	@echo " Note that the px4 builds are NOT parallel safe, NO -j flag"
	@echo ""
	@echo " Targets"
	@echo " -------"
	@echo ""
	@echo "  px4-v1 - the PX4v1 board"
	@echo "  px4-v2 - the Pixhawk"
	@echo "  px4-v4 - the XRacer"
	@echo "  pxf - the Beagle Bone Black (BBB) + PXF cape combination"
	@echo "  navio - the RaspberryPi + NavIO cape combination"
	@echo "  linux - a generic Linux build"
	@echo "  flymaple - the FlyMaple board"
	@echo "  vrbrain - the VRBrain boards"
	@echo "  sitl - the SITL Software In The Loop simulation"
	@echo "  bbbmini - the Beagle Bone Black mini"
	@echo "  bebop - the Parrot Bebop"
	@echo "  bbb - beaglebone black dual imu"
	@echo "  raspilot - the RaspberryPi + pilot cape combination"
	@echo "  erleboard - the ErleBoard legacy design"
	@echo "  minlure - the drone lure for MinnowBoard and others"
	@echo "  erlebrain2 - the Erle-Brain 2 board"
