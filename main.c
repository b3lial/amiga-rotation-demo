// Copyright 2021 Christian Ammann

#include "rotation-demo.h"

struct BitMap *mainBitmap1 = NULL;
struct BitMap *mainBitmap2 = NULL;
struct BitMap *rectBitmap = NULL;

struct Screen *mainScreen1 = NULL;
struct Screen *mainScreen2 = NULL;
struct Screen *my_wbscreen_ptr;

// empty mouse pointer because we dont want to see a mouse
UWORD *emptyPointer;

// used for double buffering
BOOL bufferSelector;
struct BitMap *currentBitmap = NULL;
struct Screen *currentScreen = NULL;

UWORD *colortable0;
UBYTE *srcBuffer;
UBYTE *destBuffer;

int main(void)
{
    struct RastPort rastPort = {0};
    struct p2cStruct p2c = {0};
    struct RotationData rd = {0};

    // hide mouse
    emptyPointer = AllocVec(22 * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    my_wbscreen_ptr = LockPubScreen("Workbench");
    SetPointer(my_wbscreen_ptr->FirstWindow, emptyPointer, 8, 8, -6, 0);
    UnlockPubScreen(NULL, my_wbscreen_ptr);

    // create pal screens for double buffering
    if (!initScreen(&mainBitmap1, &mainScreen1))
    {
        goto _exit_main;
    }
    if (!initScreen(&mainBitmap2, &mainScreen2))
    {
        goto _exit_free_first_screen;
    }
    bufferSelector = TRUE;
    currentScreen = mainScreen1; // main screen turn on ;)
    currentBitmap = mainBitmap1;

    // allocate memory for chunky buffer
    srcBuffer = AllocVec(RECT_BITMAP_WIDTH * RECT_BITMAP_HEIGHT, MEMF_FAST | MEMF_CLEAR);
    if (!srcBuffer)
    {
        printf("Error: Could not allocate memory for source chunky buffer\n");
        goto _exit_free_second_screen;
    }
    destBuffer = AllocVec(RECT_BITMAP_WIDTH * RECT_BITMAP_HEIGHT, MEMF_FAST | MEMF_CLEAR);
    if (!destBuffer)
    {
        printf("Error: Could not allocate memory for destination chunky buffer\n");
        goto _exit_free_src_chunky;
    }

    // create and assign colortable
    colortable0 = AllocVec(ROTATION_COLORS * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    if (!colortable0)
    {
        printf("Error: Could not allocate memory for space bitmap color table\n");
        goto _exit_free_dest_chunky;
    }
    colortable0[1] = 0x0f00; // color 1 == RED
    LoadRGB4(&(mainScreen1->ViewPort), colortable0, ROTATION_COLORS);
    LoadRGB4(&(mainScreen2->ViewPort), colortable0, ROTATION_COLORS);

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
    p2c.chunkybuffer = srcBuffer;
    PlanarToChunkyAsm(&p2c);

    // show rotation animation and wait for mouse click for exit
    ScreenToFront(currentScreen);
    WaitTOF();
    rd.angle = DEGREE_RESOLUTION;
    rd.src = srcBuffer;
    rd.dest = destBuffer;
    rd.width = RECT_BITMAP_WIDTH;
    rd.height = RECT_BITMAP_HEIGHT;
    while (!mouseCiaStatus())
    {
        switchScreenData();
        rotate(&rd);
        blitRotationResult();
        ScreenToFront(currentScreen);
        rd.angle = (rd.angle == 360) ? 10 : rd.angle + DEGREE_RESOLUTION;
    }

    FreeBitMap(rectBitmap);
_exit_free_colortable:
    FreeVec(colortable0);
_exit_free_dest_chunky:
    FreeVec(destBuffer);
_exit_free_src_chunky:
    FreeVec(srcBuffer);
_exit_free_second_screen:
    CloseScreen(mainScreen2);
    WaitTOF();
    FreeBitMap(mainBitmap2);
_exit_free_first_screen:
    CloseScreen(mainScreen1);
    WaitTOF();
    FreeBitMap(mainBitmap1);
_exit_main:
    // restore mouse
    my_wbscreen_ptr = LockPubScreen("Workbench");
    ClearPointer(my_wbscreen_ptr->FirstWindow);
    UnlockPubScreen(NULL, my_wbscreen_ptr);
    FreeVec(emptyPointer);

    exit(RETURN_OK);
}

void switchScreenData()
{
    if (bufferSelector)
    {
        currentScreen = mainScreen2;
        currentBitmap = mainBitmap2;
        bufferSelector = FALSE;
    }
    else
    {
        currentScreen = mainScreen1;
        currentBitmap = mainBitmap1;
        bufferSelector = TRUE;
    }
}

void blitRotationResult(void)
{
#ifdef NATIVE_CONVERTER
    struct RastPort rastPort1 = {0};
    struct RastPort rastPort2 = {0};
    struct BitMap *tempBitmap = NULL;
    UWORD backupBytesPerRow;
    UWORD backupRows;

    InitRastPort(&rastPort1);
    InitRastPort(&rastPort2);

    rastPort1.BitMap = rectBitmap;

    tempBitmap = AllocBitMap(RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT,
                             ROTATION_DEPTH, BMF_CLEAR, NULL);
    rastPort2.Layer = NULL;
    backupRows = tempBitmap->Rows;
    tempBitmap->Rows = 1;
    backupBytesPerRow = tempBitmap->BytesPerRow;
    tempBitmap->BytesPerRow = (((RECT_BITMAP_WIDTH + 15) >> 4) << 1);
    rastPort2.BitMap = tempBitmap;

    WritePixelArray8(&rastPort1, 0, 0, RECT_BITMAP_WIDTH - 1,
                     RECT_BITMAP_HEIGHT - 1, destBuffer, &rastPort2);

    tempBitmap->Rows = backupRows;
    tempBitmap->BytesPerRow = backupBytesPerRow;
    FreeBitMap(tempBitmap);
#else
    struct c2pStruct c2p;
    c2p.bmap = rectBitmap;
    c2p.startX = 0;
    c2p.startY = 0;
    c2p.width = RECT_BITMAP_WIDTH;
    c2p.height = RECT_BITMAP_HEIGHT;
    c2p.chunkybuffer = destBuffer;
    ChunkyToPlanarAsm(&c2p);
#endif
    BltBitMap(rectBitmap, 0, 0, currentBitmap,
              RECT_BITMAP_POS_X, RECT_BITMAP_POS_Y,
              RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT, 0x00C0,
              0xff, NULL);
}

BOOL initScreen(struct BitMap **b, struct Screen **s)
{
    // load onscreen bitmap which will be shown on screen
    *b = AllocBitMap(ROTATION_WIDTH, ROTATION_HEIGHT,
                     ROTATION_DEPTH, BMF_DISPLAYABLE | BMF_CLEAR,
                     NULL);
    if (!*b)
    {
        printf("Error: Could not allocate memory for screen bitmap\n");
        goto __exit_init_error;
    }

    // create one screen which contains the demo logo
    *s = createScreen(*b, TRUE, 0, 0,
                      ROTATION_WIDTH, ROTATION_HEIGHT,
                      ROTATION_DEPTH, NULL);
    if (!*s)
    {
        printf("Error: Could not allocate memory for logo screen\n");
        goto __exit_init_bitmap;
    }
    return TRUE;

__exit_init_bitmap:
    FreeBitMap(*b);
__exit_init_error:
    return FALSE;
}