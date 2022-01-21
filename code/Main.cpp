#include <stdio.h>

#define FLAP_IMPLEMENTATION_
#define FLAP_HELPER_
#include "FlaP.h"


int main(int argc, char** argv)
{
	int* mt = FLAP_DefInt("mt", "Mono Topo", 124);
	bool* verbose = FLAP_DefBool("verbose", "Strong sym", false);
	char** mode = FLAP_DefStr("mode", "Version of sym", "mobay");

	FLAP_Print(stdout);
	
	return 0;
}
