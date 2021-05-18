# Error handling

## Error enum

There is an enum that contains all errors of the module. It has the following name format:

	Enum<NameModule | NameObject>Error

For example:

	EnumTMapError
	EnumCArrayError

## Error data

If the module needs there can be a union with additional data about the error. It has the following name format:

	<NameModule | NameObject>ErrorData

For example:

	TMapErrorData
	TPFPErrorData

If Module/Object depends on other Modules/Objects then it is useful to return their errors too. It is done by including a member in the error data union of the error data union that depends on. For example: 

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

`carrayData` member incorporates a member of `enum EnumCArrayError`.

It is important to keep the tree of theese dependencies to a minimal height. First to keep the modules less coupled and second to occupy less memory.

## Error string

There are 2 functions to help with getting the corresponding string of an error.

* `<NameModule | NameObject>_GetErrorStrSize()` : returns number of bytes necessary to store the string including NULL terminator.

	For example: 

	size_t TPFP_GetErrorStrSize(enum EnumPFPError error, const union TPFPErrorData* errorData);

* `<NameModule | NameObject>_GetErrorStr()` : returns through argument the string of error.

	For example:

	void TPFP_GetErrorStr(enum EnumPFPError error, const union TPFPErrorData* errorData, char* errorStr);