#!/bin/bash 
stdbuf -oL -eL gawk ' { if (!mac[$1]) { mac[$1] = $0; print; flush }}'
