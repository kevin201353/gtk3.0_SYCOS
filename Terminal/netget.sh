#! /bin/bash

default_route=$(ip route show)
default_interface=$(echo $default_route | sed -e 's/^.*dev \([^ ]*\).*$/\1/' | head -n 1)
address=$(ip addr show label $default_interface scope global | awk '$1 == "inet" { print $2,$4}')

#ip address
ip=$(echo $address | awk '{print $1 }')
ip=${ip%%/*}

#broadcast
broadcast=$(echo $address | awk '{print $2 }')

#mask address
mask=$(route -n |grep 'U[ \t]' | head -n 1 | awk '{print $3}')

#gateway address
gateway=$(route -n | grep 'UG[ \t]' | awk '{print $2}')

#dns
dns=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}')

#echo ip:$ip,mask:$mask,broadcast:$broadcast,gateway:$gateway,dns:$dns

echo -e "$ip\n$mask\n$gateway\n$dns" > nettmp.txt
exit 0
