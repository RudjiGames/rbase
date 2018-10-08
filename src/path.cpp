//--------------------------------------------------------------------------//
/// Copyright (c) 2018 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/path.h>
#include <rbase/inc/winchar.h>
#include <rbase/inc/stringfn.h>

#if RTM_PLATFORM_WINDOWS
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#endif

namespace rtm {

template <typename CHRT = char>
static inline bool isSlash(CHRT c)
{
	return ((c==CHRT('\\')) || (c==CHRT('/')));
}

template <typename CHRT = char>
static inline void replaceSlashes(CHRT* _path, CHRT _slash)
{
	while (CHRT c = *_path)
	{
		if (isSlash(c))
			*_path = _slash;
		++_path;
	}
}

template <typename CHRT= char>
static inline void toUnixSlashes(CHRT* _path)
{
	replaceSlashes(_path, CHRT('/'));
}

static inline void fixPath(char* _path)
{
	if (_path[0] == char('.')) _path++;

	char* pos = NULL;
	while ((pos = strstr(_path, "..")) != NULL)
	{
		char* prevSlash = pos - 2;
		while (!isSlash(*prevSlash)) prevSlash--;
		char* nextDir = pos + 3;
		size_t len = strlen(nextDir) + 1;
		memmove(prevSlash+1, nextDir, len*sizeof(char));
	}

	toUnixSlashes(_path);
}

static inline void fixPath(wchar_t* _path)
{
	if (_path[0] == wchar_t('.')) _path++;

	wchar_t* pos = NULL;
	while ((pos = wcsstr(_path, L"..")) != NULL)
	{
		wchar_t* prevSlash = pos - 2;
		while (!isSlash(*prevSlash)) prevSlash--;
		wchar_t* nextDir = pos + 3;
		size_t len = wcslen(nextDir) + 1;
		memmove(prevSlash+1, nextDir, len*sizeof(wchar_t));
	}

	toUnixSlashes(_path);
}

const char* pathGetFileName(const char* _path)
{
	size_t len = strlen(_path);
	while ((_path[len] != '/') && (_path[len] != '\\') && (len>0)) --len;
	return &_path[len + 1];
}

bool pathGetFilename(const char* _path, char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strlen(_path);
	size_t saveLen = len;

	while (--len)
		if (isSlash(_path[len]))
			break;

	_buffer[0] = 0;

	if (len++)
	{
		const char* fn = &_path[len];
		len = saveLen - len;

		if (len + 1 > _bufferSize)
			return false;

		strncpy(_buffer, fn, len);
		_buffer[len] = 0;
	}

	return true;
}

bool pathGetFilenameNoExt(const char* _path, char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strlen(_path);
	size_t saveLen = len;

	while (--len)
		if (isSlash(_path[len]))
			break;

	_buffer[0] = 0;

	const char* fn = 0;
	if (len++)
		fn = &_path[len];
	else
		fn = _path;

	const char* dot = strstr(fn, ".");

	if (dot)
		len = dot - fn;
	else
		len = saveLen - len;

	if (len + 1 > _bufferSize)
		return false;

	strncpy(_buffer, fn, len);
	_buffer[len] = 0;

	return true;
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

bool pathGetExt(const char* _path, char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strlen(_path);

	while (--len)
		if ((isSlash(_path[len])) ||
			(_path[len] == '.')) break;

	_buffer[0] = 0;

	if (isSlash(_path[len]))
	{
		_buffer[0] = 0;
		return true;
	}

	if (!(_path[len++] == '.'))
		return false;

	if (len + 1 > _bufferSize)
		return false;

	strcpy(_buffer, &_path[len] );

	return true;
}

bool pathGetCurrentDirectory(char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

#if RTM_PLATFORM_WINDOWS
	DWORD size = GetCurrentDirectoryW(0, nullptr);
	if ((size == 0) || (size >= static_cast<DWORD>(_bufferSize)))
		return false;

    if (size > 4095) // trailing zero
        return false;

	wchar_t wBuffer[4096];

	DWORD len = GetCurrentDirectoryW(size, wBuffer);
	if (len == 0)
		return false;

	rtm::WideToMulti wb(wBuffer);
	return rtm::strLen(wb) == rtm::strlCpy(_buffer, (int32_t)_bufferSize, wb);

#elif RTM_PLATFORM_POSIX
	getcwd
#else
#endif
}

bool pathGetDataDirectory(char* _buffer, size_t _bufferSize)
{
#if RTM_PLATFORM_WINDOWS

	wchar_t executablePath[512];
	if (GetModuleFileNameW(GetModuleHandle(0), executablePath, 512))
	{
		fixPath(executablePath);
		wchar_t* ptr = executablePath + wcslen(executablePath);
		while (*(--ptr) != L'/');
		*ptr = 0;
	}
	else
		return false;

#if RTM_DEBUG || RTM_RELEASE

	// remove 7 dirs at end    \rtm\.build\windows\vs2015\x64\release\bajka\bin

	wchar_t* ptr = executablePath + wcslen(executablePath);

	int numSlashes = 0;
	while (numSlashes < 7)
	{
		while (*(--ptr) != L'/');
		--ptr;
		++numSlashes;
	}
	*(++ptr) = 0;
	
	wcscat(executablePath, L"/.data/windows");
#endif

#if RTM_RETAIL
	wcscat(executablePath, L"/data/");
#endif

	replaceSlashes(executablePath, L'\\');
	rtm::WideToMulti ep(executablePath);
	return rtm::strLen(ep) == rtm::strlCpy(_buffer, (int32_t)_bufferSize, ep);

#else
#endif
}

bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_appendPath, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	if (pathIsAbsolute(_appendPath))
		return false;

	size_t lenAppend = strlen(_appendPath);

	if (!_path)
	{
		if (lenAppend > _bufferSize - 1)
			return false;

		strcpy(_buffer, _appendPath);
		return true;
	}

	if (strlen(_path) > _bufferSize)
		return false;

	size_t lenPath = strlen(_path);

	size_t addLen = 0;
	if (!isSlash(_path[lenPath-1]) && !isSlash(_appendPath[0]))
		addLen = 1;

	size_t totalLen = lenPath + strlen(_appendPath) + 1 + addLen;
	if (totalLen > _bufferSize -1)
		return false;

	strcpy(_buffer, _path);
	if (addLen)
		strcat(_buffer, "/");
	strcat(_buffer, _appendPath);

	return true;
}

bool pathUp(const char* _path, char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strlen(_path);

	while (--len)
		if (isSlash(_path[len])) break;
	
	if (len++)
	{
		if (len < _bufferSize)
		{
			strncpy(_buffer, _path, len);
			_buffer[len] = 0;
			return true;
		}
	}

	return false;
}

bool pathCanonicalize(const char* _path, char* _buffer, size_t _bufferSize)
{
	RTM_UNUSED(_bufferSize);

	if (!_path)
		return false;

#if RTM_PLATFORM_WINDOWS
	if (_buffer)
	{
		RTM_ASSERT(strlen(_path) < _bufferSize, "");
		strcpy(_buffer, _path);
		fixPath(_buffer);
		return true;
	}
	return false;
#elif RTM_PLATFORM_POSIX
	realpath()
#else

#endif
}

void pathMakeAbsolute(char* _path)
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

bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, size_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	if (!pathAppend(_base, _relative, _buffer, _bufferSize))
	{
		_buffer[0] = 0;
		return false;
	}

	char tmpBuffer[1024];
	pathCanonicalize(_buffer, tmpBuffer, 1024);

	if (!pathIsAbsolute(tmpBuffer))
	{
		char cwd[512];
		if (pathGetCurrentDirectory(cwd, 512))
		{
			pathMakeAbsolute(tmpBuffer, cwd, _buffer, _bufferSize);
			fixPath(_buffer);
		}
		else
			return false;
	}
			
	return true;
}

bool pathMakeRelative(const char* _pathFrom, bool _fromDir, const char* _pathTo, bool _toDir, char* _buffer, size_t _bufferSize)
{
#if RTM_PLATFORM_WINDOWS
	wchar_t result[1024];
	rtm::MultiToWide pf(_pathFrom);
	rtm::MultiToWide pt(_pathTo);

	if (!PathRelativePathToW(	result, 
								pf.m_ptr, 
								_fromDir ? FILE_ATTRIBUTE_DIRECTORY : 0,
								pt.m_ptr,
								_toDir ? FILE_ATTRIBUTE_DIRECTORY : 0))
		return false;

	rtm::WideToMulti res(result);
	return rtm::strLen(res) == rtm::strlCpy(_buffer, (int32_t)_bufferSize, res);

#elif RTM_PLATFORM_POSIX

#else
#endif

}

bool pathIsAbsolute(const char* _path)
{
	if (!_path)
		return false;

#if RTM_PLATFORM_WINDOWS
	if (!_path || !*_path)
		return false;

	if (/*isSlash(_path[0]) || */(_path[1] == ':'))
		return true;
                
	return false;
#elif RTM_PLATFORM_POSIX

	//if (path.length() >= 1 && path[0] == '~')
	//	return (path.length() == 1 || pathIsSeparator(path[1]));
	//return pathIsSeparator(path[0]);

#else
#endif
}

bool pathExists(const char* _path)
{
	if (!_path)
		return false;

#if RTM_PLATFORM_WINDOWS
	rtm::MultiToWide wpath(_path);
	if (!wpath.m_ptr)
		return false;

	DWORD dwAttr = ::GetFileAttributesW(wpath.m_ptr);
	bool ret = dwAttr != INVALID_FILE_ATTRIBUTES;
	return ret;

#elif RTM_PLATFORM_POSIX
#else
#endif
}

bool pathIsDirectory(const char* _path)
{
	if (!_path)
		return false;

#if RTM_PLATFORM_WINDOWS
	rtm::MultiToWide wpath(_path);
	if (!wpath.m_ptr)
		return false;

	DWORD dwAttr = ::GetFileAttributesW(wpath.m_ptr);
	bool ret = false;

	if (dwAttr != INVALID_FILE_ATTRIBUTES)
		ret = (dwAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;

	return ret;

#elif RTM_PLATFORM_POSIX
#endif
}

bool pathCreateDir(const char* _path, const char* _name, bool _recurse)
{
	if (!_path)
		return false;

	if (!pathIsAbsolute(_path))
		return false;

	char buffer[2048];
	if (!pathAppend(_path, _name, buffer, 2048))
		return false;

#if RTM_PLATFORM_WINDOWS
	fixPath(buffer);
	replaceSlashes(buffer, '\\');

	bool ret = true;

	if (_recurse)
	{
		char* nextSlash = strstr(buffer, "\\");
		do 
		{
			*nextSlash = 0;
			rtm::MultiToWide ws(buffer);
			CreateDirectoryW(ws.m_ptr, 0);
			*nextSlash = '\\';
			nextSlash = strstr(nextSlash+1, "\\");
		}
		while (nextSlash);

		rtm::MultiToWide ws(buffer);
		CreateDirectoryW(ws.m_ptr, 0);
	}
	else
	{
		rtm::MultiToWide ws(buffer);
		if ((FALSE == CreateDirectoryW(ws.m_ptr, 0)) &&
			(GetLastError() == ERROR_PATH_NOT_FOUND))
		{
			ret = false;
		}
		return true;
	}

	return ret;
#elif RTM_PLATFORM_POSIX
#else
#endif
}

bool pathRemoveDir(const char* _path, const char* _name)
{
	if (!_path)
		return false;

	if (!pathIsAbsolute(_path))
		return false;

	char buffer[2048];
	if (!pathAppend(_path, _name, buffer, 2048))
		return false;

#if RTM_PLATFORM_WINDOWS
	fixPath(buffer);
	replaceSlashes(buffer, '\\');

	rtm::MultiToWide ws(buffer);
	if (FALSE == RemoveDirectoryW(ws.m_ptr))
		return false;

	return true;
#elif RTM_PLATFORM_POSIX
#else
#endif
}

static inline const char* findSlash(const char* _str)
{
    while ((*_str != '/') && (*_str != '\\'))
        ++_str;
    return _str;
}

bool pathSplit(const char* _path, uint32_t* _numDirectories, StringView* _stringViews, uint32_t _maxViews)
{
	RTM_ASSERT(_numDirectories, "");

	*_numDirectories = 0;

	if (!_path)
		return false;

	const char* ps = findSlash(_path);
	const char* pe = 0;
	
	if (ps)
		pe = findSlash(ps + 1);

	uint64_t dirOffsets[512];
	uint64_t numDirs = 0;

	while (pe)
	{
		size_t ds = ps + 1 - _path;
		size_t de = pe - _path;

		RTM_ASSERT(ds < (1<<16), "");
		size_t offsets = ds + (de << 16);

		dirOffsets[numDirs++] = offsets;

		ps = pe;
		pe = findSlash(ps + 1);
	}

	*_numDirectories = static_cast<uint32_t>(numDirs);

    if (numDirs > _maxViews)
        return false;

	for (uint64_t i=0; i<numDirs; ++i)
	{
		size_t de = (dirOffsets[i] >> 16) & 0xffffffff;
		size_t ds =  dirOffsets[i] & 0xffff;
        _stringViews[i].set(&_path[ds], de-ds);
	}

    return true;
}

} // namespace rtm
