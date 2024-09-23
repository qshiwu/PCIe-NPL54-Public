# PCIe-NPL54-Public
- Please ask your sales representative for the unzip password.


# 
```
VIDEO_ID=/dev/video1
gst-launch-1.0 v4l2src device=$VIDEO_ID ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! fpsdisplaysink name=fps video-sink=xvimagesink
gst-launch-1.0 v4l2src device=$VIDEO_ID ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! xvimagesink
```

