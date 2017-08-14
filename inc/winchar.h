//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_WINCHAR_H__
#define __RTM_RBASE_WINCHAR_H__

#include <rbase/inc/platform.h>

namespace rtm {

#if RTM_PLATFORM_WINDOWS
	class MultiToWide
	{
		static const int S_ON_STACK_SIZE = 1024;
		wchar_t		m_string[S_ON_STACK_SIZE];

	public:
		wchar_t*	m_ptr;

		MultiToWide(const char* _string, bool _path = true);
		~MultiToWide();
		operator wchar_t* () { return m_ptr; }
	};

	class WideToMulti
	{
		static const int S_ON_STACK_SIZE = 1024;
		char	m_string[S_ON_STACK_SIZE];

	public:
		char*	m_ptr;

		WideToMulti(const wchar_t* _string);
		~WideToMulti();
		operator char* () { return m_ptr; }
	};
#endif // RTM_PLATFORM_WINDOWS

} // namespace rtm

#endif // __RTM_RBASE_WINCHAR_H__
