function magnetize_tesseroids(TESS_FILENAME, IGRF_DATE)
%FILENAME name of a tesseroid model without the magnetization with a format W E S N HOT HOB DENSITY
%date that should be used to calculate the magnetic field, for example '01-Jan-2014'

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
        %it to North-X East-Y Up-Z (the input and output format magnetic tesseroids)
        Bz = - Bz;
        
        fprintf(MAGTESS_FILE, '%f %f %f %f %f %f %f %d %.20f %.20f %.20f\n', W(i), E(i), S(i), N(i), HOT(i), HOB(i), DENSITY(i), 1, Bx, By, Bz);
    end
    fclose(MAGTESS_FILE)
end