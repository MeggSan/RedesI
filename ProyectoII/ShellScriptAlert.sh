#!/bin/bash

a=0

while [ $a ]
do
   ping -c 3 "192.168.1.116" > /dev/null
   if [ $? != 0 ]
   then
   	  echo "Se ha caído la conexión"
   fi
done