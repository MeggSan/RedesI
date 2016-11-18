#!/bin/bash

a=0
now=$(date +"%r")

while [ $a ]
do
   ping -c 3 "192.168.1.116" > /dev/null
   if [ $? != 0 ]
   then
   	  echo "Se ha caído la conexión. Hora actual: $now"
   fi
done