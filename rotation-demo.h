// Copyright 2021 Christian Ammann

#ifndef ROTATION_DEMO_H_
#define ROTATION_DEMO_H_

#define MAX_CHAR_HEIGHT 33
#define MAX_CHAR_WIDTH 30
#define DEMO_STR_MAX 120

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/videocontrol.h>
#include <graphics/gfxmacros.h>

#include <hardware/custom.h>
#include <hardware/cia.h>
#include <hardware/dmabits.h>

#include "starlight/blob_controller.h"
#include "starlight/graphics_controller.h"
#include "starlight/utils.h"

#include "chunkyconverter/chunkyconverter.h"

// #define NATIVE_CONVERTER

// credits to: https://coronax.wordpress.com/2014/01/31/running-with-the-numbers/
#define FIXSHIFT 16        // shift 16 bits = scale factor 65536
#define HALFSHIFT 8
// convert float to fix (and back)
#define FLOATTOFIX(x) ((int)((x) * (1<<FIXSHIFT)))
#define FIXTOFLOAT(x) ((float)(x) / (1<<FIXSHIFT))
// convert int to fix (and back)
#define INTTOFIX(x) ((x)<<FIXSHIFT)
#define FIXTOINT(x) ((x)>>FIXSHIFT)
// multiply and divide
#define FIXMULT(x,y) (((x)>>HALFSHIFT)*((y)>>HALFSHIFT))
#define FIXDIV(x,y) (((x)/(y>>HALFSHIFT))<<HALFSHIFT)

#define ROTATION_DEPTH 1
#define ROTATION_COLORS 2
#define ROTATION_WIDTH 320
#define ROTATION_HEIGHT 256

#define RECT_BITMAP_POS_X 80
#define RECT_BITMAP_POS_Y 50
#define RECT_BITMAP_WIDTH 160
#define RECT_BITMAP_HEIGHT 160
#define RECT_WIDTH 90
#define RECT_HEIGHT 90
#define RECT_X 35
#define RECT_Y 35
#define RECT_ROTATION_STEPS 36

#define DEGREE_RESOLUTION 10
#define DEST_BUFFER_SIZE 36

struct RotationData{
    UBYTE* dest;
    USHORT angle;
};

BOOL initScreen(struct BitMap **bm, struct Screen **s);
void execute(void);
void rotateAll(void);
void rotate(struct RotationData *rd);
void convertChunkyToBitmap(UBYTE* sourceChunky, struct BitMap *destPlanar);
void switchScreenData(void);

BOOL initRotationEngine(UBYTE rs, USHORT bw, USHORT bh);
void rotatePixel(int dest_x, int *new_x, int *new_y,
                 int y_mult_sin, int y_mult_cos,
                 UWORD lookupIndex);
BOOL allocateChunkyBuffer(void);
void freeChunkyBuffer(void);
UBYTE* getSourceBuffer(void);
UBYTE* getDestBuffer(UBYTE index);

#endif  // ROTATION_DEMO_H_
