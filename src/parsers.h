/*
Input and output parsing tools.
*/


#ifndef _TESSEROIDS_PARSERS_H_
#define _TESSEROIDS_PARSERS_H_

/* Needed for definition of TESSEROID and PRISM */
#include "geometry.h"
/* Need for the definition of FILE */
#include <stdio.h>

/** Store basic input arguments and option flags */
typedef struct basic_args
{
	char *inputfname; /**< name of the input file */
	int verbose; /**< flag to indicate if verbose printing is enabled */
	int logtofile; /**< flag to indicate if logging to a file is enabled */
	char *logfname; /**< name of the log file */
} BASIC_ARGS;

typedef struct tessh_args
{
	int lon_order; /**< glq order in longitude integration */
	int lat_order; /**< glq order in latitude integration */
	int r_order; /**< glq order in radial integration */
	char *modelfname; /**< name of the file with the tesseroid model */
	int verbose; /**< flag to indicate if verbose printing is enabled */
	int logtofile; /**< flag to indicate if logging to a file is enabled */
	char *logfname; /**< name of the log file */
	int adaptative; /**< flat to indicate wether to use the adaptative size
                         of tesseroid algorithm */
	double ratio1; /**< distance-size ratio used for recusive division */
	double ratio2; /**< distance-size ratio used for recusive division */
	double ratio3; /**< distance-size ratio used for recusive division */
} TESSB_ARGS;


typedef struct gradcalc_args
{
  	int gridbx_set;
  	int gridby_set;
	int gridbz_set;

	char* gridbx_fn;
    char* gridby_fn;
    char* gridbz_fn;

	int out_set;


	int bz_NEU_NED;
	int bz_NEU_NED_set;

	int verbose; /**< flag to indicate if verbose printing is enabled */
	int logtofile; /**< flag to indicate if logging to a file is enabled */

	

        
} GRADCALC_ARGS;

int parse_basic_args(int argc, char **argv, const char *progname, BASIC_ARGS *args, void (*print_help)(void));
int parse_tessb_args(int argc, char **argv, const char *progname, TESSB_ARGS *args, void (*print_help)(const char *));

int parse_gradcalc_args(int argc, char **argv, const char *progname, GRADCALC_ARGS *args, void (*print_help)(const char *));

void strstrip(char *str);

int gets_mag_tess(const char *str, TESSEROID *tess);
TESSEROID * read_mag_tess_model(FILE *modelfile, int *size);


#endif
