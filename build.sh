if [ ! -d "./build" ]; then
    mkdir ./build
fi

cmake -S . -B build
cmake --build build
# Just in case
sleep 1
./build/lyte