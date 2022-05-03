#include "rotation-demo.h"

USHORT currentDegree = 50;
BOOL printFlag = TRUE;

float cosLookup[] = {
    0.98481,
    0.93969,
    0.86603,
    0.76604,
    0.64279,
    0.5,
    0.34202,
    0.17365,
    0.0,
    -0.17365,
    -0.34202,
    -0.5,
    -0.64279,
    -0.76604,
    -0.86603,
    -0.93969,
    -0.98481,
    -1.0,
    -0.98481,
    -0.93969,
    -0.86603,
    -0.76604,
    -0.64279,
    -0.5,
    -0.34202,
    -0.17365,
    -0.0,
    0.17365,
    0.34202,
    0.5,
    0.64279,
    0.76604,
    0.86603,
    0.93969,
    0.98481};

float sinLookup[] = {
    0.17365,
    0.34202,
    0.5,
    0.64279,
    0.76604,
    0.86603,
    0.93969,
    0.98481,
    1.0,
    0.98481,
    0.93969,
    0.86603,
    0.76604,
    0.64279,
    0.5,
    0.34202,
    0.17365,
    0.0,
    -0.17365,
    -0.34202,
    -0.5,
    -0.64279,
    -0.76604,
    -0.86603,
    -0.93969,
    -0.98481,
    -1.0,
    -0.98481,
    -0.93969,
    -0.86603,
    -0.76604,
    -0.64279,
    -0.5,
    -0.34202,
    -0.17365};

// apply rotation matrix
void rotatePixel(int x, int y, float *new_x, float *new_y)
{
    float f_x, f_y;
    UWORD i;
    f_x = (float)x;
    f_y = (float)y;

    i = currentDegree / DEGREE_RESOLUTION - 1;
    *new_x = (f_x * cosLookup[i] - f_y * sinLookup[i]);
    *new_y = (f_x * sinLookup[i] + f_y * cosLookup[i]);
}

float poorRound(float num){
    return (num - floor(num) > 0.5) ? ceil(num) : floor(num);
}

void rotate(UBYTE *src, UBYTE *dest, unsigned int size)
{
    int x, y;
    int src_index, dest_index;
    int x_base, y_base;
    float new_x, new_y;
    int new_x_int, new_y_int;

    // just a copy if rotation degree == 0
    if (!currentDegree)
    {
        memcpy(dest, src, size);
        currentDegree += DEGREE_RESOLUTION;
        return;
    }

    // iterate over array we want to rotate
    memset(dest, 0, size);
    for (y = 0; y < RECT_HEIGHT; y++)
    {
        for (x = 0; x < RECT_WIDTH; x++)
        {
            // change middle of coordinate system to
            // rotate object around itw own axis
            x_base = x - (RECT_WIDTH / 2);
            y_base = (RECT_HEIGHT / 2) - y;

            // convert x,y -> array index
            rotatePixel(x_base, y_base, &new_x, &new_y);
            new_x_int = (int) poorRound(new_x);
            new_y_int = (int) poorRound(new_y);

            // convert coordinates back to array indexes
            // so we can move the rotated pixel to its new position
            src_index = (x + RECT_X) + ((y + RECT_Y) * RECT_BITMAP_WIDTH);
            dest_index = (new_x_int + (RECT_WIDTH / 2) + RECT_X) +
                         (((-1 * new_y_int + (RECT_HEIGHT / 2)) + RECT_Y) * RECT_BITMAP_WIDTH);

            if (printFlag && src[src_index] && dest[dest_index])
            {
                printf("collision src %d,%d - dest %f,%f - casted %d,%d\n", 
                    x_base, y_base, new_x, new_y, new_x_int, new_y_int);
            }
            dest[dest_index] = src[src_index];
        }
    }
    printFlag = FALSE;

    // after one full rotation, begin again at 0 degrees
    // currentDegree+=DEGREE_RESOLUTION;
    if (currentDegree == 360)
    {
        currentDegree = 0;
    }
}