//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CPU_H
#define RTM_RBASE_CPU_H

#include <rbase/inc/platform.h>

namespace rtm {

	namespace CPU {

		/// Returns current CPU clock counter.
		///
		/// @returns current CPU clock counter.
		inline static uint64_t clock();

		/// Returns CPU clock frequency.
		///
		/// @returns  CPU clock frequency.
		inline static uint64_t frequency();

		/// Returns current time.
		///
		/// @returns current time, in seconds.
		inline static float time();

		/// Returns current time.
		///
		/// @param[in] _startClock: Start CPU clock time
		///
		/// @returns current time, in seconds.
		inline static float time(uint64_t _startClock);

		/// Calculates time based on clock and frequency.
		///
		/// @param[in] _clock: CPU clock counter
		/// @param[in] _frequency: CPU frequency
		///
		/// @returns current time, in seconds.
		inline static float time(uint64_t _clock, uint64_t _frequency);

	}  // namespace CPU
} // namespace  rtm

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif // WIN32_LEAN_AND_MEAN
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif // NOMINMAX
	#include <windows.h>
	unsigned __int64 __rdtsc(void);
	#if RTM_COMPILER_MSVC && !RTM_PLATFORM_WINRT
		#pragma intrinsic(__rdtsc)
	#endif // RTM_COMPILER_MSVC && !RTM_PLATFORM_WINRT
#elif RTM_PLATFORM_PS3
	#include <sys/sys_time.h>
#elif RTM_PLATFORM_ANDROID
	#include <time.h>
#elif RTM_PLATFORM_EMSCRIPTEN
	#include <emscripten.h>
#else
	#include <sys/time.h>
#endif

namespace rtm {

	namespace CPU {

		inline static uint64_t clock()
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			int64_t q = li.QuadPart;
#elif RTM_PLATFORM_PS3
			int64_t q = (int64_t)sys_time_get_system_time();
#elif RTM_PLATFORM_PS4
			int64_t q = sceKernelReadTsc();
#elif RTM_PLATFORM_ANDROID
			int64_t q = ::clock();
#elif RTM_PLATFORM_EMSCRIPTEN
			int64_t q = (int64_t)(emscripten_get_now() * 1000.0);
#else
			struct timeval now;
			gettimeofday(&now, 0);
			int64_t q = now.tv_sec * 1000000 + now.tv_usec;
#endif
			return q;
		}

		inline static uint64_t frequency()
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
			LARGE_INTEGER li;
			QueryPerformanceFrequency(&li);
			return li.QuadPart;
#elif RTM_PLATFORM_ANDROID
			return CLOCKS_PER_SEC;
#elif RTM_PLATFORM_PS4
			return sceKernelGetTscFrequency();
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

	} // namespace CPU

	struct Timer
	{
		uint64_t	m_timeStarted;
		uint64_t	m_timePaused;
		bool		m_started;
		bool		m_paused;

		inline Timer(bool _start = false)
		{
			m_timeStarted	= 0;
			m_timePaused	= 0;
			m_paused		= false;
			m_started		= false;

			if (_start)
				start();
		}

		inline bool isStarted() const { return m_started; }
		inline bool isStopped() const { return !m_started; }
		inline bool isPaused()  const { return m_paused; }
		inline bool isActive()  const { return !m_paused & m_started; }

		inline void pause()
		{
			RTM_ASSERT(!m_paused, "");
			RTM_ASSERT(m_started, "");

			if (m_paused || !m_started)
				return;

			m_paused		= true;
			m_timePaused	= CPU::clock();
		}

		inline void resume()
		{
			RTM_ASSERT(m_paused, "");
			RTM_ASSERT(m_started, "");

			if (!m_paused)
				return;

			m_paused		= false;
			m_timeStarted	= m_timeStarted + CPU::clock() - m_timePaused;
		}

		inline void stop()
		{
			RTM_ASSERT(m_started, "");
			m_started		= false;
		}

		inline void start()
		{
			RTM_ASSERT(!m_started, "");
			if (m_started)
				return;

			m_timeStarted	= CPU::clock();
			m_started		= true;
			m_paused		= false;
		}

		inline void reset()
		{
			RTM_ASSERT(m_started, "");
			m_timeStarted	= CPU::clock();
			m_timePaused	= 0;
			m_paused		= false;
		}

		inline float elapsed()
		{
			if (!m_started)
				return 0;

			if (m_paused)
				return CPU::time(m_timePaused - m_timeStarted, CPU::frequency());

			return CPU::time(m_timeStarted);
		}
	};

} // namespace rtm

#endif // RTM_RBASE_CPU_H
