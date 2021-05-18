#include <stdio.h>

#include <acutest.h>
#include <CArray.h>

void Test_CArray_Create_NULLArray(void)
{
	enum EnumCArrayError error = CArray_Create(NULL, 4, sizeof(int), NULL);
	TEST_CHECK(caErrorArrayArgIsNULL == error);
}

void Test_CArray_Create_0Capacity(void)
{
	struct CArray array;
	CArray_InitZero(&array);
	enum EnumCArrayError error = CArray_Create(&array, 0, sizeof(int), NULL);
	TEST_CHECK(caNoError == error);
	CArray_Clear(&array);
}

void Test_CArray_Create_0ElementSize(void)
{
	struct CArray array;
	CArray_InitZero(&array);
	enum EnumCArrayError error = CArray_Create(&array, 4, 0, NULL);
	TEST_CHECK(caErrorElementSizeArgIs0 == error);
	CArray_Clear(&array);
}

void Test_CArray_Create_Malloc(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity     = ((size_t)-1 / element_size);

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);
	TEST_CHECK(caErrorMallocFailed == error);
	CArray_Clear(&array);
}

void Test_CArray_Create_NoError(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity     = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);
	
	if(TEST_CHECK(caNoError == error))
	{ 
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);
	}

	CArray_Clear(&array);
}

void Test_CArray_At_NULLArray(void)
{
	int* val_ptr = NULL;
	enum EnumCArrayError error = CArray_At(NULL, 0, (void**)&val_ptr);

	TEST_CHECK(caErrorArrayArgIsNULL == error);
}

void Test_CArray_At_NULLElement(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		error = CArray_At(&array, 0, NULL);

		TEST_CHECK(caErrorElementArgIsNULL == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_At_ArrayNotValid(void)
{
	{ // array.data invalid
		struct CArray array;
		CArray_InitZero(&array);

		array.capacity    = 4;
		array.count       = 0;
		array.elementSize = sizeof(int);

		int* val_ptr = NULL;
		enum EnumCArrayError error = CArray_At(&array, 0, (void**)&val_ptr);

		TEST_CHECK(caErrorArrayIsNotValid == error);
	}

	{ // array.elementSize
		struct CArray array;
		CArray_InitZero(&array);

		size_t element_size = sizeof(int);
		size_t capacity = 4;

		enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

		if (TEST_CHECK(caNoError == error))
		{
			TEST_CHECK(NULL != array.data);
			TEST_CHECK(element_size == array.elementSize);
			TEST_CHECK(capacity == array.capacity);
			TEST_CHECK(0 == array.count);

			// simulate error
			array.elementSize = 0;

			int* val_ptr = NULL;
			error = CArray_At(&array, 0, (void**)&val_ptr);

			TEST_CHECK(caErrorArrayIsNotValid == error);
		}

		CArray_Clear(&array);
	}
}

void Test_CArray_At_IndexOutOfBounds(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		{ // check empty array 
			int* val_ptr = NULL;
			error = CArray_At(&array, 0, (void**)&val_ptr);

			TEST_CHECK(caErrorIndexOutOfBounds == error);
		}

		{ // check not empty array
			array.count = capacity - 1;

			int* val_ptr = NULL;
			error = CArray_At(&array, capacity, (void**)&val_ptr);

			TEST_CHECK(caErrorIndexOutOfBounds == error);
		}
	}

	CArray_Clear(&array);
}

void Test_CArray_At_NoError(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		if (array.data)
		{
			// write data so later will get it with CArray_At()
			int value = 123;
			*((int*)array.data) = value;
			array.count = 1;

			int* val_ptr = NULL;
			error = CArray_At(&array, 0, (void**)&val_ptr);

			TEST_CHECK(caNoError == error);
			TEST_CHECK(value == *val_ptr);
		}
	}

	CArray_Clear(&array);
}

void Test_CArray_ChangeCapacity_NULLArray(void)
{
	enum EnumCArrayError error = CArray_ChangeCapacity(NULL, 4);
	TEST_CHECK(caErrorArrayArgIsNULL == error);
}

void Test_CArray_ChangeCapacity_ArrayNotValid(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		// simulate elementSize = 0
		array.elementSize = 0;

		error = CArray_ChangeCapacity(&array, capacity * 2);

		TEST_CHECK(caErrorArrayIsNotValid == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_ChangeCapacity_0Capacity(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		error = CArray_ChangeCapacity(&array, 0);

		TEST_CHECK(caErrorCapacityArgIs0 == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_ChangeCapacity_Realloc(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		error = CArray_ChangeCapacity(&array, (size_t)-1 / element_size);

		TEST_CHECK(caErrorReallocFailed == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_ChangeCapacity_NoError(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		error = CArray_ChangeCapacity(&array, capacity * 2);

		TEST_CHECK(caNoError == error);
		TEST_CHECK(capacity * 2 == array.capacity);
	}

	CArray_Clear(&array);
}

void Test_CArray_PushBack_NULLArray(void)
{
	int val = 123;
	enum EnumCArrayError error = CArray_PushBack(NULL, (void*)&val);
	TEST_CHECK(caErrorArrayArgIsNULL == error);
}

void Test_CArray_PushBack_NULLElement(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		error = CArray_PushBack(&array, NULL);

		TEST_CHECK(caErrorElementArgIsNULL == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_PushBack_NoFreeSpace(void)
{
	struct CArray array;
	CArray_InitZero(&array);
	array.allowGrow = 0;

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		// simulate full array
		array.count = array.capacity;

		int val = 123;
		error = CArray_PushBack(&array, (void*)&val);

		TEST_CHECK(caErrorArrayIsFull == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_PushBack_NoError(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		int val = 123;
		error = CArray_PushBack(&array, (void*)&val);

		TEST_CHECK(caNoError == error);
	}

	CArray_Clear(&array);
}

void Test_CArray_PushBack_NoError_Grow(void)
{
	struct CArray array;
	CArray_InitZero(&array);
	array.allowGrow = 1;

	size_t element_size = sizeof(int);
	size_t capacity = 4;

	enum EnumCArrayError error = CArray_Create(&array, capacity, element_size, NULL);

	if (TEST_CHECK(caNoError == error))
	{
		TEST_CHECK(NULL != array.data);
		TEST_CHECK(element_size == array.elementSize);
		TEST_CHECK(capacity == array.capacity);
		TEST_CHECK(0 == array.count);

		// simulate full array
		array.count = array.capacity;
		size_t old_capacity = array.capacity;

		int val = 123;
		error = CArray_PushBack(&array, (void*)&val);

		TEST_CHECK(caNoError == error);
		TEST_CHECK(old_capacity < array.capacity);
	}

	CArray_Clear(&array);
}

void Test_CArray_PushBack_ArrayIsNotValid(void)
{
	struct CArray array;
	CArray_InitZero(&array);

	int val = 123;
	enum EnumCArrayError error = CArray_PushBack(&array, &val);

	TEST_CHECK(caErrorArrayIsNotValid == error);
}

void Test_CArray_GetErrorStr_Existent(void)
{
	size_t str_length = CArray_GetErrorStrSize(caErrorArrayArgIsNULL);
	char* str = (char*)malloc(str_length);
	TEST_CHECK(NULL != str);
	if (str)
	{
		CArray_GetErrorStr(caErrorArrayArgIsNULL, str);
		free(str);
	}
}

void Test_CArray_GetErrorStr_Inexistent(void)
{
	size_t str_length = CArray_GetErrorStrSize((enum EnumCArrayError)10000);
	char* str = (char*)malloc(str_length);
	TEST_CHECK(NULL != str);
	if (str)
	{
		CArray_GetErrorStr((enum EnumCArrayError)10000, str);
		free(str);
	}
}

TEST_LIST =
{
	{"Test_CArray_Create_NULLArray", Test_CArray_Create_NULLArray},
	{"Test_CArray_Create_0Capacity", Test_CArray_Create_0Capacity},
	{"Test_CArray_Create_0ElementSize", Test_CArray_Create_0ElementSize},
	{"Test_CArray_Create_Malloc", Test_CArray_Create_Malloc},
	{"Test_CArray_Create_NoError", Test_CArray_Create_NoError},

	{"Test_CArray_At_NULLArray", Test_CArray_At_NULLArray},
	{"Test_CArray_At_NULLElement", Test_CArray_At_NULLElement},
	{"Test_CArray_At_ArrayNotValid", Test_CArray_At_ArrayNotValid},
	{"Test_CArray_At_IndexOutOfBounds", Test_CArray_At_IndexOutOfBounds},
	{"Test_CArray_At_NoError", Test_CArray_At_NoError},

	{"Test_CArray_ChangeCapacity_NULLArray", Test_CArray_ChangeCapacity_NULLArray},
	{"Test_CArray_ChangeCapacity_ArrayNotValid", Test_CArray_ChangeCapacity_ArrayNotValid},
	{"Test_CArray_ChangeCapacity_0Capacity", Test_CArray_ChangeCapacity_0Capacity},
	{"Test_CArray_ChangeCapacity_Realloc", Test_CArray_ChangeCapacity_Realloc},
	{"Test_CArray_ChangeCapacity_NoError", Test_CArray_ChangeCapacity_NoError},

	{"Test_CArray_PushBack_NULLArray", Test_CArray_PushBack_NULLArray},
	{"Test_CArray_PushBack_NULLElement", Test_CArray_PushBack_NULLElement},
	{"Test_CArray_PushBack_NoFreeSpace", Test_CArray_PushBack_NoFreeSpace},
	{"Test_CArray_PushBack_NoError", Test_CArray_PushBack_NoError},
	{"Test_CArray_PushBack_NoError_Grow", Test_CArray_PushBack_NoError_Grow},
	{"Test_CArray_PushBack_ArrayIsNotValid", Test_CArray_PushBack_ArrayIsNotValid},

	{"Test_CArray_GetErrorStr_Existent", Test_CArray_GetErrorStr_Existent},
	{"Test_CArray_GetErrorStr_Inexistent", Test_CArray_GetErrorStr_Inexistent},

	{NULL, NULL}
};