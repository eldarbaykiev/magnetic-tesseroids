#include "constants.h"
#include "grav_tess.h"
#include "tessb_main.h"


/** Main tessbx*/
int main(int argc, char **argv)
{
	return run_tessb_main(argc, argv, "tessbx", 0, TESSEROID_GXX_SIZE_RATIO, TESSEROID_GXY_SIZE_RATIO, TESSEROID_GXZ_SIZE_RATIO);

}
