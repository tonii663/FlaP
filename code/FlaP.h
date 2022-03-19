#ifndef FLAP_H_
#define FLAP_H_

#include <stdint.h>
#include <stdbool.h>

int32_t* FLAP_Int(char* name, char* desc, int defValue);

int64_t* FLAP_Int64(char* name, char* desc, int64_t defValue);

uint64_t* FLAP_UInt64(char* name, char* desc, uint64_t defValue);

bool* FLAP_Bool(char* name, char* desc, bool defValue);

char** FLAP_Str(char* name, char* desc, char* defValue);

void FLAP_DefInt(int32_t* var, char* name, char* desc, int defValue);
void FLAP_DefInt64(int64_t* var, char* name, char* desc, int64_t defValue);
void FLAP_DefUInt64(uint64_t* var, char* name, char* desc, uint64_t defValue);
void FLAP_DefStr(char** var, char* name, char* desc, char* defValue);
void FLAP_DefBool(bool* var, char* name, char* desc, bool defValue);

bool FLAP_Parse(int argCount, char** arguments);

#ifndef FLAP_NO_STDIO_
void FLAP_PrintFlags();
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

typedef struct _Flag
{
	int Type;
	char* Name;
	char* Description;

	FlagField Value;
	FlagField Default;
}Flag;

#ifndef FLAG_CAPACITY
#define FLAG_CAPACITY 256
#endif

static Flag Flags[FLAG_CAPACITY];
static int FlagCount = 0;

static Flag*
FlagCreate(int type, char* name, char* desc)
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

void FLAP_DefInt64(int64_t* var, char* name, char* desc, int64_t defValue)
{
	Flag* flag = FlagCreate(FLAG_DEF_INT64, name, desc);
	flag->Default = defValue;
	*(int64_t**)&flag->Value = var;
	*(int64_t*)flag->Value = defValue;
}

void FLAP_DefUInt64(uint64_t* var, char* name, char* desc, uint64_t defValue)
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


static bool IsTrue(char* str)
{
	bool result = false;
	int len = strlen(str);
	
	if(StringCompare(str, len,
					 "true", strlen("true")))
	{
		result = true;
	}
	else if(StringCompare(str, len,
						  "True", strlen("True")))
	{
		result = true;
	}
	else if(StringCompare(str, len,
						  "T", strlen("T")))
	{
		result = true;
	}
	else if(StringCompare(str, len,
						  "t", strlen("t")))
	{
		result = true;
	}

	return result;
}


static bool IsFalse(char* str)
{
	bool result = false;
	int len = strlen(str);
	
	if(StringCompare(str, len,
					 "false", strlen("false")))
	{
		result = true;
	}
	else if(StringCompare(str, len,
						  "False", strlen("False")))
	{
		result = true;
	}
	else if(StringCompare(str, len,
						  "F", strlen("F")))
	{
		result = true;
	}
	else if(StringCompare(str, len,
						  "f", strlen("f")))
	{
		result = true;
	}

	return result;
}



static void StringSplitAt(char* str, char c, char** first, char** last)
{
	*first = str;
	
	while(str && str[0] != '\0' && str[0] != c)
		str++;

	if(str[0] != '\0')
	{
		*last = ++str;
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
		
		StringSplitAt(args[*current], '=', &first, &last);

		if(last)
		{
			*arg = ++first; // Remove the '-' or '/'
			*argLength = (int)(last - first) - 1;

			*param = last;
			*paramLength = strlen(*arg) - (*argLength + 1);
			*current++;
		}
		else
		{
			*arg = args[(*current)++]+1;
			*argLength = strlen(*arg);
			if(args[*current])
			{
				*param = args[(*current)];
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
			
			char* flag = 0;
			int flagLen = 0;
			
			char* param = 0;
			int paramLen = 0;
			
			bool success = GetArgAndParam(arguments, &argIndex,
										   &flag, &flagLen,
										   &param, &paramLen);
			
			for(int flagIndex = 0; flagIndex < FlagCount; flagIndex++)
			{
				
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
								if(param && IsInt(param))
								{
									int32_t value = strtol(param, 0, 0);
									Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;
							
							case(FLAG_DEF_INT):
							{
								if(param && IsInt(param))
								{
									int32_t value = strtol(param, 0, 0);
									*(int32_t*)Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_UINT64):
							{
								if(param && IsInt(param))
								{
									uint64_t value = strtol(param, 0, 0);
									Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_DEF_UINT64):
							{
								if(param && IsInt(param))
								{
									uint64_t value = strtol(param, 0, 0);
									*(uint64_t*)Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
 							}break;

							case(FLAG_INT64):
							{
								if(param && IsInt(param))
								{
									int64_t value = strtol(param, 0, 0);
									Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_DEF_INT64):
							{
								if(param && IsInt(param))
								{
									int64_t value = strtol(param, 0, 0);
									*(int64_t*)Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;

							
							case(FLAG_FLOAT):
							{
								if(param && IsFloat(param))
								{
									float value = (float)atof(param);
									*(float*)&Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;

							case(FLAG_DEF_FLOAT):
							{
								if(param && IsFloat(param))
								{
									float value = (float)atof(param);
									*(float*)Flags[flagIndex].Value = value;
								}
								else
								{
									error = true;
								}
							}break;
							
							case(FLAG_BOOL):
							{
								
								if(success && IsTrue(param))
								{
									Flags[flagIndex].Value = true;
								}
								else if(success && IsFalse(param))
								{
									Flags[flagIndex].Value = false;
								}
								else
								{
									Flags[flagIndex].Value = true;
									argIndex--;
								}
								
							}break;

							case(FLAG_DEF_BOOL):
							{
								if(success && IsTrue(param))
								{
									*(bool*)Flags[flagIndex].Value = true;
								}
								else if(success && IsFalse(param))
								{
									*(bool*)Flags[flagIndex].Value = false;
								}
								else
								{
									*(bool*)Flags[flagIndex].Value = true;
									argIndex--;
								}
								
							}break;

							case(FLAG_STR):
							{
								char* value = param;
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
								char* value = param;
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


#ifndef FLAP_NO_STDIO_
#include <stdio.h>


#define BUFFER_SIZE 4096
char Buffer[BUFFER_SIZE];
int BufferIndex;

static void FLAP_DisplayValue(int type, FlagField field)
{
	switch(type)
	{
		case(FLAG_DEF_INT):
		case(FLAG_INT):
		{
			BufferIndex += sprintf_s(Buffer + BufferIndex,
									BUFFER_SIZE - BufferIndex,
									"%d", (int)field);
		}break;

		case(FLAG_DEF_FLOAT):
		case(FLAG_FLOAT):
		{
			BufferIndex += sprintf_s(Buffer + BufferIndex,
								  BUFFER_SIZE - BufferIndex,
								  "%f", *(float*)&field);
		}break;

		case(FLAG_DEF_STR):
		case(FLAG_STR):
		{
			BufferIndex += sprintf_s(Buffer + BufferIndex,
									BUFFER_SIZE - BufferIndex,
									"%s", (char*)field);
		}break;

		case(FLAG_DEF_BOOL):
		case(FLAG_BOOL):
		{
			char* value = field ? "true" : "false";
			BufferIndex += sprintf_s(Buffer + BufferIndex,
									BUFFER_SIZE - BufferIndex, "%s", value);
		}break;
		
		default: break;
	}
}


void FLAP_PrintFlags()
{
	BufferIndex += sprintf_s(Buffer + BufferIndex, BUFFER_SIZE - BufferIndex,
							"<OPTIONS>:\n");

	for(int i = 0; i < FlagCount; i++)
	{
		Flag* flag = &Flags[i];

		BufferIndex += sprintf_s(Buffer + BufferIndex,
								BUFFER_SIZE - BufferIndex, "\t-%s\n",
								flag->Name);
		BufferIndex += sprintf_s(Buffer + BufferIndex,
								BUFFER_SIZE - BufferIndex,
								"\t   %s\n", flag->Description);
		BufferIndex += sprintf_s(Buffer + BufferIndex,
								BUFFER_SIZE - BufferIndex,
								"\t      Default value(");

		FLAP_DisplayValue(flag->Type, flag->Default);
		BufferIndex += sprintf_s(Buffer + BufferIndex,
								BUFFER_SIZE - BufferIndex, ")\n");
	}

	printf("%s\n", Buffer);
}

#endif // FLAP_NO_STDIO_
#endif // FLAP_IMPLEMENTATION_
#endif // FLAP_H_

