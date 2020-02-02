/*
Functions that calculate the gravitational potential and its first and second
derivatives for the tesseroid.

References
----------

* Grombein, T.; Seitz, K.; Heck, B. (2010): Untersuchungen zur effizienten
Berechnung topographischer Effekte auf den Gradiententensor am Fallbeispiel der
Satellitengradiometriemission GOCE.
KIT Scientific Reports 7547, ISBN 978-3-86644-510-9, KIT Scientific Publishing,
Karlsruhe, Germany.
*/


#include <math.h>
#include "logger.h"
#include "geometry.h"
#include "glq.h"
#include "constants.h"
#include "grav_tess.h"


/* Calculates the field of a tesseroid model at a given point. */
double calc_tess_model(TESSEROID *model, int size, double lonp, double latp, double rp, GLQ *glq_lon, GLQ *glq_lat, GLQ *glq_r, double (*field)(TESSEROID, double, double, double, GLQ, GLQ, GLQ))
{
    double res;
    int tess;

    res = 0;
    for(tess = 0; tess < size; tess++)
    {
        if(lonp >= model[tess].w && lonp <= model[tess].e &&
           latp >= model[tess].s && latp <= model[tess].n &&
           rp >= model[tess].r1 && rp <= model[tess].r2)
        {
            log_warning("Point (%g %g %g) is on tesseroid %d: %g %g %g %g %g %g %g. Can't guarantee accuracy.",
                        lonp, latp, rp - MEAN_EARTH_RADIUS, tess,
                        model[tess].w, model[tess].e, model[tess].s,
                        model[tess].n, model[tess].r2 - MEAN_EARTH_RADIUS,
                        model[tess].r1 - MEAN_EARTH_RADIUS,
                        model[tess].density);
        }
        glq_set_limits(model[tess].w, model[tess].e, glq_lon);
        glq_set_limits(model[tess].s, model[tess].n, glq_lat);
        glq_set_limits(model[tess].r1, model[tess].r2, glq_r);
        res += field(model[tess], lonp, latp, rp, *glq_lon, *glq_lat, *glq_r);
    }
    return res;
}

void calc_tess_model_triple(TESSEROID *model, int size, double lonp, double latp, double rp, GLQ *glq_lon, GLQ *glq_lat, GLQ *glq_r, void (*field_triple)(TESSEROID, double, double, double, GLQ, GLQ, GLQ, double*), double *res)
{
    double r1, r2, r3, ri[3];
    int tess;

    res[0] = 0;
    res[1] = 0;
    res[2] = 0;
    for(tess = 0; tess < size; tess++)
    {
        ri[0] = 0;
        ri[1] = 0;
        ri[2] = 0;

        if(lonp >= model[tess].w && lonp <= model[tess].e &&
           latp >= model[tess].s && latp <= model[tess].n &&
           rp >= model[tess].r1 && rp <= model[tess].r2)
        {
            log_warning("Point (%g %g %g) is on tesseroid %d: %g %g %g %g %g %g %g. Can't guarantee accuracy.",
                        lonp, latp, rp - MEAN_EARTH_RADIUS, tess,
                        model[tess].w, model[tess].e, model[tess].s,
                        model[tess].n, model[tess].r2 - MEAN_EARTH_RADIUS,
                        model[tess].r1 - MEAN_EARTH_RADIUS,
                        model[tess].density);
        }
        glq_set_limits(model[tess].w, model[tess].e, glq_lon);
        glq_set_limits(model[tess].s, model[tess].n, glq_lat);
        glq_set_limits(model[tess].r1, model[tess].r2, glq_r);
        field_triple(model[tess], lonp, latp, rp, *glq_lon, *glq_lat, *glq_r, ri);

        res[0] += ri[0];
        res[1] += ri[1];
        res[2] += ri[2];
    }
    return;
}



/* Adaptatively calculate the field of a tesseroid model at a given point */
double calc_tess_model_adapt(TESSEROID *model, int size, double lonp, double latp, double rp, GLQ *glq_lon, GLQ *glq_lat, GLQ *glq_r, double (*field)(TESSEROID, double, double, double, GLQ, GLQ, GLQ), double ratio)
{
    double res, dist, lont, latt, rt, d2r = PI/180.;
    int tess;
    TESSEROID split[8];

    res = 0;
    for(tess = 0; tess < size; tess++)
    {
        rt = model[tess].r2;
        lont = 0.5*(model[tess].w + model[tess].e);
        latt = 0.5*(model[tess].s + model[tess].n);
        dist = sqrt(rp*rp + rt*rt - 2*rp*rt*(sin(d2r*latp)*sin(d2r*latt) +
                    cos(d2r*latp)*cos(d2r*latt)*cos(d2r*(lonp - lont))));

        /* Would get stuck in infinite loop if dist = 0 and get wrong results if
           inside de tesseroid. Still do the calculation but warn user that it's
           probably wrong. */
        if(lonp >= model[tess].w && lonp <= model[tess].e &&
           latp >= model[tess].s && latp <= model[tess].n &&
           rp >= model[tess].r1 && rp <= model[tess].r2)
        {
            log_warning("Point (%g %g %g) is on top of tesseroid %d: %g %g %g %g %g %g %g. Can't guarantee accuracy.",
                        lonp, latp, rp - MEAN_EARTH_RADIUS, tess,
                        model[tess].w, model[tess].e, model[tess].s,
                        model[tess].n, model[tess].r2 - MEAN_EARTH_RADIUS,
                        model[tess].r1 - MEAN_EARTH_RADIUS,
                        model[tess].density);
            glq_set_limits(model[tess].w, model[tess].e, glq_lon);
            glq_set_limits(model[tess].s, model[tess].n, glq_lat);
            glq_set_limits(model[tess].r1, model[tess].r2, glq_r);
            res += field(model[tess], lonp, latp, rp, *glq_lon, *glq_lat,
                         *glq_r);
        }
        /* Check if the computation point is at an acceptable distance. If not
           split the tesseroid using the given ratio */
        else if(
            dist < ratio*MEAN_EARTH_RADIUS*d2r*(model[tess].e - model[tess].w) ||
            dist < ratio*MEAN_EARTH_RADIUS*d2r*(model[tess].n - model[tess].s) ||
            dist < ratio*(model[tess].r2 - model[tess].r1))
        {
            log_debug("Splitting tesseroid %d (%g %g %g %g %g %g %g) at point (%g %g %g) using ratio %g",
                      tess, model[tess].w, model[tess].e, model[tess].s,
                      model[tess].n, model[tess].r2 - MEAN_EARTH_RADIUS,
                      model[tess].r1 - MEAN_EARTH_RADIUS, model[tess].density,
                      lonp, latp, rp - MEAN_EARTH_RADIUS, ratio);
            /* Do it recursively until ratio*size is smaller than distance */
            split_tess(model[tess], split);
            res += calc_tess_model_adapt(split, 8, lonp, latp, rp, glq_lon,
                                         glq_lat, glq_r, field, ratio);
        }
        else
        {
            glq_set_limits(model[tess].w, model[tess].e, glq_lon);
            glq_set_limits(model[tess].s, model[tess].n, glq_lat);
            glq_set_limits(model[tess].r1, model[tess].r2, glq_r);
            res += field(model[tess], lonp, latp, rp, *glq_lon, *glq_lat,
                         *glq_r);
        }
    }
    return res;
}

/* Calculates gxx caused by a tesseroid. */
double tess_gxx(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r)
{
    double d2r = PI/180., l_sqr, kphi, coslatp, coslatc, sinlatp, sinlatc,
           coslon, rc, kappa, res;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));

                l_sqr = rp*rp + rc*rc - 2*rp*rc*(sinlatp*sinlatc +
                                                 coslatp*coslatc*coslon);

                kphi = coslatp*sinlatc - sinlatp*coslatc*coslon;

                kappa = rc*rc*coslatc;

                res += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*rc*kphi*rc*kphi - l_sqr)/pow(l_sqr, 2.5);
            }
        }
    }

    res *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    return res;
}


/* Calculates gxy caused by a tesseroid. */
double tess_gxy(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r)
{
    double d2r = PI/180., l_sqr, kphi, coslatp, coslatc, sinlatp, sinlatc,
           coslon, sinlon, rc, kappa, deltax, deltay, res;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));
                sinlon = sin(d2r*(glq_lon.nodes[k] - lonp));

                l_sqr = rp*rp + rc*rc - 2*rp*rc*(sinlatp*sinlatc +
                                                 coslatp*coslatc*coslon);

                kphi = coslatp*sinlatc - sinlatp*coslatc*coslon;

                kappa = rc*rc*coslatc;

                deltax = rc*kphi;

                deltay = rc*coslatc*sinlon;

                res += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltax*deltay)/pow(l_sqr, 2.5);
            }
        }
    }

    res *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    return res;
}


/* Calculates gxz caused by a tesseroid. */
double tess_gxz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r)
{
    double d2r = PI/180., l_sqr, kphi, coslatp, coslatc, sinlatp, sinlatc,
           coslon, cospsi, rc, kappa, deltax, deltaz, res;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));

                cospsi = sinlatp*sinlatc + coslatp*coslatc*coslon;

                l_sqr = rp*rp + rc*rc - 2*rp*rc*cospsi;

                kphi = coslatp*sinlatc - sinlatp*coslatc*coslon;

                kappa = rc*rc*coslatc;

                deltax = rc*kphi;

                deltaz = rc*cospsi - rp;

                res += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltax*deltaz)/pow(l_sqr, 2.5);
            }
        }
    }

    res *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    return res;
}


/* Calculates gyy caused by a tesseroid. */
double tess_gyy(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r)
{
    double d2r = PI/180., l_sqr, coslatp, coslatc, sinlatp, sinlatc,
           coslon, sinlon, rc, kappa, deltay, res;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));
                sinlon = sin(d2r*(glq_lon.nodes[k] - lonp));

                l_sqr = rp*rp + rc*rc - 2*rp*rc*(sinlatp*sinlatc +
                                                 coslatp*coslatc*coslon);

                kappa = rc*rc*coslatc;

                deltay = rc*coslatc*sinlon;

                res += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltay*deltay - l_sqr)/pow(l_sqr, 2.5);
            }
        }
    }

    res *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    return res;
}


/* Calculates gyz caused by a tesseroid. */
double tess_gyz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r)
{
    double d2r = PI/180., l_sqr, coslatp, coslatc, sinlatp, sinlatc,
           coslon, sinlon, cospsi, rc, kappa, deltay, deltaz, res;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));
                sinlon = sin(d2r*(glq_lon.nodes[k] - lonp));

                cospsi = sinlatp*sinlatc + coslatp*coslatc*coslon;

                l_sqr = rp*rp + rc*rc - 2*rp*rc*cospsi;

                kappa = rc*rc*coslatc;

                deltay = rc*coslatc*sinlon;

                deltaz = rc*cospsi - rp;

                res += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltay*deltaz)/pow(l_sqr, 2.5);
            }
        }
    }

    res *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    return res;
}


/* Calculates gzz caused by a tesseroid. */
double tess_gzz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r)
{
    double d2r = PI/180., l_sqr, coslatp, coslatc, sinlatp, sinlatc,
           coslon, cospsi, rc, kappa, deltaz, res;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));

                cospsi = sinlatp*sinlatc + coslatp*coslatc*coslon;

                l_sqr = rp*rp + rc*rc - 2*rp*rc*cospsi;

                kappa = rc*rc*coslatc;

                deltaz = rc*cospsi - rp;

                res += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltaz*deltaz - l_sqr)/pow(l_sqr, 2.5);
            }
        }
    }

    res *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    return res;
}

/*Calculate three gravity gradient components simultaneously*/
void tess_gxz_gyz_gzz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r, double *res)
{
    double d2r = PI/180., l_sqr, coslatp, coslatc, sinlatp, sinlatc, sinlon,
           coslon, cospsi, rc, kappa, deltaz, deltax, deltay, kphi,
           res_gxz, res_gyz, res_gzz;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res_gxz = 0;
    res_gyz = 0;
    res_gzz = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));
                sinlon = sin(d2r*(glq_lon.nodes[k] - lonp));
                cospsi = sinlatp*sinlatc + coslatp*coslatc*coslon;

                l_sqr = rp*rp + rc*rc - 2*rp*rc*cospsi;
                kphi = coslatp*sinlatc - sinlatp*coslatc*coslon;
                kappa = rc*rc*coslatc;

                deltax = rc*kphi;
                deltay = rc*coslatc*sinlon;
                deltaz = rc*cospsi - rp;

                res_gxz += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltax*deltaz)/pow(l_sqr, 2.5);

                res_gyz += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                              kappa*(3*deltay*deltaz)/pow(l_sqr, 2.5);

                res_gzz += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*deltaz*deltaz - l_sqr)/pow(l_sqr, 2.5);
            }
        }
    }

    res_gxz *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    res_gyz *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    res_gzz *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    res[0] = res_gxz;
    res[1] = res_gyz;
    res[2] = res_gzz;

    return;
}

void tess_gxx_gxy_gxz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r, double *res)
{
    double d2r = PI/180., l_sqr, coslatp, coslatc, sinlatp, sinlatc, sinlon,
           coslon, cospsi, rc, kappa, deltaz, deltax, deltay, kphi,
           res_gxx, res_gxy, res_gxz;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res_gxx = 0;
    res_gxy = 0;
    res_gxz = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));
                sinlon = sin(d2r*(glq_lon.nodes[k] - lonp));
                cospsi = sinlatp*sinlatc + coslatp*coslatc*coslon;

                l_sqr = rp*rp + rc*rc - 2*rp*rc*cospsi;
                kphi = coslatp*sinlatc - sinlatp*coslatc*coslon;
                kappa = rc*rc*coslatc;

                deltax = rc*kphi;
                deltay = rc*coslatc*sinlon;
                deltaz = rc*cospsi - rp;

                res_gxx += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                       kappa*(3*rc*kphi*rc*kphi - l_sqr)/pow(l_sqr, 2.5);

                res_gxy += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                              kappa*(3*deltax*deltay)/pow(l_sqr, 2.5);

                res_gxz += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                                     kappa*(3*deltax*deltaz)/pow(l_sqr, 2.5);
            }
        }
    }

    res_gxx *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    res_gxy *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    res_gxz *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
                  (tess.r2 - tess.r1)*0.125;

    res[0] = res_gxx;
    res[1] = res_gxy;
    res[2] = res_gxz;

    return;
}



void tess_gxy_gyy_gyz(TESSEROID tess, double lonp, double latp, double rp, GLQ glq_lon,
                GLQ glq_lat, GLQ glq_r, double *res)
{
    double d2r = PI/180., l_sqr, coslatp, coslatc, sinlatp, sinlatc, sinlon,
           coslon, cospsi, rc, kappa, deltaz, deltax, deltay, kphi,
           res_gxy, res_gyy, res_gyz;
    register int i, j, k;

    coslatp = cos(d2r*latp);
    sinlatp = sin(d2r*latp);

    res_gxy = 0;
    res_gyy = 0;
    res_gyz = 0;

    for(k = 0; k < glq_lon.order; k++)
    {
        for(j = 0; j < glq_lat.order; j++)
        {
            for(i = 0; i < glq_r.order; i++)
            {
                rc = glq_r.nodes[i];
                sinlatc = sin(d2r*glq_lat.nodes[j]);
                coslatc = cos(d2r*glq_lat.nodes[j]);
                coslon = cos(d2r*(lonp - glq_lon.nodes[k]));
                sinlon = sin(d2r*(glq_lon.nodes[k] - lonp));
                cospsi = sinlatp*sinlatc + coslatp*coslatc*coslon;

                l_sqr = rp*rp + rc*rc - 2*rp*rc*cospsi;
                kphi = coslatp*sinlatc - sinlatp*coslatc*coslon;
                kappa = rc*rc*coslatc;

                deltax = rc*kphi;
                deltay = rc*coslatc*sinlon;
                deltaz = rc*cospsi - rp;

                res_gxy += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                              kappa*(3*deltax*deltay)/pow(l_sqr, 2.5);

                res_gyy += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                              kappa*(3*deltay*deltay - l_sqr)/pow(l_sqr, 2.5);

                res_gyz += glq_lon.weights[k]*glq_lat.weights[j]*glq_r.weights[i]*
                              kappa*(3*deltay*deltaz)/pow(l_sqr, 2.5);

            }
        }
    }

    res_gxy *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
           (tess.r2 - tess.r1)*0.125;

    res_gyy *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
                  (tess.r2 - tess.r1)*0.125;

    res_gyz *= SI2EOTVOS*G*tess.density*d2r*(tess.e - tess.w)*d2r*(tess.n - tess.s)*
                  (tess.r2 - tess.r1)*0.125;

    res[0] = res_gxy;
    res[1] = res_gyy;
    res[2] = res_gyz;

    return;
}
