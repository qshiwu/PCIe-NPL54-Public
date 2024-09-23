#!/bin/bash

if [ "$EUID" -ne 0 ]
 then echo "Please run with sudo"
 exit
fi

INSTALL_FOLDER=$(pwd)/ES1
V4L2LOOPBACK_FOLDER=$INSTALL_FOLDER/PCIe-NPL54-ES1/v4l2loopback
LSVDMA_FOLDER=$INSTALL_FOLDER/PCIe-NPL54-ES1/lscvdma

# In case of 4 cameras
cd $V4L2LOOPBACK_FOLDER
sudo rmmod v4l2loopback
sudo modprobe v4l2loopback devices=4
ls /dev/video*
sleep 1

cd $LSVDMA_FOLDER/drvr
sudo insmod lscvdma.ko poll_mode=0
ls /dev/lscvdma0*
sleep 1

# FPGA
sudo pkill -2 AVCapture

cd $LSVDMA_FOLDER/app/GUI/AVCapture
sudo ./AVCapture -c 0 &
sudo ./AVCapture -c 1 &
sudo ./AVCapture -c 2 &
sudo ./AVCapture -c 3 &





