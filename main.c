// Copyright 2021 Christian Ammann

#include "rotation-demo.h"

struct BitMap *mainBitmap1 = NULL;
struct BitMap *mainBitmap2 = NULL;
struct BitMap *rectBitmap = NULL;

struct BitMap *tempBitmap = NULL;
UWORD backupBytesPerRow;
UWORD backupRows;

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

/*
 * Create two Screens and two BitMap as Screen content
 * for double buffering. Create a third BitMap, draw a
 * rectangle into it, rotate and blit into Screen
 */
int main(void) {
    // hide mouse
    emptyPointer = AllocVec(22 * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    my_wbscreen_ptr = LockPubScreen("Workbench");
    SetPointer(my_wbscreen_ptr->FirstWindow, emptyPointer, 8, 8, -6, 0);
    UnlockPubScreen(NULL, my_wbscreen_ptr);

    // allocate temp bitmap
    tempBitmap = AllocBitMap(RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT,
                             ROTATION_DEPTH, BMF_CLEAR, NULL);
    if (!tempBitmap) {
        goto _exit_main;
    }
    backupRows = tempBitmap->Rows;
    tempBitmap->Rows = 1;
    backupBytesPerRow = tempBitmap->BytesPerRow;
    tempBitmap->BytesPerRow = (((RECT_BITMAP_WIDTH + 15) >> 4) << 1);

    // create pal screens for double buffering
    if (!initScreen(&mainBitmap1, &mainScreen1)) {
        goto _exit_free_temp_bitmap;
    }
    if (!initScreen(&mainBitmap2, &mainScreen2)) {
        goto _exit_free_first_screen;
    }
    bufferSelector = TRUE;
    currentScreen = mainScreen1;  // main screen turn on ;)
    currentBitmap = mainBitmap1;

    // allocate source buffer and destination buffer array
    if (!initRotationEngine(360 / DEGREE_RESOLUTION, RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT)) {
        goto _exit_free_second_screen;
    }

    // create and assign colortable
    colortable0 = AllocVec(ROTATION_COLORS * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    if (!colortable0) {
        printf("Error: Could not allocate memory for space bitmap color table\n");
        goto _exit_free_chunky;
    }
    colortable0[1] = 0x0f00;  // color 1 == RED
    LoadRGB4(&(mainScreen1->ViewPort), colortable0, ROTATION_COLORS);
    LoadRGB4(&(mainScreen2->ViewPort), colortable0, ROTATION_COLORS);

    // create bitmap with rectangle which we will use for rotation
    rectBitmap = AllocBitMap(RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT,
                             ROTATION_DEPTH, BMF_CLEAR, NULL);
    if (!rectBitmap) {
        printf("Error: Could not allocate memory for rectangle bitmap\n");
        goto _exit_free_colortable;
    }

    /* main() just does boring boiler plate stuff,
     * real rotation work is done in execute()
     */
    execute();

    FreeBitMap(rectBitmap);
_exit_free_colortable:
    FreeVec(colortable0);
_exit_free_chunky:
    freeChunkyBuffer();
_exit_free_second_screen:
    CloseScreen(mainScreen2);
    WaitTOF();
    FreeBitMap(mainBitmap2);
_exit_free_first_screen:
    CloseScreen(mainScreen1);
    WaitTOF();
    FreeBitMap(mainBitmap1);
_exit_free_temp_bitmap:
    tempBitmap->Rows = backupRows;
    tempBitmap->BytesPerRow = backupBytesPerRow;
    FreeBitMap(tempBitmap);
_exit_main:
    // restore mouse
    my_wbscreen_ptr = LockPubScreen("Workbench");
    ClearPointer(my_wbscreen_ptr->FirstWindow);
    UnlockPubScreen(NULL, my_wbscreen_ptr);
    FreeVec(emptyPointer);

    exit(RETURN_OK);
}

/*
 * Paint rectangle into BitMap, tranform to chunky buffer, rotate
 * by 10 degreee in a loop, transform back to planar and draw result on
 * Screen
 */
void execute() {
    struct RastPort rastPort = {0};
    struct p2cStruct p2c = {0};
    struct RotationData rd = {0};
    BYTE i = 0;

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
    p2c.chunkybuffer = getSourceBuffer();
    PlanarToChunkyAsm(&p2c);

    // show rotation animation and wait for mouse click for exit
    ScreenToFront(currentScreen);
    WaitTOF();

    // rotate object and store rotated results in destination array chunky buffers
    rd.angle = 0;
    rd.src = getSourceBuffer();
    rd.width = RECT_BITMAP_WIDTH;
    rd.height = RECT_BITMAP_HEIGHT;
    for (i=0; i < 360 / DEGREE_RESOLUTION; i++) {
        rd.dest = getDestBuffer(i);
        rotate(&rd);
        rd.angle += DEGREE_RESOLUTION;
    }

    // show rotation animation, chunky buffer objects are converted to planar
    i = 1;
    while (!mouseCiaStatus()) {
        if (i >= (360 / DEGREE_RESOLUTION)) {
            i = 0;
        }
        switchScreenData();
        convertChunkyToBitmap(getDestBuffer(i), rectBitmap);
        BltBitMap(rectBitmap, 0, 0, currentBitmap,
                  RECT_BITMAP_POS_X, RECT_BITMAP_POS_Y,
                  RECT_BITMAP_WIDTH, RECT_BITMAP_HEIGHT, 0x00C0,
                  0xff, NULL);
        ScreenToFront(currentScreen);
        i += 1;
    }
}

void switchScreenData() {
    if (bufferSelector) {
        currentScreen = mainScreen2;
        currentBitmap = mainBitmap2;
        bufferSelector = FALSE;
    } else {
        currentScreen = mainScreen1;
        currentBitmap = mainBitmap1;
        bufferSelector = TRUE;
    }
}

void convertChunkyToBitmap(UBYTE* sourceChunky, struct BitMap *destPlanar)
{
#ifdef NATIVE_CONVERTER
    struct RastPort rastPort1 = {0};
    struct RastPort rastPort2 = {0};
    InitRastPort(&rastPort1);
    InitRastPort(&rastPort2);

    rastPort1.BitMap = destPlanar;
    rastPort2.Layer = NULL;
    rastPort2.BitMap = tempBitmap;
    WritePixelArray8(&rastPort1, 0, 0, RECT_BITMAP_WIDTH - 1,
                     RECT_BITMAP_HEIGHT - 1, sourceChunky, &rastPort2);
#else
    struct c2pStruct c2p;
    c2p.bmap = destPlanar;
    c2p.startX = 0;
    c2p.startY = 0;
    c2p.width = RECT_BITMAP_WIDTH;
    c2p.height = RECT_BITMAP_HEIGHT;
    c2p.chunkybuffer = sourceChunky;
    ChunkyToPlanarAsm(&c2p);
#endif
}

BOOL initScreen(struct BitMap **b, struct Screen **s)
{
    // load onscreen bitmap which will be shown on screen
    *b = AllocBitMap(ROTATION_WIDTH, ROTATION_HEIGHT,
                     ROTATION_DEPTH, BMF_DISPLAYABLE | BMF_CLEAR,
                     NULL);
    if (!*b) {
        printf("Error: Could not allocate memory for screen bitmap\n");
        goto __exit_init_error;
    }

    // create one screen which contains the demo logo
    *s = createScreen(*b, TRUE, 0, 0,
                      ROTATION_WIDTH, ROTATION_HEIGHT,
                      ROTATION_DEPTH, NULL);
    if (!*s) {
        printf("Error: Could not allocate memory for logo screen\n");
        goto __exit_init_bitmap;
    }
    return TRUE;

__exit_init_bitmap:
    FreeBitMap(*b);
__exit_init_error:
    return FALSE;
}
