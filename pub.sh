#!/bin/bash

index=1
while [ $index -le 10000 ]
do
	mosquitto_pub -h "localhost" -p 1883 -t "MessageHandler/publish" -m "viewActuator[$index]:{\"pid\":\"warningpage\",\"warningpage\":[{\"pid\":\"brakefluidlow\",\"status\":\"ON\"}],\"status\":\"ON\"}"
	echo "publish[$index]"
	let index++
	sleep 1
done
