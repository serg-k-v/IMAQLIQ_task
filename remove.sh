#!/bin/bash

systemctl stop imaqliq-server
sleep 2s
rm -rf /tmp/imaqliq_task/
sudo rm /etc/systemd/system/imaqliq-server.s*
systemctl daemon-reload