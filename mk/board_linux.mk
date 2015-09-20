TOOLCHAIN = NATIVE
BOARD_LINUX_HOST = debian@192.168.2.30
include $(MK_DIR)/find_tools.mk

# Linux build is just the same as SITL for now
LIBS = -lm -lpthread -lrt
include $(MK_DIR)/board_native.mk
include $(MK_DIR)/upload_firmware.mk
