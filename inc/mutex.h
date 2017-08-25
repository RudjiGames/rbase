//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_MUTEX_H__
#define __RTM_RBASE_MUTEX_H__

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif // RTM_PLATFORM_WINDOWS

#if RTM_PLATFORM_ANDROID || RTM_PLATFORM_LINUX
	#include <pthread.h>
#endif

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XB1
	typedef CRITICAL_SECTION rtm_mutex;

	static inline void rtm_mutex_init(rtm_mutex* _mutex) {
		InitializeCriticalSection(_mutex);
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

#elif RTM_PLATFORM_LINUX || RTM_PLATFORM_OSX || RTM_PLATFORM_ANDROID
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

		Mutex(const Mutex& _rhs);
		Mutex& operator=(const Mutex& _rhs);
		
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

		ScopedMutexLocker();
		ScopedMutexLocker(const ScopedMutexLocker&);
		ScopedMutexLocker& operator = (const ScopedMutexLocker&);

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

#endif // __RTM_RBASE_MUTEX_H__
