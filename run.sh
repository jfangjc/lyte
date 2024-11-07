if [ ! -d "./bin" ]; then
    mkdir ./bin
fi

if [[ "$OSTYPE" == "linux" ]]; then
    if [ ! -d "./bin/linux-arm64" ]; then
        mkdir ./bin/linux-arm64
        cmake --preset linux-arm64
    fi
    echo Compiling for linux
    cmake --build --preset linux-arm64 && ./bin/linux-arm64/lyte
elif [[ "$OSTYPE" == "darwin" ]]; then
    if [ ! -d "./bin/darwin-arm64" ]; then
        mkdir ./bin/darwin-arm64
        cmake --preset darwin-arm64
    fi
    echo Compiling for mac
    cmake --build --preset darwin-arm64 && ./bin/darwin-arm64/lyte
else
    if [ ! -d "./bin/win-arm64" ]; then
        mkdir ./bin/win-arm64
        cmake --preset win-arm64
    fi
    echo Compiling for windwos
    cmake --build --preset win-arm64 && ./bin/win-arm64/lyte
fi
