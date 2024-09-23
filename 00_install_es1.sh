#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run with sudo"
  exit
fi

echo "█████╗  ██████╗██╗███████╗    ███╗   ██╗██████╗ ██╗     ███████╗██╗  ██╗"
echo "██╔══██╗██╔════╝██║██╔════╝    ████╗  ██║██╔══██╗██║     ██╔════╝██║  ██║"
echo "██████╔╝██║     ██║█████╗█████╗██╔██╗ ██║██████╔╝██║     ███████╗███████║"
echo "██╔═══╝ ██║     ██║██╔══╝╚════╝██║╚██╗██║██╔═══╝ ██║     ╚════██║╚════██║"
echo "██║     ╚██████╗██║███████╗    ██║ ╚████║██║     ███████╗███████║     ██║"
echo "╚═╝      ╚═════╝╚═╝╚══════╝    ╚═╝  ╚═══╝╚═╝     ╚══════╝╚══════╝     ╚═╝"

echo "This script will install the necessary files for PCIe-NPL54"
echo "Please ensure that you are connected to the Internet."
sleep 3



# ssh
sudo apt-get update
sudo apt-get install -y openssh-server
sudo service ssh start
sudo systemctl enable ssh

# ifconfig
sudo apt-get install -y net-tools

# git
sudo apt-get install -y git

# compile tools
sudo apt-get install -y make gcc g++
sudo apt-get install -y libglfw3-dev mesa-utils libglew-dev
sudo dpkg --configure -a
sudo apt-get install -y libasound2-dev
sudo apt-get install dwarves

# ffplay
sudo apt-get install -y ffmpeg

# GStreamer with plugins
sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio 

clear 
echo "Please contact the sales representative for the password."
sleep 3
# FPGA kernel modules
INSTALL_FOLDER=$(pwd)/ES1
mkdir -p $INSTALL_FOLDER
cp pcie-npl54-es1.zip $INSTALL_FOLDER
cd $INSTALL_FOLDER
unzip pcie-npl54-es1.zip

cd $INSTALL_FOLDER/PCIe-NPL54-ES1/lscvdma
make clean
make 

cd $INSTALL_FOLDER/PCIe-NPL54-ES1/v4l2loopback
make clean
make install

sudo chown -R $SUDO_USER $INSTALL_FOLDER
