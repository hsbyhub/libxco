set -e
yum install -y boost-devel
rm -rf build
mkdir -p build
cd build
cmake ..
make -j4
make install
ldconfig
echo "libxco install done."
