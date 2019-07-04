//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_WINCHAR_H
#define RTM_RBASE_WINCHAR_H

#include <rbase/inc/platform.h>

namespace rtm {

#if RTM_PLATFORM_WINDOWS
	class MultiToWide
	{
		static const int S_ON_STACK_SIZE = 1024;
		wchar_t		m_string[S_ON_STACK_SIZE];
		uint32_t	m_size;

	public:
		wchar_t*	m_ptr;

		MultiToWide(const char* _string, bool _path = true);
		~MultiToWide();
		inline uint32_t size() const { return m_size; }
		operator wchar_t* () { return m_ptr; }
	};

	class WideToMulti
	{
		static const int S_ON_STACK_SIZE = 1024;
		char		m_string[S_ON_STACK_SIZE];
		uint32_t	m_size;

	public:
		char*	m_ptr;

		WideToMulti(const wchar_t* _string);
		~WideToMulti();
		inline uint32_t size() const { return m_size; }
		operator char* () { return m_ptr; }
	};
#endif // RTM_PLATFORM_WINDOWS

} // namespace rtm

#endif // RTM_RBASE_WINCHAR_H
