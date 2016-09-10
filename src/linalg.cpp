#include "linalg.h"
#include "constants.h"
#include <math.h>

void conv_vect(double *vect, double lon1, double lat1, double lon2, double lat2, double *res)
{
	double d2r = PI/180.;
	double a1 = d2r*lat1;
	double b1 = d2r*lon1;
    double a2 = d2r*lat2;
    double b2 = d2r*lon2;

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
	double d2r = PI/180.;
	double a1 = d2r*lat1;
	double b1 = d2r*lon1;
    double a2 = d2r*lat2;
    double b2 = d2r*lon2;

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