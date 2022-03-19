#include <stdio.h>

#define FLAP_IMPLEMENTATION_
#define FLAP_HELPER_
#include "FlaP.h"

#include <stdint.h>


int main(int argc, char** argv)
{
	bool help;
	FLAP_DefBool(&help, "h", "Activate help menu", false);
	
	int* width = FLAP_Int("w", "Width of the image.", 800);
	uint64_t* height = FLAP_UInt64("height", "Height of the image.", 800);
	char** mode = FLAP_Str("m", "Generation mode.", "mt");
	
	// uint64_t* colorChanels = FLAP_UInt64("cc", "# of color channels.", 8);
	
	// int64_t* bitPerPixl = FLAP_Int64("bpp", "Bits per color channel", 24);

	//  bool* help = FLAP_Bool("h", "Activate help menu", false);
	
	

	bool s = FLAP_Parse(argc, argv);
			  
	if(s)
	{
		if(help)
		{
			FLAP_PrintFlags();
		}
		else
		{
			// ....

			printf("Width: %d\n", *width);
			printf("Height: %lld\n", *height);
			printf("Mode: %s\n", *mode);
		}
	}
	else
	{
		printf("Errors with flags.");
	}
	return 0;
}

