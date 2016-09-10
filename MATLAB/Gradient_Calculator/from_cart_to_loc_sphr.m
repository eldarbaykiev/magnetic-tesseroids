function [res_vect] = from_cart_to_loc_sphr(colatitude, longitude, vect_xyzglob)


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

    res_vect = ([phi_unit, lambda_unit, r_unit].')*vect_xyzglob;

end