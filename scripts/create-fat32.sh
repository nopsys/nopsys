#!/bin/sh

# This script creates a disk image file in raw format. The disk has the
# specified size (in mb), and only one primary partition with a fat32
# filesystem, to which it copies fs-contents 

if [ ! $# -eq 3 ]
then
	echo Usage: `basename $0` size-in-mb fs-contents output-file
	echo Example: `basename $0` 33 disk.img
	exit
fi

if [ $1 -le 32 ]
then
	echo "Minimum fat32 size is 65527 clusters, which gives around 32MB."
	echo "So, you have to use disk sizes of over 32MB."
	exit
fi

sizeMB=$1
contents=$2
imageFile=$3

size=$(echo $(($sizeMB*1024*1024/512)))               # set size of disk
sizeInBytes=$(echo $(($sizeMB*1024*1024)))               # set size of disk

dd if=/dev/zero of=$imageFile bs=512 count=$size    # equivalent to: qemu-img create -f raw harddisk.img 100M
parted $imageFile mktable msdos                     # create partition table
parted $imageFile "mkpart primary fat32 2 -0"             # make primary partition, type fat32 from 1 to end
parted $imageFile toggle 1 boot                     # make partition 1 bootable
parted $imageFile unit b print
rm -f $imageFile-partition
fallocate -l $sizeInBytes $imageFile-partition
mkfs.fat -F 32 $imageFile-partition                 # make fat32 filesystem on partition

mcopy -s -i $imageFile-partition $contents/* ::

if [ ! -d "fileBenchData" ]
then
	mkdir "fileBenchData"
fi
pushd "fileBenchData"    
for ((i=1;i<=100;i++));
do
	echo $i > "$i.data"
done
popd > /dev/null

mcopy -s -i $imageFile-partition "fileBenchData" ::

#sudo mount -o loop $imageFile-partition build/mount/
#cp -r $contents build/mount
#sudo umount build/mount

offset=`parted $imageFile unit b print | tail -2 | head -1 | cut -f 1 --delimit="B" | cut -c 9-`
echo partition offset is $offset


dd if=$imageFile-partition of=$imageFile bs=512 seek=$(($offset/512))
rm $imageFile-partition
