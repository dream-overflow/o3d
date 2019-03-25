# INSTALL #

Here the informations to build the API and somees dependencies :
* tinyXml
* triStripper
* nvTriStrip.

# GNU/Linux systems #

See the CMake toolchain. Builds using GCC 5+ or LLVM 5+ CLANG and make

If you wish to install the library into your /usr/local type make install,
or changes the install prefix to set another destination.

## Dependencies ##

### Packages ###

apt-get install build-essentials autoconf cmake xorg-dev libfreetype6-dev libopenal-dev libjpeg-dev libpng-dev libvorbis-dev libogg-dev zlib1g-dev

### Environment ###

It is prefered to create a developpement environment.

### Cmake ###

From the dreamoverflow/cmake copy the modules.

# Window 7/8/10 systems #

At this time the prefered solution is to uses mingw64 compiler with the CMake toolchain.

## Dependencies ##

See the windows-deps repository.

