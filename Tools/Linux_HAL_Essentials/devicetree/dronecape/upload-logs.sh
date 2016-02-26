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

echo "copying /var/APM/logs to missionplanner/logs on " $UPLOAD_TARGET
sudo scp /var/APM/logs/*.bin $UPLOAD_USER@$UPLOAD_TARGET:$UPLOAD_DIR

echo "deleting logs..."
sudo rm -rf /var/APM/logs/*.*

