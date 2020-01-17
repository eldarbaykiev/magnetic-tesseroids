all: tessbx tessby tessbz tessutil_magnetize_model tessutil_gradient_calculator tessutil_combine_grids

tessbx:
	c++  -Wall src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbx.cpp src/version.cpp -o tessbx -framework Accelerate

tessby:
	c++  -Wall src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessby.cpp src/version.cpp -o tessby -framework Accelerate

tessbz:
	c++  -Wall src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbz.cpp src/version.cpp -o tessbz -framework Accelerate


clean:
	rm tessbx tessby tessbz
