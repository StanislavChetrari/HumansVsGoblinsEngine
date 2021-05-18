#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include <acutest.h>
#include <CArray.h>
#include <PropertiesFileParser.h>

typedef void(*VerifyPropertiesFunc)(struct CArray* properties);

struct TFileTestCase
{
	const char*                           fileName;
	enum EnumPropertiesFileError          error;
	enum EnumPropertiesFileParserExpected expectedError;
	VerifyPropertiesFunc                  verifyPropertiesFunc;
};

/*
	After "numVals" there will be numVals pairs of SimpleValueType, SimpleValueData.

	VerifyProperty(properties, 0, "prop_name", pfpSimpleValue, 1, pfpIntSimpleValue, 4); // Simple value
	VerifyProperty(properties, 0, "prop_name", pfpArrayValue, 2, pfpIntSimpleValue, 4, pfpStringSimpleValue, "qwert"); // Array value
*/
void VerifyProperty(struct CArray* properties, size_t propertyIndex, const char* propertyName, enum EnumPFPValueType valueType, size_t numVals, ...)
{
	va_list valist;
	va_start(valist, numVals * 2);

	struct TPFPProperty* property = NULL;
	if (TEST_CHECK(caNoError == CArray_At(properties, propertyIndex, &property)))
	{
		TEST_CHECK(0 == strcmp(property->name, propertyName));
		if (pfpSimpleValue == valueType)
		{
			TEST_CHECK(property->type == pfpSimpleValue);

			TEST_CHECK(1 == numVals);

			enum EnumPFPSimpleValueType simple_value_type = va_arg(valist, enum EnumPFPSimpleValueType);
			if (pfpIntSimpleValue == simple_value_type)
			{
				TEST_CHECK(property->value.simpleValue.type == pfpIntSimpleValue);

				int val = va_arg(valist, int);
				TEST_CHECK(property->value.simpleValue.data.intData == val);
			}
			else if (pfpFloatSimpleValue == simple_value_type)
			{
				TEST_CHECK(property->value.simpleValue.type == pfpFloatSimpleValue);

				float val = (float)va_arg(valist, double);
				TEST_CHECK(fabsf(property->value.simpleValue.data.floatData - val) < 0.00001);
			}
			else if (pfpStringSimpleValue == simple_value_type)
			{
				TEST_CHECK(property->value.simpleValue.type == pfpStringSimpleValue);

				const char* val = va_arg(valist, const char*);
				TEST_CHECK(strcmp(property->value.simpleValue.data.stringData, val) == 0);
			}
			else
				TEST_CHECK(0);
		}
		else if (pfpArrayValue == valueType)
		{
			TEST_CHECK(numVals == property->value.arrayOfSimpleValues.count);

			for (size_t elem_i = 0; elem_i < numVals; ++elem_i)
			{
				struct TPFPSimpleValue* element = NULL;
				if (TEST_CHECK(caNoError == CArray_At(&property->value.arrayOfSimpleValues, elem_i, &element)))
				{
					enum EnumPFPSimpleValueType simple_value_type = va_arg(valist, enum EnumPFPSimpleValueType);
					if (pfpIntSimpleValue == simple_value_type)
					{
						TEST_CHECK(element->type == pfpIntSimpleValue);

						int val = va_arg(valist, int);
						TEST_CHECK(element->data.intData == val);
					}
					else if (pfpFloatSimpleValue == simple_value_type)
					{
						TEST_CHECK(element->type == pfpFloatSimpleValue);

						float val = (float)va_arg(valist, double);
						TEST_CHECK(fabsf(element->data.floatData - val) < 0.00001);
					}
					else if (pfpStringSimpleValue == simple_value_type)
					{
						TEST_CHECK(element->type == pfpStringSimpleValue);

						const char* val = va_arg(valist, const char*);
						TEST_CHECK(strcmp(element->data.stringData, val) == 0);
					}
					else
						TEST_CHECK(0);
				}
			}
		}
		else
			TEST_CHECK(0);
	}

	va_end(valist);
}

void VerifyProperties_0_txt(struct CArray* properties)
{
	TEST_CHECK(properties->count == 0);
}

void VerifyProperties_1_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 1))
	{
		// prop_name=123;
		VerifyProperty(properties, 0, "prop_name", pfpSimpleValue, 1, pfpIntSimpleValue, 123);
	}
}

void VerifyProperties_2_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 1))
	{
		// prop_name=1.234;
		VerifyProperty(properties, 0, "prop_name", pfpSimpleValue, 1, pfpFloatSimpleValue, 1.234f);
	}
}

void VerifyProperties_3_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 1))
	{
		// prop_name="qwerty";
		VerifyProperty(properties, 0, "prop_name", pfpSimpleValue, 1, pfpStringSimpleValue, "qwerty");
	}
}

void VerifyProperties_4_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 1))
	{
		// prop_name=[];
		VerifyProperty(properties, 0, "prop_name", pfpArrayValue, 0);
	}
}

void VerifyProperties_5_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 1))
	{
		// prop_name=[1];
		VerifyProperty(properties, 0, "prop_name", pfpArrayValue, 1, pfpIntSimpleValue, 1);
	}
}

void VerifyProperties_6_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 1))
	{
		// prop_name=[1, 2, 3, 4, 5];
		VerifyProperty(properties, 0, "prop_name", pfpArrayValue, 5, pfpIntSimpleValue, 1, pfpIntSimpleValue, 2, pfpIntSimpleValue, 3, pfpIntSimpleValue, 4, pfpIntSimpleValue, 5);
	}
}

void VerifyProperties_7_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 4))
	{
		// prop1=123;
		VerifyProperty(properties, 0, "prop1", pfpSimpleValue, 1, pfpIntSimpleValue, 123);

		//prop2=1.123;
		VerifyProperty(properties, 1, "prop2", pfpSimpleValue, 1, pfpFloatSimpleValue, 1.123f);

		// prop3="qwerty";
		VerifyProperty(properties, 2, "prop3", pfpSimpleValue, 1, pfpStringSimpleValue, "qwerty");

		// prop4=[1, 2, 3];
		VerifyProperty(properties, 3, "prop4", pfpArrayValue, 3, pfpIntSimpleValue, 1, pfpIntSimpleValue, 2, pfpIntSimpleValue, 3);
	}
}

void VerifyProperties_8_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 4))
	{
		// prop1=123;
		VerifyProperty(properties, 0, "prop1", pfpSimpleValue, 1, pfpIntSimpleValue, 123);

		//prop2=1.123;
		VerifyProperty(properties, 1, "prop2", pfpSimpleValue, 1, pfpFloatSimpleValue, 1.123f);

		// prop3="qwerty";
		VerifyProperty(properties, 2, "prop3", pfpSimpleValue, 1, pfpStringSimpleValue, "qwerty");

		// prop4=[1, 2, 3];
		VerifyProperty(properties, 3, "prop4", pfpArrayValue, 3, pfpIntSimpleValue, 1, pfpIntSimpleValue, 2, pfpIntSimpleValue, 3);
	}
}

void VerifyProperties_9_txt(struct CArray* properties)
{
	TEST_CHECK(properties->count == 0);
}

void VerifyProperties_10_txt(struct CArray* properties)
{
	TEST_CHECK(properties->count == 0);
}

void VerifyProperties_11_txt(struct CArray* properties)
{
	TEST_CHECK(properties->count == 0);
}

void VerifyProperties_12_txt(struct CArray* properties)
{
	TEST_CHECK(properties->count == 0);
}

void VerifyProperties_13_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 4))
	{
		// prop1=123;
		VerifyProperty(properties, 0, "prop1", pfpSimpleValue, 1, pfpIntSimpleValue, 123);

		//prop2=1.123;
		VerifyProperty(properties, 1, "prop2", pfpSimpleValue, 1, pfpFloatSimpleValue, 1.123f);

		// prop3="qwerty";
		VerifyProperty(properties, 2, "prop3", pfpSimpleValue, 1, pfpStringSimpleValue, "qwerty");

		// prop4=[1, 2, 3];
		VerifyProperty(properties, 3, "prop4", pfpArrayValue, 3, pfpIntSimpleValue, 1, pfpIntSimpleValue, 2, pfpIntSimpleValue, 3);
	}
}

void VerifyProperties_26_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 4))
	{
		// prop1=101;
		VerifyProperty(properties, 0, "prop1", pfpSimpleValue, 1, pfpIntSimpleValue, 101);
		// prop2=-102;
		VerifyProperty(properties, 1, "prop2", pfpSimpleValue, 1, pfpIntSimpleValue, -102);
		// prop3=103;
		VerifyProperty(properties, 2, "prop3", pfpSimpleValue, 1, pfpIntSimpleValue, 103);
		// prop4=+104;
		VerifyProperty(properties, 3, "prop4", pfpSimpleValue, 1, pfpIntSimpleValue, +104);
	}
}

void VerifyProperties_27_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 4))
	{
		// prop1=1.5;
		VerifyProperty(properties, 0, "prop1", pfpSimpleValue, 1, pfpFloatSimpleValue, 1.5f);
		// prop2=-2.5;
		VerifyProperty(properties, 1, "prop2", pfpSimpleValue, 1, pfpFloatSimpleValue, -2.5f);
		// prop3=3.5;
		VerifyProperty(properties, 2, "prop3", pfpSimpleValue, 1, pfpFloatSimpleValue, 3.5f);
		// prop4=+4.5;
		VerifyProperty(properties, 3, "prop4", pfpSimpleValue, 1, pfpFloatSimpleValue, +4.5f);
	}
}

void VerifyProperties_28_txt(struct CArray* properties)
{
	if (TEST_CHECK(properties->count == 4))
	{
		// prop1="str1";
		VerifyProperty(properties, 0, "prop1", pfpSimpleValue, 1, pfpStringSimpleValue, "str1");
		// prop2="str2";
		VerifyProperty(properties, 1, "prop2", pfpSimpleValue, 1, pfpStringSimpleValue, "str2");
		// prop3="str3";
		VerifyProperty(properties, 2, "prop3", pfpSimpleValue, 1, pfpStringSimpleValue, "str3");
		// prop4="str4";
		VerifyProperty(properties, 3, "prop4", pfpSimpleValue, 1, pfpStringSimpleValue, "str4");
	}
}

static const struct TFileTestCase filesTestCase[] =
{
	{"0.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_0_txt},
	{"1.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_1_txt},
	{"2.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_2_txt},
	{"3.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_3_txt},
	{"4.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_4_txt},
	{"5.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_5_txt},
	{"6.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_6_txt},
	{"7.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_7_txt},
	{"8.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_8_txt},
	{"9.txt",  pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_9_txt},
	{"10.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_10_txt},
	{"11.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_11_txt},
	{"12.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_12_txt},
	{"13.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_13_txt},
	{"14.txt", pfpParseError, pfpExpectedIdentifier,                NULL},
	{"15.txt", pfpParseError, pfpExpectedEqualSign,                 NULL},
	{"16.txt", pfpParseError, pfpExpectedSimpleOrArrayValue,        NULL},
	{"17.txt", pfpParseError, pfpExpectedSemicolon,                 NULL},
	{"18.txt", pfpParseError, pfpExpectedDigitAfterDecimalPoint,    NULL},
	{"19.txt", pfpParseError, pfpExpectedStringEndingQuotationMark, NULL},
	{"20.txt", pfpParseError, pfpExpectedEndOfArray,                NULL},
	{"21.txt", pfpParseError, pfpExpectedEndOfArray,                NULL},
	{"22.txt", pfpParseError, pfpExpectedEndOfArray,                NULL},
	{"23.txt", pfpParseError, pfpExpectedEndOfArray,                NULL},
	{"24.txt", pfpParseError, pfpExpectedSimpleValue,               NULL},
	{"25.txt", pfpParseError, pfpExpectedEndOfArray,                NULL},
	{"26.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_26_txt},
	{"27.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_27_txt},
	{"28.txt", pfpNoError,    pfpExpectedUndefined,                 VerifyProperties_28_txt}
};

void Test_PropertiesFileParser_FilesTestCase(void)
{
	size_t count_files = sizeof(filesTestCase) / sizeof(struct TFileTestCase);

	for (size_t i = 0; i < count_files; ++i)
	{
		char*  buffer      = NULL;
		size_t buffer_size = 0;

		char file_path[128] = {'\0'};
		strcat(file_path, "Test_PropertiesFileParser_TestFiles/");
		strcat(file_path, filesTestCase[i].fileName);

		TEST_CASE(filesTestCase[i].fileName);

		FILE* file_in = fopen(file_path, "rb");

		TEST_CHECK(NULL != file_in);

		if (file_in)
		{
			fseek(file_in, 0, SEEK_END);
			size_t file_size = ftell(file_in);
			fseek(file_in, 0, SEEK_SET);

			buffer = (char*)malloc(file_size + 1);

			TEST_CHECK(NULL != buffer);

			if (buffer)
			{
				if (fread(buffer, sizeof(char), file_size, file_in) == file_size)
				{
					buffer[file_size] = '\0';
					buffer_size = file_size + 1;
				}
				else
				{
					free(buffer);
					buffer = NULL;
				}
			}

			fclose(file_in);
		}

		TEST_CHECK(NULL != buffer);

		if (buffer)
		{
			struct CArray properties;
			CArray_InitZero(&properties);
			enum EnumCArrayError ca_error = CArray_Create(&properties, 2, sizeof(struct TPFPProperty), TPFPProperty_Clear);
			if (TEST_CHECK(caNoError == ca_error))
			{
				union TPFPErrorData error_data;

				enum EnumPFPError pfp_error = PFP_ParseFromMemory(buffer, &properties, &error_data);

				if (pfpNoError == filesTestCase[i].error)
				{
					TEST_CHECK(pfpNoError == pfp_error);

					if (filesTestCase[i].verifyPropertiesFunc)
						filesTestCase[i].verifyPropertiesFunc(&properties);
				}
				else if (pfpParseError == filesTestCase[i].error)
				{
					TEST_CHECK(pfpNoError != pfp_error);
					TEST_CHECK(pfpParseError == pfp_error);
					TEST_CHECK(filesTestCase[i].expectedError == error_data.parseData.expected);
				}

				CArray_Clear(&properties);
			}

			free(buffer);
		}
	}
}

TEST_LIST =
{
	{"Test_PropertiesFileParser_FilesTestCase", Test_PropertiesFileParser_FilesTestCase},
	{NULL, NULL}
};