TARGET_IP="192.168.1.173"

#scp tbox root@$TARGET_IP:/home/root/

ssh root@$TARGET_IP "adb -s 0123456789ABCDEF pull /data/nvm"
ssh root@$TARGET_IP "scp /home/root/nvm renhono@192.168.1.18:/home/renhono/CLionProjects/GG1663/new_framework_vv/bin/"
