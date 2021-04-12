#!/bin/bash

index=1
while [ $index -le 10000 ]
do
	./mosquitto_pub -h "localhost" -p 1883 -t "BDSTAR/view" -m "{\"keyname1\":\"brakefluidlow\",\"keyname2\":\"brakefluidlow\",\"onoff\":\"ON\"}"
	echo "publish[$index]"
	let index++
	sleep 1
done
