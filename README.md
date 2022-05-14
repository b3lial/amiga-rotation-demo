# amiga-rotation-demo
Amiga example which displays a rotating square on a PAL screen.

## Build

Can be 

* crosscompiled with makefile, [GCC](http://aminet.net/package/dev/gcc/m68k-amigaos-gcc) and vasm
* or install my [amiga build](https://hub.docker.com/r/phobosys/amiga-gcc-builder) 
docker container and run `docker-run.sh`

The planar to chunky buffer conversion is perfomed via `PlanarToChunkyAsm()` written
by Morten Eriksen. The chunky to planar conversion can be controlled at compile
time by `#define NATIVE_CONVERTER`:

* If defined, AmigaOS native function `WritePixelArray8()` is used.
* If not defined, the converter invokes `ChunkyToPlanarAsm()`.

Important: `WritePixelArray8()` seems to have problems with padding
bytes if `buffer width % 8 != 0`. I assume there is a mistake on my
side, so use with caution ;)

## Architecture
This implemetation starts with a series of boiler plate code statements:

* Set an empty mouse pointer via `SetPointer()` to hide it from screen
* Allocate a temporary bitmap which can be later used by `WritePixelArray8()`
* We use double buffering, therefore we create two Screens (including the corresponding BitMaps)
* Allocate two chunky buffer in fast memory (`MEMF_FAST` parameter can be ommitted but
I wanted to make sure it really uses the memory of my accelerator card).
* Allocate memory for color table
* Create `rectBitmap` which will contain the square

Afterwards, this example runs in a loop which

* Rotates the square by 10 degrees
* And shows the result on screen
* Checks whether mouse key was pressed. If yes: Terminate

In case of program termination, the previously allocated buffers are freed.

## Rotation Algorithm
...
