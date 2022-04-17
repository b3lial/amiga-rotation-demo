// Copyright 2021 Christian Ammann

#include "rotation-demo.h"

// empty mouse pointer because we dont want to see a mouse
UWORD *emptyPointer;

struct BitMap *mainBitmap = NULL;
struct Screen *mainScreen = NULL;

int main(void)
{
    struct Screen *my_wbscreen_ptr;

    // hide mouse
    emptyPointer = AllocVec(22 * sizeof(UWORD), MEMF_CHIP | MEMF_CLEAR);
    my_wbscreen_ptr = LockPubScreen("Workbench");
    SetPointer(my_wbscreen_ptr->FirstWindow, emptyPointer, 8, 8, -6, 0);
    UnlockPubScreen(NULL, my_wbscreen_ptr);

    //create pal screen
    if(!initScreen()){
        goto _exit_main;
    }

    // restore mouse on every window
_exit_main:
    my_wbscreen_ptr = LockPubScreen("Workbench");
    ClearPointer(my_wbscreen_ptr->FirstWindow);
    UnlockPubScreen(NULL, my_wbscreen_ptr);
    FreeVec(emptyPointer);

    exit(RETURN_OK);
}

BOOL initScreen(void)
{
    // load onscreen bitmap which will be shown on screen
    mainBitmap = AllocBitMap(SHOWLOGO_BLOB_WIDTH, SHOWLOGO_BLOB_HEIGHT,
                             SHOWLOGO_BLOB_DEPTH, BMF_DISPLAYABLE | BMF_CLEAR,
                             NULL);
    if (!mainBitmap)
    {
        printf("Error: Could not allocate memory for screen bitmap\n");
        goto __exit_init_error;
    }

    // create one screen which contains the demo logo
    mainScreen = createScreen(mainBitmap, TRUE, 0, 0,
                              SHOWLOGO_BLOB_WIDTH, SHOWLOGO_BLOB_HEIGHT,
                              SHOWLOGO_BLOB_DEPTH, NULL);
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