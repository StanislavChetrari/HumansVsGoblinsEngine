#ifndef _PROPERTIES_FILE_PARSER_H_
#define _PROPERTIES_FILE_PARSER_H_

#include <string.h>

#include <EngineAPI.h>

#include <CArray.h>

/*
	PFP is abreviation of PropertiesFileParser
*/

enum EnumPFPValueType
{
	pfpUndefinedValue = 0,
	pfpSimpleValue    = 1,
	pfpArrayValue     = 2
};

enum EnumPFPSimpleValueType
{
	pfpIntSimpleValue       = 1,
	pfpFloatSimpleValue     = 2,
	pfpStringSimpleValue    = 3
};

struct TPFPSimpleValue
{
	size_t offset;
	enum EnumPFPSimpleValueType type;

	union
	{
		int   intData;
		float floatData;
		char* stringData;
	} data;
};

struct TPFPProperty
{
	size_t offset;
	char* name;
	
	enum EnumPFPValueType type;
	union
	{
		struct TPFPSimpleValue simpleValue;
		struct CArray          arrayOfSimpleValues;
	} value;
};

DECL_API void TPFPProperty_InitZero(void* property);
DECL_API void TPFPProperty_Clear(void* property);

/*
	Main error types of PropertiesFileParser.
*/
enum EnumPFPError
{
	// There were no errors.
	pfpNoError = 0,

	// Error occured when parsing the data. Look at what was expected and at what offset.
	pfpParseError = 1,
	// Error occured in the CArray function. Look at CArray.h for more details.
	pfpCArrayError = 2,

	// Error, "buffer" argument in PropertiesFileParser_ParseFromMemory() is NULL.
	pfpBufferArgIsNULLError = 3,
	pfpArrayOfTPFPPropertiesArgIsNULL = 4,

	// Error, failed to allocate memory with malloc.
	pfpMallocError = 5
};

/*
	When occures a pfParseError one of these is set for expected.
*/
enum EnumPFPExpected
{
	pfpExpectedUndefined = 0,

	pfpExpectedIdentifier                 = 1,
	pfpExpectedEqualSign                  = 2,
	pfpExpectedSimpleOrArrayValue         = 3,
	pfpExpectedSimpleValue                = 4,
	pfpExpectedSemicolon                  = 5,
	pfpExpectedEndOfArray                 = 6,
	pfpExpectedDigitAfterDecimalPoint     = 7,
	pfpExpectedStringEndingQuotationMark  = 8,
	pfpExpectedEndOfMultilineComment      = 9
};

/*
	This structure contains the error type and data for each error type.
*/
union TPFPErrorData
{
	// When error is pfParseError
	struct
	{
		enum EnumPFPExpected expected;
		size_t               offset;
	} parseData;
	// When error is pfErrorCArray
	struct
	{
		enum EnumCArrayError error;
	} carrayData;
};

/*
	Main function to parse data.

	Arguments:
		buffer             : is pointer to string with data to parse
		propertiesFileFuncs: callbacks to process values. Can be NULL to not call any callback.
		error              : the error data argument.
		userData           : void pointer to a user data that will be passed to process callback from "propertiesFileFuncs" argument. Can be NULL.

	Returns:
		pfpNoError
		pfpParseError	
		pfpCArrayError
		pfpBufferArgIsNULLError
		pfpArrayOfTPFPPropertiesArgIsNULL
		pfpMallocError
*/
DECL_API enum EnumPFPError PFP_ParseFromMemory(const char* buffer, struct CArray* arrayOfTPFPProperties, union TPFPErrorData* errorData);

/*
	Appends to errorStr the string text that coresponds to error.
	Make shure there is enough space left!
	If errorStr is NULL the function will not do anything.
*/
DECL_API void TPFP_GetErrorStr(enum EnumPFPError error, union TPFPErrorData* errorData, char* errorStr);

/*
	Returns size in bytes that is needed to store error string with TPFP_GetErrorStr(). It includes NULL terminator.
*/
DECL_API size_t TPFP_GetErrorStrSize(enum EnumPFPError error, union TPFPErrorData* errorData);

#endif /* _PROPERTIES_FILE_PARSER_H_ */