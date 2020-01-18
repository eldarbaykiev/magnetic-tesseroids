CC=
CFLAGS=

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
  CC=gcc

endif
ifeq ($(UNAME), Darwin)
  CC=clang
	CFLAGS += -framework Accelerate
endif

all: tessbx tessby tessbz

tessbx:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbx.cpp src/version.cpp -o tessbx $(CFLAGS)

tessby:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessby.cpp src/version.cpp -o tessby $(CFLAGS)

tessbz:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbz.cpp src/version.cpp -o tessbz $(CFLAGS)

clean:
	rm tessbx tessby tessbz
