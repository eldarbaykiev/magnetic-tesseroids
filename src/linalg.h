#ifndef _LINALG_H_
#define _LINALG_H_

void conv_vect_cblas(double *vect, double lon1, double lat1, double lon2, double lat2, double *res);

void from_loc_sphr_to_cart(double* columnvect_xyzloc, double colatitude, double longitude, double* columnvect_res);
void from_cart_to_loc_sphr(double* columnvect_xyzglob, double colatitude, double longitude, double* columnvect_res);
void from_loc_sphr_to_loc_sphr(double* columnvect_xyzloc, double colatitude1, double longitude1, double colatitude2, double longitude2, double* columnvect_res);

#endif
