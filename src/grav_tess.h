/*
Functions that calculate the gravitational potential and its first and second
derivatives for the tesseroid.

The gravity gradients can be calculated using the general formula of
Grombein et al. (2010).
The integrals are solved using the Gauss-Legendre Quadrature rule
(Asgharzadeh et al., 2007).

The derivatives of the potential are made with respect to the local coordinate
system x->North, y->East, z->Up (away from center of the Earth).

To maintain the standard convention, only for component gz the z axis is
inverted, so a positive density results in positive gz.

Example
-------

To calculate the gzz component due to a tesseroid on a regular grid:

    #include <stdio.h>
    #include "glq.h"r
    #include "constants.h"
    #include "grav_tess.h"

    int main()
    {
        TESSEROID tess = {1000, 44, 46, -1, 1, MEAN_EARTH_RADIUS - 100000,
                          MEAN_EARTH_RADIUS};
        GLQ *glqlon, *glqlat, *glqr;
        double lon, lat, r = MEAN_EARTH_RADIUS + 1500000, res;
        int order = 8;

        glqlon = glq_new(order, tess.w, tess.e);
        glqlat = glq_new(order, tess.s, tess.n);
        glqr = glq_new(order, tess.r1, tess.r2);

        for(lat = 20; lat <= 70; lat += 0.5)
        {
            for(lon = -25; lon <= 25; lon += 0.5)
            {
                res = tess_gzz(tess, lon, lat, r, *glqlon, *glqlat, *glqr);
                printf("%g %g %g\n", lon, lat, res);
            }
        }

        glq_free(glqlon);
        glq_free(glqlat);
        glq_free(glqr);

        return 0;
    }

References
----------

Asgharzadeh, M.F., von Frese, R.R.B., Kim, H.R., Leftwich, T.E. & Kim, J.W.
(2007): Spherical prism gravity effects by Gauss-Legendre quadrature integration.
Geophysical Journal International, 169, 1-11.

Grombein, T.; Seitz, K.; Heck, B. (2010): Untersuchungen zur effizienten
Berechnung topographischer Effekte auf den Gradiententensor am Fallbeispiel der
Satellitengradiometriemission GOCE.
KIT Scientific Reports 7547, ISBN 978-3-86644-510-9, KIT Scientific Publishing,
Karlsruhe, Germany.
*/

#ifndef _TESSEROIDS_GRAV_TESS_H_
#define _TESSEROIDS_GRAV_TESS_H_


/* Needed for definition of TESSEROID */
#include "geometry.h"
/* Needed for definition of GLQ */
#include "glq.h"

double calc_tess_model(TESSEROID *model, int size, double lonp, double latp, double rp, GLQ *glq_lon, GLQ *glq_lat, GLQ *glq_r, double (*field)(TESSEROID, double, double, double, GLQ, GLQ, GLQ));
double calc_tess_model_adapt(TESSEROID *model, int size, double lonp, double latp, double rp, GLQ *glq_lon, GLQ *glq_lat, GLQ *glq_r, double (*field)(TESSEROID, double, double, double, GLQ, GLQ, GLQ), double ratio);

double tess_gxx(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon, GLQ glq_lat, GLQ glq_r);
double tess_gxy(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon, GLQ glq_lat, GLQ glq_r);
double tess_gxz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon, GLQ glq_lat, GLQ glq_r);
double tess_gyy(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon, GLQ glq_lat, GLQ glq_r);
double tess_gyz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon, GLQ glq_lat, GLQ glq_r);
double tess_gzz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon, GLQ glq_lat, GLQ glq_r);

#endif
