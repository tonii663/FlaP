#include <stdio.h>

#define FLAP_IMPLEMENTATION_
#define FLAP_HELPER_
#include "FlaP.h"

#include <stdint.h>


int main(int argc, char** argv)
{

	int* width = FLAP_Int("w", "Width of the image.", 800);
	
	uint64_t* colorChanels = FLAP_UInt64("cc", "# of color channels.", 8);

	int64_t* bitPerPixl = FLAP_Int64("bpp", "Bits per color channel", 24);

	bool* help = FLAP_Bool("h", "Activate help menu", false);


	// FLAP_Parse(argc, argv);
	
	if(*help)
	{
		FLAP_PrintFlags(stdout);
	}
	else
	{
		// ....
	}
	
	return 0;
}
