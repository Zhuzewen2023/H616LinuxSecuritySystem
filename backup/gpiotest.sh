#!/bin/bash

if !which gpio >/dev/null 2>&1; then
		echo "please install wiringOP first"
		exit 1
fi

gpio mode 2 out #livingroom light
gpio mode 5 out #bedroom light
gpio mode 7 out #fan
gpio mode 8 out #lock
gpio mode 9 out #beep

for i in 2 5 7 8 9
do
		gpio write $i 1
done

for i in 2 5 7 8 9
do
		gpio write $i 0
		sleep 3
		gpio write $i 1
done

gpio mode 6 in #smoke

gpio readall
sleep 5
gpio readall

