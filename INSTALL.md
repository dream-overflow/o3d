# INSTALL #

Here the informations to build the API and somees dependencies :
* tinyXml
* triStripper
* nvTriStrip.

## GNU/Linux systems ##

See the CMake toolchain. Builds using GCC 5+ or LLVM 5+ CLANG and make

If you wish to install the library into your /usr/local type make install,
or changes the install prefix to set another destination.

### Dependencies ###

#### Packages ####

apt-get install build-essentials autoconf cmake xorg-dev libfreetype6-dev libopenal-dev libjpeg-dev libpng-dev libvorbis-dev libogg-dev zlib1g-dev zip

#### Environment ####

It is prefered to create a developpement environment.

...

#### Cmake ####

From the dreamoverflow/cmake copy the modules.

# Window 7/8/10 systems #

At this time the prefered solution is to uses mingw64 compiler with the CMake toolchain.

## Dependencies ##

See the windows-deps repository.

## Shaders ##

Before calling cmake you can build the shaders zip files :

$ src/buildshader.sh

will create src/shaders/glsl<vers>.zip files needed later.

## Build ##

### Linux ###

Create a build-<target>-<mode> directory like for example :

$ mkdir build-amd64-release
$ cmake -DCMAKE_BUILD_TYPE=Release -DO3D_USE_SSE2=1 -DCMAKE_MODULE_PATH=$PREFIX/share/cmake/Modules -DCMAKE_INSTALL_PREFIX=$PREFIX ..
$ make -jN  # (where N is your max CPU number)
$ make install

### Windows ###

The prefered way is the cross-build using mingw64

@todo


## Options ##

The cmake flag -DO3D_CORE_ONLY=1 allow you to build only the core part of O3D, without application window, image, audio, engine, gui, neither geometry.
This build mode made O3D as a cool base C++ API for starting most of your projects :)
