//--------------------------------------------------------------------------//
/// Copyright 2026 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------// 

#include <rbase_pch.h>
#include <rbase/inc/stringfn.h>
#include <rbase/inc/widechar.h>

#include <stdlib.h> // mbstowcs, wcstombs

namespace rtm {

// Per-thread reusable conversion buffers: grow once to the largest string seen and reuse, so repeated
// conversions of long strings (demangled template names, long paths) stop hitting the heap every time.
// A nested conversion on the same thread (buffer busy) falls back to a per-instance heap buffer.
namespace {
	template <typename T>
	struct Scratch
	{
		T*		m_buf  = nullptr;
		size_t	m_cap  = 0;
		bool	m_busy = false;
		~Scratch() { ::free(m_buf); }
		T* acquire(size_t _count)
		{
			if (m_busy) return nullptr;
			if (m_cap < _count)
			{
				T* nb = (T*)::realloc(m_buf, _count * sizeof(T));
				if (!nb) return nullptr;
				m_buf = nb; m_cap = _count;
			}
			m_busy = true;
			return m_buf;
		}
		void release() { m_busy = false; }
	};
	static thread_local Scratch<wchar_t> s_wScratch;
	static thread_local Scratch<char>    s_cScratch;
}

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
	RTM_UNUSED(_path);

	m_size			= 0;
	m_ptr			= &m_string[0];
	m_fromScratch	= false;
	*m_ptr			= 0;

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
		wchar_t* tls = s_wScratch.acquire(len + 1);	// reuse the per-thread buffer instead of new[] each call
		if (tls) { m_ptr = tls; m_fromScratch = true; }
		else m_ptr = new wchar_t[len + 1];			// nested conversion on this thread / OOM -> heap
	}

	const char* stringToConvert = _string;

#if RTM_PLATFORM_WINDOWS
	if (_path)
	{
		char tempBuffer[CHARS_ON_STACK];
		char* tmpBuff = tempBuffer;
		bool  tmpScratch = false;
		if (len > CHARS_ON_STACK)
		{
			char* tls = s_cScratch.acquire(len + 1);
			if (tls) { tmpBuff = tls; tmpScratch = true; }
			else tmpBuff = new char[len + 1];
		}
		stringToConvert = makeLongPath(_string, 0, tmpBuff, len);
		m_size = uint32_t(mbstowcs(m_ptr, stringToConvert, len + 1));
		if (tmpScratch)					s_cScratch.release();
		else if (tmpBuff != tempBuffer)	delete[] tmpBuff;
	}
	else
#endif // RTM_PLATFORM_WINDOWS
	{
		m_size = uint32_t(mbstowcs(m_ptr, stringToConvert, len + 1));
	}

	RTM_ASSERT(static_cast<size_t>(-1) != m_size, "");
}

MultiToWide::~MultiToWide()
{
	if (m_fromScratch)				s_wScratch.release();
	else if (m_ptr != &m_string[0])	delete[] m_ptr;
}

WideToMulti::WideToMulti(const wchar_t* _string)
{
	m_size			= 0;
	m_ptr			= &m_string[0];
	m_fromScratch	= false;
	m_string[0]		= 0;

	if (!_string)
		return;

	// get multibyte string length
	uint32_t len = (uint32_t)wcstombs(0, _string, 0);

	if (len + 1 > CHARS_ON_STACK)
	{
		char* tls = s_cScratch.acquire(len + 1);	// reuse the per-thread buffer instead of new[] each call
		if (tls) { m_ptr = tls; m_fromScratch = true; }
		else m_ptr = new char[len + 1];
	}

	m_size = uint32_t(wcstombs(m_ptr, _string, len + 1));
	RTM_ASSERT(static_cast<size_t>(-1) != m_size, "");
}

WideToMulti::~WideToMulti()
{
	if (m_fromScratch)				s_cScratch.release();
	else if (m_ptr != &m_string[0])	delete[] m_ptr;
}

} // namespace rtm
