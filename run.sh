#!/bin/bash
#make &&
./update_image.sh 

brctl addbr br0
tunctl -t tap0 -u rajnesh
brctl addif br0 eth0
brctl addif br0 tap0
ifconfig eth0 up
ifconfig tap0 up
ifconfig br0 10.0.2.21/24
ifconfig eth0 0.0.0.0 promisc
qemu -m 1G -fda floppy.img -net nic,vlan=1,macaddr=00:aa:00:60:00:01,model=rtl8139 -net tap,vlan=1,ifname=tap0,script=no
ifconfig br0 down
brctl delbr br0



#another example
#sh ../ifdown.sh
#sh ../ifup.sh
#qemu -hda ../../linuxdisk/linux-0.2.img -kernel arch/x86/boot/bzImage -net nic -net tap,ifname=tap0,script=no -append root=/dev/sda