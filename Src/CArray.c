#include "CArray.h"

void CArray_InitZero(void* array)
{
	if (array)
	{
		struct CArray* t_array = array;

		t_array->data        = NULL;
		t_array->capacity    = 0;
		t_array->count       = 0;
		t_array->elementSize = 0;
		t_array->allowGrow   = 1;
		t_array->elementClearFunc = NULL;
	}
}

enum EnumCArrayError CArray_Create(struct CArray* array, size_t capacity, size_t elementSize, CArrayElementClearFunc elementClearFunc)
{
	if (NULL == array)
		return caErrorArrayArgIsNULL;
	if (0 == elementSize)
		return caErrorElementSizeArgIs0;

	void* t_ptr = NULL;
	if (0 == capacity)
		t_ptr = NULL;
	else
	{
		t_ptr = malloc(capacity * elementSize);
		if (!t_ptr)
			return caErrorMallocFailed;
	}

	array->data             = t_ptr;
	array->capacity         = capacity;
	array->count            = 0;
	array->elementSize      = elementSize;
	array->elementClearFunc = elementClearFunc;

	return caNoError;
}

enum EnumCArrayError CArray_At(const struct CArray* array, size_t index, void** element)
{
	if (NULL == array)
		return caErrorArrayArgIsNULL;
	if (NULL == element)
		return caErrorElementArgIsNULL;
	if (NULL == array->data || 0 == array->elementSize)
		return caErrorArrayIsNotValid;

	if (index < array->count)
	{
		*element = (void*)((char*)array->data + array->elementSize * index);
		return caNoError;
	}
	else
		return caErrorIndexOutOfBounds;
}

enum EnumCArrayError CArray_ChangeCapacity(struct CArray* array, size_t newCapacity)
{
	if (NULL == array)
		return caErrorArrayArgIsNULL;
	if (0 == newCapacity)
		return caErrorCapacityArgIs0;
	if (0 == array->elementSize)
		return caErrorArrayIsNotValid;

	void* t_ptr = realloc(array->data, array->elementSize * newCapacity);
	if (t_ptr)
	{
		array->data = t_ptr;
		array->capacity = newCapacity;

		// In case the array shrinks
		if (array->count > array->capacity)
			array->count = array->capacity;

		return caNoError;
	}
	else
		return caErrorReallocFailed;
}

enum EnumCArrayError CArray_PushBack(struct CArray* array, void* element)
{
	if (NULL == array)
		return caErrorArrayArgIsNULL;
	if (NULL == element)
		return caErrorElementArgIsNULL;
	if(0 == array->elementSize)
		return caErrorArrayIsNotValid;

	if (array->count >= array->capacity)
	{
		if (array->allowGrow)
		{
			size_t new_capacity = 1;
			if (0 < array->capacity)
				new_capacity = array->capacity * 2;
			enum EnumCArrayError error = CArray_ChangeCapacity(array, new_capacity);
			if (caNoError == error)
			{
				memcpy((void*)((char*)array->data + array->elementSize * array->count), element, array->elementSize);
				array->count++;
			}
			else
				return error;
		}
		else
			return caErrorArrayIsFull;
	}
	else
	{
		memcpy((void*)((char*)array->data + array->elementSize * array->count), element, array->elementSize);
		array->count++;
	}

	return caNoError;
}

void CArray_Clear(void* array)
{
	if (array)
	{
		struct CArray* t_array = array;

		if (t_array->elementClearFunc)
		{
			for (size_t elem_i = 0; elem_i < t_array->count; ++elem_i)
			{
				void* element = NULL;
				if (caNoError == CArray_At(t_array, elem_i, &element))
					t_array->elementClearFunc(element);
			}
		}

		if (t_array->data)
			free(t_array->data);
		CArray_InitZero(t_array);
	}
}

static const char* caErrors[] =
{
	/* caNoError */                "No error.", 
	/* caErrorArrayArgIsNULL */    "Argument 'array' is NULL.",
	/* caErrorCapacityArgIs0 */    "Argument 'capacity' is 0.", 
	/* caErrorElementSizeArgIs0 */ "Argument 'elementSize' is 0.", 
	/* caErrorElementArgIsNULL */  "Argument 'element' is NULL.", 
	/* caErrorIndexOutOfBounds */  "Index is out of bounds.", 
	/* caErrorMallocFailed */      "malloc() failed.", 
	/* caErrorReallocFailed */     "realloc() failed.",
	/* caErrorArrayIsNotValid */   "Array is not valid.", 
	/* caErrorArrayIsFull */       "Array is full."
};

static const char* errorNotFound = "No string for this error.";

void CArray_GetErrorStr(enum EnumCArrayError error, char* errorStr)
{
	if (NULL == errorStr)
		return;

	size_t errors_count = sizeof(caErrors) / sizeof(const char*);
	if ((size_t)error < errors_count)
		strcpy(errorStr, caErrors[(size_t)error]);
	else
		strcpy(errorStr, errorNotFound);
}

size_t CArray_GetErrorStrSize(enum EnumCArrayError error)
{
	size_t errors_count = sizeof(caErrors) / sizeof(const char*);
	
	if (error < errors_count)
		return (strlen(caErrors[error]) + 1);
	else
		return (strlen(errorNotFound) + 1);
}