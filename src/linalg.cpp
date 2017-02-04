#include "linalg.h"
#include "constants.h"
#include <math.h>

void conv_vect(double *vect, double lon1, double lat1, double lon2, double lat2, double *res)
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

	double Z1[9] = {cos_b1, -sin_b1, 0.0, sin_b1, cos_b1, 0.0, 0.0, 0.0, 1.0};
	double Y1[9] = {cos_a1, 0.0, sin_a1, 0.0, 1.0, 0.0, -sin_a1, 0.0, cos_a1};

	double Z2[9] = {cos_b2, -sin_b2, 0.0, sin_b2, cos_b2, 0.0, 0.0, 0.0, 1.0};
	double Y2[9] = {cos_a2, 0.0, sin_a2, 0.0, 1.0, 0.0, -sin_a2, 0.0, cos_a2};

	double Z1Y1[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double Z2Y2t[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	for (int i = 0; i != 3; ++i) 
	{
 		for (int j = 0; j != 3; ++j)
		{
			double sum1 = 0;
			double sum2 = 0;
			for (int k = 0; k != 3; ++k)
			{
				sum1 += Z1[3*i+k] * Y1[3*k+j];
				sum2 += Z2[3*i+k] * Y2[3*k+j];
			}
			Z1Y1[3*i+j] = sum1;
			Z2Y2t[3*j+i] = sum2;
		}
	}

	double E[9] = {-1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

	
	double EZ2Y2t[9];
	for (int i = 0; i != 3; ++i) 
	{
 		for (int j = 0; j != 3; ++j)
		{
			double sum = 0;
			for (int k = 0; k != 3; ++k)
			{
				sum += E[3*i+k] * Z2Y2t[3*k+j];
			}
			EZ2Y2t[3*i+j] = sum;
		}
	}
	

	double EZ2Y2tZ1Y1[9];
	for (int i = 0; i != 3; ++i) 
	{
 		for (int j = 0; j != 3; ++j)
		{
			double sum = 0;
			for (int k = 0; k != 3; ++k)
			{
				sum += EZ2Y2t[3*i+k] * Z1Y1[3*k+j];
			}
			EZ2Y2tZ1Y1[3*i+j] = sum;
		}
	}

	double EZ2Y2tZ1Y1E[9];
	for (int i = 0; i != 3; ++i) 
	{
 		for (int j = 0; j != 3; ++j)
		{
			double sum = 0;
			for (int k = 0; k != 3; ++k)
			{
				sum += EZ2Y2tZ1Y1[3*i+k] * E[3*k+j];
			}
			EZ2Y2tZ1Y1E[3*i+j] = sum;
		}
	}

	
	for (int i = 0; i != 3; ++i) 
	{
		res[i] = 0;
 		for (int j = 0; j != 3; ++j)
		{
			res[i] = res[i] + EZ2Y2tZ1Y1E[3*i+j] * vect[j];
		}
	}
	
	return;
}

void conv_vect_fast(double *vect, double lon1, double lat1, double lon2, double lat2, double *res)
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
	for (int i = 0; i != 3; ++i) 
	{
 		for (int j = 0; j != 3; ++j)
		{
			double sum = 0;
			for (int k = 0; k != 3; ++k)
			{
				sum += Z2Y2t[3*i+k] * Z1Y1[3*k+j];
			}
			R[3*i+j] = sum;
		}
	}

	R[0] = -R[0];
	R[3] = -R[3];
	R[6] = -R[6];

	for (int i = 0; i != 3; ++i) 
	{
		res[i] = 0;
 		for (int j = 0; j != 3; ++j)
		{
			res[i] += R[3*i+j] * vect[j];
		}
	}
	
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

