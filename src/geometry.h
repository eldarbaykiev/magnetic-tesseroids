/*
Data structures for geometric elements and functions that operate on them.
Defines the TESSEROID, SPHERE, and PRISM structures.
*/

#ifndef _TESSEROIDS_GEOMETRY_H_
#define _TESSEROIDS_GEOMETRY_H_


/* Store information on a tesseroid */
typedef struct tess_struct {
    /* s, n, w, e in degrees. r1 and r2 are the smaller and larger radius */
    double density; /* in SI units */
    double w; /* western longitude border in degrees */
    double e; /* eastern longitude border in degrees */
    double s; /* southern latitude border in degrees */
    double n; /* northern latitude border in degrees */
    double r1; /* smallest radius border in SI units */
    double r2; /* largest radius border in SI units */
	double suscept; /* magnetic susceptibility */
	double Bx; /* x-component of ambient magnetic field */
	double By; /* y-component of ambient magnetic field */
	double Bz; /* z-component of ambient magnetic field */
	//double Rx;
	//double Ry;
	//double Rz;
} TESSEROID;


void split_tess(TESSEROID tess, TESSEROID *split);

#endif
