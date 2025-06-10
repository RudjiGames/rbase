//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_MUTEX_H
#define RTM_RBASE_MUTEX_H

#include <rbase/inc/platform.h>

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	typedef CRITICAL_SECTION mutex;
#elif RTM_PLATFORM_POSIX
	#include <pthread.h>
	typedef pthread_mutex_t mutex;
#elif RTM_PLATFORM_PS4 || RTM_PLATFORM_PS5
	typedef ScePthreadMutex mutex;
#endif

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path: File system path
	///
	/// @returns the pointer to file name portion of the path, nullptr if dir.
	static inline void mutexInit(mutex* _mutex);

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path: File system path
	///
	/// @returns the pointer to file name portion of the path, nullptr if dir.
	static inline void mutexDestroy(mutex* _mutex);

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path: File system path
	///
	/// @returns the pointer to file name portion of the path, nullptr if dir.
	static inline void mutexLock(mutex* _mutex);

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path: File system path
	///
	/// @returns the pointer to file name portion of the path, nullptr if dir.
	static inline int mutexTryLock(mutex* _mutex);

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path: File system path
	///
	/// @returns the pointer to file name portion of the path, nullptr if dir.
	static inline void mutexUnlock(mutex* _mutex);

} // namespace rtm

/// ---------------------------------------------------------------------- ///
///  Implementation                                                        ///
/// ---------------------------------------------------------------------- ///

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	typedef CRITICAL_SECTION mutex;

	static inline void mutexInit(mutex* _mutex) {
#if RTM_PLATFORM_WINRT
		InitializeCriticalSectionEx(_mutex, 4000, 0);
#else 
		InitializeCriticalSection(_mutex);
#endif
	}

	static inline void mutexDestroy(mutex* _mutex) {
		DeleteCriticalSection(_mutex);
	}

	static inline void mutexLock(mutex* _mutex) {
		EnterCriticalSection(_mutex);
	}

	static inline int mutexTryLock(mutex* _mutex)	{
		return TryEnterCriticalSection(_mutex) ? 0 : 1;
	}

	static inline void mutexUnlock(mutex* _mutex)	{
		LeaveCriticalSection(_mutex);
	}

#elif RTM_PLATFORM_POSIX
	typedef pthread_mutex_t mutex;

	static inline void mutexInit(mutex* _mutex) {
		pthread_mutex_init(_mutex, 0);
	}

	static inline void mutexDestroy(mutex* _mutex) {
		pthread_mutex_destroy(_mutex);
	}

	static inline void mutexLock(mutex* _mutex) {
		pthread_mutex_lock(_mutex);
	}

	static inline int mutexTryLock(mutex* _mutex) {
		return pthread_mutex_trylock(_mutex);
	}

	static inline void mutexUnlock(mutex* _mutex) {
		pthread_mutex_unlock(_mutex);
	}

#elif RTM_PLATFORM_PS4 || RTM_PLATFORM_PS5
	typedef ScePthreadMutex mutex;

	static inline void mutexInit(mutex* _mutex) {
		ScePthreadMutexattr mutexAttr;
		scePthreadMutexattrInit(&mutexAttr);
		scePthreadMutexattrSettype(&mutexAttr, SCE_PTHREAD_MUTEX_RECURSIVE);
		scePthreadMutexInit(_mutex, &mutexAttr, 0);
		scePthreadMutexattrDestroy(&mutexAttr);
	}

	static inline void mutexDestroy(mutex* _mutex) {
		scePthreadMutexDestroy(_mutex);
	}

	static inline void mutexLock(mutex* _mutex) {
		scePthreadMutexLock(_mutex);
	}

	static inline int mutexTryLock(mutex* _mutex) {
		return (scePthreadMutexTrylock(_mutex) == 0) ? 0 : 1;
	}

	static inline void mutexUnlock(mutex* _mutex) {
		scePthreadMutexUnlock(_mutex);
	}
	
#endif

	//--------------------------------------------------------------------------
	class Mutex
	{
		mutex m_mutex;

		Mutex(const Mutex& _rhs) = delete;
		Mutex& operator=(const Mutex& _rhs) = delete;
		
	public:

		inline Mutex()
		{
			mutexInit(&m_mutex);
		}

		inline ~Mutex() 
		{
			mutexDestroy(&m_mutex);
		}

		inline void lock()
		{
			mutexLock(&m_mutex);
		}

		inline void unlock()
		{
			mutexUnlock(&m_mutex);
		}

		inline bool tryLock()
		{
			return (mutexTryLock(&m_mutex) == 0);
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
