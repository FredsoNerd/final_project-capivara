cd capivara
# remove old
rm -rf build
rm structures.so
# create new
mkdir build
cd build
cmake ../..
make
# copy build
cd ..
cp build/structures.so .
# remove temp
rm -rf build