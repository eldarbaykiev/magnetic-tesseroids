%%Author: Eldar Baykiev
%%Please cite as Eldar Baykiev, Jörg Ebbing, Marco Brönner, Karl Fabian - Sensitivity of satellite magnetic field gradients to the lithosphere – a case study from Fennoscandia (2016, in press)


clear all;
close all;


EARTH_RADIUS_TESS_M = 6378137.0;


GRAD_WITH_ROTATION = 1;
GRAD_CENTRAL_DIFF = 1;

%% NAME OF DATASET
DATASET = 'enter_your_dataset_name'; 
%name of the folder with calculated magnetic field grids
%all grids in the folder must have the same gridpoints
%IMPORTANT: grids should be produced by tessgrd program from tesseroids-1.1 
%				  should start from the most south-western point
%				  the increment should be in longitudes for each next point until the eastern border
%initial tesseroid model files, that were used, should be present in the
%folder

%there is a naming rule for grids and tesseroid files
%here is an example, how one tesseroid model and its calculated grids
%should look like in the folder:
%[tesseroid model name].magtess - model file
%[tesseroid model name].magtess_Bx.txt - Bx component grid
%[tesseroid model name].magtess_By.txt - By component grid
%[tesseroid model name].magtess_Bz.txt - Bz component grid



MATLABPATH = userpath;
PATH_DATA = strcat(MATLABPATH(1: end-1), '\Gradient_Calculator\'); 
%path to this folder with \ on the end

PATH = strcat(PATH_DATA, DATASET,'\');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CREATE LIST OF FILES
FILENAMES = dir(fullfile(strcat(PATH, '*.magtess')));
N_FILES = length(FILENAMES);

fprintf('Files to process:\n');
for n = 1 : N_FILES
    fprintf('  %d. %s\n', n, FILENAMES(n).name);
end

%% FIRST GRID READING
MODEL_NAME = strcat(PATH, FILENAMES(1).name);
    
BX_FILENAME = strcat(MODEL_NAME, '_Bx.txt');
BY_FILENAME = strcat(MODEL_NAME, '_By.txt');
BZ_FILENAME = strcat(MODEL_NAME, '_Bz.txt');

Bx_lines = textread(BX_FILENAME,'%f', 'commentstyle', 'shell');
By_lines = textread(BY_FILENAME,'%f', 'commentstyle', 'shell');
Bz_lines = textread(BZ_FILENAME,'%f', 'commentstyle', 'shell');
grid_num_pts = length(Bx_lines)/4;
    
Bx_vals = reshape(Bx_lines, 4, grid_num_pts)';
By_vals = reshape(By_lines, 4, grid_num_pts)';
Bz_vals = reshape(Bz_lines, 4, grid_num_pts)';
    
grid_west_border = min(Bx_vals(:, 1));
grid_east_border = max(Bx_vals(:, 1));

for grid_num_lon_pts = 2 : grid_num_pts
    if(Bx_vals(1, 1) == Bx_vals(grid_num_lon_pts, 1))
        grid_num_lon_pts = grid_num_lon_pts - 1;
        break;
    end
end

grid_lon_step = abs(Bx_vals(1, 1) - Bx_vals(2, 1));

grid_num_lat_pts = grid_num_pts / grid_num_lon_pts;
grid_south_border = min(Bx_vals(1:grid_num_lat_pts:grid_num_pts, 2));
grid_north_border = max(Bx_vals(1:grid_num_lat_pts:grid_num_pts, 2));

grid_lat_step = abs(Bx_vals(1, 2) - Bx_vals(1+grid_num_lon_pts, 2));

grid_height = Bx_vals(1, 3);

%% INITIALIZE ARRAYS WITH GRID DATA
grid_field_bx = zeros(N_FILES, grid_num_pts);
grid_field_by = zeros(N_FILES, grid_num_pts);
grid_field_bz = zeros(N_FILES, grid_num_pts);

grid_field_bx(1, :) = Bx_vals(:, 4);
grid_field_by(1, :) = By_vals(:, 4);
grid_field_bz(1, :) = Bz_vals(:, 4);


%% READ OTHER GRIDS

for i = 2 : N_FILES
  
    MODEL_NAME = strcat(PATH, FILENAMES(i).name);

    BX_FILENAME = strcat(MODEL_NAME, '_Bx.txt');
    BY_FILENAME = strcat(MODEL_NAME, '_By.txt');
    BZ_FILENAME = strcat(MODEL_NAME, '_Bz.txt');

    Bx_lines = textread(BX_FILENAME,'%f', 'commentstyle', 'shell');
    By_lines = textread(BY_FILENAME,'%f', 'commentstyle', 'shell');
    Bz_lines = textread(BZ_FILENAME,'%f', 'commentstyle', 'shell');

    Bx_vals = reshape(Bx_lines, 4, grid_num_pts)';
    By_vals = reshape(By_lines, 4, grid_num_pts)';
    Bz_vals = reshape(Bz_lines, 4, grid_num_pts)';

    grid_field_bx(i, :) = Bx_vals(:, 4);
    grid_field_by(i, :) = By_vals(:, 4);
    grid_field_bz(i, :) = Bz_vals(:, 4); 
  
end

suscept = ones(N_FILES, 1)*(-100);

field_bx = zeros(grid_num_pts, 1);
field_by = zeros(grid_num_pts, 1);
field_bz = zeros(grid_num_pts, 1);

%% CHECKING IF THERE IS A CONFIG FILE WITH PREDESCRIBED SUSCEPTIBILITIES
%if there is no config file, one have to enter the susceptibilites of each
%tesseroid model in the folder manually
if exist(strcat(PATH, 'config.txt'), 'file') == 2
    CONFIG_FILE = fopen(strcat(PATH, 'config.txt'));
    CONFIG_LINES = textscan(CONFIG_FILE,'%s %f');
    
    for i = 1 : length(CONFIG_LINES{2})
        for j = 1 : N_FILES
            if strcmp(CONFIG_LINES{1}{i}, FILENAMES(j).name)
                suscept(j) = CONFIG_LINES{2}(i);
            end
        end
    end
     
    fclose(CONFIG_FILE); 
end


%% APPLYING SUSCEPTIBILITIES TO THE GRIDS OF EACH MODEL
for i = 1 : N_FILES
    if suscept(i) == -100
        suscept(i) = input(strcat('Susceptibility for the layer [', FILENAMES(i).name, ']:'));
        CONFIG_FILE = fopen(strcat(PATH, 'config.txt'), 'a+');
        fseek(CONFIG_FILE, 0, 'eof');
        fprintf(CONFIG_FILE, '%s %f\n', FILENAMES(i).name, suscept(i));
        fclose(CONFIG_FILE); 
    end
    
    if (suscept(i) ~= 0)
        field_bx = field_bx + grid_field_bx(i, :)'*suscept(i);
        field_by = field_by + grid_field_by(i, :)'*suscept(i);
        field_bz = field_bz + grid_field_bz(i, :)'*suscept(i);  
    end
end


%% RESULTING GRIDS - ROWS
res_X_north_cmp = field_bx;
res_Y_east_cmp = field_by;
res_Z_vert_cmp = -field_bz; %NOTE: sign change here is done to change the coordinate system to North East Down, since magnetic tesseroids are calculating the field components in Noth East Up coordinate system.

sus_string = strrep(mat2str(suscept'), ' ', '_');
sus_string = strrep(sus_string, '.', '');
DATASET = strcat(DATASET);%, '_', sus_string); %%%%%%%%%%%%%%


%% REUSLTING GRIDS - GRD
res_X_north_cmp = reshape(res_X_north_cmp,grid_num_lon_pts, grid_num_lat_pts);
res_Y_east_cmp = reshape(res_Y_east_cmp,grid_num_lon_pts, grid_num_lat_pts);
res_Z_vert_cmp = reshape(res_Z_vert_cmp,grid_num_lon_pts, grid_num_lat_pts);

res_X = res_X_north_cmp';
res_Y = res_Y_east_cmp';
res_Z = res_Z_vert_cmp';

%% SAVE ALL GRIDS
lon_coords = linspace(grid_west_border, grid_east_border, grid_num_lon_pts);
lat_coords = linspace(grid_south_border, grid_north_border, grid_num_lat_pts);

save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bx.xyz'), lon_coords, lat_coords, res_X);
save_grid_to_xyz_file(strcat(PATH, DATASET, '_By.xyz'), lon_coords, lat_coords, res_Y); 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bz.xyz'), lon_coords, lat_coords, res_Z);

%% FIGURE 1 Z COMPONENT
fig = figure('Name', strcat(DATASET, ' Z (Vert. - Down)'));
pcolor(linspace(grid_west_border, grid_east_border, grid_num_lon_pts), linspace(grid_south_border, grid_north_border, grid_num_lat_pts), res_Z_vert_cmp');
shading flat
shading interp
cb_main = colorbar;
xlabel(cb_main, '[nT]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT]');

%% GRADIENT
%%VIA CENTRAL DIFFERENCES

Bx = res_X;
By = res_Y;
Bz = res_Z;

C_ALT = grid_height;
C_LON = reshape(Bx_vals(:, 1),grid_num_lon_pts, grid_num_lat_pts);
C_LON = C_LON';

C_LAT = reshape(Bx_vals(:, 2),grid_num_lon_pts, grid_num_lat_pts);
C_LAT = C_LAT';

Bxx = Bx*0;
Byx = Bx*0;
Bzx = Bx*0;
Bxy = Bx*0;
Byy = Bx*0;
Bzy = Bx*0;


for i = 2 : grid_num_lat_pts -1
    for j = 2 : grid_num_lon_pts -1

        %with rotation
        if GRAD_WITH_ROTATION == -1
            vect_w = from_loc_sphr_to_loc_sphr([Bx(i, j-1); By(i, j-1); Bz(i, j-1)], C_LON(i, j-1), C_LAT(i, j-1), C_LON(i, j), C_LAT(i, j));
            vect_e = from_loc_sphr_to_loc_sphr([Bx(i, j+1); By(i, j+1); Bz(i, j+1)], C_LON(i, j+1), C_LAT(i, j+1), C_LON(i, j), C_LAT(i, j));
            vect_s = from_loc_sphr_to_loc_sphr([Bx(i-1, j); By(i-1, j); Bz(i-1, j)], C_LON(i-1, j), C_LAT(i-1, j), C_LON(i, j), C_LAT(i, j));
            vect_n = from_loc_sphr_to_loc_sphr([Bx(i+1, j); By(i+1, j); Bz(i+1, j)], C_LON(i+1, j), C_LAT(i+1, j), C_LON(i, j), C_LAT(i, j));   
        else
            %%% no_rotation
            vect_w = [Bx(i, j-1); By(i, j-1); Bz(i, j-1)];
            vect_e = [Bx(i, j+1); By(i, j+1); Bz(i, j+1)];
            vect_s = [Bx(i-1, j); By(i-1, j); Bz(i-1, j)];
            vect_n = [Bx(i+1, j); By(i+1, j); Bz(i+1, j)];
        end
            
        vect_c = [Bx(i, j); By(i, j); Bz(i, j)];

        
        r_w = C_ALT + EARTH_RADIUS_TESS_M;
        lat_w = C_LAT(i, j-1)*pi/180.0;
        lon_w = C_LON(i, j-1)*pi/180.0;
        
        r_e = C_ALT + EARTH_RADIUS_TESS_M;
        lat_e = C_LAT(i, j+1)*pi/180.0;
        lon_e = C_LON(i, j+1)*pi/180.0;
        
        r_s = C_ALT + EARTH_RADIUS_TESS_M;
        lat_s = C_LAT(i-1, j)*pi/180.0;
        lon_s = C_LON(i-1, j)*pi/180.0;
        
        r_n = C_ALT + EARTH_RADIUS_TESS_M;
        lat_n = C_LAT(i+1, j)*pi/180.0;
        lon_n = C_LON(i+1, j)*pi/180.0;
        
        r_c = C_ALT + EARTH_RADIUS_TESS_M;
        lat_c = C_LAT(i, j)*pi/180.0;
        lon_c = C_LON(i, j)*pi/180.0;
        
        
        [x_e, y_e, z_e] = sph2cart(lon_e,lat_e,r_e);
        [x_n, y_n, z_n] = sph2cart(lon_n,lat_n,r_n);
        
        if GRAD_CENTRAL_DIFF == 1
            [x_w, y_w, z_w] = sph2cart(lon_w,lat_w,r_w);
            ew_dist = norm([x_w, y_w, z_w]-[x_e, y_e, z_e]);

            [x_s, y_s, z_s] = sph2cart(lon_s,lat_s,r_s);
            ns_dist = norm([x_s, y_s, z_s]-[x_n, y_n, z_n]);
        
            Bxyz_y = (vect_e - vect_w)/(ew_dist) ;
            Bxyz_x = (vect_n - vect_s)/(ns_dist) ;

            Bxx(i, j) = Bxyz_x(1)*1000;
            Byx(i, j) = Bxyz_x(2)*1000;
            Bzx(i, j) = Bxyz_x(3)*1000;

            Bxy(i, j) = Bxyz_y(1)*1000;
            Byy(i, j) = Bxyz_y(2)*1000;
            Bzy(i, j) = Bxyz_y(3)*1000;
        else
            [x_c, y_c, z_c] = sph2cart(lon_c,lat_c,r_c);
            ew_dist = norm([x_c, y_c, z_c]-[x_e, y_e, z_e]);
            ns_dist = norm([x_c, y_c, z_c]-[x_n, y_n, z_n]);

            Bxyz_y = (vect_e - vect_c)/(ew_dist) ;
            Bxyz_x = (vect_n - vect_c)/(ns_dist) ;

            Bxx(i, j) = Bxyz_x(1)*1000;
            Byx(i, j) = Bxyz_x(2)*1000;
            Bzx(i, j) = Bxyz_x(3)*1000;
            Bxy(i, j) = Bxyz_y(1)*1000;
            Byy(i, j) = Bxyz_y(2)*1000;
            Bzy(i, j) = Bxyz_y(3)*1000;
        end 
    end
end

Bzz = -Bxx-Byy;


figure('Name', strcat(DATASET, ' Gradients'))
sp_bxx = subplot(3, 3, 1);
pcolor(C_LON, C_LAT, Bxx);
shading flat
shading interp
cb_bxx = colorbar;
xlabel(cb_bxx, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_bxx,'B_{xx}')

sp_byx = subplot(3, 3, 4);
pcolor(C_LON, C_LAT, Byx);
shading flat
shading interp
cb_byx = colorbar;
xlabel(cb_byx, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_byx,'B_{yx}')

sp_bzx = subplot(3, 3, 7);
pcolor(C_LON, C_LAT, Bzx);
shading flat
shading interp
cb_bzx = colorbar;
xlabel(cb_bzx, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_bzx,'B_{zx}')

sp_bxy = subplot(3, 3, 2);
pcolor(C_LON, C_LAT, Bxy);
shading flat
shading interp
cb_bxy = colorbar;
xlabel(cb_bxy, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_bxy,'B_{xy}');

sp_byy = subplot(3, 3, 5);
pcolor(C_LON, C_LAT, Byy);
shading flat
shading interp
cb_byy = colorbar;
xlabel(cb_byy, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_byy,'B_{yy}');


sp_bzy = subplot(3, 3, 8);
pcolor(C_LON, C_LAT, Bzy);
shading flat
shading interp
cb_bzy = colorbar;
xlabel(cb_bzy, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_bzy,'B_{zy}');
    
sp_bzz = subplot(3, 3, 9);
pcolor(C_LON, C_LAT, -Bxx-Byy);
shading flat
shading interp
cb_bzz = colorbar;
xlabel(cb_bzz, '[nT/km]');
axis tight;
axis square;
ylabel('latitude [deg]');
xlabel('longitude [deg]');
zlabel('[nT/km]');
title(sp_bzz,'B_{zz}');


%% SAVE ALL GRIDS
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bxx.xyz'), lon_coords, lat_coords, Bxx*1000) 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Byx.xyz'), lon_coords, lat_coords, Byx*1000) 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bzx.xyz'), lon_coords, lat_coords, Bzx*1000) 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bxy.xyz'), lon_coords, lat_coords, Bxy*1000) 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Byy.xyz'), lon_coords, lat_coords, Byy*1000) 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bzy.xyz'), lon_coords, lat_coords, Bzy*1000) 
save_grid_to_xyz_file(strcat(PATH, DATASET, '_Bzz.xyz'), lon_coords, lat_coords, (-Bxx-Byy)*1000)  
    
    
 







