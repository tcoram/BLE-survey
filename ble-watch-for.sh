#!/bin/bash

if [[ $# < 3 ]] ; then
    echo "Usage: $0 target-mac min-rssi cmd [args]" >&2
    exit 1
fi

TARGET=$1; shift
MINRSSI=$1; shift

echo "Watching for $TARGET (min rssi=$MINRSSI)"
sudo  timeout -sSIGINT 20 stdbuf -oL ./ble-scan | \
    gawk -vtarget="$TARGET" -vrssi="$MINRSSI" -vcmd="$*" -- \
	'$1 == target &&  $2 >= rssi {
	             "date"|getline ts; printf("%s,%s,%s\n",$1,$2,ts); 
                     system(cmd); exit(0);}'
