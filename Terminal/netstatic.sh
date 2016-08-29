#!/bin/bash
INTER_DIR=/etc/network
ROOT_UID=0
if [ "$UID" -ne "$ROOT_UID" ];then
  sudo echo "Must be root to do it"
  exit 1
fi
cd $INTER_DIR ||
{
   sudo echo "The dir $INTER_DIR is not exist"
   exit 1
}

sudo echo -e "auto lo \niface lo inet loopback \nauto eth0 \niface eth0 inet static \naddress $1 \nnetmask $2 \ngateway $3" > interfaces

sudo rmmod r8169
sudo insmod /lib/modules/4.4.0-34-generic/kernel/drivers/net/ethernet/realtek/r8169.ko
sleep 1
exit 0
