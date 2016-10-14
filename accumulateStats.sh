#!/bin/bash

#Date, Time, miles, speed, elevation
while [[ $# -gt 1 ]]
 do
 key="$1"
 
 case $key in
   -T)
   TIME="$2"
   shift
   ;;
   -D)
   DATE="$2"
   shift
   ;;
   -M)
   MILES="$2"
   shift
   ;;
   -S)
   SPEED="$2"
   shift
   ;;
   -E)
   ELEVATION="$2"
   shift
   ;;
   *)
   ;;
 esac
 shift
 done

 DATE=`date -d ${DATE}T${TIME} +%s`

 echo "$DATE $MILES $SPEED $ELEVATION" >> toms_bike_ride.txt
