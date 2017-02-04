/*
Input and output parsing tools.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "logger.h"
#include "version.h"
#include "parsers.h"
#include "constants.h"
#include "geometry.h"


/* Parse basic command line arguments for programs */
int parse_basic_args(int argc, char **argv, const char *progname,
                     BASIC_ARGS *args, void (*print_help)(void))
{
    int bad_args = 0, parsed_args = 0, total_args = 1, i;
    char *params;

    /* Default values for options */
    args->verbose = 0;
    args->logtofile = 0;
    /* Parse arguments */
    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'h':
                    if(argv[i][2] != '\0')
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                        break;
                    }
                    print_help();
                    return 2;
                case 'v':
                    if(argv[i][2] != '\0')
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                        break;
                    }
                    if(args->verbose)
                    {
                        log_error("repeated option -v");
                        bad_args++;
                        break;
                    }
                    args->verbose = 1;
                    break;
                case 'l':
                {
                    if(args->logtofile)
                    {
                        log_error("repeated option -l");
                        bad_args++;
                        break;
                    }
                    params = &argv[i][2];
                    if(strlen(params) == 0)
                    {
                        log_error("bad input argument -l. Missing filename.");
                        bad_args++;
                    }
                    else
                    {
                        args->logtofile = 1;
                        args->logfname = params;
                    }
                    break;
                }
                case '-':
                {
                    params = &argv[i][2];
                    if(strcmp(params, "version"))
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                    }
                    else
                    {
                        print_version(progname);
                        return 2;
                    }
                    break;
                }
                default:
                    log_error("invalid argument '%s'", argv[i]);
                    bad_args++;
                    break;
            }
        }
        else
        {
            if(parsed_args == 0)
            {
                args->inputfname = argv[i];
                parsed_args++;
            }
            else
            {
                log_error("invalid argument '%s'. Already given model file %s",
                          argv[i], args->inputfname);
                bad_args++;
            }
        }
    }
    /* Check if parsing went well */
    if(parsed_args > total_args)
    {
        log_error("%s: too many input arguments. given %d, max %d.",
                    progname, parsed_args, total_args);
    }
    if(bad_args > 0)
    {
        log_error("%d bad input argument(s)", bad_args);
        return 1;
    }
    if(parsed_args < total_args)
    {
        return 3;
    }
    return 0;
}



/* Parse command line arguments for tessh* programs */
int parse_tessb_args(int argc, char **argv, const char *progname,
                     TESSB_ARGS *args, void (*print_help)(const char *))
{
    int bad_args = 0, parsed_args = 0, total_args = 1,  parsed_order = 0,
        parsed_ratio1 = 0, parsed_ratio2 = 0, parsed_ratio3 = 0, i, nchar, nread;
    char *params;

    /* Default values for options */
    args->verbose = 0;
    args->logtofile = 0;
    args->lon_order = 2;
    args->lat_order = 2;
    args->r_order = 2;
    args->adaptative = 1;
    args->ratio1 = 0; /* zero means use the default for the program */
	args->ratio2 = 0;
	args->ratio3 = 0;
    /* Parse arguments */
    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'h':
                    if(argv[i][2] != '\0')
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                        break;
                    }
                    print_help(progname);
                    return 2;
                case 'v':
                    if(argv[i][2] != '\0')
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                        break;
                    }
                    if(args->verbose)
                    {
                        log_error("repeated option -v");
                        bad_args++;
                        break;
                    }
                    args->verbose = 1;
                    break;
                case 'l':
                {
                    if(args->logtofile)
                    {
                        log_error("repeated option -l");
                        bad_args++;
                        break;
                    }
                    params = &argv[i][2];
                    if(strlen(params) == 0)
                    {
                        log_error("bad input argument -l. Missing filename.");
                        bad_args++;
                    }
                    else
                    {
                        args->logtofile = 1;
                        args->logfname = params;
                    }
                    break;
                }
                case '-':
                {
                    params = &argv[i][2];
                    if(strcmp(params, "version"))
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                    }
                    else
                    {
                        print_version(progname);
                        return 2;
                    }
                    break;
                }
                case 'a':
                    if(argv[i][2] != '\0')
                    {
                        log_error("invalid argument '%s'", argv[i]);
                        bad_args++;
                        break;
                    }
                    if(!args->adaptative)
                    {
                        log_error("repeated option -a");
                        bad_args++;
                        break;
                    }
                    args->adaptative = 0;
                    break;
                case 'o':
                {
                    if(parsed_order)
                    {
                        log_error("repeated option -o");
                        bad_args++;
                        break;
                    }
                    params = &argv[i][2];
                    nchar = 0;
                    nread = sscanf(params, "%d/%d/%d%n", &(args->lon_order),
                                &(args->lat_order), &(args->r_order), &nchar);
                    if(nread != 3 || *(params + nchar) != '\0')
                    {
                        log_error("bad input argument '%s'", argv[i]);
                        bad_args++;
                    }
                    parsed_order = 1;
                    break;
                }
                case 't':
                {
					//ELDAR BAYKIEV///////////////////////////////////////////////////////////////////
					switch(argv[i][2])
					{
						case '1':
						{
							if(parsed_ratio1)
							{
								log_error("repeated option -t1");
								bad_args++;
								break;
							}
							params = &argv[i][3];
							nchar = 0;
							nread = sscanf(params, "%lf%n", &(args->ratio1), &nchar);
							if(nread != 1 || *(params + nchar) != '\0')
							{
								log_error("bad input argument '%s'", argv[i]);
								bad_args++;
							}
							parsed_ratio1 = 1;
							break;
						}

						case '2':
						{
							if(parsed_ratio2)
							{
								log_error("repeated option -t2");
								bad_args++;
								break;
							}
							params = &argv[i][3];
							nchar = 0;
							nread = sscanf(params, "%lf%n", &(args->ratio2), &nchar);
							if(nread != 1 || *(params + nchar) != '\0')
							{
								log_error("bad input argument '%s'", argv[i]);
								bad_args++;
							}
							parsed_ratio2 = 1;
							break;
						}

						case '3':
						{
							if(parsed_ratio3)
							{
								log_error("repeated option -t3");
								bad_args++;
								break;
							}
							params = &argv[i][3];
							nchar = 0;
							nread = sscanf(params, "%lf%n", &(args->ratio3), &nchar);
							if(nread != 1 || *(params + nchar) != '\0')
							{
								log_error("bad input argument '%s'", argv[i]);
								bad_args++;
							}
							parsed_ratio3 = 1;
							break;
						}
					}
					//ELDAR BAYKIEV///////////////////////////////////////////////////////////////////        
                }
                default:
                    log_error("invalid argument '%s'", argv[i]);
                    bad_args++;
                    break;
            }
        }
        else
        {
            if(parsed_args == 0)
            {
                args->modelfname = argv[i];
                parsed_args++;
            }
            else
            {
                log_error("invalid argument '%s'. Already given model file %s",
                          argv[i], args->modelfname);
                bad_args++;
            }
        }
    }
    /* Check if parsing went well */
    if(bad_args > 0 || parsed_args != total_args)
    {
        if(parsed_args < total_args)
        {
            log_error("%s: missing input file.",
                      progname, parsed_args, total_args);
        }
        if(parsed_args > total_args)
        {
            log_error("%s: too many input arguments. given %d, max %d.",
                      progname, parsed_args, total_args);
        }
        if(bad_args > 0)
        {
            log_error("%d bad input argument(s)", bad_args);
        }
        return 1;
    }
    return 0;
}

//parse arguments for gradient calculator
int parse_gradcalc_args(int argc, char **argv, const char *progname, GRADCALC_ARGS *args, void (*print_help)(const char *))
{
	int bad_args = 0, parsed_args = 0, total_args = 5, i;
	char *params;

	 /* Default values for options */
	args->verbose = 0;
	args->logtofile = 0;

	args->gridbx_set = FALSE;
  	args->gridby_set = FALSE;
	args->gridbz_set = FALSE;
	args->out_set = -1;

	args->bz_NEU_NED = -1;
	args->bz_NEU_NED_set = FALSE;


	/* Parse arguments */
	for(i = 1; i < argc; i++)
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
					print_help(progname);
					return 2;
				case '-':
               				{
					params = &argv[i][2];
					if(strcmp(params, "version"))
					{
						printf("invalid argument '%s'\n", argv[i]);
						bad_args++;
					}
					else
					{
						print_version(progname);
						return 2;
					}
					break;
					}
				case 'b':
					params = &argv[i][2];
					if(strlen(params) <= 1)
					{
						printf("bad input argument -b. Missing component and filename\n");
						bad_args++;
						break;
                    			}
					else
					{
						switch(argv[i][2])
						{
							case 'x':
								if(args->gridbx_set)
								{
									printf("invalid argument '%s', gridfile bx already set\n", argv[i]);
									bad_args++;
									break;   
								}
								else
								{
				    					args->gridbx_set = 1; 
									args->gridbx_fn = &argv[i][3];
								}
                                				break;
			    				case 'y':
								if(args->gridby_set)
								{
									printf("invalid argument '%s', gridfile by already set\n", argv[i]);
									bad_args++;
									break;   
								}
								else
								{
									args->gridby_set = 1;
									args->gridby_fn = &argv[i][3];
								}
								break;
							case 'z':
								if(args->gridbz_set)
								{
									printf("invalid argument '%s', gridfile by already set\n", argv[i]);
									bad_args++;
									break;   
								}
								else
								{
									args->gridbz_set = 1;
									args->gridbz_fn = &argv[i][3];
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

					if(args->bz_NEU_NED_set)
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
						args->bz_NEU_NED_set = 1;
						args->bz_NEU_NED = 1;
						break;
					}
					else if(argv[i][2] == '2')
					{
						args->bz_NEU_NED_set = 1;
						args->bz_NEU_NED = -1;
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

					if(args->out_set>=0)
					{
						printf("invalid argument '%s', output format is already set\n", argv[i]);
						bad_args++;
						break;   
					}

					if(strlen(params) != 1)
					{
						printf("invalid argument '%s', specify output format\n", argv[i]);
						bad_args++;
						break;
					}
					//TODO Add check if it is integer
					args->out_set = atoi(params);
					break;
				default:
					printf("invalid argument '%s'\n", argv[i]);
					bad_args++;
					break;
			}
		}
	}

	if(parsed_args > total_args)
	{
		//log_error("%s: too many input arguments. given %d, max %d.", progname, parsed_args, total_args);
	}
	if(bad_args > 0)
	{
		//log_error("%d bad input argument(s)", bad_args);
		return 1;
	}
	if(parsed_args < total_args)
	{
		return 3;
	}
	return 0;
}

/* Strip trailing spaces and newlines from the end of a string */
void strstrip(char *str)
{
    int i;
    for(i = strlen(str) - 1; i >= 0; i--)
    {
        if(str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\0')
            break;
    }
    str[i + 1] = '\0';
}


/* Read a single tesseroid from a string */
int gets_mag_tess(const char *str, TESSEROID *tess)
{
    double w, e, s, n, top, bot, dens, suscept, Bx, By, Bz, Rx, Ry, Rz;
    int nread, nchars;

    nread = sscanf(str, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf%n", &w, &e, &s,
                    &n, &top, &bot, &dens, &suscept, &Bx, &By, &Bz, &nchars);
    if(nread != 11 || str[nchars] != '\0')
    {
        return 1;
    }
    tess->w = w;
    tess->e = e;
    tess->s = s;
    tess->n = n;
    tess->r1 = MEAN_EARTH_RADIUS + bot;
    tess->r2 = MEAN_EARTH_RADIUS + top;
    tess->density = dens;
	tess->suscept = suscept;
	tess->Bx = Bx;
	tess->By = By;
	tess->Bz = Bz;
	//tess->Rx = Rx;
	//tess->Ry = Ry;
	//tess->Rz = Rz;
    return 0;
}

//ELDAR BAYKIEV////////////////////////////////
TESSEROID * read_mag_tess_model(FILE *modelfile, int *size)
{
    TESSEROID *model, *tmp;
    int buffsize = 300, line, badinput = 0, error_exit = 0;
    char sbuff[10000];

    /* Start with a single buffer allocation and expand later if necessary */
    model = (TESSEROID *)malloc(buffsize*sizeof(TESSEROID));
    if(model == NULL)
    {
        log_error("problem allocating initial memory to load tesseroid model.");
        return NULL;
    }
    *size = 0;
    for(line = 1; !feof(modelfile); line++)
    {
        if(fgets(sbuff, 10000, modelfile) == NULL)
        {
            if(ferror(modelfile))
            {
                log_error("problem encountered reading line %d.", line);
                error_exit = 1;
                break;
            }
        }
        else
        {
            /* Check for comments and blank lines */
            if(sbuff[0] == '#' || sbuff[0] == '\r' || sbuff[0] == '\n')
            {
                continue;
            }
            if(*size == buffsize)
            {
                buffsize += buffsize;
                tmp = (TESSEROID *)realloc(model, buffsize*sizeof(TESSEROID));
                if(tmp == NULL)
                {
                    /* Need to free because realloc leaves unchanged in case of
                       error */
                    free(model);
                    log_error("problem expanding memory for tesseroid model.\nModel is too big.");
                    return NULL;
                }
                model = tmp;
            }
            /* Remove any trailing spaces or newlines */
            strstrip(sbuff);
            if(gets_mag_tess(sbuff, &model[*size]))
            {
                log_warning("bad/invalid tesseroid at line %d.", line);
                badinput = 1;
                continue;
            }
            (*size)++;
        }
    }
    if(badinput || error_exit)
    {
        free(model);
        return NULL;
    }
    /* Adjust the size of the model */
    if(*size != 0)
    {
        tmp = (TESSEROID *)realloc(model, (*size)*sizeof(TESSEROID));
        if(tmp == NULL)
        {
            /* Need to free because realloc leaves unchanged in case of
                error */
            free(model);
            log_error("problem freeing excess memory for tesseroid model.");
            return NULL;
        }
        model = tmp;
    }
    return model;
}


/* Read a single rectangular prism from a string */
