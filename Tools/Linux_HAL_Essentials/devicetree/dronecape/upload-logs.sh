#!/bin/bash
#

##    upload-logs.sh
#     ardupilot log file upload to mission planner log directory
#
#

LOCAL_LOG_DIR=/var/APM/logs
UPLOAD_TARGET=192.168.2.30
UPLOAD_USER=lynne
UPLOAD_DIR=~/missionplanner/logs

echo "copying /var/APM/logs to missionplanner/logs on laptop..."
#sudo scp /var/APM/logs/*.bin lynne@192.168.2.17:~/missionplanner/logs
sudo scp /var/APM/logs/*.bin $UPLOAD_USER@$UPLOAD_TARGET:$UPLOAD_DIR

#echo "deleting logs..."
#sudo rm -rf /var/APM/logs/*.*

