/*
Generic main function for the tessb* programs.
*/

#ifndef _TESSEROIDS_TESSH_MAIN_H_
#define _TESSEROIDS_TESSH_MAIN_H_

#include "glq.h"
#include "geometry.h"

void print_tessb_help(const char *progname);
int run_tessb_main(int argc, char **argv, const char *progname, double (*field)(TESSEROID, double, double, double, GLQ, GLQ, GLQ), double ratio1, double ratio2, double ratio3);

#endif
