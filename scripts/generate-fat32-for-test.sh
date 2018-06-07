#!/bin/bash

# this script generates a raw FAT32 file that is to be used in FAT32 filesystem tests inside the image.
# The contents of the generated file should be sinchronised with the contents that test cases expect.

sizeMB=40 # if smaller than this gparted will fail
generatedFileName="testdata/ExampleFAT32.raw"

./scripts/create-fat32.sh $sizeMB $generatedFileName

offset=$(parted $generatedFileName unit b print | tail -2 | head -1 | cut -f 1 --delimit="B" | cut -c 9-) 

echo $offset

loopDevice=$(losetup -f)
losetup -o $offset $loopDevice $generatedFileName
mount -t vfat $loopDevice ./mount/

# create / copy files
touch mount/empty.txt
echo zaraza >mount/ascii
echo 123456789 >mount/asciinumbers

cp SqueakNOS.changes mount/

echo " " >mount/morethanonesector
for i in {1..100}
do
   echo "1234567890abc" >>mount/morethanonesector
done

echo " " >mount/morethanonecluster
for i in {1..10000}
do
   echo "1234567890abc" >>mount/morethanonecluster
done

mkdir mount/dira
echo a >mount/dira/file.txt

mkdir mount/dirlongname
echo a >mount/dirlongname/file.txt

umount ./mount/

losetup -d $loopDevice


