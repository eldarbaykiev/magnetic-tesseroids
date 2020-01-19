/*
Functions matrix and vector multiplications.

*/

#include "linalg.h"
#include "constants.h"
#include <math.h>

//macOS only!
//#include <Accelerate/Accelerate.h>

#ifdef __linux__ // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
	#include <cblas.h>
#elif defined(__APPLE__) && defined(__MACH__)
  #include <Accelerate/Accelerate.h>
#else
	//
#endif

/* Calculate magnetization vector in a coordinate system of a given point */
void conv_vect_cblas(double *vect, double lon1, double lat1, double lon2, double lat2, double *res)
{
	double a1 = DEG2RAD*lat1;
	double b1 = DEG2RAD*lon1;
  double a2 = DEG2RAD*lat2;
  double b2 = DEG2RAD*lon2;

	double cos_a1 = cos(PI/2.0-a1);
	double sin_a1 = sin(PI/2.0-a1);
	double cos_a2 = cos(PI/2.0-a2);
	double sin_a2 = sin(PI/2.0-a2);

	double cos_b1 = cos(b1);
	double sin_b1 = sin(b1);
	double cos_b2 = cos(b2);
	double sin_b2 = sin(b2);

	double Z1Y1[9] = {cos_a1*cos_b1, -sin_b1, cos_b1*sin_a1, cos_a1*sin_b1, cos_b1, sin_a1*sin_b1, -sin_a1, 0, cos_a1};
	double Z2Y2t[9] = {-cos_a2*cos_b2, -cos_a2*sin_b2, sin_a2, -sin_b2, cos_b2, 0, cos_b2*sin_a2, sin_a2*sin_b2, cos_a2};
	double R[9];

	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 3, 3, 3, 1.0, Z1Y1, 3, Z2Y2t, 3, 0.0, R, 3);

	R[0] = -R[0];
	R[3] = -R[3];
	R[6] = -R[6];

  cblas_dgemv(CblasRowMajor, CblasNoTrans, 3, 3, 1.0, R, 3, vect, 1, 0.0, res, 1);


	return;
}


void conv_vect_cblas_precalc(double *vect, double cos_a1, double sin_a1, double cos_b1, double sin_b1, double cos_a2, double sin_a2, double cos_b2, double sin_b2, double *res)
{
//	double a1 = DEG2RAD*lat1;
//	double b1 = DEG2RAD*lon1;
//  double a2 = DEG2RAD*lat2;
//  double b2 = DEG2RAD*lon2;

//	double cos_a1 = cos(PI/2.0-a1);
//	double sin_a1 = sin(PI/2.0-a1);
//	double cos_a2 = cos(PI/2.0-a2);
//	double sin_a2 = sin(PI/2.0-a2);

//	double cos_b1 = cos(b1);
//	double sin_b1 = sin(b1);
//	double cos_b2 = cos(b2);
//	double sin_b2 = sin(b2);

	double Z1Y1[9] = {cos_a1*cos_b1, -sin_b1, cos_b1*sin_a1, cos_a1*sin_b1, cos_b1, sin_a1*sin_b1, -sin_a1, 0, cos_a1};
	double Z2Y2t[9] = {-cos_a2*cos_b2, -cos_a2*sin_b2, sin_a2, -sin_b2, cos_b2, 0, cos_b2*sin_a2, sin_a2*sin_b2, cos_a2};
	double R[9];

	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 3, 3, 3, 1.0, Z1Y1, 3, Z2Y2t, 3, 0.0, R, 3);

	R[0] = -R[0];
	R[3] = -R[3];
	R[6] = -R[6];

  cblas_dgemv(CblasRowMajor, CblasNoTrans, 3, 3, 1.0, R, 3, vect, 1, 0.0, res, 1);


	return;
}

void from_loc_sphr_to_cart(double* columnvect_xyzloc, double colatitude, double longitude, double* columnvect_res)
{
/*IMPORTANT: this subroutine is in the coordinate system NED*/

    double phi = colatitude*DEG2RAD;
    double lambda = longitude*DEG2RAD;

    double cos_phi = cos(phi);
    double sin_phi = sin(phi);
    double cos_lambda = cos(lambda);
    double sin_lambda = sin(lambda);

    double columnvect_phi_unit[3] = {-sin_phi*cos_lambda, -sin_phi*sin_lambda, cos_phi};
    double columnvect_lambda_unit[3] = {-sin_lambda, cos_lambda, 0};
    double columnvect_r_unit[3] = {cos_phi*cos_lambda, cos_phi*sin_lambda, sin_phi};

    columnvect_res[0] = columnvect_phi_unit[0]*columnvect_xyzloc[0]+columnvect_lambda_unit[0]*columnvect_xyzloc[1]+columnvect_r_unit[0]*columnvect_xyzloc[2];
    columnvect_res[1] = columnvect_phi_unit[1]*columnvect_xyzloc[0]+columnvect_lambda_unit[1]*columnvect_xyzloc[1]+columnvect_r_unit[1]*columnvect_xyzloc[2];
    columnvect_res[2] = columnvect_phi_unit[2]*columnvect_xyzloc[0]+columnvect_lambda_unit[2]*columnvect_xyzloc[1]+columnvect_r_unit[2]*columnvect_xyzloc[2];

    return;
}

void from_cart_to_loc_sphr(double* columnvect_xyzglob, double colatitude, double longitude, double* columnvect_res)
{
/*IMPORTANT: this subroutine is in the coordinate system NED*/

    double phi = colatitude*DEG2RAD;
    double lambda = longitude*DEG2RAD;

    double cos_phi = cos(phi);
    double sin_phi = sin(phi);
    double cos_lambda = cos(lambda);
    double sin_lambda = sin(lambda);

    double rowvect_phi_unit[3] = {-sin_phi*cos_lambda, -sin_phi*sin_lambda, cos_phi};
    double rowvect_lambda_unit[3] = {-sin_lambda, cos_lambda, 0};
    double rowvect_r_unit[3] = {cos_phi*cos_lambda, cos_phi*sin_lambda, sin_phi};

    columnvect_res[0] = rowvect_phi_unit[0]*columnvect_xyzglob[0]+rowvect_phi_unit[1]*columnvect_xyzglob[1]+rowvect_phi_unit[2]*columnvect_xyzglob[2];
    columnvect_res[1] = rowvect_lambda_unit[0]*columnvect_xyzglob[0]+rowvect_lambda_unit[1]*columnvect_xyzglob[1]+rowvect_lambda_unit[2]*columnvect_xyzglob[2];
    columnvect_res[2] = rowvect_r_unit[0]*columnvect_xyzglob[0]+rowvect_r_unit[1]*columnvect_xyzglob[1]+rowvect_r_unit[2]*columnvect_xyzglob[2];

    return;
}

void from_loc_sphr_to_loc_sphr(double* columnvect_xyzloc, double colatitude1, double longitude1, double colatitude2, double longitude2, double* columnvect_res)
{
    double columnvect_xyzglob[3];
    from_loc_sphr_to_cart(columnvect_xyzloc, colatitude1, longitude1, columnvect_xyzglob);
    from_cart_to_loc_sphr(columnvect_xyzglob, colatitude2, longitude2, columnvect_res);
    return;

}
