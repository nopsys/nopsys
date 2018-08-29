#!/bin/bash

SCRIPT_PATH="$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )"
if [ ! -d $SCRIPT_PATH ]; then
    echo "Could not determine absolute dir of $0"
    echo "Maybe accessed with symlink"
fi

#######################################################################
#                                                                     #
# Create Virtualbox VM for running a Nopsys OS                        #
# This script creates a simple VirtualBox host with:                  #
# - one CPU, max. capacity 75% of host                                #
# - one network card bridged to LAN, cable connected                  #
# - audiocard to be selected in VM settings below (ac97 or none)      #
# - one SATA hard disk                                                #
# - one IDE DVD player                                                #
#                                                                     #
# Edit the VM settings below to your needs                            #
#                                                                     #
#######################################################################

if [ -z "$1" ]; 
then
	echo "The first arguments to this scripts must be the filename of the ISO or VMDK with the nopsys installation"
	exit 1
else
	FILE="$1"
fi

STORAGE="${FILE##*.}"

if [[ ! "$STORAGE" = "vmdk" &&  ! "$STORAGE" = "iso" ]]; 
then
	echo "The first arguments to this scripts must be the filename of the ISO or VMDK with the nopsys installation"
	exit 1
fi 

IN_DEV=`[ -d .git ] || git rev-parse --git-dir > /dev/null 2>&1 || echo "false"`
if [ ! "$IN_DEV" = "false" ]
then
	VMNAME="CogNOS-$STORAGE-debug"
    RUN_PATH=$SCRIPT_PATH/../build/
else
	VMNAME="CogNOS-$STORAGE"
    RUN_PATH="bundles/"
fi

OSTYPE="Other_64"
ISOFILE="nopsys.iso"
HDFILE="nopsys.vmdk"
MEMORY=1024

RESULT=`vboxmanage list vms | grep $VMNAME`
if [ -z "$RESULT" ]
then
	VBoxManage createvm --name $VMNAME --ostype $OSTYPE --register
	VBoxManage modifyvm $VMNAME --memory $MEMORY
	VBoxManage storagectl $VMNAME --name "IDE Controller" --add ide
	# Enable Serial Port
	VBoxManage modifyvm $VMNAME --uart1 0x3F8 4
	VBoxManage modifyvm $VMNAME --uartmode1 file "$(pwd)/serial-output.txt"
fi

if [ $STORAGE = "iso" ]; then
	VBoxManage storageattach "$VMNAME" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium $RUN_PATH/$ISOFILE
else
	VBoxManage storageattach $VMNAME --storagectl "IDE Controller" --port 0 --device 0 --type hdd --medium $RUN_PATH/$HDFILE
	ABSOLUTE_PATH="$(cd $(dirname $RUN_PATH/$HDFILE); pwd)/$(basename $RUN_PATH/$HDFILE)"
	UUID=`VBoxManage list hdds | grep -B 4 $ABSOLUTE_PATH | grep "^UUID" | cut -d: -f 2 | xargs`
	VBoxManage internalcommands sethduuid $RUN_PATH/$HDFILE $UUID
fi

vboxmanage startvm "$VMNAME"

