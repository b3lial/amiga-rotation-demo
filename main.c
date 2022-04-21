// Copyright 2021 Christian Ammann

#include "rotation-demo.h"

// empty mouse pointer because we dont want to see a mouse
UWORD *emptyPointer;

struct BitMap *mainBitmap = NULL;
struct BitMap *rectBitmap = NULL;
struct Screen *mainScreen = NULL;
struct Screen *my_wbscreen_ptr;

UWORD *colortable0;
UBYTE *chunkyBuffer;

int main(void)
{
    struct RastPort rastPort = {0};
    struct p2cStruct p2c = {0};

    // hide mouse
    emptyPointer = AllocVec(22 * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    my_wbscreen_ptr = LockPubScreen("Workbench");
    SetPointer(my_wbscreen_ptr->FirstWindow, emptyPointer, 8, 8, -6, 0);
    UnlockPubScreen(NULL, my_wbscreen_ptr);

    // create pal screen
    if (!initScreen())
    {
        goto _exit_main;
    }

    // allocate memory for chunky buffer
    chunkyBuffer = AllocVec(RECT_BITMAP_WIDTH * RECT_BITMAP_HEIGHT, NULL);
    if (!chunkyBuffer)
    {
        printf("Error: Could not allocate memory for chunky buffer\n");
        goto _exit_free_screen;
    }

    // create and assign colortable
    colortable0 = AllocVec(ROTATION_COLORS * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    if (!colortable0)
    {
        printf("Error: Could not allocate memory for space bitmap color table\n");
        goto _exit_free_chunky;
    }
    colortable0[1] = 0x0f00; // color 1 == RED
    LoadRGB4(&(mainScreen->ViewPort), colortable0, ROTATION_COLORS);

    // create bitmap with rectangle which we will use for rotation
    rectBitmap = AllocBitMap(RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT,
                             ROTATION_DEPTH, BMF_CLEAR, NULL);
    if (!rectBitmap)
    {
        printf("Error: Could not allocate memory for rectangle bitmap\n");
        goto _exit_free_colortable;
    }

    // draw red rectangle into bitmap
    InitRastPort(&rastPort);
    rastPort.BitMap = rectBitmap;
    SetAPen(&rastPort, 1);
    RectFill(&rastPort, RECT_X, RECT_Y,
             RECT_X + RECT_WIDTH,
             RECT_Y + RECT_HEIGHT);

    // convert planar buffer to chunky
    p2c.bmap = rectBitmap;
    p2c.startX = 0;
    p2c.startY = 0;
    p2c.width = RECT_BITMAP_WIDTH;
    p2c.height = RECT_BITMAP_HEIGHT;
    p2c.chunkybuffer = chunkyBuffer;
    PlanarToChunkyAsm(&p2c);

    // show rotation animation and wait for mouse click for exit
    ScreenToFront(mainScreen);
    WaitTOF();
    while (!mouseCiaStatus())
    {
        calculateRotation();
        WaitTOF();
        performRotation();
    }

    FreeBitMap(rectBitmap);
_exit_free_colortable:
    FreeVec(colortable0);
_exit_free_chunky:
    FreeVec(chunkyBuffer);
_exit_free_screen:
    CloseScreen(mainScreen);
    WaitTOF();
    FreeBitMap(mainBitmap);
_exit_main:
    // restore mouse
    my_wbscreen_ptr = LockPubScreen("Workbench");
    ClearPointer(my_wbscreen_ptr->FirstWindow);
    UnlockPubScreen(NULL, my_wbscreen_ptr);
    FreeVec(emptyPointer);

    exit(RETURN_OK);
}

void calculateRotation(void)
{
}

void performRotation(void)
{
    BltBitMap(rectBitmap, 0, 0, mainBitmap,
              RECT_BITMAP_POS_X, RECT_BITMAP_POS_Y,
              RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT, 0x00C0,
              0xff, NULL);
}

BOOL initScreen(void)
{
    // load onscreen bitmap which will be shown on screen
    mainBitmap = AllocBitMap(ROTATION_WIDTH, ROTATION_HEIGHT,
                             ROTATION_DEPTH, BMF_DISPLAYABLE | BMF_CLEAR,
                             NULL);
    if (!mainBitmap)
    {
        printf("Error: Could not allocate memory for screen bitmap\n");
        goto __exit_init_error;
    }

    // create one screen which contains the demo logo
    mainScreen = createScreen(mainBitmap, TRUE, 0, 0,
                              ROTATION_WIDTH, ROTATION_HEIGHT,
                              ROTATION_DEPTH, NULL);
    if (!mainScreen)
    {
        printf("Error: Could not allocate memory for logo screen\n");
        goto __exit_init_bitmap;
    }
    return TRUE;

__exit_init_bitmap:
    FreeBitMap(mainBitmap);
__exit_init_error:
    return FALSE;
}