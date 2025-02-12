#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run with sudo"
  exit
fi

echo "█████╗   ██████╗██╗███████╗    ███╗   ██╗██████╗ ██╗     ███████╗██╗  ██╗"
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

# xdotool
sudo apt-get install -y xdotool

# compile tools
sudo apt-get install -y make gcc g++ g++-12
sudo apt-get install -y libglfw3-dev mesa-utils libglew-dev
sudo dpkg --configure -a
sudo apt-get install -y libasound2-dev
sudo apt-get install help2man

# Skipping BTF generation xxx. due to unavailability of vmlinux
# https://askubuntu.com/questions/1348250/skipping-btf-generation-xxx-due-to-unavailability-of-vmlinux-on-ubuntu-21-04
sudo apt-get install -y dwarves
sudo cp /sys/kernel/btf/vmlinux /usr/lib/modules/`uname -r`/build/

# add script to run in shutdown
sudo rm /usr/lib/systemd/system-shutdown/shutdown.sh
sudo rm /etc/systemd/system/shutdown-script.service
sudo systemctl daemon-reload
sudo cp app/NPL54Capture/script/shutdown.sh /usr/lib/systemd/system-shutdown/
sudo cp app/NPL54Capture/script/shutdown-script.service /etc/systemd/system/
sudo systemctl daemon-reload
sleep 1
sudo systemctl enable shutdown-script.service

# ffplay
sudo apt-get install -y ffmpeg

# GStreamer with plugins
sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio 

clear 
echo "Compile kernel module."
sleep 2
# FPGA kernel modules
INSTALL_FOLDER=$(pwd)
cd $INSTALL_FOLDER
make clean
make 

# https://github.com/umlaeute/v4l2loopback/issues/382
# Enable automatic module loading for v4l2loopback
# (write v4l2loopback into /etc/modules-load.d/v4l2loopback.conf )
echo ""
echo ""
echo "Compile v4l2loopback module."
sleep 2
cd $INSTALL_FOLDER/v4l2loopback
sudo make clean
sudo make KCPPFLAGS="-DMAX_DEVICES=100" install-all
make

# sudo touch /etc/modules-load.d/v4l2loopback.conf
# sudo chmod 777 /etc/modules-load.d/v4l2loopback.conf
# sudo echo v4l2loopback > /etc/modules-load.d/v4l2loopback.conf

sudo chown -R $SUDO_USER $INSTALL_FOLDER
