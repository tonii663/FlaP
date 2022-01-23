
// Declarations
#ifndef FLAP_H_
#define FLAP_H_

int*   FLAP_DefInt(char* name, char* desc, int defValue);
bool*  FLAP_DefBool(char* name, char* desc, bool defValue);
char** FLAP_DefBool(char* name, char* desc, char* defValue);
bool   FLAP_Parse(int argCount, char** arguments);
#endif

// Definitions
#ifdef FLAP_IMPLEMENTATION_
#include <string.h>
#include <stdlib.h>


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
	flag->Value.Int = defValue;
	flag->Default.Int = defValue;
	return &flag->Value.Int;
}


char** FLAP_DefStr(char* name, char* desc, char* defValue)
{
	Flag* flag = FlagCreate(FLAG_STR, name, desc);
	flag->Value.Str = defValue;
	flag->Default.Str = defValue;
	return &flag->Value.Str;
}


bool* FLAP_DefBool(char* name, char* desc, bool defValue)
{
	Flag* flag = FlagCreate(FLAG_BOOL, name, desc);
	// flag->Value.Bool = defValue;
	flag->Default.Bool = defValue;
	return &flag->Value.Bool;
}



static int GetFlagIfValid(char* test, int testLen, char** out)
{
	int result = 0;
	if(*test == '-' || *test == '/')
	{
		*out = test+1;
		result = strlen(*out);
	}

	return result;
}

static bool StringCompare(char* str1, int str1Len,
						  char* str2, int str2Len)
{
	bool result = false;
	if(str1Len == str2Len)
	{
		result = true;
		
		for(int i = 0; i < str1Len; i++)
		{
			if(str1[i] != str2[i])
			{
				result = false;;
				break;
			}
		}
	}

	return result;
}

static bool IsInt(char* str)
{
	bool result = true;
	while(*str)
	{
		if(*str >= '0' && *str <= '9')
			str++;
		else
		{
			result = false;
			break;
		}
	}

	return result;
}


bool FLAP_Parse(int argCount, char** arguments)
{
	bool result = false;
	if(argCount > 1)
	{
		result = true;
		
		for(int argIndex = 1; argIndex < argCount; argIndex++)
		{
			for(int flagIndex = 0; flagIndex < FlagCount; flagIndex++)
			{
				char* flag = 0;
				int flagLen = GetFlagIfValid(arguments[argIndex],
											 strlen(arguments[argIndex]),
											 &flag);

				if(flag)
				{
					if(StringCompare(flag, flagLen,
									 Flags[flagIndex].Name,
									 strlen(Flags[flagIndex].Name)))
					{
						bool error = false;
						switch(Flags[flagIndex].Type)
						{
							case(FLAG_INT):
							{
								if(IsInt(arguments[++argIndex]))
								{
									Flags[flagIndex].Value.Int =
										atoi(arguments[argIndex]);
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_BOOL):
							{
								Flags[flagIndex].Value.Bool = true;
							}break;

							case(FLAG_STR):
							{
								char* value = arguments[++argIndex];
								if(value)
								{
									Flags[flagIndex].Value.Str = value;
								}
								else
								{
									error = true;
								}
								
							}break;

							default:
							{
								// TODO(afb) :: Log error
								error = true;
							}break;
						}

						if(error)
						{
							result = false;
						}

						break;
					}

				}
				else
				{
					// TODO(afb) :: Log error flag
					result = false;
					break;
				}
			}
		}
	}

	return result;
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
		fprintf(stream, "\t   %s\n", flag->Description);
		fprintf(stream, "\t      Default value(");
		FLAP_DisplayValue(stream, flag->Type, flag->Default);
		fprintf(stream, ")\n");
	}
}

#endif // FLAP_HELPER_
#endif // FLAP_IMPLEMENTATION_
