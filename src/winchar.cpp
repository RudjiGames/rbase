//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------// 

#include <rbase_pch.h>
#include <rbase/inc/stringfn.h>
#include <rbase/inc/winchar.h>

#if RTM_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rtm {

static const int S_LONG_PATH_LEN = 4; // wcslen(L"\\\\?\\");
static const int S_LONG_PATH_UNC_LEN = S_LONG_PATH_LEN + 3; // wcslen(L"\\\\?\\UNC");

static char* makeLongPath(const char* _path, const char* _name, char* _outBuff, size_t _outBuffSize);

template <typename CHRT = char>
static inline bool isSlash(CHRT c)
{
	return ((c == CHRT('\\')) || (c == CHRT('/')));
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

static char* makeLongPath(const char* _path, const char* _name, char* _outBuff, size_t _outBuffSize)
{
	const int32_t outBuffSize = (int32_t)_outBuffSize;

	const char* longPathPrefix = "\\\\?\\";
	strlncpy(_outBuff, outBuffSize, longPathPrefix);

	int additionChars = S_LONG_PATH_LEN;
	if (_path)
	{
		if ((_path[0] == L'\\') && (_path[1] == L'\\'))
		{
			// UNC path -
			additionChars = S_LONG_PATH_UNC_LEN;
			strlncat(_outBuff, outBuffSize, "UNC");
			strlncat(_outBuff, outBuffSize, _path);
		}
		else
			strlncat(_outBuff, outBuffSize, _path);
	}

	if (_name)
		strlncat(_outBuff, outBuffSize, _name);

	size_t pathLength = strlen(_outBuff);
	if (pathLength - additionChars > MAX_PATH)
	{
		replaceSlashes(_outBuff, '/');
		return _outBuff;
	}
	else
	{
		replaceSlashes(_outBuff, '\\');
		return _outBuff + additionChars;
	}
}

MultiToWide::MultiToWide(const char* _string, bool _path)
{
	m_ptr = &m_string[0];
	*m_ptr = 0;

	if (!_string)
		return;

	size_t strLen = strlen(_string) + S_LONG_PATH_UNC_LEN + 2; // 2: last slash and null term
	char* tmpBuff = 0;

	const char* pathToConvert = _string;
	if (_path)
	{
		tmpBuff = new char[strLen];
		pathToConvert = makeLongPath(_string, 0, tmpBuff, strLen);
	}

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, pathToConvert, -1, NULL, 0);
	if (size_needed > S_ON_STACK_SIZE)
		m_ptr = new  wchar_t[size_needed + 1];
	else
		m_ptr = &m_string[0];

	MultiByteToWideChar(CP_UTF8, 0, pathToConvert, -1, m_ptr, size_needed);

	if (_path)
		delete[] tmpBuff;
}

MultiToWide::~MultiToWide()
{
	if (m_ptr != &m_string[0])
		delete[] m_ptr;
}

WideToMulti::WideToMulti(const wchar_t* _string)
{
	m_ptr = &m_string[0];
	m_string[0] = 0;

	if (!_string)
		return;

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, _string, -1, NULL, 0, NULL, NULL);
	if (size_needed > S_ON_STACK_SIZE)
		m_ptr = new char[size_needed + 1];
	else
		m_ptr = &m_string[0];

	WideCharToMultiByte(CP_UTF8, 0, _string, -1, m_ptr, size_needed, NULL, NULL);
}

WideToMulti::~WideToMulti()
{
	if (m_ptr != &m_string[0])
		delete[] m_ptr;
}

} // namespace rtm

#endif // RTM_PLATFORM_WINDOWS
