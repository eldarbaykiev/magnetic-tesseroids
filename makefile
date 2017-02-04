all: tessbx tessby tessbz tessutil_magnetize_model tessutil_gradient_calculator tessutil_combine_grids
 
tessbx:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbx.cpp src/version.cpp -o tessbx

tessby:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessby.cpp src/version.cpp -o tessby

tessbz:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbz.cpp src/version.cpp -o tessbz

tessutil_magnetize_model:
	gcc src/tessutil_magnetize_model.c -lm -o tessutil_magnetize_model

tessutil_gradient_calculator:
	g++ -Wall src/tessutil_gradient_calculator.cpp  src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/version.cpp -o tessutil_gradient_calculator

tessutil_combine_grids:
	g++ -Wall  src/tessutil_combine_grids.cpp -o tessutil_combine_grids

clean:
	find . -maxdepth 1 -type f -executable -delete



