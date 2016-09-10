/*
Data structures for geometric elements and functions that operate on them.
Defines the TESSEROID, SPHERE, and PRISM structures.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "constants.h"
#include "logger.h"
#include "geometry.h"


/* Split a tesseroid into 8. */
void split_tess(TESSEROID tess, TESSEROID *split)
{
    double dlon = 0.5*(tess.e - tess.w),
           dlat = 0.5*(tess.n - tess.s),
           dr = 0.5*(tess.r2 - tess.r1),
           ws[2], ss[2], r1s[2];
    int i, j, k, t = 0;

    ws[0] = tess.w;
    ws[1] = tess.w + dlon;
    ss[0] = tess.s;
    ss[1] = tess.s + dlat;
    r1s[0] = tess.r1;
    r1s[1] = tess.r1 + dr;
    for(k = 0; k < 2; k++)
    {
        for(j = 0; j < 2; j++)
        {
            for(i = 0; i < 2; i++)
            {
                split[t].w = ws[i];
                split[t].e = ws[i] + dlon;
                split[t].s = ss[j];
                split[t].n = ss[j] + dlat;
                split[t].r1 = r1s[k];
                split[t].r2 = r1s[k] + dr;
                split[t].density = tess.density;
                t++;
            }
        }
    }
}
