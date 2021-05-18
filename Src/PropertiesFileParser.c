#include "PropertiesFileParser.h"

#include <stdio.h>

void TPFPProperty_InitZero(void* property)
{
	struct TPFPProperty* t_property = property;

	if (t_property)
	{
		t_property->offset = 0;
		t_property->name   = NULL;
		t_property->type   = pfpUndefinedValue;
	}
}

void TPFPProperty_Clear(void* property)
{
	struct TPFPProperty* t_property = property;

	if (t_property)
	{
		if (t_property->name)
			free(t_property->name);

		if(pfpArrayValue == t_property->type)
			CArray_Clear(&t_property->value.arrayOfSimpleValues);

		TPFPProperty_InitZero(property);
	}
}

void CreateError(enum EnumPFPError* error, enum EnumPFPError errorValue)
{
	if (error)
	{
		*error = errorValue;
	}
}

void CreateParseError(enum EnumPFPError* error, union TPFPErrorData* errorData, enum EnumPFPExpected expectedEnum, size_t offset)
{
	if (error)
		*error = pfpParseError;

	if (errorData)
	{
		errorData->parseData.expected = expectedEnum;
		errorData->parseData.offset   = offset;
	}
}

void CreateCArrayError(enum EnumPFPError* error, union TPFPErrorData* errorData, enum EnumCArrayError caError)
{
	if (error)
		*error = pfpCArrayError;

	if (errorData)
	{
		errorData->carrayData.error = caError;
	}
}

/*
	It can return 0: fail, >0: success.
*/
int IntFromString(const char* str, size_t strLength, int* val)
{
	if (!str || !val || 0 == strLength)
		return 0;

	int t_val = 0;
	int t_sign = 1; // 1 for + and -1 for -

	size_t i = 0;

	if ('+' == *str)
	{
		t_sign = 1;
		i++;
	}
	else if ('-' == *str)
	{
		t_sign = -1;
		i++;
	}

	for (; i < strLength; ++i)
	{
		if ('0' <= str[i] && '9' >= str[i])
			t_val = t_val * 10 + (str[i] - '0');
		else
			return 0;
	}

	*val = t_val * t_sign;
	return 1;
}

/*
	It can return 0: fail, >0: success.
*/
int FloatFromString(const char* str, size_t strLength, float* val)
{
	if (!str || !val || 0 == strLength)
		return 0;

	float t_val = 0.0f;
	float t_divider = 1.0f;
	float t_sign = 1.0f;

	size_t i = 0;

	if ('+' == str[0])
	{
		t_sign = 1.0f;
		i++;
	}
	else if ('-' == str[0])
	{
		t_sign = -1.0f;
		i++;
	}

	int found_point = 0;

	for (; i < strLength; ++i)
	{
		if ('0' <= str[i] && '9' >= str[i])
		{
			if (found_point)
				t_divider *= 10.0f;
			t_val = t_val * 10.0f + (str[i] - '0');
		}
		else if ('.' == str[i] && !found_point)
			found_point = 1;
		else
			return 0;
	}

	*val = t_sign * (t_val / t_divider);
	return 1;
}

int ParseWhitespacesAndComments(const char* buffer, char** curBuff, enum EnumPFPError* error, union TPFPErrorData* errorData)
{
	if (!buffer || !curBuff || !(*curBuff))
		return 0;

	int success = 1;

	char* cur_buff = *curBuff;

	while ('\0' != *cur_buff)
	{
		// Comment
		if ('/' == *cur_buff)
		{
			cur_buff++;

			// One line comment
			if ('/' == *cur_buff)
			{
				while ('\0' != *cur_buff)
				{
					if ('\r' == *cur_buff || '\n' == *cur_buff)
					{
						cur_buff++;
						break;
					}
					else
						cur_buff++;
				}
			}
			// Multiline comment
			else if ('*' == *cur_buff)
			{
				int comment_success = 0;

				int found_asterisk = 0;
				while ('\0' != *cur_buff)
				{
					if (found_asterisk)
					{
						if ('/' == *cur_buff)
						{
							cur_buff++;
							comment_success = 1;
							break;
						}
						else
							found_asterisk = 0;
					}
					else
					{
						if ('*' == *cur_buff)
							found_asterisk = 1;
					}

					cur_buff++;
				}

				if (!comment_success)
				{
					CreateParseError(error, errorData, pfpExpectedEndOfMultilineComment, cur_buff - buffer);
					success = 0;
				}
			}
			else
				break;
		}
		// Whitespace
		else if (' ' == *cur_buff || '\r' == *cur_buff || '\n' == *cur_buff || '\t' == *cur_buff)
			cur_buff++;
		else
			break;
	}

	*curBuff = cur_buff;

	return success;
}

/*
	It can return 0: fail, >0: success.
*/
int ParseIdentifier(const char* buffer, char** curBuff, size_t* beginIndex, size_t* endIndex, enum EnumPFPError* error, union TPFPErrorData* errorData)
{
	if (!buffer || !curBuff || !(*curBuff) || !beginIndex || !endIndex)
		return 0;

	int success = 0;

	char* cur_buff = *curBuff;

	size_t begin_index = 0;
	size_t end_index   = 0;

	// Parse first character of IDENTIFIER
	if ('_' == *cur_buff || ('a' <= *cur_buff && 'z' >= *cur_buff) || ('A' <= *cur_buff && 'Z' >= *cur_buff))
	{
		begin_index = cur_buff - buffer;
		end_index   = cur_buff - buffer;
		cur_buff++;

		// Parse the rest of the characters of IDENTIFIER
		while ('\0' != *cur_buff)
		{
			if ('_' == *cur_buff || ('a' <= *cur_buff && 'z' >= *cur_buff) || ('A' <= *cur_buff && 'Z' >= *cur_buff) || ('0' <= *cur_buff && '9' >= *cur_buff))
			{
				cur_buff++;
			}
			else
			{
				end_index = (cur_buff - 1) - buffer;
				break;
			}
		}

		success = 1;
	}
	
	if (success)
	{
		*beginIndex = begin_index;
		*endIndex   = end_index;

		*curBuff = cur_buff;
	}

	return success;
}

/*
	It can return 0: fail, >0: success.
*/
int ParseSimpleValue(const char* buffer, char** curBuff, struct TPFPSimpleValue* simpleValue, enum EnumPFPError* error, union TPFPErrorData* errorData)
{
	if (!buffer || !curBuff || !(*curBuff) || !simpleValue)
		return 0;

	int success = 0;

	char* cur_buff = *curBuff;

	enum EnumPFPSimpleValueType value_type;
	size_t                      data_begin_index = 0;
	size_t                      data_end_index   = 0;

	// Check first character of value
	if ('-' == *cur_buff || '+' == *cur_buff || ('0' <= *cur_buff && '9' >= *cur_buff)) // simple_value
	{
		data_begin_index = cur_buff - buffer;
		value_type       = pfpIntSimpleValue;

		// Parse sign
		if ('-' == *cur_buff || '+' == *cur_buff)
			cur_buff++;

		int is_float = 0;

		// Parse the rest digits
		while ('0' <= *cur_buff && '9' >= *cur_buff)
			cur_buff++;

		// Check if it is float
		if ('.' == *cur_buff)
		{
			value_type = pfpFloatSimpleValue;

			// Check if there is a digit after decimal dot
			cur_buff++;
			if ('0' <= *cur_buff && '9' >= *cur_buff)
			{
				// Skip other digits and find the last one
				while ('\0' != *cur_buff)
				{
					if ('0' <= *cur_buff && '9' >= *cur_buff)
						cur_buff++;
					else
					{
						data_end_index = (cur_buff - 1) - buffer;
						success = 1;
						break;
					}
				}
			}
			else
			{
				CreateParseError(error, errorData, pfpExpectedDigitAfterDecimalPoint, cur_buff - buffer);
			}
		}
		else
		{
			data_end_index = (cur_buff - 1) - buffer;
			success = 1;
		}
	}
	else if ('\"' == *cur_buff) // String
	{
		data_begin_index = cur_buff - buffer;
		value_type       = pfpStringSimpleValue;
		cur_buff++;

		// Find string ending
		while ('\0' != *cur_buff)
		{
			if ('\"' == *cur_buff)
			{
				cur_buff++;
				data_end_index = (cur_buff - 1) - buffer;
				success = 1;
				break;
			}
			else
				cur_buff++;
		}
		
		if (!success)
		{
			CreateParseError(error, errorData, pfpExpectedStringEndingQuotationMark, cur_buff - buffer);
		}
	}

	if (success)
	{
		simpleValue->offset = data_begin_index;
		simpleValue->type   = value_type;
		if (pfpIntSimpleValue == value_type)
		{
			int val = 0;
			if (IntFromString(buffer + data_begin_index, data_end_index - data_begin_index + 1, &val))
				simpleValue->data.intData = val;
		}
		else if (pfpFloatSimpleValue == value_type)
		{
			float val = 0.0f;
			if (FloatFromString(buffer + data_begin_index, data_end_index - data_begin_index + 1, &val))
				simpleValue->data.floatData = val;
		}
		else if (pfpStringSimpleValue == value_type)
		{
			size_t val_length = data_end_index - data_begin_index - 1;
			char* val = malloc(val_length + 1);
			if (val)
			{
				memcpy(val, buffer + data_begin_index + 1, val_length);
				val[val_length] = '\0';

				simpleValue->data.stringData = val;
			}
			else
				CreateError(error, pfpMallocError);
		}
		
		*curBuff = cur_buff;
	}

	return success;
}

/*
	It can return 0: fail, >0: success.
*/
int ParseArrayValue(const char* buffer, char** curBuff, struct CArray* arrayValue, enum EnumPFPError* error, union TPFPErrorData* errorData)
{
	if (!buffer || !curBuff || !(*curBuff) || !arrayValue)
		return 0;

	int success = 0;

	char* cur_buff = *curBuff;

	if ('[' == *cur_buff)
	{
		cur_buff++;

		struct TPFPSimpleValue simple_value;

		if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
		{
			if (']' == *cur_buff)
			{
				cur_buff++;

				success = 1;
			}
			else if (ParseSimpleValue(buffer, &cur_buff, &simple_value, error, errorData))
			{
				enum EnumCArrayError ca_error = CArray_PushBack(arrayValue, (void*)&simple_value);
				if (caNoError == ca_error)
				{
					if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
					{
						while (',' == *cur_buff)
						{
							cur_buff++;

							if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
							{
								if (ParseSimpleValue(buffer, &cur_buff, &simple_value, error, errorData))
								{
									ca_error = CArray_PushBack(arrayValue, (void*)&simple_value);
									if (caNoError != ca_error)
									{
										CreateCArrayError(error, errorData, ca_error);
										break;
									}
								}
								else
								{
									CreateParseError(error, errorData, pfpExpectedSimpleValue, cur_buff - buffer);
									break;
								}

								ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData);
							}
							else
							{
								break;
							}
						}

						if (pfpNoError == *error)
						{
							if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
							{
								if (']' == *cur_buff)
								{
									cur_buff++;

									success = 1;
								}
								else
								{
									CreateParseError(error, errorData, pfpExpectedEndOfArray, cur_buff - buffer);
								}
							}
						}
					}
				}
				else
				{
					CreateCArrayError(error, errorData, ca_error);
				}
			}
			else
			{
				CreateParseError(error, errorData, pfpExpectedEndOfArray, cur_buff - buffer);
			}
		}
	}

	if(success)
		*curBuff = cur_buff;

	return success;
}

/*
	It can return 0: fail, >0: success.
*/
int ParseProperty(const char* buffer, char** curBuff, struct CArray* properties, enum EnumPFPError* error, union TPFPErrorData* errorData)
{
	if (!buffer || !curBuff || !(*curBuff) || !properties)
		return 0;

	int success = 0;

	struct TPFPProperty t_property;
	TPFPProperty_InitZero(&t_property);
	size_t name_begin_index = 0;
	size_t name_end_index   = 0;

	char* cur_buff = (char*)*curBuff;

	// Parse IDENTIFIER
	t_property.offset = cur_buff - buffer;
	if (ParseIdentifier(buffer, &cur_buff, &name_begin_index, &name_end_index, error, errorData))
	{
		if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
		{
			// Parse "="
			if ('=' == *cur_buff)
			{
				cur_buff++;

				if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
				{
					// Parse simple_value
					if (ParseSimpleValue(buffer, &cur_buff, &t_property.value.simpleValue, error, errorData))
					{
						t_property.type = pfpSimpleValue;
					}
					else
					{
						if (pfpNoError == *error)
						{
							// Parse array_value
							CArray_InitZero(&t_property.value.arrayOfSimpleValues);
							enum EnumCArrayError ca_error = CArray_Create(&t_property.value.arrayOfSimpleValues, 2, sizeof(struct TPFPSimpleValue), NULL);
							if (caNoError == ca_error)
							{
								t_property.type = pfpArrayValue;
								if (!ParseArrayValue(buffer, &cur_buff, &t_property.value.arrayOfSimpleValues, error, errorData))
								{
									CArray_Clear(&t_property.value.arrayOfSimpleValues);
									if (pfpNoError == *error)
										CreateParseError(error, errorData, pfpExpectedSimpleOrArrayValue, cur_buff - buffer);
								}
							}
							else
								CreateCArrayError(error, errorData, ca_error);
						}
					}

					if (pfpNoError == *error)
					{
						if (ParseWhitespacesAndComments(buffer, &cur_buff, error, errorData))
						{
							// Parse semicolon
							if (';' == *cur_buff)
							{
								cur_buff++;

								success = 1;
							}
							else
							{
								CreateParseError(error, errorData, pfpExpectedSemicolon, cur_buff - buffer);
							}
						}
					}
				}
			}
			else
			{
				CreateParseError(error, errorData, pfpExpectedEqualSign, cur_buff - buffer);
			}
		}
	}
	else
	{
		CreateParseError(error, errorData, pfpExpectedIdentifier, cur_buff - buffer);
	}

	if (success)
	{
		size_t name_length = name_end_index - name_begin_index + 1;
		t_property.name = malloc(name_length + 1);
		if (t_property.name)
		{
			memcpy(t_property.name, buffer + name_begin_index, name_length);
			t_property.name[name_length] = '\0';

			enum EnumCArrayError ca_error = CArray_PushBack(properties, (void*)&t_property);
			if (caNoError == ca_error)
				*curBuff = cur_buff;
			else
			{
				success = 0;
				CreateCArrayError(error, errorData, ca_error);
			}
		}
		else
		{
			success = 0;
			CreateError(error, pfpMallocError);
		}
	}

	if (!success)
	{
		TPFPProperty_Clear(&t_property);
	}

	return success;
}

enum EnumPFPError PFP_ParseFromMemory(const char* buffer, struct CArray* arrayOfTPFPProperties, union TPFPErrorData* errorData)
{
	if (!buffer)
		return pfpBufferArgIsNULLError;
	if (!arrayOfTPFPProperties)
		return pfpArrayOfTPFPPropertiesArgIsNULL;

	enum EnumPFPError error = pfpNoError;

	char* cur_buff = (char*)buffer;

	while('\0' != *cur_buff)
	{
		if (ParseWhitespacesAndComments(buffer, &cur_buff, &error, errorData) && ('\0' != *cur_buff))
		{
			if (!ParseProperty(buffer, &cur_buff, arrayOfTPFPProperties, &error, errorData))
				break;
		}
		else
			break;
	}

	return error;
}

static const char* parseExpectedStrs[] =
{
	/* pfpUndefined */ "Unknown error",

	/* pfpExpectedIdentifier */                "Expected identifier.",
	/* pfpExpectedEqualSign */                 "Expected equal sign '='.",
	/* pfpExpectedSimpleOrArrayValue */        "Expected simple value or array value.",
	/* pfpExpectedSimpleValue */               "Expected simple value.",
	/* pfpExpectedSemicolon */                 "Expected semicolon ';'.",
	/* pfpExpectedEndOfArray */                "Expected end of array ']'.",
	/* pfpExpectedDigitAfterDecimalPoint */    "Expected at least one digit after decimal point.",
	/* pfpExpectedStringEndingQuotationMark */ "Expected string ending quotation mark '\"'.",
	/* pfpExpectedEndOfMultilineComment */     "Expected end of multiline comment \"*/\"."
};

static const char* errorNotFoundStr = "No string for this error.";

static const char* noErrorStr                             = "No error.";
static const char* parseErrorStr                          = "Parse error.";
static const char* carrayErrorStr                         = "CArray error.";
static const char* bufferArgIsNULLErrorStr                = "Buffer argument is NULL.";
static const char* arrayOfTPFPPropertiesArgIsNULLErrorStr = "Array of TPFPProperties argument is NULL.";
static const char* mallocErrorStr                         = "Malloc function failed.";

void TPFP_GetErrorStr(enum EnumPFPError error, union TPFPErrorData* errorData, char* errorStr)
{
	if (!error || !errorStr)
		return;

	if (pfpNoError == error)
	{
		strcpy(errorStr, noErrorStr);
	}
	else if (pfpParseError == error)
	{
		if (errorData)
		{
			size_t expected_strs_count = sizeof(parseExpectedStrs) / sizeof(const char*);

			if ((size_t)errorData->parseData.expected < expected_strs_count)
			{
				strcat(errorStr, parseErrorStr);
				strcat(errorStr, " ");
				strcat(errorStr, parseExpectedStrs[(size_t)errorData->parseData.expected]);

				char offset_str[128] = { '\0' };
				sprintf(offset_str, " Offset %zd.", (size_t)errorData->parseData.offset);
				strcat(errorStr, offset_str);
			}
			else
				strcpy(errorStr, errorNotFoundStr);
		}
		else
			strcpy(errorStr, parseErrorStr);
	}
	else if (pfpCArrayError == error)
	{
		strcpy(errorStr, carrayErrorStr);
		strcpy(errorStr, " ");
		CArray_GetErrorStr(errorData->carrayData.error, errorStr);
	}
	else if (pfpBufferArgIsNULLError == error)
	{
		strcpy(errorStr, bufferArgIsNULLErrorStr);
	}
	else if (pfpArrayOfTPFPPropertiesArgIsNULL == error)
	{
		strcpy(errorStr, arrayOfTPFPPropertiesArgIsNULLErrorStr);
	}
	else if (pfpMallocError == error)
	{
		strcpy(errorStr, mallocErrorStr);
	}
}

size_t TPFP_GetErrorStrSize(enum EnumPFPError error, union TPFPErrorData* errorData)
{
	if (!error)
		return 0;

	if (pfpNoError == error)
	{
		return (strlen(noErrorStr) + 1);
	}
	else if (pfpParseError == error)
	{
		if (errorData)
		{
			size_t expected_strs_count = sizeof(parseExpectedStrs) / sizeof(const char*);

			if ((size_t)errorData->parseData.expected < expected_strs_count)
			{
				size_t size = strlen(parseErrorStr) + strlen(" ");
				size += strlen(parseExpectedStrs[(size_t)errorData->parseData.expected]) + strlen(" ");
				
				char offset_str[128] = { '\0' };
				sprintf(offset_str, " Offset %zd.", (size_t)errorData->parseData.offset);
				size += strlen(offset_str);

				size += 1;

				return size;
			}
			else
				return (strlen(errorNotFoundStr) + 1);
		}
		else
			return strlen(parseErrorStr + 1);
	}
	else if (pfpCArrayError == error)
	{
		size_t size = strlen(carrayErrorStr) + strlen(" ");
		size += CArray_GetErrorStrSize(errorData->carrayData.error);
		size += 1;

		return size;
	}
	else if (pfpBufferArgIsNULLError == error)
	{
		return (strlen(bufferArgIsNULLErrorStr) + 1);
	}
	else if (pfpArrayOfTPFPPropertiesArgIsNULL == error)
	{
		return (strlen(arrayOfTPFPPropertiesArgIsNULLErrorStr) + 1);
	}
	else if (pfpMallocError == error)
	{
		return (strlen(mallocErrorStr) + 1);
	}

	return 0;
}