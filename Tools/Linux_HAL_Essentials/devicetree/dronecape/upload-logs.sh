#!/bin/bash
#

##
#     upload-logs.sh
#     ardupilot log file upload to mission planner log directory
#     jan-2016 stevenharsanyi@gmail.com
##

LOCAL_LOG_DIR=/var/APM/logs
UPLOAD_TARGET=192.168.2.2
UPLOAD_USER=lynne
UPLOAD_DIR=/home/lynne/missionplanner/logs

echo "copying" $LOCAL_LOG_DIR " to " $UPLOAD_DIR
sudo scp $LOCAL_LOG_DIR/*.bin $UPLOAD_USER@$UPLOAD_TARGET:$UPLOAD_DIR

echo "deleting logs..."
sudo rm -rf $LOCAL_LOG_DIR/*.*

