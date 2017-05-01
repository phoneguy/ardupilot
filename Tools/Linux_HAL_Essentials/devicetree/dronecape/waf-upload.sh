#!/bin/bash

LOCAL_LOG_DIR=/var/APM/logs
LOCAL_UPLOAD_DIR=/home/hp/code/phonepilot/build/bbbmini/bin
UPLOAD_TARGET=192.168.2.14
UPLOAD_USER=root
UPLOAD_DIR=/home/debian/phonepilot/bin

echo "copying" $LOCAL_LOG_DIR " to " $UPLOAD_DIR
scp $LOCAL_UPLOAD_DIR/* $UPLOAD_USER@$UPLOAD_TARGET:$UPLOAD_DIR

