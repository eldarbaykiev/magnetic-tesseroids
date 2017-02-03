all: tessbx tessby tessbz

tessbx:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbx.cpp src/version.cpp -o tessbx

tessby:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessby.cpp src/version.cpp -o tessby

tessbz:
	g++ -Wall src/dipoleb_main.cpp src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbz.cpp src/version.cpp -o tessbz


