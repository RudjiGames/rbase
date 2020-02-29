//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_MUTEX_H
#define RTM_RBASE_MUTEX_H

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	#include <windows.h>
#endif // RTM_PLATFORM_WINDOWS

#if RTM_PLATFORM_POSIX
	#include <pthread.h>
#endif

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	typedef CRITICAL_SECTION rtm_mutex;

	static inline void rtm_mutex_init(rtm_mutex* _mutex) {
#if RTM_PLATFORM_WINRT
		InitializeCriticalSectionEx(_mutex, 4000, 0);
#else 
		InitializeCriticalSection(_mutex);
#endif
	}

	static inline void rtm_mutex_destroy(rtm_mutex* _mutex) {
		DeleteCriticalSection(_mutex);
	}

	static inline void rtm_mutex_lock(rtm_mutex* _mutex) {
		EnterCriticalSection(_mutex);
	}

	static inline int rtm_mutex_trylock(rtm_mutex* _mutex)	{
		return TryEnterCriticalSection(_mutex) ? 0 : 1;
	}

	static inline void rtm_mutex_unlock(rtm_mutex* _mutex)	{
		LeaveCriticalSection(_mutex);
	}

#elif RTM_PLATFORM_POSIX
	typedef pthread_mutex_t rtm_mutex;

	static inline void rtm_mutex_init(rtm_mutex* _mutex) {
		pthread_mutex_init(_mutex, 0);
	}

	static inline void rtm_mutex_destroy(rtm_mutex* _mutex) {
		pthread_mutex_destroy(_mutex);
	}

	static inline void rtm_mutex_lock(rtm_mutex* _mutex) {
		pthread_mutex_lock(_mutex);
	}

	static inline int rtm_mutex_trylock(rtm_mutex* _mutex) {
		return pthread_mutex_trylock(_mutex);
	}

	static inline void rtm_mutex_unlock(rtm_mutex* _mutex) {
		pthread_mutex_unlock(_mutex);
	}

#elif RTM_PLATFORM_PS4
	typedef ScePthreadMutex rtm_mutex;

	static inline void rtm_mutex_init(rtm_mutex* _mutex) {
		ScePthreadMutexattr mutexAttr;
		scePthreadMutexattrInit(&mutexAttr);
		scePthreadMutexattrSettype(&mutexAttr, SCE_PTHREAD_MUTEX_RECURSIVE);
		scePthreadMutexInit(_mutex, &mutexAttr, 0);
		scePthreadMutexattrDestroy(&mutexAttr);
	}

	static inline void rtm_mutex_destroy(rtm_mutex* _mutex) {
		scePthreadMutexDestroy(_mutex);
	}

	static inline void rtm_mutex_lock(rtm_mutex* _mutex) {
		scePthreadMutexLock(_mutex);
	}

	static inline int rtm_mutex_trylock(rtm_mutex* _mutex) {
		return (scePthreadMutexTrylock(_mutex) == 0) ? 0 : 1;
	}

	static inline void rtm_mutex_unlock(rtm_mutex* _mutex) {
		scePthreadMutexUnlock(_mutex);
	}
	
#endif

	//--------------------------------------------------------------------------
	class Mutex
	{
		rtm_mutex m_mutex;

		Mutex(const Mutex& _rhs) = delete;
		Mutex& operator=(const Mutex& _rhs) = delete;
		
	public:

		inline Mutex()
		{
			rtm_mutex_init(&m_mutex);
		}

		inline ~Mutex() 
		{
			rtm_mutex_destroy(&m_mutex);
		}

		inline void lock()
		{
			rtm_mutex_lock(&m_mutex);
		}

		inline void unlock()
		{
			rtm_mutex_unlock(&m_mutex);
		}

		inline bool tryLock()
		{
			return (rtm_mutex_trylock(&m_mutex) == 0);
		}
	};

	//--------------------------------------------------------------------------
	class ScopedMutexLocker
	{
		Mutex& m_mutex;

		ScopedMutexLocker() = delete;
		ScopedMutexLocker(const ScopedMutexLocker&) = delete;
		ScopedMutexLocker& operator = (const ScopedMutexLocker&) = delete;

	public:

		inline ScopedMutexLocker(Mutex& _mutex) :
			m_mutex(_mutex)
		{
			m_mutex.lock();
		}

		inline ~ScopedMutexLocker()
		{
			m_mutex.unlock();
		}
	};

} // namespace rtm

#endif // RTM_RBASE_MUTEX_H
