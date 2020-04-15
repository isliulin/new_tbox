
adb shell killall -9 monitorTbox
adb shell killall -9 tbox
adb shell killall -9 gdbserver

adb push tbox /data/

#adb shell gdbserver :2345 /data/tbox