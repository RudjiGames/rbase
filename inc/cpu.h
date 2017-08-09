//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_CPU_H__
#define __RTM_RBASE_CPU_H__

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS && defined(_MSC_VER)  && (_MSC_VER >= 1400)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
	unsigned __int64 __rdtsc(void);
	#pragma intrinsic(__rdtsc)
#elif RTM_PLATFORM_PS3
	#include <sys/sys_time.h>
#elif RTM_PLATFORM_ANDROID
	#include <time.h>
#else
	#include <sys/time.h>
#endif

namespace rtm {

	struct CPU
	{
		inline static uint64_t clock()
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOX360 || RTM_PLATFORM_XBOXONE
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			int64_t q = li.QuadPart;
#elif RTM_PLATFORM_PS3
			int64_t q = (int64_t)sys_time_get_system_time();
#elif RTM_PLATFORM_PS4
			int64_t q = sceKernelReadTsc();
#elif RTM_PLATFORM_ANDROID
			int64_t q = clock();
#else
			struct timeval now;
			gettimeofday(&now, 0);
			int64_t q = now.tv_sec * 1000000 + now.tv_usec;
#endif
			return q;
		}

		inline static uint64_t frequency()
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XENON
			LARGE_INTEGER li;
			QueryPerformanceFrequency(&li);
			return li.QuadPart;
#elif RTM_PLATFORM_ANDROID
			return CLOCKS_PER_SEC;
#else
			return 1000000;
#endif
		}

		inline static float time()
		{
			return float(clock()) / float(frequency());
		}
		
		inline static float time(uint64_t _startClock)
		{
			return time(clock() - _startClock, frequency());
		}
		
		inline static float time(uint64_t _clock, uint64_t _frequency)
		{
			return float(_clock) / float(_frequency);
		}
	};

} // namespace rtm

#endif // __RTM_RBASE_CPU_H__

