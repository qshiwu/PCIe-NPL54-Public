#!/bin/bash

if [ "$EUID" -eq 0 ]
 then echo "Please run WITHOUT sudo"
 exit
fi


pkill -2 gst-launch-1.0
pkill -2 ffplay

# GStreamer

gnome-terminal -- gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! fpsdisplaysink name=fps video-sink=xvimagesink
gnome-terminal -- gst-launch-1.0 v4l2src device=/dev/video1 ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! fpsdisplaysink name=fps video-sink=xvimagesink
gnome-terminal -- gst-launch-1.0 v4l2src device=/dev/video2 ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! fpsdisplaysink name=fps video-sink=xvimagesink
gnome-terminal -- gst-launch-1.0 v4l2src device=/dev/video3 ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! fpsdisplaysink name=fps video-sink=xvimagesink




