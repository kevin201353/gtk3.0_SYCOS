#!/bin/bash
INTER_DIR=~/../etc/network
ROOT_UID=0
if [ "$UID" -ne "$ROOT_UID" ];then
  echo "Must be root to do it"
  exit 1
fi
cd $INTER_DIR ||
{
   echo "The file $INTER_DIR is not exist"
   exit 1
}

cat /dev/null > interfaces_zh && echo -e "auto lo\niface lo inet loopback" > interfaces_zh
#sudo /etc/init.d/networking restart
exit 0
