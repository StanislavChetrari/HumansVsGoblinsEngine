# Object model

Objects are structures with related functions that do operation on them.

## Object

Name can be any, but usually starts with T letter. For example:

	CArray
	TMap
	TPFPProperty

Properties of the struct must not be changed manually unless it is specified in the comment of the member. For example:

	struct TCamera
	{
		// Horizontal field of view in radians
		float horizontalFOV; 
		// It is safe to set position directly in the struct.
		Vec2f position;
		Vec2f forward;
		Vec2f right;
		// tan(0.5 * horizontalFOV)
		float halfWidth;
	};

Here is written that `position` member can be changed by hand.

`But other members must be changed through functions!`

## Object functions

Name of object functions have the following format:

	<NameObject>_<NameFunction>(<PointerObject>, ...);

For example:

	void TCamera_SetForward(TCamera* camera, const Vec2f* forwardDirection);

The object pointer must be defined with `const` if the function doesn't change the object or without const if it changes the object.

There are some prederined functions that handle initialisation, creation, ... . They are: 

* `_InitZero(void* object)` : initialises object to a default state
* `_Clear(void* object)` : deallocates all data of the object and calls _InitZero()
* `_Create(...)` : creates the object. Allocates data, assigns arguments to object members
* `_GetErrorStr(...)` : get string of error. See `ErrorHandling` in Docs
* `_GetErrorStrSize(...)` : get size in bytes of string of error. See `ErrorHandling` in Docs

## Important

Call `_InitZero()` right you create the object.

Call `_Clear()` when you don't need the object any more.

Use `Set`/`Get` functions for most data members unless it is specified that a member can be modified directly.