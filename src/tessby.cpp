#include "constants.h"
#include "grav_tess.h"
#include "tessb_main.h"


/** Main tessby*/
int main(int argc, char **argv)
{
	return run_tessb_main(argc, argv, "tessby", 0, TESSEROID_GXX_SIZE_RATIO, TESSEROID_GXY_SIZE_RATIO, TESSEROID_GXZ_SIZE_RATIO);

}
