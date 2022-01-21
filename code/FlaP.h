
// Declarations
#ifndef FLAP_H_
#define FLAP_H_

int*  FLAP_DefInt(char* name, char* desc, int defValue);
bool* FLAP_DefBool(char* name, char* desc, bool defValue);
char** FLAP_DefBool(char* name, char* desc, char* defValue);

#endif

// Definitions
#ifdef FLAP_IMPLEMENTATION_

enum FlagType
{
	FLAG_INT,
	FLAG_STR,
	FLAG_BOOL,
};

struct Flag
{
	FlagType Type;
	char* Name;
	char* Description;

	union
	{
		int ValueInt;
		char* ValueStr;
		bool ValueBool;
	};
};

#ifndef FLAG_CAPACITY
#define FLAG_CAPACITY 256
#endif

static Flag Flags[FLAG_CAPACITY]
static int FlagCount;

static void
FlagCreate(FlagType type, char* name, char* desc)
{
	// TODO(afb) :: Assert hasn't reached capacity
	// Flag* result = 
}
#endif
