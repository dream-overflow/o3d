INSTALL
=======

Here the informations to build the API, dependencies and examples.


GNU/Linux systems :
===================

make [MODE<default|debug|optimized|optimizeddebug>] [DEFINES] [V=0|1]

  - use V=1 for verbose mode
  - optimized mode use -O2 optimization

build the API, tinyXml, triStripper, nvTriStrip, and all examples.

If you wish to install the library into your /usr/local/include and /usr/local/lib
type make install


Window XP/Vista/7/8 systems :
=============================

Visual C++ 9 solutions are contained respectivly in :
  
  - build/vc9/ for the API, tinyXml, triStripper and nvTriStrip
  - examples/ for te examples
