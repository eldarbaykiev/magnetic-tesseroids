CC=
CFLAGS=
POSTFIX=
CFLAGSOPT=

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	CC=gcc
	CFLAGS += -lopenblas -lm $(CFLAGSOPT)
	POSTFIX=linux

endif
ifeq ($(UNAME), Darwin)
	CC=clang
	CFLAGS += -framework Accelerate $(CFLAGSOPT)
	POSTFIX=mac
endif

all: tessbx tessby tessbz

tools: tessutil_combine_grids tessutil_magnetize_model tessutil_gradient_calculator

tessbx:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbx.cpp src/version.cpp -o tessbx_$(POSTFIX) $(CFLAGS)

tessby:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessby.cpp src/version.cpp -o tessby_$(POSTFIX) $(CFLAGS)

tessbz:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessbz.cpp src/version.cpp -o tessbz_$(POSTFIX) $(CFLAGS)

tessutil_combine_grids:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessutil_combine_grids.cpp src/version.cpp -o tessutil_combine_grids_$(POSTFIX) $(CFLAGS)

tessutil_magnetize_model:
	$(CC)  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/tessutil_magnetize_model.c src/version.cpp -o tessutil_magnetize_model_$(POSTFIX) $(CFLAGS)

tessutil_gradient_calculator:
	$(CC)  src/tessutil_gradient_calculator.cpp  src/geometry.cpp src/glq.cpp src/grav_tess.cpp src/linalg.cpp src/logger.cpp src/parsers.cpp src/tessb_main.cpp src/version.cpp -o tessutil_gradient_calculator_$(POSTFIX) $(CFLAGS)



clean:
	rm tessbx_$(POSTFIX) tessby_$(POSTFIX) tessbz_$(POSTFIX) tessutil_combine_grids_$(POSTFIX) tessutil_magnetize_model_$(POSTFIX) tessutil_gradient_calculator_$(POSTFIX)
