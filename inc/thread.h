//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_THREAD_H
#define RTM_RBASE_THREAD_H

#include <rbase/inc/platform.h>
#include <rbase/inc/semaphore.h>

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE
	#include <immintrin.h>
#elif RTM_PLATFORM_WINRT
	#include <processthreadsapi.h>
#elif RTM_PLATFORM_POSIX
	#include <pthread.h>
	#include <sched.h>	// sched_yield
	#include <unistd.h>	// syscall
	#if ! RTM_PLATFORM_PS4
	#include <sys/syscall.h>
	#endif
	#include <time.h> // nanosleep
#else // RTM_PLATFORM_POSIX
	#error "Unsupported platform/compiler!"
#endif

namespace rtm {

	typedef int32_t (*ThreadEntry)(void* _userData);
	
	class Thread
	{
		RTM_CLASS_NO_COPY(Thread)

	protected:
		ThreadEntry	m_entry;
		void*		m_userData;
		bool		m_started;
		bool		m_entryDone;
		int32_t		m_exitCode;
		Semaphore	m_semaphore;
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
		HANDLE		m_handle;
#elif RTM_PLATFORM_POSIX
		pthread_t	m_handle;
#else
#error "Unsupported platform!"
#endif

	public:
		Thread() :
			m_entry(0),
			m_userData(0),
			m_started(false),
			m_entryDone(false),
			m_exitCode(0)
		{
		}

		virtual ~Thread()
		{
			if (m_started)
				stop();
		}

		void start(ThreadEntry _entry, void* _userData, uint32_t _stackSize = 0)
		{
			m_entry		= _entry;
			m_userData	= _userData;

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
			m_handle = CreateThread(NULL, _stackSize, threadEntry, this, 0, NULL);
#elif RTM_PLATFORM_POSIX
			int result;
			(void)result;

			pthread_attr_t attr;
			result = pthread_attr_init(&attr);
			RTM_ASSERT(0 == result, "pthread_attr_init failed!");

			if (0 != _stackSize)
			{
				result = pthread_attr_setstacksize(&attr, _stackSize);
				RTM_ASSERT(0 == result, "pthread_attr_setstacksize failed!");
			}

			result = pthread_create(&m_handle, &attr, &threadEntry, this);
			RTM_ASSERT(0 == result, "pthread_create failed!");
#else
#error "Unsupported platform!"
#endif
			m_started	= true;
			m_semaphore.wait();
		}

		void stop()
		{
			if (!m_started)
				return;

			RTM_ASSERT(m_started, "Thread was not started!");
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
			WaitForSingleObjectEx(m_handle, INFINITE, 0);
			GetExitCodeThread(m_handle, (DWORD*)&m_exitCode);
			CloseHandle(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
#elif RTM_PLATFORM_POSIX
			union { void* ptr; int32_t i; } cast;
			pthread_join(m_handle, &cast.ptr);
			m_exitCode = cast.i;
			m_handle = 0;
#else
#error "Unsupported platform!"
#endif
			m_started = false;
		}

		bool isStarted() const
		{
			return m_started;
		}

		bool isEntryDone() const
		{
			return m_entryDone;
		}

		int32_t getExitCode() const
		{
			return m_exitCode;
		}

		int32_t entry()
		{
			m_semaphore.post();
			int32_t ret = m_entry(m_userData);
			m_entryDone = true;
			return ret;
		}

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
		static DWORD WINAPI threadEntry(LPVOID _this)
		{
			Thread* thread = (Thread*)_this;
			int32_t result = thread->entry();
			return result;
		}
#elif RTM_PLATFORM_POSIX
		static void* threadEntry(void* _this)
		{
			Thread* thread = (Thread*)_this;
			union { void* ptr; int32_t i; } cast;
			cast.i = thread->entry();
			return cast.ptr;
		}
#else
#error "Unsupported platform!"
#endif

		static inline uint64_t getThreadID()
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
			return (uint64_t)GetCurrentThreadId();
#elif RTM_PLATFORM_LINUX
			return (uint64_t)syscall(SYS_gettid);
#elif RTM_PLATFORM_IOS || RTM_PLATFORM_OSX
			return (mach_port_t)::pthread_mach_thread_np(pthread_self() );
#elif RTM_PLATFORM_PS4
			return scePthreadGetthreadid();
#elif RTM_PLATFORM_PS3
			sys_ppu_thread_t tid;
			sys_ppu_thread_get_id(&tid);
			return (uint64_t)tid;
#elif RTM_PLATFORM_ANDROID || RTM_PLATFORM_EMSCRIPTEN || RTM_PLATFORM_SWITCH
			return pthread_self();
#else
			#error "Undefined platform!"
#endif
		}

		static inline void sleep(uint32_t _ms)
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
			::Sleep(_ms);
#else
			timespec req = {(time_t)_ms/1000, (long)((_ms%1000)*1000000)};
			timespec rem = {0, 0};
			nanosleep(&req, &rem);
#endif
		}

		static inline void yield()
		{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE
			::SwitchToThread();
#elif RTM_PLATFORM_WINRT
			RTM_ERROR("yield not implemented!");
#elif RTM_PLATFORM_ANDROID || RTM_PLATFORM_LINUX || RTM_PLATFORM_OSX || RTM_PLATFORM_EMSCRIPTEN || RTM_PLATFORM_SWITCH
			::sched_yield();
#elif RTM_PLATFORM_PS4
			scePthreadYield();
#else
			#error "Unsupported platform/compiler!"
#endif
		}
	};

} // namespace rtm

#endif // RTM_RBASE_THREAD_H
