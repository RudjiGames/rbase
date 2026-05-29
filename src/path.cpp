//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/path.h>
#include <rbase/inc/widechar.h>
#include <rbase/inc/mutex.h>

#if RTM_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif RTM_PLATFORM_POSIX
#include <unistd.h>
#include <limits.h>
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
	if (isSlash(_path[len]))
		return &_path[len + 1];
	else
		return &_path[len];
}

bool pathGetFileName(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	uint32_t saveLen = strLen(_path);
	uint32_t len = saveLen;

	_buffer[0] = 0;

	while (len > 0)
	{
		--len;
		if (isSlash(_path[len]))
		{
			const char* fn = &_path[len + 1];
			uint32_t fnLen = saveLen - (len + 1);
			if (fnLen + 1 > _bufferSize)
				return false;
			rtm::strlCpy(_buffer, _bufferSize, fn, fnLen);
			return true;
		}
	}

	// no slash — whole path is the filename
	if (saveLen + 1 > _bufferSize)
		return false;
	rtm::strlCpy(_buffer, _bufferSize, _path, saveLen);
	return true;
}

bool pathGetFilenameNoExt(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_bufferSize > 0, "");

	uint32_t saveLen = rtm::strLen(_path);
	uint32_t len = saveLen;

	while (len > 0)
	{
		--len;
		if (isSlash(_path[len]))
		{
			++len;
			break;
		}
	}

	_buffer[0] = 0;

	const char* fn = &_path[len];

	const char* dot = 0;
	for (const char* p = fn; *p; ++p)
		if (*p == '.') dot = p;

	uint32_t outLen;
	if (dot)
		outLen = (uint32_t)(dot - fn);
	else
		outLen = saveLen - len;

	if (outLen + 1 > _bufferSize)
		return false;

	rtm::strlCpy(_buffer, _bufferSize, fn, outLen);

	return true;
}

const char* pathGetExt(const char* _path)
{
	RTM_ASSERT(_path != 0, "");

	const size_t length = strLen(_path);
	if (length == 0)
		return &_path[0];

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

	const size_t length = strLen(_path);
	_buffer[0] = 0;
	if (length == 0)
		return true;

	size_t len = length;
	while (--len)
		if ((isSlash(_path[len])) ||
			(_path[len] == '.')) break;

	if (isSlash(_path[len]))
	{
		_buffer[0] = 0;
		return true;
	}

	if (!(_path[len++] == '.'))
		return false;

	const size_t extLen = length - len;
	if (extLen + 1 > _bufferSize)
		return false;

	strlCpy(_buffer, _bufferSize, &_path[len]);

	return true;
}

bool pathGetCurrentDirectory(char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

#if RTM_PLATFORM_WINDOWS
	DWORD size = GetCurrentDirectoryW(0, nullptr);
	if (size > 4095) // trailing zero
		return false;

	if ((size == 0) || (size >= static_cast<DWORD>(_bufferSize)))
		return false;

	wchar_t wBuffer[4096];
	DWORD len = GetCurrentDirectoryW(size, wBuffer);
	if (len == 0)
		return false;

	// append a trailing slash without the CRT wcscat; GetCurrentDirectoryW
	// returns the character count (excluding null) and size <= 4095, so
	// len+1 is always within wBuffer
	wBuffer[len]	 = L'\\';
	wBuffer[len + 1] = L'\0';
	toUnixSlashes(wBuffer);

	WideToMulti wb(wBuffer);
	return wb.size() == strlCpy(_buffer, (int32_t)_bufferSize, wb);

#elif RTM_PLATFORM_POSIX

#if !RTM_PLATFORM_PS4 && !RTM_PLATFORM_PS5
		if (!(_buffer == getcwd(_buffer, _bufferSize)))
		return false;
#endif

	if (!pathIsDirectory(_buffer))
		strlCat(_buffer, _bufferSize, "/");

	return true;
#else
	return false;
#endif
}

bool pathGetDataDirectory(char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	_buffer[0] = 0;

#if RTM_PLATFORM_WINDOWS

#if RTM_DEBUG || RTM_RELEASE || RTM_RETAIL
	wchar_t executablePath[1024];
	if (!GetModuleFileNameW(GetModuleHandle(0), executablePath, 1024))
		return false;

	WideToMulti mb(executablePath);
	pathCanonicalize(mb);
#endif

#if RTM_DEBUG || RTM_RELEASE

	// remove 8 slashes at end    \rtm\.build\windows\vs2017\x64\project\x64\bin\project.exe

	uint32_t len = strLen(mb);
	char* const mbStart = (char*)mb;
	char* ptr = mbStart + len;

	int numSlashes = 0;
	while (numSlashes < 8)
	{
		while ((ptr > mbStart) && (*(--ptr) != '/'));
		if (ptr <= mbStart)
			return false;
		--ptr;
		++numSlashes;
	}
	*(++ptr) = 0;

	strlCpy(ptr, uint32_t(mbStart + len - ptr), "/.data/windows/");
	return strlCpy(_buffer, _bufferSize, mb) == strLen(mb);
#endif

#if RTM_RETAIL
	char dataPath[512];
	strlCpy(dataPath, 512, mb);
	const char* exeName = pathGetFileName(dataPath);
	strlCpy((char*)exeName, uint32_t(dataPath + 512 - exeName), "data/");
	return strlCpy(_buffer, _bufferSize, dataPath) == strLen(dataPath);
#endif

	return false;

#elif RTM_PLATFORM_POSIX

#if !RTM_PLATFORM_PS4 && !RTM_PLATFORM_PS5
	ssize_t rl = readlink("/proc/self/exe", _buffer, _bufferSize - 1);
	if (rl == -1)
		return false;
	_buffer[rl] = '\0';
#endif

	#if RTM_DEBUG || RTM_RELEASE

		// remove 8 slashes at end    \rtm\.build\windows\vs2017\x64\project\x64\bin\project.exe
		uint32_t len = strLen(_buffer);
		char* ptr = _buffer + len;

		int numSlashes = 0;
		while (numSlashes < 8)
		{
			while ((ptr > _buffer) && (*(--ptr) != '/'));
			if (ptr <= _buffer)
				return false;
			--ptr;
			++numSlashes;
		}
		*(++ptr) = 0;

		#if RTM_PLATFORM_LINUX
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/linux/");
		#elif RTM_PLATFORM_IOS
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/ios/");
		#elif RTM_PLATFORM_OSX
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/osx/");
		#elif RTM_PLATFORM_ANDROID
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/android/");
		#elif RTM_PLATFORM_WASM
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/wasm/");
		#elif RTM_PLATFORM_SWITCH
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/switch/");
		#elif RTM_PLATFORM_PS4
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/ps4/");
		#elif RTM_PLATFORM_PS5
			return strlCpy(ptr, uint32_t(_buffer + len - ptr), "/.data/ps5/");
		#else
			#error
		#endif

	#endif

	#if RTM_RETAIL
		const char* fn = pathGetFileName(_buffer);
		return 0 != strlCpy((char*)fn, _bufferSize - uint32_t(fn - _buffer), "data/");
	#endif

		return false;

#endif
	return false;
}

bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_appendPath, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	if (pathIsAbsolute(_appendPath))
		return false;

	size_t lenAppend = strLen(_appendPath);

	if (!_path)
	{
		if (lenAppend > _bufferSize - 1)
			return false;

		strlCpy(_buffer, _bufferSize, _appendPath);
		return true;
	}

	if (!pathIsDirectory(_path))
		return false;

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
		rtm::strlCat(_buffer, _bufferSize, "/");

	rtm::strlCat(_buffer, _bufferSize, _appendPath);

	return true;
}

bool pathUp(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	uint32_t len = strLen(_path);
	if (len == 0)
		return false;

	uint32_t slashes = pathIsDirectory(_path) ? 2 : 1;

	while ((len > 0) && slashes)
	{
		--len;
		if (isSlash(_path[len])) --slashes;
	}

	if (slashes == 0)
	{
		++len; // include the slash so the result is a directory path
		if (len < _bufferSize)
		{
			strlCpy(_buffer, _bufferSize, _path, len);
			return true;
		}
	}

	return false;
}

bool pathCanonicalize(const char* _path, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");


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
	RTM_ASSERT(_path, "");

	const char* pos = 0;
	while ((pos = rtm::strStr(_path, "..")) != 0)
	{
		// require "/.." (or "\..") — otherwise ".." is part of a name like "..x"
		if ((pos == _path) || !isSlash(*(pos - 1)))
			break;

		const char* prevSlash = pos - 2;
		while ((prevSlash >= _path) && !isSlash(*prevSlash))
			--prevSlash;
		if (prevSlash < _path)
			break;

		const char* nextDir = pos + 2;
		if (isSlash(*nextDir))
			++nextDir;
		else if (*nextDir != '\0')
			break;

		size_t len = strLen(nextDir) + 1;
		rtm::memMove((void*)(prevSlash + 1), nextDir, len);
	}

	toUnixSlashes(_path);
}

bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_relative, "");
	RTM_ASSERT(_base, "");
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

static inline bool isEqualPathChar(char _c1, char _c2)
{
	return (charToUpper(_c1) == charToUpper(_c2)) || (isSlash(_c1) && isSlash(_c2));
}

bool pathMakeRelative(const char* _pathFrom, const char* _pathTo, char* _buffer, uint32_t _bufferSize)
{
	RTM_ASSERT(_pathFrom, "");
	RTM_ASSERT(_pathTo, "");
	RTM_ASSERT(_buffer, "");
	RTM_ASSERT(_bufferSize > 0, "");

	if (!_pathFrom  || !_pathTo)
		return false;

	uint32_t srcLen = strLen(_pathFrom);

	int cr=0;
	int lastSlash = 0;
	while (_pathFrom[cr] && _pathTo[cr] && isEqualPathChar(_pathFrom[cr], _pathTo[cr]))
	{
		if (isSlash(_pathFrom[cr]))
			lastSlash = cr;
		++cr;
	}

	if ((!isSlash(_pathFrom[srcLen - 1])) && (isSlash(_pathTo[cr])))
	{
		lastSlash = cr - 1;
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
	RTM_ASSERT(_path, "");

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

bool pathIsDirectory(const char* _path)
{
	RTM_ASSERT(_path, "");

	if (!_path)
		return false;

	uint32_t len = strLen(_path);
	if (len == 0) return false;
	return isSlash(_path[len - 1]);
}

static inline const char* findSlash(const char* _str)
{
    while ((*_str != '\0') && !isSlash(*_str))
        ++_str;
    return _str;
}

bool pathSplit(const char* _path, uint32_t* _numDirectories, StringView* _dirList, uint32_t _maxDirs)
{
	RTM_ASSERT(_path, "");
	RTM_ASSERT(_numDirectories, "");
	RTM_ASSERT(_dirList, "");
	RTM_ASSERT(_maxDirs > 0, "");

	*_numDirectories = 0;

	if (!_path)
		return false;

	const char* ps = findSlash(_path);
	const char* pe = ps;

	if (*ps != '\0')
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

    if (numDirs > _maxDirs)
    {
        return false;
    }

	for (uint32_t i=0; i<numDirs; ++i)
	{
		uint32_t de = (dirOffsets[i] >> 16) & 0xffff;
		uint32_t ds =  dirOffsets[i] & 0xffff;
        _dirList[i].set(&_path[ds], (uint32_t)(de-ds));
	}

    return true;
}

} // namespace rtm
