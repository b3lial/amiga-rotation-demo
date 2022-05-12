#include "rotation-demo.h"

USHORT currentDegree = DEGREE_RESOLUTION;

int cosLookup[] = {\
   FLOATTOFIX(1.0),\
   FLOATTOFIX(0.9848),\
   FLOATTOFIX(0.9397),\
   FLOATTOFIX(0.866),\
   FLOATTOFIX(0.766),\
   FLOATTOFIX(0.6428),\
   FLOATTOFIX(0.5),\
   FLOATTOFIX(0.342),\
   FLOATTOFIX(0.1736),\
   FLOATTOFIX(0.0),\
   FLOATTOFIX(-0.1736),\
   FLOATTOFIX(-0.342),\
   FLOATTOFIX(-0.5),\
   FLOATTOFIX(-0.6428),\
   FLOATTOFIX(-0.766),\
   FLOATTOFIX(-0.866),\
   FLOATTOFIX(-0.9397),\
   FLOATTOFIX(-0.9848),\
   FLOATTOFIX(-1.0),\
   FLOATTOFIX(-0.9848),\
   FLOATTOFIX(-0.9397),\
   FLOATTOFIX(-0.866),\
   FLOATTOFIX(-0.766),\
   FLOATTOFIX(-0.6428),\
   FLOATTOFIX(-0.5),\
   FLOATTOFIX(-0.342),\
   FLOATTOFIX(-0.1736),\
   FLOATTOFIX(-0.0),\
   FLOATTOFIX(0.1736),\
   FLOATTOFIX(0.342),\
   FLOATTOFIX(0.5),\
   FLOATTOFIX(0.6428),\
   FLOATTOFIX(0.766),\
   FLOATTOFIX(0.866),\
   FLOATTOFIX(0.9397),\
   FLOATTOFIX(0.9848)\
};

int sinLookup[] = {\
   FLOATTOFIX(0.0),\
   FLOATTOFIX(0.1736),\
   FLOATTOFIX(0.342),\
   FLOATTOFIX(0.5),\
   FLOATTOFIX(0.6428),\
   FLOATTOFIX(0.766),\
   FLOATTOFIX(0.866),\
   FLOATTOFIX(0.9397),\
   FLOATTOFIX(0.9848),\
   FLOATTOFIX(1.0),\
   FLOATTOFIX(0.9848),\
   FLOATTOFIX(0.9397),\
   FLOATTOFIX(0.866),\
   FLOATTOFIX(0.766),\
   FLOATTOFIX(0.6428),\
   FLOATTOFIX(0.5),\
   FLOATTOFIX(0.342),\
   FLOATTOFIX(0.1736),\
   FLOATTOFIX(0.0),\
   FLOATTOFIX(-0.1736),\
   FLOATTOFIX(-0.342),\
   FLOATTOFIX(-0.5),\
   FLOATTOFIX(-0.6428),\
   FLOATTOFIX(-0.766),\
   FLOATTOFIX(-0.866),\
   FLOATTOFIX(-0.9397),\
   FLOATTOFIX(-0.9848),\
   FLOATTOFIX(-1.0),\
   FLOATTOFIX(-0.9848),\
   FLOATTOFIX(-0.9397),\
   FLOATTOFIX(-0.866),\
   FLOATTOFIX(-0.766),\
   FLOATTOFIX(-0.6428),\
   FLOATTOFIX(-0.5),\
   FLOATTOFIX(-0.342),\
   FLOATTOFIX(-0.1736)\
};

// apply rotation matrix
void rotatePixel(int dest_x, int *src_x, int *src_y,
                 int y_mult_sin, int y_mult_cos,
                 UWORD i)
{
    int f_x = INTTOFIX(dest_x);
    *src_x = FIXTOINT(FIXMULT(f_x, cosLookup[i]) - y_mult_sin);
    *src_y = FIXTOINT(FIXMULT(f_x, sinLookup[i]) + y_mult_cos);
}

void rotate(struct RotationData *rd)
{
    int x, y;
    int src_index, dest_index;
    int dest_x, dest_y;
    int src_x, src_y;
    int y_mult_sin, y_mult_cos;
    UWORD lookupIndex;

    // iterate over destination array
    lookupIndex = (360 - rd->angle) / DEGREE_RESOLUTION;
    for (y = 0; y < rd->height; y++)
    {
        // precalculate these values to speed things up
        dest_y = (rd->height / 2) - y;
        y_mult_sin = FIXMULT(INTTOFIX(dest_y), sinLookup[lookupIndex]);
        y_mult_cos = FIXMULT(INTTOFIX(dest_y), cosLookup[lookupIndex]);

        for (x = 0; x < rd->width; x++)
        {
            // calculate src x/y coordinates
            dest_x = x - (rd->width / 2);
            rotatePixel(dest_x, &src_x, &src_y,
                        y_mult_sin, y_mult_cos, lookupIndex);

            // convert coordinates back to array indexes
            // so we can move the rotated pixel to its new position
            dest_index = x + y * rd->width;
            src_index = (src_x + (rd->width / 2)) +
                        ((src_y + (rd->height / 2)) * rd->width);
            if (src_index < 0 || src_index >= (rd->height * rd->width))
            {
                continue;
            }
            (rd->dest)[dest_index] = (rd->src)[src_index];
        }
    }
}