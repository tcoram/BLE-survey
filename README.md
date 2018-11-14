# BLE-survey
Some BLE survey and detection scripts for Raspberry Pi.
Documentation coming soon...

Build: 

 $ sudo apt-get install pi-bluetooth libbluetooth-dev

 $ sudo apt-get install gawk

 $ gcc -o ble-scan ble-scan.c -lbluetooth

 $ chmod +x *.sh

Example usage:

 $ sudo ./ble-scan | ./ble-uniq.sh | ./ble-pp.sh
 
 $ sudo ./ble-scan | ./ble-track.sh XX:XX... -50
