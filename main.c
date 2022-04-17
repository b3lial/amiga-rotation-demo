// Copyright 2021 Christian Ammann

#include "rotation-demo.h"

// empty mouse pointer because we dont want to see a mouse
UWORD *emptyPointer;

struct BitMap *mainBitmap = NULL;
struct Screen *mainScreen = NULL;
UWORD *colortable0;

int main(void)
{
    struct Screen *my_wbscreen_ptr;
    initLog();

    // hide mouse
    emptyPointer = AllocVec(22 * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    my_wbscreen_ptr = LockPubScreen("Workbench");
    SetPointer(my_wbscreen_ptr->FirstWindow, emptyPointer, 8, 8, -6, 0);
    UnlockPubScreen(NULL, my_wbscreen_ptr);

    //create pal screen
    if(!initScreen()){
        goto _exit_main;
    }

    //create and assign colortable
    colortable0 = AllocVec(ROTATION_COLORS * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    if (!colortable0)
    {
        printf("Error: Could not allocate memory for space bitmap color table\n");
        goto _exit_free_screen;
    }
    LoadRGB4(&(mainScreen->ViewPort), colortable0, ROTATION_COLORS);

    //show screen and wait for mouse click
    ScreenToFront(mainScreen);
    WaitTOF();
    while(!mouseCiaStatus()){
        WaitTOF();
    }

    FreeVec(colortable0);
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