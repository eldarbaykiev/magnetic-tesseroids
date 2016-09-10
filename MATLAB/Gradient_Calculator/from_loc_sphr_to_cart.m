function [res_vect] = from_loc_sphr_to_cart(colatitude, longitude, vect_xyzloc)


    phi = colatitude*pi/180.0;
    lambda = longitude*pi/180.0;

    r_unit = [cos(phi)*cos(lambda); ...
              cos(phi)*sin(lambda); ...
              sin(phi); ];

    phi_unit = [-sin(phi)*cos(lambda); ...
                -sin(phi)*sin(lambda); ...
                cos(phi); ];

    lambda_unit = [-sin(lambda); ...
                   cos(lambda); ...
                   0;];

    res_vect = [phi_unit, lambda_unit, r_unit]*vect_xyzloc;
end