

function save_grid_to_xyz_file(filefile_name, lons, lats, values)
%lons = linspace(grid_west_border, grid_east_border, grid_num_lon_pts), 
%lats = linspace(grid_south_border, grid_north_border, grid_num_lat_pts)
%values = X_n_s (already transposed)

    OUTPUT_NAME = fopen(filefile_name, 'w');

        
        for j = 1 : length(lats) 
            for i = 1 : length(lons)
                fprintf(OUTPUT_NAME, '%.2f %.2f %.12f\n', lons(i), lats(j), values(j, i));

            end 
        end

    fclose(OUTPUT_NAME);
    
end