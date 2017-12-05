clear

rm -f *.o
rm -f *.so

# not just gnueabi -> gnueabihf else doesn't work
export arch="arm-linux-gnueabihf-"
export arch="/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-"
export JNI_INCLUDE="/buildroot/output/build/classpath-0.99/include/"

# for PC
export arch=""
export JNI_INCLUDE=""


export GPP="$arch"g++


echo "Will use : $GPP"
#exit

echo "Compilling JNI"
"$GPP" -fPIC -I /usr/lib/java/include/ -I /usr/lib/java/include/linux/ -I . -I /usr/include/ -I "$JNI_INCLUDE" -c com_xtase_hardware_TFTScreen.cpp

echo "Compiling lib"
"$GPP" -fPIC -I /usr/lib/java/include/ -I /usr/lib/java/include/linux/ -I . -I "$JNI_INCLUDE" -c FrameBuffer.cpp

echo "Linkking lib"
#g++ -shared -o libcom_xtase_hardware_TFTScreen.so com_xtase_hardware_TFTScreen.o FrameBuffer.o
"$GPP" -shared -o libTFTScreen.so com_xtase_hardware_TFTScreen.o FrameBuffer.o

echo "Bye."
#ls -alh

file libTFTScreen.so


