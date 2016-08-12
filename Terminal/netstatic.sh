#!/bin/bash
INTER_DIR=~/../etc/network
ROOT_UID=0
if [ "$UID" -ne "$ROOT_UID" ];then
  echo "Must be root to do it"
  exit 1
fi
cd $INTER_DIR ||
{
   echo "The dir $INTER_DIR is not exist"
   exit 1
}

echo -e "auto lo \niface lo inet loopback \nauto eth0 \niface eth0 inet static \naddress $1 \nnetmask $2 \ngateway $3" > interfaces

#cd ~/../etc
#echo -e "nameserver 127.0.1.1 \nnameserver $4" > resolv.conf
#sudo /etc/init.d/networking restart
sudo ifdown eth0
sudo ifup eth0
exit 0
