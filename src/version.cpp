/*
Hold the version number of the project.
*/

#include <stdio.h>
#include "version.h"


/* Print version and license information */
void print_version(const char *progname)
{
    printf("%s (Magnetic Tesseroids) %s\n\n", progname, tesseroids_version);
    printf("Copyright (C) 2015, Eldar Baykiev.\n");
    printf("This program is based on the code of Leonardo Uieda.\n");
    printf("<http://tesseroids.readthedocs.org/>\n");
    printf("Developed by Eldar Baykiev.\n");
}
