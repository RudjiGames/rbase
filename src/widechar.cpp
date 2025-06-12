//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------// 

#include <rbase_pch.h>
#include <rbase/inc/stringfn.h>
#include <rbase/inc/widechar.h>

#include <stdlib.h> // mbstowcs, wcstombs

namespace rtm {

#if RTM_PLATFORM_WINDOWS

#define MAX_PATH_LOCAL 260

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
	strlCpy(_outBuff, outBuffSize, longPathPrefix);

	int additionChars = S_LONG_PATH_LEN;
	if (_path)
	{
		if ((_path[0] == L'\\') && (_path[1] == L'\\'))
		{
			// UNC path -
			additionChars = S_LONG_PATH_UNC_LEN;
			strlCat(_outBuff, outBuffSize, "UNC");
			strlCat(_outBuff, outBuffSize, _path);
		}
		else
			strlCat(_outBuff, outBuffSize, _path);
	}

	if (_name)
		strlCat(_outBuff, outBuffSize, _name);

	size_t pathLength = strLen(_outBuff);
	if (pathLength - additionChars > MAX_PATH_LOCAL)
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
#endif // RTM_PLATFORM_WINDOWS

MultiToWide::MultiToWide(const char* _string, bool _path)
{
	m_size	= 0;
	m_ptr	= &m_string[0];
	*m_ptr	= 0;

	if (!_string)
		return;

	uint32_t len = strLen(_string);
#if RTM_PLATFORM_WINDOWS
	if (_path)
	{
		len += S_LONG_PATH_UNC_LEN + 2;
	}
#endif

	if (len + 1 > CHARS_ON_STACK)
	{
		wchar_t* allocString = new wchar_t[len + 1];
		m_ptr = allocString;
	}

	const char* stringToConvert = _string;

#if RTM_PLATFORM_WINDOWS
	if (_path)
	{
		char tempBuffer[CHARS_ON_STACK];
		char* tmpBuff = tempBuffer;
		if (len > CHARS_ON_STACK)
		{
			tmpBuff = new char[len + 1];
		}
		stringToConvert = makeLongPath(_string, 0, tmpBuff, len);
		m_size = uint32_t(mbstowcs(m_ptr, tmpBuff, CHARS_ON_STACK));
		if (tmpBuff != tempBuffer)
		{
			delete[] tmpBuff;
		}
	}
	else
#endif // RTM_PLATFORM_WINDOWS
	{
		m_size = uint32_t(mbstowcs(m_ptr, stringToConvert, CHARS_ON_STACK));
	}

	RTM_ASSERT(static_cast<size_t>(-1) != m_size, "");
}

MultiToWide::~MultiToWide()
{
	if (m_ptr != &m_string[0])
		delete[] m_ptr;
}

WideToMulti::WideToMulti(const wchar_t* _string)
{
	m_size		= 0;
	m_ptr		= &m_string[0];
	m_string[0] = 0;

	if (!_string)
		return;

	uint32_t len = strLen(_string);
	if (len + 1 > CHARS_ON_STACK)
	{
		char* allocString = new char[len + 1];
		m_ptr = allocString;
	}

	m_size = uint32_t(wcstombs(m_ptr, _string, CHARS_ON_STACK));
	RTM_ASSERT(static_cast<size_t>(-1) != m_size, "");
}

WideToMulti::~WideToMulti()
{
	if (m_ptr != &m_string[0])
		delete[] m_ptr;
}

} // namespace rtm
