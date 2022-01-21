#include <stdio.h>

#define FLAP_IMPLEMENTATION_
#include "FlaP.h"


int main(int argc, char** argv)
{
	int* mt = FLAP_DefInt("mt", "Mono Topo", 0);
	bool* verbose = FLAP_DefBool("verbose", "Strong sym", false);
	char** mode = FLAP_DefStr("mode", "Version of sym", "mobay");
	
	return 0;
}
