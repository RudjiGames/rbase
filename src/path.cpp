//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/path.h>

#include <string.h>

namespace rtm {

void pathRemoveRelative(char* _path)
{
	char* pos = NULL;
	while ((pos = strstr(_path, "..")) != NULL)
	{
		char* prevSlash = pos - 2;
		while ((*prevSlash != '\\') && (*prevSlash != '/')) prevSlash--;
		char* nextDir = pos + 3;
		size_t len = strlen(nextDir) + 1;
		memmove(prevSlash+1, nextDir, len*2);
	}
	size_t len = strlen(_path);
	for (size_t i=0; i<len; i++)
		if (_path[i] == '/')
			_path[i] = '\\';
}

const char* pathGetFileName(const char* _path)
{
	size_t len = strlen(_path);
	while ((_path[len] != '/') && (_path[len] != '\\') && (len>0)) --len;
	return &_path[len + 1];
}

const char* pathGetExt(const char* _path)
{
	size_t len = strlen(_path);

	while (--len)
		if (_path[len] == '.')
			break;

	if (_path[len++] == '.')
		return &_path[len];

	return 0;
}

} // namespace rtm
