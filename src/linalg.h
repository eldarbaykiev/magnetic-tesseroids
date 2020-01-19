#ifndef _LINALG_H_
#define _LINALG_H_

void conv_vect_cblas(double *vect, double lon1, double lat1, double lon2, double lat2, double *res);
void conv_vect_cblas_precalc(double *vect, double cos_a1, double sin_a1, double cos_b1, double sin_b1, double cos_a2, double sin_a2, double cos_b2, double sin_b2, double *res);

void from_loc_sphr_to_cart(double* columnvect_xyzloc, double colatitude, double longitude, double* columnvect_res);
void from_cart_to_loc_sphr(double* columnvect_xyzglob, double colatitude, double longitude, double* columnvect_res);
void from_loc_sphr_to_loc_sphr(double* columnvect_xyzloc, double colatitude1, double longitude1, double colatitude2, double longitude2, double* columnvect_res);

#endif
