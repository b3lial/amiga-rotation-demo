// Copyright 2021 Christian Ammann

#ifndef __ROTATION_DEMO_H__
#define __ROTATION_DEMO_H__

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

#define ROTATION_DEPTH 8
#define ROTATION_COLORS 256
#define ROTATION_WIDTH 320
#define ROTATION_HEIGHT 256

#define RECT_BITMAP_POS_X 50
#define RECT_BITMAP_POS_Y 50
#define RECT_BITMAP_WIDTH 160
#define RECT_BITMAP_HEIGHT 160
#define RECT_WIDTH 80
#define RECT_HEIGHT 80
#define RECT_X 40
#define RECT_Y 40

#define DEGREE_RESOLUTION 10

BOOL initScreen(struct BitMap **bm, struct Screen **s);
void rotate(UBYTE *src, UBYTE *dest, unsigned int size);
void blitRotationResult(void);
void switchScreenData();
void rotatePixel(int dest_x, int *new_x, int *new_y,
                 float y_mult_sin, float y_mult_cos,
                 UWORD lookupIndex);

#endif
