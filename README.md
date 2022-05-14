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
* Paint square into `rectBitmap`, transform to chunky format and save in `srcBuffer`

Afterwards, this example runs in a loop which

* Rotates the square by 10 degrees
* Shows the result on screen
* Checks whether mouse key was pressed. If yes: Terminate

In case of program termination, the previously allocated buffers are freed.

## Rotation Algorithm
The rotation is perfomed in `rotate()` which gets the following parameters:

* Source buffer `srcBuffer` (contains the original square)
* Destination buffer `destBuffer`
* Rotation angle in in degrees
* Buffer width and height

The rotation algorithm iterates through each y and x pixel
of the destination buffer. Each of these pixels is multiplied
with a rotation matrix:

* x = `x*cos(360 - alpha) - y*sin(360 - alpha)`
* y = `x*sin(360 - alpha) + y*cos(360 - alpha)`

The resulting (x,y) coordinates are transfomed into array indices and used to to write the color values
of `srcBuffer` into `destBuffer`:

```C
dest_index = x + y * rd->width;
src_index = (src_x + (rd->width / 2)) +
            ((src_y + (rd->height / 2)) * rd->width);
if (src_index < 0 || src_index >= (rd->height * rd->width))
{
    continue;
}
(rd->dest)[dest_index] = (rd->src)[src_index];
```

## Optimsation
The first implementation was really slow, maybe one frame per 5 seconds on
my A1200 with 40Mhz 68030 CPU. So, I added the following optimsations:

* I moved some multiplications which need the current _y_ value from `rotatePixel()`
into the _y_ for loop. This way, the calculations are only performed once
per row and not for each pixel.
* The scripts [lookup_cos.py](lookup_cos.py) and [lookup_sin.py](lookup_sin.py)
precalculate the values of the trigonometric functions and create lookup tables.
* I removed the float calculations and switched to fix point with a set of basic
macros I found [here](https://coronax.wordpress.com/2014/01/31/running-with-the-numbers/).
