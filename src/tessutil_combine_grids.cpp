#include <stdio.h>
#include <stdlib.h>            
#include <string.h>
#include <math.h> 

#define MAX_GRID_POINTS 16000

#define GRID_FORMAT "%lf %lf %f %lf"

void printresult_withalt(double* longitudes, double* latitudes, float* altitudes, double* values, int n_values)
{

  
	for (int h = 0; h < n_values; h++)
        printf( "%lf %lf %f %lf\n", longitudes[h], latitudes[h], altitudes[h], values[h]);

	return;
}

void printresult(double* longitudes, double* latitudes, double* values, int n_values)
{

  
	for (int h = 0; h < n_values; h++)
        printf( "%lf %lf %lf\n", longitudes[h], latitudes[h], values[h]);

	return;
}


int main(int argc, char**argv)
{
	int n_files = (argc-1)/2;
	
	double lons[MAX_GRID_POINTS];
	double lats[MAX_GRID_POINTS];
	float alts[MAX_GRID_POINTS];

	double vals[MAX_GRID_POINTS];
	
	double factor = 1;
	
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	
	int n_lines = 0;
	
	for (int i = 0; i < n_files; i++) 
	{
		FILE * fp = fopen(argv[1+2*i], "r");
		sscanf(argv[1+2*i+1], "%lf", &factor);
		
		if (fp == NULL)
		{
			printf("ERROR: Can not open file with grid values.\n");
			exit(EXIT_FAILURE);
		}
		
		n_lines = 0;
		while ((read = getline(&line, &len, fp )) != -1)
		{

			if ((line[0] != '#') && (strlen(line) > 2))
			{
				n_lines++;
				if (n_lines>MAX_GRID_POINTS)
				{
					printf("ERROR: Too many grid points (> %d) in the input. Recompile program with a bigger value of MAX_GRID_POINTS.\n", n_lines);
					exit(EXIT_FAILURE);
				}
				double value;
			
				sscanf(line, GRID_FORMAT, &lons[n_lines-1], &lats[n_lines-1], &alts[n_lines-1], &value);
				vals[n_lines-1] = vals[n_lines-1] + value*factor;

			}
		}
		fclose(fp);
	}
	
	int no_alt = 0;
	
	if (no_alt)
		printresult(lons, lats, vals,  n_lines);
    else
		printresult_withalt(lons, lats, alts, vals, n_lines);
	
}
