//--------------------------------------------------------------------------//
/// Copyright (c) 2018 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/path.h>
#include <rbase/inc/winchar.h>
#include <rbase/inc/stringfn.h>

#if RTM_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif RTM_PLATFORM_POSIX
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/stat.h>
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

template <typename CHRT = char>
static inline void toUnixSlashes(CHRT* _path)
{
	replaceSlashes(_path, CHRT('/'));
}

const char* pathGetFileName(const char* _path)
{
	size_t len = strLen(_path);
	while ((_path[len] != '/') && (_path[len] != '\\') && (len>0)) --len;
	return &_path[len + 1];
}

bool pathGetFilename(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strLen(_path);
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

bool pathGetFilenameNoExt(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strLen(_path);
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
	RTM_ASSERT(_path != 0, "");

	const size_t length = strLen(_path);
	size_t len = length;

	while (--len)
		if (_path[len] == '.')
			break;

	if (_path[len++] == '.')
		return &_path[len];

	return &_path[length];
} 

bool pathGetExt(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	size_t len = strLen(_path);

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

	strlCpy(_buffer, _bufferSize, &_path[len] );

	return true;
}

bool pathGetCurrentDirectory(char* _buffer, uint32_t _bufferSize)
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
	wcscat(wBuffer, L"\\");
	if (len == 0)
		return false;

	rtm::WideToMulti wb(wBuffer);
	return rtm::strLen(wb) == rtm::strlCpy(_buffer, (int32_t)_bufferSize, wb);

#elif RTM_PLATFORM_POSIX
	if (!(_buffer == getcwd(_buffer, _bufferSize)))
		return false;

	if (!pathIsDirectory(_buffer))
		strlCat(_buffer, _bufferSize, "/");

	return true;
#endif
}

bool pathGetDataDirectory(char* _buffer, uint32_t _bufferSize)
{
#if RTM_PLATFORM_WINDOWS

	wchar_t executablePath[512];
	if (!GetModuleFileNameW(GetModuleHandle(0), executablePath, 512))
		return false;

	WideToMulti mb(executablePath);
	pathCanonicalize(mb);

#if RTM_DEBUG || RTM_RELEASE

	// remove 8 slashes at end    \rtm\.build\windows\vs2017\x64\project\x64\bin\project.exe

	uint32_t len = strLen(mb);
	char* ptr = mb + len;

	int numSlashes = 0;
	while (numSlashes < 8)
	{
		while (*(--ptr) != L'/');
		--ptr;
		++numSlashes;
	}
	*(++ptr) = 0;
	
	strlCat(ptr, uint32_t(mb - ptr), "/.data/windows/");
	return strlCpy(_buffer, _bufferSize, mb) == strLen(mb);
#endif

#if RTM_RETAIL
	char dataPath[512];
	strlCpy(dataPath, 512, mb);
	const char* exeName = pathGetFileName(dataPath);
	strlCpy((char*)exeName, strLen(exeName), "data/");
	return strlCpy(_buffer, _bufferSize, dataPath) == strLen(dataPath);
#endif

#elif RTM_PLATFORM_POSIX
    return -1 != readlink("/proc/self/exe", _buffer, _bufferSize);
#endif
}

bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_appendPath, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	if (pathIsAbsolute(_appendPath))
		return false;

	if (!pathIsDirectory(_path))
		return false;

	size_t lenAppend = strLen(_appendPath);

	if (!_path)
	{
		if (lenAppend > _bufferSize - 1)
			return false;

		strlCpy(_buffer, _bufferSize, _appendPath);
		return true;
	}

	if (strLen(_path) > _bufferSize)
		return false;

	size_t lenPath = strLen(_path);

	size_t addLen = 0;
	if (!isSlash(_path[lenPath-1]) && !isSlash(_appendPath[0]))
		addLen = 1;

	size_t totalLen = lenPath + strLen(_appendPath) + 1 + addLen;
	if (totalLen > _bufferSize -1)
		return false;

	strlCpy(_buffer, _bufferSize,_path);
	if (addLen)
		strcat(_buffer, "/");
	strcat(_buffer, _appendPath);

	return true;
}

bool pathUp(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	uint32_t len = strLen(_path);

	uint32_t slashes = pathIsDirectory(_path) ? 2 : 1;

	while (--len && slashes)
		if (isSlash(_path[len])) --slashes;
	
	if (len++)
	{
		if (len < _bufferSize)
		{
			strlCpy(_buffer, _bufferSize, _path, len+1);
			_buffer[len+1] = 0;
			return true;
		}
	}

	return false;
}

bool pathCanonicalize(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_UNUSED(_bufferSize);

	if (!_path)
		return false;

	if (_buffer)
	{
		RTM_ASSERT(strLen(_path) < _bufferSize, "");
		strlCpy(_buffer, _bufferSize, _path);
		pathCanonicalize(_buffer);
		return true;
	}
	return false;
}

void pathCanonicalize(char* _path)
{
	const char* pos = 0;
	while ((pos = strStr(_path, "..")) != 0)
	{
		const char* prevSlash = pos - 2;
		while ((*prevSlash != '\\') && (*prevSlash != '/')) prevSlash--;
		const char* nextDir = pos + 3;
		size_t len = strLen(nextDir) + 1;
		memmove((void*)(prevSlash+1), nextDir, len*2);
	}

	toUnixSlashes(_path);
}

bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	char tmpBuffer[4096];
	if (!pathAppend(_base, _relative, tmpBuffer, 4096))
	{
		_buffer[0] = 0;
		return false;
	}

	return pathCanonicalize(tmpBuffer, _buffer, _bufferSize);
}

bool pathMakeRelative(const char* _pathFrom, const char* _pathTo, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");

	if (!_pathFrom  || !_pathTo)
		return false;

	int cr=0;
	int lastSlash = 0;
	while (_pathFrom[cr] == _pathTo[cr])
	{
		if (isSlash(_pathFrom[cr]))
			lastSlash = cr;
		++cr;
	}

	if (lastSlash < 2) // no common root, only slash or nothing
		return false;

	int dirs = 0;
	int c = lastSlash + 1;
	while (_pathFrom[c] != '\0')
		if (isSlash(_pathFrom[c++]))
			++dirs;

	_buffer[0] = 0;
	while (dirs-- && (_bufferSize > 3))
	{
		strlCat(_buffer, _bufferSize, "../");
		_bufferSize -= 3;
	}

	uint32_t remLen = strLen(&_pathTo[lastSlash+1]);
	if (_bufferSize > remLen)
		strlCat(_buffer, _bufferSize, &_pathTo[lastSlash+1]);
	else
	{
		_buffer[0] = 0;
		return false;
	}

	return true;
}

bool pathIsAbsolute(const char* _path)
{
	if (!_path)
		return false;

	if (isSlash(_path[0]))
		return true;

#if RTM_PLATFORM_WINDOWS
	if (!*_path)
		return false;

	if (_path[1] == ':')
		return true;
#endif

	return false;
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
	return dwAttr != INVALID_FILE_ATTRIBUTES;

#elif RTM_PLATFORM_POSIX
	struct stat info;
	if (stat(_path, &info) != 0)
		return false; // cannot access

	return (info.st_mode & (S_IFDIR | S_IFREG /*| S_IFLNK*/)) != 0;
#endif
}

bool pathIsDirectory(const char* _path)
{
	if (!_path)
		return false;

	uint32_t len = strLen(_path);
	return isSlash(_path[len - 1]);
}

bool pathCreateDir(const char* _path, const char* _name, bool _recurse)
{
	if (!_path)
		return false;

	if (!pathIsAbsolute(_path))
		return false;

	if (!pathIsDirectory(_path))
		return false;

	char buffer[8192];
	if (!pathAppend(_path, _name, buffer, 8192))
		return false;

	pathCanonicalize(buffer);

#if RTM_PLATFORM_WINDOWS
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
			nextSlash = strstr(nextSlash + 1, "\\");
		} while (nextSlash);

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
	if (_recurse)
	{
		char *sep = strrchr(buffer, '/');
		if (sep)
		{
			*sep = 0;
			mkdir(buffer, 0777);
			*sep = '/';
		}
	}

	return (0 == mkdir(buffer, 0777));
#endif
}

bool pathRemoveDir(const char* _path, const char* _name)
{
	if (!_path)
		return false;

	if (!pathIsAbsolute(_path))
		return false;

#if RTM_PLATFORM_WINDOWS
	char buffer[8192];
	if (!pathAppend(_path, _name, buffer, 8192))
		return false;

	pathCanonicalize(buffer);
	replaceSlashes(buffer, '\\');

	rtm::MultiToWide ws(buffer);
	if (FALSE == RemoveDirectoryW(ws.m_ptr))
		return false;

	return true;

#elif RTM_PLATFORM_POSIX
	char buffer[PATH_MAX+1];
	if (!pathAppend(_path, _name, buffer, PATH_MAX + 1))
		return false;

	return 0 == rmdir(buffer);
#endif
}

static inline const char* findSlash(const char* _str)
{
    while ((*_str != '\0') && !isSlash(*_str))
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

	uint32_t dirOffsets[512];
	uint32_t numDirs = 0;

	while ((*pe != '\0') && (numDirs < 512))
	{
		uint32_t ds = (uint32_t)(ps + 1 - _path);
		uint32_t de = (uint32_t)(pe - _path);

		RTM_ASSERT(ds < (1<<16), "");
		uint32_t offsets = ds + (de << 16);

		dirOffsets[numDirs++] = offsets;

		ps = pe;
		pe = findSlash(ps + 1);
	}

	*_numDirectories = static_cast<uint32_t>(numDirs);

    if (numDirs > _maxViews)
    {
        return false;
    }

	for (uint32_t i=0; i<numDirs; ++i)
	{
		uint32_t de = (dirOffsets[i] >> 16) & 0xffffffff;
		uint32_t ds =  dirOffsets[i] & 0xffff;
        _stringViews[i].set(&_path[ds], (uint32_t)(de-ds));
	}

    return true;
}

} // namespace rtm
