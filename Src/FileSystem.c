#include "FileSystem.h"

#include <string.h>

void GetPathWithoutFile(const char* path, char* pathWithoutFile)
{
	size_t length = strlen(path);
	for (int i = (int)length; i >= 0; --i)
		if ('/' == path[i])
		{
			strncpy(pathWithoutFile, path, i);
			pathWithoutFile[i] = '\0';
			break;
		}
}