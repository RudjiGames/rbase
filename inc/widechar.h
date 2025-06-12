//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_WIDECHAR_H
#define RTM_RBASE_WIDECHAR_H

#include <rbase/inc/platform.h>

namespace rtm {

	//--------------------------------------------------------------------------
	/// Converts multi byte string to wide character string, on stack if possible.
	//--------------------------------------------------------------------------
	class MultiToWide
	{
		static const int CHARS_ON_STACK = 2048;

		wchar_t		m_string[CHARS_ON_STACK];
		uint32_t	m_size;

	public:
		wchar_t*	m_ptr;

		MultiToWide(const char* _string, bool _path = true);
		~MultiToWide();
		inline uint32_t size() const { return m_size; }
		inline operator wchar_t* () { return m_ptr; }
	};

	//--------------------------------------------------------------------------
	/// Converts wide character string to multi byte string, on stack if possible.
	//--------------------------------------------------------------------------
	class WideToMulti
	{
		static const int CHARS_ON_STACK = 2048;

		char		m_string[CHARS_ON_STACK];
		uint32_t	m_size;

	public:
		char*	m_ptr;

		WideToMulti(const wchar_t* _string);
		~WideToMulti();
		inline uint32_t size() const { return m_size; }
		inline operator char* () { return m_ptr; }
	};

} // namespace rtm

#endif // RTM_RBASE_WIDECHAR_H
