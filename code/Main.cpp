#include <stdio.h>

#define FLAP_IMPLEMENTATION_
#define FLAP_HELPER_
#include "FlaP.h"


int main(int argc, char** argv)
{
	int* mt = FLAP_DefInt("mt", "Mono Topo", 124);
	int* lpas = FLAP_DefInt("lpas", "sopo", 4);
	int* fg = FLAP_DefInt("fg", "Chronos Topo", 45);
	
	// bool* verbose = FLAP_DefBool("verbose", "Strong sym", false);
	// char** mode = FLAP_DefStr("mode", "Version of sym", "mobay");

	FLAP_Print(stdout);


	bool s = FLAP_Parse(argc, argv);
	if(s)
	{
		printf("%d\n", *mt);
		printf("%d\n", *lpas);
		printf("%d\n", *fg);
	}
	return 0;
}
