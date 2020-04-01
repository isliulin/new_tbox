TARGET_IP="192.168.1.173"

scp tbox root@$TARGET_IP:/home/root/
ssh root@$TARGET_IP "adb -s 0123456789ABCDEF shell killall -9 monitorTbox"
ssh root@$TARGET_IP "adb -s 0123456789ABCDEF shell killall -9 tbox"
ssh root@$TARGET_IP "adb -s 0123456789ABCDEF push tbox /data/"
ssh root@$TARGET_IP "adb -s 0123456789ABCDEF shell sync"

#  od -N 4 -w8 -t x  nvm

#od -Ax -tx1 nvm
