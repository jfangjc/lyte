# Getting Started with Lyte

## Prerequisites
- CMake 3.10 or higher
- C compiler (GCC or Clang)
- Make or Ninja

## Building the Project
1. Clone the repository.
2. Run the build script:
```bash
./build.sh
```
Or manually:
```bash
mkdir build
cd build
cmake ..
make
   ```

## Running the Compiler
After building, the `lyte` executable will be in the `build` directory.
```bash
./build/lyte <input_file>
```

## Running Tests
To run the unit tests:
```bash
./build/unit_test
```
