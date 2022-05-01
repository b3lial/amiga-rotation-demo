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
    int src_index, dst_index;
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
    for (x = 0; x < RECT_X; x++) {
        for (y = 0; y < RECT_Y; y++) {
            // convert x,y -> middle of coordinates
            // convert x,y -> array index
            rotatePixel(x, y, &new_x, &new_y);
            // convert middle of coordinates -> array index
            // write src -> dest
        }
    }

    // after one full rotation, begin again at 0 degrees
    currentDegree+=DEGREE_RESOLUTION;
    if(currentDegree == 360){
        currentDegree=0;
    }
}