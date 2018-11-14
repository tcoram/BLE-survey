#!/bin/bash
stdbuf -oL -eL gawk '{ printf("%s, %d, %s\n", $1, $2, strftime("%c",$3)); flush }'

