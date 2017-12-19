clear

rm -f *.o
rm -f *.so

# not just gnueabi -> gnueabihf else doesn't work
#export arch="arm-linux-gnueabihf-"

export buildroot_dir="/vm_mnt/dev/RPI0W/buildroot/buildroot-2017.02.8"
#export buildroot_dir="/vm_mnt/dev/RPI0W/buildroot/buildroot"
export arch="/vm_mnt/dev/RPI0W/buildroot/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-"
#export arch="$buildroot_dir/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-"
export JNI_INCLUDE="$buildroot_dir/output/build/classpath-0.98/include/"
export STDIO_INCLUDE="$buildroot_dir/output/build/uclibc-1.0.22/include/stdio.h"
#export arch="/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-"
#export JNI_INCLUDE="/buildroot/output/build/classpath-0.99/include/"
export suffix="_ARM"

# for PC
#export arch=""
#export JNI_INCLUDE="/usr/lib/java/include/"
# + /usr/lib/java/include/linux/
#export STDIO_INCLUDE="/usr/include/"
#export suffix="_x64"

export GPP="$arch"g++

rm "libTFTScreen.so$suffix"



echo "Will use : $GPP"
#exit

echo "Compilling JNI"
"$GPP" -fPIC -I . -I "$STDIO_INCLUDE" -I "$JNI_INCLUDE" -c com_xtase_hardware_TFTScreen.cpp

echo "Compiling lib"
"$GPP" -fPIC -I . -I "$JNI_INCLUDE" -c FrameBuffer.cpp

echo "Linkking lib"
#g++ -shared -o libcom_xtase_hardware_TFTScreen.so com_xtase_hardware_TFTScreen.o FrameBuffer.o
"$GPP" -shared -o "libTFTScreen.so$suffix" com_xtase_hardware_TFTScreen.o FrameBuffer.o

echo "Cleaning"
rm *.o

echo "Bye."
#ls -alh

file "libTFTScreen.so$suffix"


