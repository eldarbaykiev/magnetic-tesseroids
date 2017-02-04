all: tessbx tessby tessbz tessutil_magnetize_model tessutil_gradient_calculator
 
tessbx:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbx.cpp src/version.cpp -o tessbx

tessby:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessby.cpp src/version.cpp -o tessby

tessbz:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbz.cpp src/version.cpp -o tessbz

tessutil_magnetize_model:
	gcc src/tessutil_magnetize_model.c -lm -o tessutil_magnetize_model

tessutil_gradient_calculator:
	g++ src/tessutil_gradient_calculator.cpp src/linalg.cpp -o tessutil_gradient_calculator


