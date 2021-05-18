#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include <EngineAPI.h>

/*
	Returns path without file name. Path separator is /

	For /aaa/bbb/ccc/ddd.txt you will get /aaa/bbb/ccc
*/
DECL_API void GetPathWithoutFile(const char* path, char* pathWithoutFile);

#endif /* #ifndef _FILE_SYSTEM_H_ */