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

echo "This script will install the necessary files for PCIe-NPL54 on ~/Desktop"
echo "Please ensure that you are connected to the Internet."
echo "Please contact the sales representative for the password."
sleep 5


DESKTOP=/home/$SUDO_USER/Desktop/
cp pcie-npl54-es1.zip $DESKTOP
cd $DESKTOP
unzip pcie-npl54-es1.zip
sudo chown -R $SUDO_USER PCIe-NPL54-ES1

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

# ffplay
sudo apt-get install -y ffmpeg

## GStreamer with plugins
sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio 

