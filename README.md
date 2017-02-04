# Forward modeling of magnetic field in spherical coordinates

Magnetic tesseroids is a collection of command-line tools for modeling the magnetic field.
The magnetization models can be made of right rectangular prisms or tesseroids (spherical prisms). 

To cite _magnetic tesseroids_ in publications, please use our paper published in Computers & Geosciences:

>**Eldar Baykiev**, **Jörg Ebbing**, **Marco Brönner**, **Karl Fabian**, Forward modeling magnetic fields of induced and remanent magnetization in the lithosphere using tesseroids, _Computers & Geosciences_, Volume 96, November 2016, Pages 124-135, ISSN 0098-3004, http://dx.doi.org/10.1016/j.cageo.2016.08.004.

Article can also be found here http://goo.gl/x9g7gi (researchgate).

## Usage of _magnetic tesseroids_
_Magnetic tesseroids_ are based on the existing program of Leonardo Uieda called tesseroids (Uieda, 2013) of version 1.1 (https://github.com/leouieda/tesseroids/releases/tag/v1.1). It inherits the interface of tesseroids-1.1 but with several changes. Present appendix describes constants and units used be the program, as well as input and output format.
### Constants
1. Geocentric mean Earth's radius  _R_E_ = 6378.137 km. 
1. Magnetic permeability of a free space _µ_0_ = 4π × 10^-7 H·m^-1

### List of programs
The tessbx, tessby, tessbz are programs that calculate the corresponding components (x - north, y - east, **z - up**) of the magnetic field of the tesseroid model on the computational grid. 

### Input: tesseroid model
The input model file should be a text file where each line describe one tesseroid in such space separated format:
> `W E S N HEIGHT_OF_TOP HEIGHT_OF_BOTTOM DENSITY SUSCEPTIBILITY BX BY BZ`

`W`, `E`, `S`, `N` correspond to the western, eastern, southern and northern edges of a tesseroid (_λ_1_, _λ_2_, _ϕ_1_, _ϕ_2_ respectively) expressed in decimal degrees [°].  
`HEIGHT_OF_TOP` and `HEIGHT_OF_BOTTOM` define the top and the bottom of tesseroid (_r_2_ and _r_1_ respectively). Both are counted from geocentric mean Earth's radius in meters [m]. If a tesseroid should be placed beneath the mean surface, than the values of these parameters should be negative. Note that `HEIGHT_OF_TOP` > `HEIGHT_OF_BOTTOM`.
`DENSITY` is the density _ρ_ of tesseroid in kilogram per cubic meter [kg/m^3]
`SUSCEPTIBILITY` is the susceptibility _χ_ of tesseroid in SI units.
`BX`, `BY` and `BZ` are the components of the magnetizing field in the local North-East-Up Cartesian coordinate system of a tesseroids' geometric center. They can be taken from any core field's model. Values are given in nanotesla [nT].
In case of remanent magnetic field modeling, susceptibility must be set 1 SI and `BX`, `BY` and `BZ` values than would define the direction of remanent magnetization vector.
This example shows a model made of 3 neighboring tesseroids near the North Pole:
> `-74 -73 89 90 -1000.000000 -11650.000000 1.000000 1.000000 334.9504973176 -1969.9308033594 -56572.6324041700`

> `-73 -72 89 90 -1000.000000 -11650.000000 1.000000 1.000000 370.1879538142 -1968.1093976826 -56571.2826313492`

> `-72 -71 89 90 -1000.000000 -11650.000000 1.000000 1.000000 405.4388222633 -1965.6409379187 -56569.9502088641`

### Input: computation grid
Computation grid can be regular or irregular and should be also a text file where each line describe the position of one computation point in such space separated format:
>`LON 	LAT ALT`

`LON` and `LAT` correspond to the longitude and latitude of the point in decimal degrees [°].
`ALT` corresponds to the altitude of the point above the mean surface in meters [m].
Note that the program tessgrd from original tesseroids-1.1 can be used to create a regular computation grid (see Uieda, 2013).
This example shows a grid made of 6 points with the same latitude and the altitude of 400 km:
> `-6 	51 400000 `

> `-5.8 51 400000 `

> `-5.6 51 400000 `

> `-5.4 51 400000 `

> `-5.2 51 400000 `

> `-5 	51 400000` 

### Performing calculations
Example: to calculate the vertical component of the magnetic field of a model in file modelfile.txt on a grid from file gridpoints.txt one can simply use a console command:
> `tessbz modelfile.txt < gridpoints.txt > gz_output.txt`

The result would be written in the file gz_output.txt.
### Output format
The programs' output is a modified grid file where in the end of each line the calculated value of a corresponding magnetic field component would be written. Values are given in nanotesla [nT] in the local North-East-Up coordinate system of a computational point. 
### Additional features
Magnetic tesseroids support features like piping and integration accuracy adjustment from tesseroids-1.1. Please, check sections in the tesseroids-1.1 manual (Uieda, 2013) relative to the gravity calculation programs to get more information.

## Utilities
### tessutil_magnetize_model
This program is made to 'magnetize' any existing tesseroid model by any given main field spherical harmonic model.
Usage: 
> `tessutil_magnetize_model [SH coeff file] [input tesseroid model file] [day] [month] [year] [output tesseroid model file] `

### tessutil_gradient_calculator
Gradient calculator (Baykiev et al., in press).
Usage: 
> `tessutil_gradient_calculator -bx[Bx grid file] -by[By grid file] -bz[Bx grid file] -o[output component] -c2 >> output_file.dat`

All grid files should be in tessgrd format. With option -c1 program read input grid bz as its direction is upward, with option -c2 - downward, just as in magnetic tesseroids output. Output of gradient calculator is always in North-East-Down coordinate system.

### tessutil_combine_grids
Sums calculated grids.
Usage:
> `tessutil_combine_grids [grid file1] [factor1] ... [grid fileN] [factorN] >> output_file.dat`

Each grid is multiplied by factor (susceptibility) and then the sum of all grids is calculated.