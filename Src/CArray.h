#ifndef _C_ARRAY_H_
#define _C_ARRAY_H_

#include <stdlib.h>
#include <string.h>

#include <EngineAPI.h>

enum EnumCArrayError
{
	caNoError = 0,

	caErrorArrayArgIsNULL    = 1,
	caErrorCapacityArgIs0    = 2,
	caErrorElementSizeArgIs0 = 3,
	caErrorElementArgIsNULL  = 4,
	caErrorIndexOutOfBounds  = 5,
	caErrorMallocFailed      = 6,
	caErrorReallocFailed     = 7,
	caErrorArrayIsNotValid   = 8,
	caErrorArrayIsFull       = 9
};

typedef void(*CArrayElementClearFunc)(void* element);

struct CArray
{
	// This is the array data.
	// Use function CArray_At(...) to get an element. It makes some checks to be safe.
	// Also you can cast this pointer, be careful!
	// Default value: NULL
	void*  data;
	// How many elements will fit in the allocated memory.
	// Default value: 0
	size_t capacity;
	// How many elements are already in the array.
	// Default value: 0
	size_t count;
	// The size in bytes of an element.
	// Default value: 0
	size_t elementSize;
	// 0 : not allow grow when calling CArray_PushBack(...) or other functions.
	// !0: alloc grow when calling CArray_PushBack(...) or other functions.
	// Default value: 1
	int allowGrow;
	// This function is called in CArray_Clear() for each element.
	// Can be NULL.
	CArrayElementClearFunc elementClearFunc;
};

/* 
	This function sets all fields of the struct to default values.
*/
DECL_API void CArray_InitZero(void* array);

/*
	Deallocates all data in the array and sets all fields to default values.
	If elementClearFunc is not NULL then it is called for each element.
*/
DECL_API void CArray_Clear(void* array);

/*
	This function allocates memory for an array.

	Arguments: 
		array      : array structure
		capacity   : size of the array in elements
		elementSize: size of the element in bytes
		elementClearFunc : this function will be assigned to array.elementClearFunc. Can be NULL.

	Return:
		caNoError
		caErrorArrayArgIsNULL
		caErrorElementSizeArgIs0
		caErrorMallocFailed
*/
DECL_API enum EnumCArrayError CArray_Create(struct CArray* array, size_t capacity, size_t elementSize, CArrayElementClearFunc elementClearFunc);

/*
	This function will return through element argument pointer to the begining of 
	the desired element.
	It makes check to see if index is not outside of bounds.

	Arguments:
		array: array
		index: [0 ... array->count) index
		element: pointer to element
		         will be NULL if failed to get

	Return:
		caNoError
		caErrorArrayArgIsNULL
		caErrorElementArgIsNULL
		caErrorArrayIsNotValid
		caErrorIndexOutOfBounds

*/
DECL_API enum EnumCArrayError CArray_At(const struct CArray* array, size_t index, void** element);

/*
	This function changes capacity of the array.

	Arguments:
		array      : the array
		newCapacity: the new capacity of the array

	Return: 
		caNoError
		caErrorArrayArgIsNULL
		caErrorArrayIsNotValid
		caErrorCapacityArgIs0
		caErrorReallocFailed
*/
DECL_API enum EnumCArrayError CArray_ChangeCapacity(struct CArray* array, size_t newCapacity);

/*
	Appends element to the array. If capacity of the array doesn't allow it the capacity 
	of the array will grow 2x times with CArray_ChangeCapacity(...).

	If array->allowGrow != 0, this function will return error when there is no place to push back!

	Arguments:
		array  : the array
		element: pointer to element

	Returns: 
		caNoError
		caErrorArrayArgIsNULL
		caErrorElementArgIsNULL
		caErrorArrayIsFull
		caErrorArrayIsNotValid
*/
DECL_API enum EnumCArrayError CArray_PushBack(struct CArray* array, void* element);

/*
	Appends to errorStr the string text that coresponds to error.
	Make shure there is enough space left!
	If errorStr is NULL the function will not do anything.
*/
DECL_API void CArray_GetErrorStr(enum EnumCArrayError error, char* errorStr);

/*
	Returns size in bytes that is needed to store error string with CArray_GetErrorStr(). It includes NULL terminator.
*/
DECL_API size_t CArray_GetErrorStrSize(enum EnumCArrayError error);

#endif /* #ifndef _C_ARRAY_H_ */