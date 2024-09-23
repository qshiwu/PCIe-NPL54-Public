#!/bin/bash

if [ "$EUID" -eq 0 ]
 then echo "Please run WITHOUT sudo"
 exit
fi

pkill -2 gst-launch-1.0
pkill -2 ffplay

# GStreamer
gnome-terminal -- ffplay /dev/video0
gnome-terminal -- ffplay /dev/video1
gnome-terminal -- ffplay /dev/video2
gnome-terminal -- ffplay /dev/video3




