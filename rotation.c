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
void rotatePixel(int x, int y, int* new_x, int* new_y){
    float f_x, f_y;
    UWORD i;
    f_x = (float) x;
    f_y = (float) y;

    i = currentDegree / DEGREE_RESOLUTION - 1;
    *new_x = (int)(f_x * cosLookup[i] - f_y * sinLookup[i]);
    *new_y = (int)(f_x * sinLookup[i] + f_y * cosLookup[i]);
}

void rotate(UBYTE *src, UBYTE *dest, unsigned int size)
{
    int x, y;
    int src_index, dest_index;
    int x_base, y_base;
    int new_x, new_y;

    // just a copy if rotation degree == 0
    if(!currentDegree){
        memcpy(dest, src, size);
        currentDegree+=DEGREE_RESOLUTION;
        return;
    }

    // iterate over array we want to rotate
    memset(dest,0,size);
    for (y = 0; y < RECT_HEIGHT; y++) {
        for (x = 0; x < RECT_WIDTH; x++) {
            // change middle of coordinate system to
            // rotate object around itw own axis
            x_base = x - (RECT_WIDTH / 2);
            y_base = (RECT_HEIGHT / 2) - y;

            // convert x,y -> array index
            rotatePixel(x_base, y_base, &new_x, &new_y);

            // convert coordinates back to array indexes
            // so we can move the rotated pixel to its new position
            src_index = (x+RECT_X) + ((y+RECT_Y)*RECT_BITMAP_WIDTH);
            dest_index = (new_x+(RECT_WIDTH / 2)+RECT_X) + (((-1*new_y+(RECT_HEIGHT / 2))+RECT_Y)*RECT_BITMAP_WIDTH);
            dest[dest_index] = src[src_index];
        }
    }

    // after one full rotation, begin again at 0 degrees
    currentDegree+=DEGREE_RESOLUTION;
    if(currentDegree == 360){
        currentDegree=0;
    }
}