cd lib/
rm libTFTScreen.so
cp ../src_cpp/libTFTScreen.so_ARM ./
ln -s libTFTScreen.so_ARM libTFTScreen.so
cd ..
echo "CHANGED libTFTScreen.so to ARM CPU"
