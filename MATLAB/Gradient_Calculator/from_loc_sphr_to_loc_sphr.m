function [v_sph] = from_loc_sphr_to_loc_sphr(vect, longitude1, colatitude1, longitude2, colatitude2)

    v_cart = from_loc_sphr_to_cart(colatitude1, longitude1, vect);
    v_sph = from_cart_to_loc_sphr(colatitude2, longitude2, v_cart);
    return
end