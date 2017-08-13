//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_WINPATH_H__
#define __RTM_RBASE_WINPATH_H__

#include <rbase/inc/platform.h>

namespace rtm {

#if RTM_PLATFORM_WINDOWS
	class WinPath
	{
		static const int S_ON_STACK_SIZE = 1024;
		wchar_t		m_string[S_ON_STACK_SIZE];

	public:
		wchar_t*	m_ptr;

		WinPath(const char* _path);
		~WinPath();
	};
#endif 

} // namespace rtm

#endif // __RTM_RBASE_WINPATH_H__
