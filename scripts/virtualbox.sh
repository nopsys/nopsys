#!/bin/bash
SCRIPT_PATH=`dirname $0`;
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
# Edit the VM settings below to your neccesity                        #
#                                                                     #
#######################################################################

## The VM name and type
STORAGE=iso
if [ ! -z "$1" ]; then
	STORAGE=$1
fi

RELEASE=${RELEASE:-debug}

VMNAME="CogNOS"-$STORAGE-$RELEASE
OSTYPE="Other_64"
ISOFILE="nopsys.iso"
HDFILE="nopsys.vmdk"
MEMORY=1024

if [ $RELEASE = "debug" ]; then
	RUN_PATH=$SCRIPT_PATH/../build/
	echo path is $RUN_PATH
else
    RUN_PATH = .
fi

RESULT=`vboxmanage list vms | grep $VMNAME`
if [ -z "$RESULT" ]
then
	VBoxManage createvm --name $VMNAME --ostype $OSTYPE --register
	VBoxManage storagectl $VMNAME --name "IDE Controller" --add ide
	if [ $STORAGE = "iso" ]; then
		VBoxManage storageattach $VMNAME --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium $RUN_PATH/$ISOFILE
	else
		VBoxManage storageattach $VMNAME --storagectl "IDE Controller" --port 0 --device 0 --type hdd --medium $RUN_PATH/$HDFILE
	fi
	VBoxManage modifyvm $VMNAME --memory $MEMORY
fi
vboxmanage startvm $VMNAME

