function magtess_magnetize_tesseroids(TESS_FILENAME, IGRF_DATE)
%magtess_magnetize_tesseroids - Magnetize tesseroid model with IGRF main field for the magnetic-tesseroids v0.1
%
%Requires IGRF Magnetic Field calculator by Charles Rino
%(http://de.mathworks.com/matlabcentral/fileexchange/28874-igrf-magnetic-field)
%
% Syntax:  magtess_magnetize_tesseroids(TESS_FILENAME, IGRF_DATE)
%
% Inputs:
%    TESS_FILENAME - name of a tesseroid model file with a format W E S N HeightOfTop HeightOfBottom Density
%    IGRF_DATE - date that should be used to calculate the magnetic field, for example '01-Jan-2014'
%
% Other m-files required: igrf.m (from IGRF Magnetic Field calculator by Charles Rino)
% Subfunctions: none
% MAT-files required: none
%

% Author: Eldar Baykiev
% email: eldar.baykiev@ntnu.no
% November 2015; 


    EARTH_RADIUS_IGRF_KM = 6371.2;
    EARTH_RADIUS_TESS_M = 6378137.0;
    
    IGRF_DATENUM = datenum(IGRF_DATE);
    
    tess_model = load(TESS_FILENAME);
    number_of_tesseroids = length(tess_model(:, 1));
    W = tess_model(:, 1);
    E = tess_model(:, 2);
    S = tess_model(:, 3);
    N = tess_model(:, 4);
    HOT = tess_model(:, 5);
    HOB = tess_model(:, 6);
    DENSITY = tess_model(:, 7);
    
    
    MAGTESS_FILE = fopen(strcat(TESS_FILENAME, '_IGRF_DATE_', IGRF_DATE, '.magtess'), 'w');
    for i = 1 : number_of_tesseroids
        %calculate the position of the tesseroid center
        lon_c = 0.5*(W(i) + E(i));
        lat_c = 0.5*(S(i) + N(i));
        alt_c = 0.5*(HOT(i) + HOB(i));
        
        %convert for the altitude above the reference earth radius of tesseroid-program and
        altitude_igrf = (alt_c + EARTH_RADIUS_TESS_M - EARTH_RADIUS_IGRF_KM*1000.0)/1000.0; 
        
        %calculate the field
        [Bx, By, Bz] = igrf(IGRF_DATENUM, lat_c, lon_c, altitude_igrf);
        
        %because Bx By Bz are in North-X East-Y Down-Z we need to convert
        %it to North-X East-Y Up-Z (the input and output format magnetic-tesseroids v0.1)
        Bz = - Bz;
        
        fprintf(MAGTESS_FILE, '%f %f %f %f %f %f %f %d %.20f %.20f %.20f\n', W(i), E(i), S(i), N(i), HOT(i), HOB(i), DENSITY(i), 1, Bx, By, Bz);
    end
    fclose(MAGTESS_FILE);
end