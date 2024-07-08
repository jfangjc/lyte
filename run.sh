if [ ! -d "./bin/Debug_Unix" ]; then
    mkdir ./bin
    mkdir ./bin/Debug_Unix
    cmake --preset Debug_Unix
elif [ ! -d "./bin/Debug_Darwin" ]; then
    mkdir ./bin
    mkdir ./bin/Debug_Darwin
    cmake --preset Debug_Darwin
elif [ ! -d "./bin/Debug_Windows" ]; then
    mkdir ./bin
    mkdir ./bin/Debug_Windows
    cmake --preset Debug_Windows
fi

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    echo Compiling for linux
    cmake --build --preset Unix && ./bin/Debug_Unix/Lyte
elif [[ "$OSTYPE" == "darwin" ]]; then
    echo Compiling for mac
    cmake --build --preset Darwin && ./bin/Debug_Darwin/Lyte
else
    echo Compiling for windwos
    cmake --build --preset Windows && ./bin/Debug_Win/Lyte
fi
