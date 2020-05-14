#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.


OUTDIR=/tmp/ecen5013
REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
#DIR is the path where writer executable, finder.sh and tester.sh are stored.
#Refer: https://www.electrictoolbox.com/bash-script-directory/
DIR="$( cd "$(dirname "$0")" ; pwd -P )"

if [ $# -lt 1 ]
then
	echo -e "\nUSING DEFAULT DIRECTORY ${OUTDIR} TO STORE FILES\n"
else
	OUTDIR=$1
	echo -e "\n USING ${OUTDIR} DIRECTORY TO STORE FILES\n"
fi

if [ -d "$OUTDIR" ]
then
	echo -e "\nDIRECTORY ALREADY EXISTS\n"
#	rm -rf "$OUTDIR"/*
else
	mkdir -p "$OUTDIR"
	
	if [ -d "$OUTDIR" ]
	then
		echo -e "\n${OUTDIR} CREATED\n"
	else
		echo -e "\nCANNOT CREATE ${OUTDIR}\n"
	exit 1
	fi
fi



#Installing Dependencies
echo -e "\nINSTALLING THE REQUIRED DEPENDENCIES FOR THE INSTALLATION\n"
"${DIR}"/dependencies.sh


#TODO: Call a script to perform this function here or place a logic inline
#Installing Linux Kernel and Build it 
#Check for any existence of Kernel repository 

if [ ! -d "$OUTDIR/linux" ] 
then 
echo -e "\nINSTALLING LINUX KERNEL\n"
cd $OUTDIR
git clone https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git    #Clone the git Repository 
cd linux
git checkout v5.1.10
make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- mrproper		# Kernel Building Process 
make ARCH=arm versatile_defconfig
make -j4 ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- zImage
make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- modules
make -j4 ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- dtbs
else 
echo -e "\nLINUX DIRECTORY ALREADY EXISTS\n"
fi

#Copy the zImage File and versatil-pb.dtb files  to the OUTDIR Folder 
cd $OUTDIR			
cp ./linux/arch/arm/boot/zImage $OUTDIR
cp ./linux/arch/arm/boot/dts/versatile-pb.dtb $OUTDIR

#TODO: Call a script to perform this function here or place a logic inline
#Installing Rootfs using Busybox
if [ ! -d "$OUTDIR/rootfs" ] 
then 
echo -e "\nINSTALLING ROOTFS USING BUSYBOX\n"
mkdir $OUTDIR/rootfs
cd $OUTDIR/rootfs
mkdir bin dev etc home lib proc sbin sys tmp usr var	#Creating File structure for Kernel 
mkdir usr/bin usr/lib usr/sbin
mkdir -p var/log
tree -d
sudo chown -R root:root *				#Giving root access to all the files
git clone git://busybox.net/busybox.git			#Cloning busybox and installing it 
cd busybox
git checkout 1_31_stable
make distclean
make defconfig
sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- CONFIG_PREFIX=$OUTDIR/rootfs install 
else 
echo -e "DIRECTORY ALREADY SETUP" 
fi



#COPYING SYMOBLIC LINKS TO LIB FOLDER
cd $OUTDIR/rootfs
export SYSROOT=$(arm-unknown-linux-gnueabi-gcc -print-sysroot)
sudo cp -a $SYSROOT/lib/ld-linux.so.3 lib
sudo cp -a $SYSROOT/lib/ld-2.29.so lib
sudo cp -a $SYSROOT/lib/libm.so.6 lib
sudo cp -a $SYSROOT/lib/libresolv.so.2 lib
sudo cp -a $SYSROOT/lib/libresolv-2.29.so lib
sudo cp -a $SYSROOT/lib/libc.so.6 lib
sudo cp -a $SYSROOT/lib/libm-2.29.so lib 
sudo cp -a $SYSROOT/lib/libc-2.29.so lib 

#CREATING NULL AND CONSOLE IN DEVICE FOLDER IN ROOTFS
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1
ls -l dev 


#Making and Copying writer executable, tester.sh and finder.sh here
echo -e "\nCOPYING WRITER EXECUTABLE, TESTER.SH AND FINDER.SH TO ${OUTDIR}/rootfs/bin\n"
cd $DIR
make clean
make all CROSS_COMPILE=arm-unknown-linux-gnueabi-
sudo cp writer $OUTDIR/rootfs/home/
sudo cp finder.sh $OUTDIR/rootfs/home/
sudo cp tester.sh $OUTDIR/rootfs/home/


#TODO: Call a script to perform this function here or place a logic inline
#Creating Standalone initramfs.
echo -e "\nCREATING STANDALONE INITRAMFS\n"	
cd $OUTDIR/rootfs
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
cd .. 
gzip -f initramfs.cpio
mkimage -A arm -O linux -T ramdisk -d initramfs.cpio.gz uRamdisk

	
