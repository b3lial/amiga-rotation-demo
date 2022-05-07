#include "rotation-demo.h"

USHORT currentDegree = DEGREE_RESOLUTION;

float cosLookup[] = {\
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
void rotatePixel(int dest_x, int dest_y, int* src_x, int* src_y){
    float f_x, f_y;
    UWORD i;
    f_x = (float) dest_x;
    f_y = (float) dest_y;

    i = currentDegree / DEGREE_RESOLUTION - 1;
    *src_x = (int)(f_x * cosLookup[i] - f_y * sinLookup[i]);
    *src_y = (int)(f_x * sinLookup[i] + f_y * cosLookup[i]);
}

void rotate(UBYTE *src, UBYTE *dest, unsigned int size)
{
    int x, y;
    int src_index, dest_index;
    int dest_x, dest_y;
    int src_x, src_y;

    // just a copy if rotation degree == 0
    if(!currentDegree){
        memcpy(dest, src, size);
        currentDegree+=DEGREE_RESOLUTION;
        return;
    }

    // iterate over destination array
    memset(dest,0,size);
    for (y = 0; y < RECT_HEIGHT; y++) {
        for (x = 0; x < RECT_WIDTH; x++) {
            // change middle of coordinate system to
            // rotate object around itw own axis
            dest_x = x - (RECT_WIDTH / 2);
            dest_y = (RECT_HEIGHT / 2) - y;

            // calculate src x/y coordinates
            rotatePixel(dest_x, dest_y, &src_x, &src_y);

            // convert coordinates back to array indexes
            // so we can move the rotated pixel to its new position
            src_index = (x+RECT_X) + ((y+RECT_Y)*RECT_BITMAP_WIDTH);
            dest_index = (src_x+(RECT_WIDTH / 2)+RECT_X) + (((-1*src_y+(RECT_HEIGHT / 2))+RECT_Y)*RECT_BITMAP_WIDTH);
            dest[dest_index] = src[src_index];
        }
    }

    // after one full rotation, begin again at 0 degrees
    currentDegree+=DEGREE_RESOLUTION;
    if(currentDegree == 360){
        currentDegree=0;
    }
}