//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------// 

#include <rbase_pch.h>
#include <rbase/inc/winpath.h>

#if RTM_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rtm {

static const int S_LONG_PATH_LEN = 4; // wcslen(L"\\\\?\\");

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

WinPath::WinPath(const char* _path)
{
	m_ptr = &m_string[0];
	*m_ptr = 0;

	if (!_path)
		return;

	size_t strLen = strlen(_path) + S_LONG_PATH_LEN + 2; // 2: last slash and null term
	char* tmpBuff = new char[strLen];
	char* pathToConvert = makeLongPath(_path, 0, tmpBuff, strLen);

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, pathToConvert, -1, NULL, 0);
	if (size_needed > S_ON_STACK_SIZE)
		m_ptr = new  wchar_t[size_needed + 1];
	else
		m_ptr = &m_string[0];

	MultiByteToWideChar(CP_UTF8, 0, pathToConvert, -1, m_ptr, size_needed);
	delete[] tmpBuff;
}

WinPath::~WinPath()
{
	if (m_ptr != &m_string[0])
		delete[] m_ptr;
}

static char* makeLongPath(const char* _path, const char* _name, char* _outBuff, size_t _outBuffSize)
{
	const char* longPathPrefix = "\\\\?\\";
	strcpy_s(_outBuff, _outBuffSize, longPathPrefix);

	if (_path)
	{
		if ((_path[0] == L'\\') && (_path[1] == L'\\'))
		{
			// UNC path - 
			strcat_s(_outBuff, _outBuffSize, "UNC\\");
			strcat_s(_outBuff, _outBuffSize, _path + 1);
		}
		else
			strcat_s(_outBuff, _outBuffSize, _path);
	}

	if (_name)
		strcat_s(_outBuff, _outBuffSize, _name);

	size_t pathLength = strlen(_outBuff);
	if (pathLength - 4 > MAX_PATH)
	{
		replaceSlashes(_outBuff, '/');
		return _outBuff;
	}
	else
	{
		replaceSlashes(_outBuff, '\\');
		return _outBuff + 4;
	}
}

} // namespace rtm

#endif // RTM_PLATFORM_WINDOWS
