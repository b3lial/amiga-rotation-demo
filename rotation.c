#include "rotation-demo.h"

USHORT currentDegree = DEGREE_RESOLUTION;

float cosLookup[] = {\
   1.0,\
   0.98481,\
   0.93969,\
   0.86603,\
   0.76604,\
   0.64279,\
   0.5,\
   0.34202,\
   0.17365,\
   0.0,\
   -0.17365,\
   -0.34202,\
   -0.5,\
   -0.64279,\
   -0.76604,\
   -0.86603,\
   -0.93969,\
   -0.98481,\
   -1.0,\
   -0.98481,\
   -0.93969,\
   -0.86603,\
   -0.76604,\
   -0.64279,\
   -0.5,\
   -0.34202,\
   -0.17365,\
   -0.0,\
   0.17365,\
   0.34202,\
   0.5,\
   0.64279,\
   0.76604,\
   0.86603,\
   0.93969,\
   0.98481\
};

float sinLookup[] = {\
   0.0,\
   0.17365,\
   0.34202,\
   0.5,\
   0.64279,\
   0.76604,\
   0.86603,\
   0.93969,\
   0.98481,\
   1.0,\
   0.98481,\
   0.93969,\
   0.86603,\
   0.76604,\
   0.64279,\
   0.5,\
   0.34202,\
   0.17365,\
   0.0,\
   -0.17365,\
   -0.34202,\
   -0.5,\
   -0.64279,\
   -0.76604,\
   -0.86603,\
   -0.93969,\
   -0.98481,\
   -1.0,\
   -0.98481,\
   -0.93969,\
   -0.86603,\
   -0.76604,\
   -0.64279,\
   -0.5,\
   -0.34202,\
   -0.17365\
};

// apply rotation matrix
void rotatePixel(int dest_x, int *src_x, int *src_y,
                 float y_mult_sin, float y_mult_cos,
                 UWORD i)
{
    float f_x = (float) dest_x;
    *src_x = (int)(f_x * cosLookup[i] - y_mult_sin);
    *src_y = (int)(f_x * sinLookup[i] + y_mult_cos);
}

void rotate(UBYTE *src, UBYTE *dest, unsigned int size)
{
    int x, y;
    int src_index, dest_index;
    int dest_x, dest_y;
    int src_x, src_y;
    float y_mult_sin, y_mult_cos;
    UWORD lookupIndex;

    // iterate over destination array
    lookupIndex = (360 - currentDegree) / DEGREE_RESOLUTION;
    for (y = 0; y < RECT_BITMAP_HEIGHT; y++)
    {
        // precalculate these values to speed things up
        dest_y = (RECT_BITMAP_HEIGHT / 2) - y;
        y_mult_sin = ((float) dest_y) * sinLookup[lookupIndex];
        y_mult_cos = ((float) dest_y) * cosLookup[lookupIndex];

        for (x = 0; x < RECT_BITMAP_WIDTH; x++)
        {
            // calculate src x/y coordinates
            dest_x = x - (RECT_BITMAP_WIDTH / 2);
            rotatePixel(dest_x, &src_x, &src_y,
                        y_mult_sin, y_mult_cos, lookupIndex);

            // convert coordinates back to array indexes
            // so we can move the rotated pixel to its new position
            dest_index = x + y * RECT_BITMAP_WIDTH;
            src_index = (src_x + (RECT_BITMAP_WIDTH / 2)) +
                        ((src_y + (RECT_BITMAP_HEIGHT / 2)) * RECT_BITMAP_WIDTH);
            if (src_index < 0 || src_index >= size)
            {
                continue;
            }
            dest[dest_index] = src[src_index];
        }
    }

    // after one full rotation, begin again at 0 degrees
    currentDegree = (currentDegree == 360) ? 10 : currentDegree + DEGREE_RESOLUTION;
}