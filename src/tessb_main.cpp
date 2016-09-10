/*
Generic main function for the tessb* programs.
*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "logger.h"
#include "version.h"
#include "grav_tess.h"
#include "glq.h"
#include "constants.h"
#include "geometry.h"
#include "parsers.h"
#include "tessb_main.h"
#include "linalg.h"


/* Print the help message for tessh* programs */
void print_tessb_help(const char *progname)
{
	printf("MAGNETIC TESSEROIDS\n");
    printf("Usage: %s MODELFILE [OPTIONS]\n\n", progname);
    if(strcmp(progname + 4, "pot") == 0)
    {
        printf("Calculate the potential due to a tesseroid model on\n");
    }
    else
    {
        printf("Calculate the %s component due to a tesseroid model on\n",
               progname + 4);
    }
    printf("TODO\n");
}


/* Run the main for a generic tessh* program */
int run_tessb_main(int argc, char **argv, const char *progname,
    double (*field)(TESSEROID, double, double, double, GLQ, GLQ, GLQ),
    double ratio1, double ratio2, double ratio3)
{
    TESSB_ARGS args;
    GLQ *glq_lon, *glq_lat, *glq_r;
    TESSEROID *model;
	
    int modelsize, rc, line, points = 0, error_exit = 0, bad_input = 0;
    char buff[10000];
	
    double lon, lat, height, res;
    FILE *logfile = NULL, *modelfile = NULL;
    time_t rawtime;
    clock_t tstart;
    struct tm * timeinfo;

	double (*field1)(TESSEROID, double, double, double, GLQ, GLQ, GLQ);
	double (*field2)(TESSEROID, double, double, double, GLQ, GLQ, GLQ);
	double (*field3)(TESSEROID, double, double, double, GLQ, GLQ, GLQ);
	double ggt_1, ggt_2, ggt_3;
	int n_tesseroid;


    log_init(LOG_INFO);

    rc = parse_tessb_args(argc, argv, progname, &args, &print_tessb_help);
    if(rc == 2)
    {
        return 0;
    }
    if(rc == 1)
    {
        log_warning("Terminating due to bad input");
        log_warning("Try '%s -h' for instructions", progname);

        return 1;
    }

    /* Set the appropriate logging level and log to file if necessary */
    if(!args.verbose)
    {
        log_init(LOG_WARNING);
    }
    if(args.logtofile)
    {
        logfile = fopen(args.logfname, "w");
        if(logfile == NULL)
        {
            log_error("unable to create log file %s", args.logfname);
            log_warning("Terminating due to bad input");
            log_warning("Try '%s -h' for instructions", progname);
            return 1;
        }
        log_tofile(logfile, LOG_DEBUG);
    }

    /* Check if a custom distance-size ratio is given */
    if(args.ratio1 != 0)
    {
        ratio1 = args.ratio1;
    }
	if(args.ratio2 != 0)
    {
        ratio2 = args.ratio2;
    }
	if(args.ratio3 != 0)
    {
        ratio3 = args.ratio3;
    }

    /* Print standard verbose */
    log_info("%s (Tesseroids project) %s", progname, tesseroids_version);
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    log_info("(local time) %s", asctime(timeinfo));
    log_info("Use recursive division of tesseroids: %s",
             args.adaptative ? "True" : "False");
    log_info("Distance-size ratio1 for recusive division: %g", ratio1);
	log_info("Distance-size ratio2 for recusive division: %g", ratio2);
	log_info("Distance-size ratio3 for recusive division: %g", ratio3);

    /* Make the necessary GLQ structures */
    log_info("Using GLQ orders: %d lon / %d lat / %d r", args.lon_order,
             args.lat_order, args.r_order);
    glq_lon = glq_new(args.lon_order, -1, 1);
    glq_lat = glq_new(args.lat_order, -1, 1);
    glq_r = glq_new(args.r_order, -1, 1);
    if(glq_lon == NULL || glq_lat == NULL || glq_r == NULL)
    {
        log_error("failed to create required GLQ structures");
        log_warning("Terminating due to bad input");
        log_warning("Try '%s -h' for instructions", progname);
        if(args.logtofile)
            fclose(logfile);
        return 1;
    }

    /* Read the tesseroid model file */
    log_info("Reading magnetic tesseroid model from file %s", args.modelfname);
    modelfile = fopen(args.modelfname, "r");
    if(modelfile == NULL)
    {
        log_error("failed to open model file %s", args.modelfname);
        log_warning("Terminating due to bad input");
        log_warning("Try '%s -h' for instructions", progname);
        if(args.logtofile)
            fclose(logfile);
        return 1;
    }
    model = read_mag_tess_model(modelfile, &modelsize);
    fclose(modelfile);
    if(modelsize == 0)
    {
        log_error("tesseroid file %s is empty", args.modelfname);
        log_warning("Terminating due to bad input");
        log_warning("Try '%s -h' for instructions", progname);
        if(args.logtofile)
            fclose(logfile);
        return 1;
    }
    if(model == NULL)
    {
        log_error("failed to read model from file %s", args.modelfname);
        log_warning("Terminating due to bad input");
        log_warning("Try '%s -h' for instructions", progname);
        if(args.logtofile)
            fclose(logfile);
        return 1;
    }
    log_info("Total of %d tesseroid(s) read", modelsize);

    /* Print a header on the output with provenance information */
    if(strcmp(progname + 4, "pot") == 0)
    {
        printf("# Potential calculated with %s %s:\n", progname,
               tesseroids_version);
    }
    else
    {
        printf("# %s component calculated with %s %s:\n", progname+4, progname,
               tesseroids_version);
    }
    printf("#   local time: %s", asctime(timeinfo));
    printf("#   model file: %s (%d tesseroids)\n", args.modelfname, modelsize);
    printf("#   GLQ order: %d lon / %d lat / %d r\n", args.lon_order,
           args.lat_order, args.r_order);
    printf("#   Use recursive division of tesseroids: %s\n",
           args.adaptative ? "True" : "False");
    printf("#   Distance-size ratio1 for recusive division: %g\n", ratio1);
	printf("#   Distance-size ratio2 for recusive division: %g\n", ratio2);
	printf("#   Distance-size ratio3 for recusive division: %g\n", ratio3);

	/////////////ELDAR BAYKIEV//////////////
	if (!strcmp("tessbx", progname))
	{
		field1 = &tess_gxx;
		field2 = &tess_gxy;
		field3 = &tess_gxz;
	}

	if (!strcmp("tessby", progname))
	{
		field1 = &tess_gxy;
		field2 = &tess_gyy;
		field3 = &tess_gyz;
	}

	if (!strcmp("tessbz", progname))
	{
		field1 = &tess_gxz;
		field2 = &tess_gyz;
		field3 = &tess_gzz;
	}
	/////////////ELDAR BAYKIEV//////////////

    /* Read each computation point from stdin and calculate */
    log_info("Calculating (this may take a while)...");
    tstart = clock();



	////////////////

	/*for(line = 1; !feof(stdin); line++)
    {
		if(fgets(filebuff[line], 10000, stdin) == NULL)
        {
            if(ferror(stdin))
            {
                log_error("problem encountered reading line %d", line);
                error_exit = 1;
                break;
            }
        }


	}*/


	//MAKE PRELOADED FILE
	///////////////
    for(line = 1; !feof(stdin); line++)
    {
        if(fgets(buff, 10000, stdin) == NULL)
        {
            if(ferror(stdin))
            {
                log_error("problem encountered reading line %d", line);
                error_exit = 1;
                break;
            }
        }
        else
        {
            /* Check for comments and blank lines */
            if(buff[0] == '#' || buff[0] == '\r' || buff[0] == '\n')
            {
                printf("%s", buff);
                continue;
            }
            if(sscanf(buff, "%lf %lf %lf", &lon, &lat, &height) != 3)
            {
                log_warning("bad/invalid computation point at line %d", line);
                log_warning("skipping this line and continuing");
                bad_input++;
                continue;
            }
            /* Need to remove \n and \r from end of buff first to print the
               result in the end */
            strstrip(buff);

			/////////////ELDAR BAYKIEV//////////////
			res = 0;			
            if(args.adaptative)
            {
				for(n_tesseroid = 0; n_tesseroid < modelsize; n_tesseroid++)
				{
					double B_to_H = model[n_tesseroid].suscept/(M_0);//IMPORTANT
					double M_vect[3] = {model[n_tesseroid].Bx * B_to_H, model[n_tesseroid].By * B_to_H, model[n_tesseroid].Bz * B_to_H};
					double M_vect_p[3] = {0, 0, 0};

					conv_vect_fast(M_vect, (model[n_tesseroid].w + model[n_tesseroid].e)*0.5, (model[n_tesseroid].s + model[n_tesseroid].n)*0.5, lon, lat, M_vect_p);
					
					ggt_1 = calc_tess_model_adapt(&model[n_tesseroid], 1, lon, lat, height + MEAN_EARTH_RADIUS, glq_lon, glq_lat, glq_r, field1, ratio1);
					ggt_2 = calc_tess_model_adapt(&model[n_tesseroid], 1, lon, lat, height + MEAN_EARTH_RADIUS, glq_lon, glq_lat, glq_r, field2, ratio2);
					ggt_3 = calc_tess_model_adapt(&model[n_tesseroid], 1, lon, lat, height + MEAN_EARTH_RADIUS, glq_lon, glq_lat, glq_r, field3, ratio3);

					res = res + M_0*EOTVOS2SI*(ggt_1 * M_vect_p[0]  + ggt_2 * M_vect_p[1] + ggt_3 * M_vect_p[2]) /(G*model[n_tesseroid].density*4*PI);

					//printf("res %g\n", res);
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            else
            {	
				/////////////////////////////////////////////////////////////////////////////////////////////////////////
				for(n_tesseroid = 0; n_tesseroid < modelsize; n_tesseroid++)
				{
					double B_to_H = model[n_tesseroid].suscept/(M_0);//IMPORTANT
					double M_vect[3] = {model[n_tesseroid].Bx * B_to_H, model[n_tesseroid].By * B_to_H, model[n_tesseroid].Bz * B_to_H};
					double M_vect_p[3] = {0, 0, 0};

					conv_vect_fast(M_vect, (model[n_tesseroid].w + model[n_tesseroid].e)*0.5, (model[n_tesseroid].s + model[n_tesseroid].n)*0.5, lon, lat, M_vect_p);
					
					ggt_1 = calc_tess_model(&model[n_tesseroid], 1, lon, lat, height + MEAN_EARTH_RADIUS, glq_lon, glq_lat, glq_r, field1);
					ggt_2 = calc_tess_model(&model[n_tesseroid], 1, lon, lat, height + MEAN_EARTH_RADIUS, glq_lon, glq_lat, glq_r, field2);
					ggt_3 = calc_tess_model(&model[n_tesseroid], 1, lon, lat, height + MEAN_EARTH_RADIUS, glq_lon, glq_lat, glq_r, field3);

					res = res + M_0*EOTVOS2SI*(ggt_1 * M_vect_p[0]  + ggt_2 * M_vect_p[1] + ggt_3 * M_vect_p[2]) /(G*model[n_tesseroid].density*4*PI);
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            printf("%s %.15g\n", buff, res);
            points++;
        }
    }
    if(bad_input)
    {
        log_warning("Encountered %d bad computation points which were skipped",
                    bad_input);
    }
    if(error_exit)
    {
        log_warning("Terminating due to error in input");
        log_warning("Try '%s -h' for instructions", progname);
    }
    else
    {
        log_info("Calculated on %d points in %.5g seconds", points,
                 (double)(clock() - tstart)/CLOCKS_PER_SEC);
    }
    /* Clean up */
    free(model);
    glq_free(glq_lon);
    glq_free(glq_lat);
    glq_free(glq_r);
    log_info("Done");
    if(args.logtofile)
        fclose(logfile);
    return 0;
}
