#!/bin/sh

# This script installs grub into a fat partition. It has to be run with sudo

DISK=$1
MOUNT=$2

# get a free loop device number for the full disk image
LOOP_DEVICE_ALL=`sudo losetup -f`
losetup $LOOP_DEVICE_ALL $DISK

# get another free loop device for the fat partition table
LOOP_DEVICE_FAT_PART=`sudo losetup -f`
OFFSET=`parted $DISK unit b print | tail -2 | head -1 | cut -f 1 --delimit="B" | cut -c 9-`
losetup $LOOP_DEVICE_FAT_PART $DISK -o $OFFSET

mount -t vfat $LOOP_DEVICE_FAT_PART $MOUNT/
grub-install --target=i386-pc --no-floppy --boot-directory=$MOUNT/boot/ --modules="normal part_msdos fat multiboot" $LOOP_DEVICE_ALL

# cleanup
umount $MOUNT
losetup -d $LOOP_DEVICE_FAT_PART
losetup -d $LOOP_DEVICE_ALL

