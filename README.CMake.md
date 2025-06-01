# Transition to CMake Build System

As Qt seem to pick CMake as the successor of QMake, it is possible to build this project
with CMake.
[https://doc.qt.io/qt-6/qt6-buildsystem.html](https://doc.qt.io/qt-6/qt6-buildsystem.html)

There are two possiblities on how to use CMake for building the project,
with presets or manually.


## Requirements
The CMake based build system requires additional external build tools on top of Qt6.
So install them for instance with:
```
# Debian/Ubuntu
sudo apt-get install -y \
    cmake \
    ninja
    

# Arch Linux
sudo pacman -Syy \
    cmake \
    ninja
    
    ...
```


## Build with Presets
```shell
    cmake --preset release@kbh
    cmake --build --preset build-release@kbh
```
The build output is placed in the `build/release@kbh` subfolder of the project
The output can be installed with:
```shell
DESTDIR=$(pwd)/stagingdir cmake --install .
```
The environment variable `DESTDIR` is optional and can be ommited to install kleiner-brauhelfer directly into the system.

## Build Manually
```shell
cmake -S . -B builddir -G Ninja
cd builddir
ninja
```