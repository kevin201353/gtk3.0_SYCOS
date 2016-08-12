#!/bin/bash
sudo echo "nameserver $1" > /etc/resolv.conf
cd /etc/resolvconf/resolv.conf.d
sudo echo "nameserver $1" > tail
sudo chmod +x tail
exit 0 
