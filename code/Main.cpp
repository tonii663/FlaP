#include <stdio.h>

#define FLAP_IMPLEMENTATION_
#define FLAP_HELPER_
#include "FlaP.h"


int main(int argc, char** argv)
{
	int* mt = FLAP_DefInt("mt", "Mono Topo", 124);
	int* lpas = FLAP_DefInt("lpas", "sopo", 4);
	char** mode = FLAP_DefStr("mode", "Version of sym", "mobay");
	int* fg = FLAP_DefInt("fg", "Chronos Topo", 45);
	bool* help = FLAP_DefBool("help", "Command list", false);
	bool* verbose = FLAP_DefBool("verbose", "Strong sym", false);
	   
	bool s = FLAP_Parse(argc, argv);

	if(s)
	{
		if(*help)
		{
			FLAP_Print(stdout);
		}
		else
		{
			printf("mode: %s\n", *mode);
			printf("mt: %d\n", *mt);
		}
	}
	else
	{
		printf("ERROR with flags\n");
	}
	
	return 0;
}
