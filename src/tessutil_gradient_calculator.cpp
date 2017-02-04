#include <stdio.h>
#include <stdlib.h>            
#include <string.h>
#include <math.h> 

#include "constants.h" 
#include "linalg.h"

#define MAX_GRID_POINTS 5000

#define GRID_FORMAT "%lf %lf %f %lf"




// TODO conversion of input/output units nT/km pT/km nT/m pT/m


void save_to_file(char* filename, double* longitudes, double* latitudes, double* values, int n_values)
{
    FILE * fp = fopen(filename, "w");
    if (fp == NULL)
    {

        printf("ERROR: Can not open file to save calculated values.\n");
        return;
    }
  
  

    for (int h = 0; h < n_values; h++)
        fprintf(fp, "%lf %lf %lf\n", longitudes[h], latitudes[h], values[h]);

    fclose(fp);
    return;
}

void save_all_to_file(char* filename, double* longitudes, double* latitudes, int n_values,  double* values1,  double* values2,  double* values3,  double* values4,  double* values5,  double* values6,  double* values7)
{
    FILE * fp = fopen(filename, "w");
    if (fp == NULL)
    {

        printf("ERROR: Can not open file to save calculated values.\n");
        return;
    }
  
  

    for (int h = 0; h < n_values; h++)
        fprintf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", longitudes[h], latitudes[h], values1[h], values2[h], values3[h], values4[h], values5[h], values6[h], values7[h]);

    fclose(fp);
    return;
}




int main(int argc, char**argv)
{
    int gridbx_set = 0;
    int gridby_set = 0;
    int gridbz_set = 0;
    int bz_NEU_NED = -1;
    int bz_NEU_NED_set = 0;

    int output_file_set = 0;

    char* gridbx_fn;
    char* gridby_fn;
    char* gridbz_fn;

    char* out_fn;

    char *params;

    int bad_args = 0, parsed_args = 0, total_args = 1;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'h':
                    if(argv[i][2] != '\0')
                    {
                        //log_error("invalid argument '%s'", argv[i]);
			printf("invalid argument '%s'\n", argv[i]); 																																	
                        bad_args++;
                        break;
   		    }
		    printf("here should be help\n");
                    return 2;
		case 'b':
                    params = &argv[i][2];
                    if(strlen(params) <= 1)
                    {
                        printf("bad input argument -b. Missing component and filename\n");
                        bad_args++;
                    }
                    else
                    {
                        switch(argv[i][2])
			{
			    case 'x':
				if(gridbx_set)
				{
				    printf("invalid argument '%s', gridfile bx already set\n", argv[i]);
				    bad_args++;
                                    break;   
				}
				else
				{
				    gridbx_set = 1;
				   
				    gridbx_fn = &argv[i][3];
				}
                                break;
			    case 'y':
				if(gridby_set)
				{
				    printf("invalid argument '%s', gridfile by already set\n", argv[i]);
				    bad_args++;
                                    break;   
				}
				else
				{
				    gridby_set = 1;
				    gridby_fn = &argv[i][3];
				}
                                break;
			    case 'z':
				if(gridbz_set)
				{
				    printf("invalid argument '%s', gridfile by already set\n", argv[i]);
				    bad_args++;
                                    break;   
				}
				else
				{
				    gridbz_set = 1;
				    gridbz_fn = &argv[i][3];
				}
                                break;
			    default:
				printf("invalid argument '%s', wrong component\n", argv[i]);
                    		bad_args++;
                    		break;
			}
                    }
                    break;
 		case 'c':
		    params = &argv[i][2];

		    if(bz_NEU_NED_set)
		    {
			printf("invalid argument '%s', coordinate system is already set\n", argv[i]);
			bad_args++;
                        break;   
		    }
                    if(strlen(params) > 1)
                    {
                        printf("invalid argument '%s', specify coordinate system in the input grids\n", argv[i]);
                        bad_args++;
			break;
                    }
		    if(argv[i][2] == '1')
                    {
                        bz_NEU_NED_set = 1;
			bz_NEU_NED = 1;
                        break;
                    }
                    else if(argv[i][2] == '2')
                    {
                        bz_NEU_NED_set = 1;
			bz_NEU_NED = -1;
                        break;
                    }
		    else
                    {
                        printf("invalid argument '%s', there are only NED (1) and NEU (2, default) coordinate systems\n", argv[i]);
                        bad_args++;
			break;
                    }
                    break;
		case 'o':
		    params = &argv[i][2];

                    
		    if( output_file_set)
		    {
			printf("invalid argument '%s', output filename is already set\n", argv[i]);
			bad_args++;
                        break;   
		    }

		    
                    if(strlen(params) == 0)
                    {
                        printf("invalid argument '%s', specify output filename\n", argv[i]);
                        bad_args++;
			break;
                    }
                    out_fn = params;
		    output_file_set = 1;
		    break;
              
		default:
		    printf("invalid argument '%s'\n", argv[i]);
                    bad_args++;
	            break;
	    }
	}
    }
    
    

    if ((gridbx_set == 0) || (gridbx_set == 0) || (gridbx_set == 0))
    {
	printf("no input grids\n");
	exit(EXIT_FAILURE);
    }

    printf("bz_NEU_NED: %d\n", bz_NEU_NED);

    //double columnvect[3] = {1, 1, 2};
    //double columnvectnew[3];
    //from_loc_sphr_to_loc_sphr(columnvect, 14, 12, 13, 50, columnvectnew);
    //printf("%f %f %f\n", columnvectnew[0], columnvectnew[1], columnvectnew[2]);

    //
    double lons[MAX_GRID_POINTS];
    double lats[MAX_GRID_POINTS];
    float alts[MAX_GRID_POINTS];

    double bx[MAX_GRID_POINTS];
    double by[MAX_GRID_POINTS];
    double bz[MAX_GRID_POINTS];
    //read file with bx

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int n_lines = 0;


    FILE * bxfp = fopen(gridbx_fn, "r");
    if (bxfp == NULL)
    {
        printf("ERROR: Can not open file with Bx values.\n");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, bxfp )) != -1)
    {

	if ((line[0] != '#') && (strlen(line) > 2))
        {
            n_lines++;
	    if (n_lines>MAX_GRID_POINTS)
            {
		printf("ERROR: Too many grid points (> %d) in the input. Recompile program with a bigger value of MAX_GRID_POINTS.\n", n_lines);
		exit(EXIT_FAILURE);
	    }
	    

	    //printf("%s", line);
	    sscanf(line, GRID_FORMAT, &lons[n_lines-1], &lats[n_lines-1], &alts[n_lines-1], &bx[n_lines-1]);

        }


    }
    fclose(bxfp);

    printf("\n\n");

    /*number of grid points*/
    printf("Number of grid points: %d\n", n_lines);

    

    /*grid spacing*/

    double lon_min = lons[0];
    double lon_max = lons[n_lines-1];

    double lon_step = lons[1]-lons[0];
    if ((lon_step == 0) || (lon_step <= 0) || (lon_max < lon_min))
    {
	printf("ERROR: Wrong grid format. Longitudes must increment first. Use the format of tessgrd.\n");
	exit(EXIT_FAILURE);
    }

    int lon_n = 1;
    while ((lats[lon_n] == lats[0]) && (lon_n < n_lines))
    {
	lon_n++;
    }
    double lat_step = lats[lon_n]-lats[0];

    int lat_n = n_lines / lon_n;

    double lat_min = lats[0];
    double lat_max = lats[n_lines-1];



    if ((lat_step == 0) || (lat_step <= 0) || (lat_max < lat_min))
    {
	printf("ERROR: Wrong grid format. Latitudes must increment. Use the format of tessgrd.\n");
	exit(EXIT_FAILURE);
    }

    printf("Longitudinal step: %lf, latitudinal step: %lf \n", lon_step, lat_step);
    printf("Longitudinal points: %d, latitudinal points: %d \n", lon_n, lat_n);
    printf("Edges: W %lf, E %lf, S %lf, N %lf \n", lon_min, lon_max, lat_min, lat_max);

    /* read other grids */
    // By
    FILE * byfp = fopen(gridby_fn, "r");
    if (byfp == NULL)
    {
        printf("ERROR: Can not open file with Bx values.\n");
        exit(EXIT_FAILURE);
    }
    
    int n_lines2 = 0;
    while ((read = getline(&line, &len, byfp )) != -1)
    {

	if ((line[0] != '#') && (strlen(line) > 2))
        {
            n_lines2++;
	    //printf("%s", line);
            double dummy1, dummy2; 
	    float dummy3;
	    sscanf(line, GRID_FORMAT , &dummy1, &dummy2, &dummy3, &by[n_lines2-1]);

        }


    }
    fclose(byfp);

    if (n_lines2 != n_lines)
    {
        printf("ERROR: Grid points of Bx and By do not coincide.\n");
        exit(EXIT_FAILURE);
    }

    // Bz
    FILE * bzfp = fopen(gridbz_fn, "r");
    if (bzfp == NULL)
    {
        printf("ERROR: Can not open file with Bx values.\n");
        exit(EXIT_FAILURE);
    }
    
    n_lines2 = 0;
    while ((read = getline(&line, &len, bzfp )) != -1)
    {

	if ((line[0] != '#') && (strlen(line) > 2))
        {
            n_lines2++;
	    //printf("%s", line);
            double dummy1, dummy2; 
	    float dummy3;
	    double bz_curr;
	    sscanf(line, GRID_FORMAT , &dummy1, &dummy2, &dummy3,&bz_curr);	

	    bz[n_lines2-1] = bz_NEU_NED* bz_curr; //COORDINATE SYSTEM NEU or NED
	    

        }


    }
    fclose(byfp);

    if (n_lines2 != n_lines)
    {
        printf("ERROR: Grid points of Bx and Bz do not coincide.\n");
        exit(EXIT_FAILURE);
    }
    
    //int i = 2;
    //int j = 1;
    //printf("lon: %f i: %d lats: %f j: %d val: %f\n", lons[j*lon_n+i], i, lats[j*lon_n+i], j, bx[j*lon_n+i]);

    /* calculate gradients */


    double bxx[MAX_GRID_POINTS];
    double byx[MAX_GRID_POINTS];
    double bzx[MAX_GRID_POINTS];

    double bxy[MAX_GRID_POINTS];
    double byy[MAX_GRID_POINTS];
    double bzy[MAX_GRID_POINTS];

    double bzz[MAX_GRID_POINTS];


    for (int i = 1; i<lon_n-1; i++)
    {
	for (int j = 1; j<lat_n-1; j++)
	{
            int cent_ind 	= (j)	*lon_n	+	(i);
	    int west_ind 	= (j)	*lon_n	+	(i-1);
            int east_ind 	= (j)	*lon_n	+	(i+1);
	    int south_ind 	= (j-1)	*lon_n	+	(i);
	    int north_ind 	= (j+1)	*lon_n	+	(i);

	    double vect_cent[3] = {bx[cent_ind], by[cent_ind], bz[cent_ind]};
	    double vect_west[3] = {bx[west_ind], by[west_ind], bz[west_ind]};
	    double vect_east[3] = {bx[east_ind], by[east_ind], bz[east_ind]};
	    double vect_south[3] = {bx[south_ind], by[south_ind], bz[south_ind]};
	    double vect_north[3] = {bx[north_ind], by[north_ind], bz[north_ind]};

	    double vect_c_west[3];
	    double vect_c_east[3];
	    double vect_c_south[3];
	    double vect_c_north[3];

	    from_loc_sphr_to_loc_sphr(vect_west, lats[west_ind], lons[west_ind], lats[cent_ind], lons[cent_ind], vect_c_west);
	    from_loc_sphr_to_loc_sphr(vect_east, lats[east_ind], lons[east_ind], lats[cent_ind], lons[cent_ind], vect_c_east);
	    from_loc_sphr_to_loc_sphr(vect_south, lats[south_ind], lons[south_ind], lats[cent_ind], lons[cent_ind], vect_c_south);
	    from_loc_sphr_to_loc_sphr(vect_north, lats[north_ind], lons[north_ind], lats[cent_ind], lons[cent_ind], vect_c_north);

	    double cent_ang_sn = acos(sin(lats[south_ind]*DEG2RAD)*sin(lats[north_ind]*DEG2RAD) + cos(lats[south_ind]*DEG2RAD)*cos(lats[north_ind]*DEG2RAD)*cos(abs(lons[south_ind]-lons[north_ind])*DEG2RAD));
	    double cent_ang_we = acos(sin(lats[west_ind]*DEG2RAD)*sin(lats[east_ind]*DEG2RAD) + cos(lats[west_ind]*DEG2RAD)*cos(lats[east_ind]*DEG2RAD)*cos(abs(lons[west_ind]-lons[east_ind])*DEG2RAD));

	    double dist_sn = (MEAN_EARTH_RADIUS + alts[cent_ind])*cent_ang_sn;
	    double dist_we = (MEAN_EARTH_RADIUS + alts[cent_ind])*cent_ang_we;

            bxx[cent_ind] = -(vect_c_south[0]-vect_c_north[0])/dist_sn*1000.0;
	    byx[cent_ind] = -(vect_c_south[1]-vect_c_north[1])/dist_sn*1000.0;
	    bzx[cent_ind] = -(vect_c_south[2]-vect_c_north[2])/dist_sn*1000.0;

	    //printf("%lf %lf %lf \n", bxy[cent_ind], byy[cent_ind], bzy[cent_ind]);

	    bxy[cent_ind] = -(vect_c_west[0]-vect_c_east[0])/dist_we*1000.0;
	    byy[cent_ind] = -(vect_c_west[1]-vect_c_east[1])/dist_we*1000.0;
	    bzy[cent_ind] = -(vect_c_west[2]-vect_c_east[2])/dist_we*1000.0;

	    bzz[cent_ind] = -bxx[cent_ind] - byy[cent_ind];

	    //printf("%lf %lf %lf %lf %lf %lf %lf\n", bxx[cent_ind], byx[cent_ind], bzx[cent_ind], bxy[cent_ind], byy[cent_ind], bzy[cent_ind], bzz[cent_ind]);


	}
    }

    if(output_file_set)
	save_all_to_file(out_fn, lons, lats, n_lines, bxx, byx, bzx, bxy, byy, bzy, bzz);
    else
    {

        save_to_file("Bxx.txt", lons, lats, bxx, n_lines);
        save_to_file("Byx.txt", lons, lats, byx, n_lines);
        save_to_file("Bzx.txt", lons, lats, bzx, n_lines);

        save_to_file("Bxy.txt", lons, lats, bxy, n_lines);
        save_to_file("Byy.txt", lons, lats, byy, n_lines);
        save_to_file("Bzy.txt", lons, lats, bzy, n_lines);

        save_to_file("Bzz.txt", lons, lats, bzz, n_lines);
    }
    
}


