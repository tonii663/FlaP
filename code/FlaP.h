// Declarations
#ifndef FLAP_H_
#define FLAP_H_
#include <stdint.h>

int32_t*  FLAP_Int(char* name, char* desc, int defValue);
int64_t*  FLAP_Int64(char* name, char* desc, int64_t defValue);
uint64_t* FLAP_UInt64(char* name, char* desc, uint64_t defValue);
bool*     FLAP_Bool(char* name, char* desc, bool defValue);
char**    FLAP_Str(char* name, char* desc, char* defValue);

void FLAP_DefInt(int32_t* var, char* name, char* desc, int defValue);
void FLAP_DefInt64(int32_t* var, char* name, char* desc, int defValue);
void FLAP_DefUInt64(int32_t* var, char* name, char* desc, int defValue);
void FLAP_DefStr(char** var, char* name, char* desc, char* defValue);
void FLAP_DefBool(bool* var, char* name, char* desc, bool defValue);

bool FLAP_Parse(int argCount, char** arguments);
void FLAP_PrintFlags(FILE* stream);
#endif


#ifdef FLAP_IMPLEMENTATION_
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef intptr_t FlagField;

enum FlagType
{
	FLAG_INT,
	FLAG_INT64,
	FLAG_UINT64,
	
	FLAG_DEF_INT,
	FLAG_DEF_UINT64,
	FLAG_DEF_INT64,
	
	FLAG_FLOAT,
	FLAG_DEF_FLOAT,
	
	FLAG_BOOL,
	FLAG_DEF_BOOL,
	
	FLAG_STR,
	FLAG_DEF_STR,
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
	assert(FlagCount < FLAG_CAPACITY && "Too much flags. Increase capacity.");
	Flag* result = &Flags[FlagCount++];
	result->Name = name;
	result->Description = desc;
	result->Type = type;
	return result;
}


void FLAP_DefInt(int32_t* var, char* name, char* desc, int defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_INT, name, desc);
	flag->Default = defValue;
	*(int32_t**)&flag->Value = var;
	*(int32_t*)flag->Value = defValue;
}

void FLAP_DefInt64(int64_t* var, char* name, char* desc,
					   int64_t defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_INT64, name, desc);
	flag->Default = defValue;
	*(int64_t**)&flag->Value = var;
	*(int64_t*)flag->Value = defValue;
}

void FLAP_DefUInt64(uint64_t* var, char* name, char* desc,
						 uint64_t defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_UINT64, name, desc);
	flag->Default = defValue;
	*(uint64_t**)&flag->Value = var;
	*(uint64_t*)flag->Value = defValue;
}

void FLAP_DefFloat(float* var, char* name, char* desc,
				   float defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_FLOAT, name, desc);
	*(float*)&flag->Default = defValue;
	*(float**)&flag->Value = var;
	*(float*)flag->Value = defValue;
}


void FLAP_DefStr(char** var, char* name, char* desc, char* defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_STR, name, desc);
	*(char**)&flag->Default = defValue;
	*(char***)&flag->Value = var;
	*(char**)flag->Value = defValue;
}

void FLAP_DefBool(bool* var, char* name, char* desc, bool defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_BOOL, name, desc);
	*(bool**)&flag->Value = var;
	*(bool*)flag->Value = defValue;
	flag->Default = defValue;
}


uint64_t* FLAP_UInt64(char* name, char* desc, uint64_t defValue)
{
	Flag* flag = FlagCreate(FLAG_INT, name, desc);
	flag->Value = defValue;
	flag->Default = defValue;
	return (uint64_t*)(&flag->Value);
}

int64_t* FLAP_Int64(char* name, char* desc, int64_t defValue)
{
	Flag* flag = FlagCreate(FLAG_INT, name, desc);
	flag->Value = defValue;
	flag->Default = defValue;
	return (int64_t*)(&flag->Value);
}

int32_t* FLAP_Int(char* name, char* desc, int defValue)
{
	Flag* flag = FlagCreate(FLAG_INT, name, desc);
	flag->Value = defValue;
	flag->Default = defValue;
	return (int32_t*)(&flag->Value);
}

char** FLAP_Str(char* name, char* desc, char* defValue)
{
	Flag* flag = FlagCreate(FLAG_STR, name, desc);
	*(char**)&flag->Value =  defValue;
	*(char**)&flag->Default = defValue;
	return (char**)(&flag->Value);
}

bool* FLAP_Bool(char* name, char* desc, bool defValue)
{
	Flag* flag = FlagCreate(FLAG_BOOL, name, desc);
	flag->Value = defValue;
	flag->Default = defValue;
	return (bool*)(&flag->Value);
}

float* FLAP_Float(char* name, char* desc, float defValue)
{
	Flag* flag = FlagCreate(FLAG_FLOAT, name, desc);
	*(float*)&flag->Value = defValue;
	*(float*)&flag->Default = defValue;
	return (float*)(&flag->Value);
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
	if(!str)
		return false;

	bool result = true;
	if(*str == '-')
		str++;

	if(str)
	{
		if(str != 0 && (str+1) != 0 && str[0] == '0'  && str[1] == 'x')
		{
			str += 2;
		
			while(*str)
			{
				if((*str >= '0' && *str <= '9') ||
				   (*str >= 'a' && *str <= 'f') ||
				   (*str >= 'A' && *str <= 'F'))
					str++;
				else
				{
					result = false;
					break;
				}
			}
		}
		else
		{
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
		}
	}
	else
	{
		result = false;
	}
	
	return result;
}

static bool IsFloat(char* str)
{
	if(!str)
		return false;
		
	bool result = true;

	int dotCount = 0;
	
	while(*str)
	{
		if((*str >= '0' && *str <= '9') || *str == '.')
		{
			if(*str == '.')
			{
				dotCount++;				
			}
		}
		else
		{
			result = false;
			break;
		}

		str++;
	}
	result = result && dotCount < 2;
	
	return result;
}


static void StringSplitAtFirst(char* str, char c, char** first, char** last)
{
	*first = str;
	
	while(str && str[0] != c)
		str++;

	if(str)
	{
		*last = str;
	}
	else
	{
		*last = 0;
	}
}


static bool GetArgAndParam(char** args, int* current,
						   char** arg, int* argLength,
						   char** param, int* paramLength)
{
	bool result = true;

	if(args[*current] &&
	   (args[*current][0] == '-' || args[*current][0] == '/'))
	{
		char* first = 0;
		char* last = 0;

		StringSplitAtFirst(args[*current], '=', &first, &last);

		if(last)
		{
			*arg = first;
			*argLength = (int)(last - first);

			*param = last;
			*paramLength = strlen(*arg) - *argLength;
			*current++;
		}
		else
		{
			*arg = args[*current++];
			*argLength = strlen(*arg);
			if(args[*current])
			{
				*param = args[*current++];
				*paramLength = strlen(*param);
			}
			else
			{
				result = false;
			}
		}
	}
	else
	{
		result = false;
	}
	return result;
}

bool FLAP_Parse(int argCount, char** arguments)
{
	bool result = true;
	if(argCount > 1)
	{
		for(int argIndex = 1; argIndex < argCount; argIndex++)
		{
			for(int flagIndex = 0; flagIndex < FlagCount; flagIndex++)
			{
				char* flag = 0;
				int flagLen = 0;
				
				char* param = 0;
				int paramLen = 0;
				
				bool success =  GetArgAndParam(arguments, &argIndex,
											   &flag, &flagLen,
											   &param, &paramLen);

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
									// TODO(afb) :: Check for errors
									int32_t value =
										strtol(arguments[argIndex], 0, 0);
									Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;
							
							case(FLAG_DEF_INT):
							{
								if(IsInt(arguments[++argIndex]))
								{
									int32_t value =
										strtol(arguments[argIndex], 0, 0);
									*(int32_t*)Flags[flagIndex].Value =
										value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_UINT64):
							{
								if(IsInt(arguments[++argIndex]))
								{
									uint64_t value =
										strtoll(arguments[argIndex], 0, 0);
									Flags[flagIndex].Value =
										value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_DEF_UINT64):
							{
								if(IsInt(arguments[++argIndex]))
								{
									uint64_t value =
										strtoll(arguments[argIndex], 0, 0);

									*(uint64_t*)Flags[flagIndex].Value =
										value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_INT64):
							{
								if(IsInt(arguments[++argIndex]))
								{
									int64_t value =
										strtoll(arguments[argIndex], 0, 0);
									
									Flags[flagIndex].Value =
										value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_DEF_INT64):
							{
								if(IsInt(arguments[++argIndex]))
								{
									int64_t value =
										strtoll(arguments[argIndex], 0, 0);

									*(int64_t*)Flags[flagIndex].Value =
										value;
								}
								else
								{
									error = true;
								}
							}break;

							
							case(FLAG_FLOAT):
							{
								if(IsFloat(arguments[++argIndex]))
								{
									*(float*)&Flags[flagIndex].Value =
										(float)atof(arguments[argIndex]);
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_DEF_FLOAT):
							{
								if(IsFloat(arguments[++argIndex]))
								{
									*(float*)Flags[flagIndex].Value =
										(float)atof(arguments[argIndex]);
								}
								else
								{
									error = true;
								}
							}break;
							
							case(FLAG_BOOL):
							{
								Flags[flagIndex].Value = true;
							}break;

							case(FLAG_DEF_BOOL):
							{
								*(bool*)Flags[flagIndex].Value = true;
							}break;

							
							case(FLAG_STR):
							{
								char* value = arguments[++argIndex];
								if(value)
								{
									*(char**)&Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
								
							}break;

							case(FLAG_DEF_STR):
							{
								char* value = arguments[++argIndex];
								if(value)
								{
									**(char***)&Flags[flagIndex].Value = value;
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
		case(FLAG_DEF_INT):
		case(FLAG_INT):
		{
			fprintf(stream, "%d", (int)field);
		}break;

		case(FLAG_DEF_FLOAT):
		case(FLAG_FLOAT):
		{
			fprintf(stream, "%f", *(float*)&field);
		}break;

		case(FLAG_DEF_STR):
		case(FLAG_STR):
		{
			fprintf(stream, "%s", (char*)field);
		}break;

		case(FLAG_DEF_BOOL):
		case(FLAG_BOOL):
		{
			char* value = field ? "true" : "false";
			fprintf(stream, "%s", value);
		}break;
		
		default: break;
	}
}

#ifdef FLAP_HELPER_
#include <stdio.h>

void FLAP_PrintFlags(FILE* stream)
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
