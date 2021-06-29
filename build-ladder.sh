rm -Rf build
mkdir build
cd build
cmake -DBUILD_FOR_LADDER=ON -DSC2_VERSION=4.10.0 ../
make
cd ..