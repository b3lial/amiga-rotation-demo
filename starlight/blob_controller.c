#include "rotation-demo.h"

/**
 * Loads a set of unsigned words from file and copies them into
 * an array to use them as input for background color registers
 */
BOOL loadColorMap(char *fileName, UWORD *map, UWORD mapLength)
{
    LONG dataRead = 0;
    BPTR mapFileHandle = NULL;
    writeLogFS("Trying to load color table %s\n", fileName);

    //Open input file
    mapFileHandle = Open(fileName, MODE_OLDFILE);
    if (!mapFileHandle)
    {
        writeLogFS("Error: Could not read %s\n", fileName);
        return FALSE;
    }

    dataRead = Read(mapFileHandle, map, mapLength * 2);
    if (dataRead == -1)
    {
        writeLog("Error: Could not read from color map input file\n");
        Close(mapFileHandle);
        return FALSE;
    }
    writeLogFS("Read %d bytes into color table buffer\n", dataRead);

    Close(mapFileHandle);
    return TRUE;
}

/**
 * Loads a colormap from file.
 *
 * input file format:
 *
 * Set of ULONG with (0,r,g,b), (0,r,g,b), ...
 *
 * map format:
 *
 * First ULONG is reserved for (colorAmount,startregister),
 * followed by (r,g,b) (each color value is a ULONG), final character in array is 0.
 * For usage with LoadRGB32(),
 * see http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node02FB.html
 * for details.
 */
BOOL loadColorMap32(char *fileName, ULONG *map, UWORD colorAmount)
{
    ULONG *buffer;
    UWORD i;
    UBYTE r;
    UBYTE g;
    UBYTE b;

    //we need a buffer which contains the input file for further processing
    buffer = AllocMem(colorAmount * sizeof(ULONG), MEMF_ANY);
    if (!buffer)
    {
        writeLog("Error: Could not allocate memory for 32 bit color table input file buffer\n");
        return FALSE;
    }
    writeLogFS("Allocated %d bytes for 32 bit color table input file buffer\n",
               colorAmount * sizeof(ULONG));

    //reuse loadColorMap() because as a first step we need its content in a buffer
    if (!loadColorMap(fileName, (UWORD *)buffer, colorAmount * 2))
    {
        FreeMem(buffer, colorAmount * sizeof(ULONG));
        writeLogFS("Freeing %d bytes of 32 bit color table input file buffer\n",
                   colorAmount * sizeof(ULONG));
        return FALSE;
    }
    writeLog("Loaded 32 bit color table\n");

    //header
    map[0] = (((ULONG)colorAmount) << 16) + 0;

    //convert rgb bytes to ulong triples
    for (i = 0; i < colorAmount; i++)
    {
        b = (UBYTE)(0x000000ff & buffer[i]);
        g = (UBYTE)((0x0000ff00 & buffer[i]) >> 8);
        r = (UBYTE)((0x00ff0000 & buffer[i]) >> 16);
        map[1 + i * 3 + 0] = SPREAD(r);
        map[1 + i * 3 + 1] = SPREAD(g);
        map[1 + i * 3 + 2] = SPREAD(b);
    }

    //null termination ulong
    map[COLORMAP32_LONG_SIZE(colorAmount) - 1] = 0;

    FreeMem(buffer, colorAmount * sizeof(ULONG));
    writeLogFS("Freeing %d bytes of 32 bit color table input file buffer\n",
               colorAmount * sizeof(ULONG));
    return TRUE;
}

/**
 * Load non-interlaced graphic blob from file system and copy it into bitplanes
 */
struct BitMap *loadBlob(const char *fileName, UBYTE depth, UWORD width,
                        UWORD height)
{
    LONG fileSize, planeSize, dataRead = 0;
    UWORD rowSize = 0;
    UWORD destRowSize = 0;
    UWORD i = 0;
    UWORD j = 0;
    UBYTE *data;
    ULONG dataIndex = 0;

    struct BitMap *blobBitMap = NULL;
    BPTR blobFileHandle = NULL;
    writeLogFS("Trying to load blob %s\n", fileName);

    //Open input file
    blobFileHandle = Open(fileName, MODE_OLDFILE);
    if (!blobFileHandle)
    {
        writeLogFS("Error: Could not read %s\n", fileName);
        return NULL;
    }

    //Get file size
    Seek(blobFileHandle, 0, OFFSET_END);
    fileSize = Seek(blobFileHandle, 0, OFFSET_BEGINNING);
    writeLogFS("Blob %s has file size %d\n", fileName, fileSize);

    //Copy file content to raster
    blobBitMap = AllocBitMap(width, height, depth,
                             BMF_DISPLAYABLE | BMF_CLEAR, NULL);
    if (blobBitMap == NULL)
    {
        return NULL;
    }

    /*
     * Calculate line size of input file and target
     * bitmap based. Get the delta and use it to load
     * from file to target. Thhis is neccessary because
     * row sizes differ due to memory alignment restrictions.
     */
    rowSize = width / 8;
    rowSize += (width % 8 > 0) ? 1 : 0;
    planeSize = (blobBitMap->Rows) * rowSize;
    destRowSize = (blobBitMap->BytesPerRow);
    writeLogFS("Calculated input file plane size: %d\n",
               (blobBitMap->Rows) * rowSize);
    writeLogFS("Calculated target buffer plane size: %d\n",
               (blobBitMap->Rows) * (blobBitMap->BytesPerRow));

    // use the delta to load line by line from file to target bitmap
    for (i = 0; i < depth; i++)
    {
        dataIndex = 0;
        data = blobBitMap->Planes[i];
        dataRead = 0;
        for (j = 0; j < (blobBitMap->Rows); j++)
        {
            dataRead += Read(blobFileHandle, &(data[dataIndex]), rowSize);
            dataIndex += destRowSize;
            if (dataRead == -1)
            {
                writeLog("Error: Could not read from Blob input file\n");
                Close(blobFileHandle);
                return NULL;
            }
            else if (dataRead == 0)
            {
                writeLogFS("All data read at bitplane %d\n", i);
                Close(blobFileHandle);
                return blobBitMap;
            }
            else
            {
                continue;
            }

            writeLogFS("Read %d bytes for bitplane %d\n", dataRead, i);
        }
    }

    Close(blobFileHandle);
    return blobBitMap;
}
