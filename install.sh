mkdir build
cd build
cmake ../src && make -j4 && make install
ldconfig
echo "libxco Install done."