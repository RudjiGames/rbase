//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CPU_H
#define RTM_RBASE_CPU_H

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif // NOMINMAX
	#include <windows.h>
	unsigned __int64 __rdtsc(void);
	#if RTM_COMPILER_MSVC
		#pragma intrinsic(__rdtsc)
	#endif // RTM_COMPILER_MSVC
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
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XB1
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
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XB1
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

#endif // RTM_RBASE_CPU_H

