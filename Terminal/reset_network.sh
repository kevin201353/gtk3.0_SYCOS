#@/bin/bash

sudo rmmod r8169
sudo insmod /lib/modules/4.4.0-34-generic/kernel/drivers/net/ethernet/realtek/r8169.ko
sleep 1


