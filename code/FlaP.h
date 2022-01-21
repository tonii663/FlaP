
// Declarations
#ifndef FLAP_H_
#define FLAP_H_

int*   FLAP_DefInt(char* name, char* desc, int defValue);
bool*  FLAP_DefBool(char* name, char* desc, bool defValue);
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


union FlagField
{
	int   Int;
	char* Str;
	bool  Bool;
};

struct Flag
{
	FlagType Type;
	char* Name;
	char* Description;

	FlagField Value;
	FlagField Default;
};

#ifndef FLAG_CAPACITY
#define FLAG_CAPACITY 256
#endif

static Flag Flags[FLAG_CAPACITY];
static int FlagCount = 0;

static Flag*
FlagCreate(FlagType type, char* name, char* desc)
{
	// TODO(afb) :: Assert hasn't reached capacity
	Flag* result = &Flags[FlagCount++];
	result->Name = name;
	result->Description = desc;
	result->Type = type;
	return result;
}

int* FLAP_DefInt(char* name, char* desc, int defValue)
{
	Flag* flag = FlagCreate(FLAG_INT, name, desc);
	flag->Default.Int = defValue;
	return &flag->Value.Int;
}


char** FLAP_DefStr(char* name, char* desc, char* defValue)
{
	Flag* flag = FlagCreate(FLAG_STR, name, desc);
	flag->Default.Str = defValue;
	return &flag->Value.Str;
}


bool* FLAP_DefBool(char* name, char* desc, bool defValue)
{
	Flag* flag = FlagCreate(FLAG_BOOL, name, desc);
	flag->Default.Bool = defValue;
	return &flag->Value.Bool;
}

static void FLAP_DisplayValue(FILE* stream, FlagType type, FlagField field)
{
	switch(type)
	{
		case(FLAG_INT):
		{
			fprintf(stream, "%d", field.Int);
		}break;

		case(FLAG_STR):
		{
			fprintf(stream, "%s", field.Str);
		}break;

		case(FLAG_BOOL):
		{
			char* value = field.Bool ? "true" : "false";
			fprintf(stream, "%s", value);
		}break;
		
		default: break;
	}
}

#ifdef FLAP_HELPER_
#include <stdio.h>

void FLAP_Print(FILE* stream)
{
	fprintf(stream, "<OPTIONS>:\n");
	for(int i = 0; i < FlagCount; i++)
	{
		Flag* flag = &Flags[i];
		fprintf(stream, "\t-%s\n", flag->Name);
		fprintf(stream, "\t   %s", flag->Description);
		fprintf(stream, "(Defaults to ");
		FLAP_DisplayValue(stream, flag->Type, flag->Default);
		fprintf(stream, ")\n");
	}
}

#endif

#endif
