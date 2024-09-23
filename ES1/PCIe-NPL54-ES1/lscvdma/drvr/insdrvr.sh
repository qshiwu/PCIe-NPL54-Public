#!/bin/sh

#----------------------------------------------------------------------
# This shell script installs the the Lattice lscvdma Driver Module
# into the Linux kernel.
#----------------------------------------------------------------------
#

module="lscvdma"

if [ `whoami` != root ]; then
	echo "ERROR! Must be root to install driver."
	exit 1
fi

echo "Installing driver module "${module}" into kernel."

# Install the driver module (pass any command line args - none expected)
/sbin/insmod ./${module}.ko $* || exit 1


echo "Done."

