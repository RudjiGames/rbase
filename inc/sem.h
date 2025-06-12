//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_SEMAPHORE_H
#define RTM_RBASE_SEMAPHORE_H

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winbase.h>
	typedef HANDLE semaphore_t;
#elif RTM_PLATFORM_POSIX
	struct semaphore_t;
	#include <semaphore.h>
	#include <time.h>
	#include <pthread.h>
#else
	#error "Unsupported platform/compiler!"
#endif

namespace rtm {

	/// Initializes a semaphore.
	///
	/// @param[in] _sem   : Semaphore pointer
	///
	/// @returns true on success.
	static inline bool semaphore_init(semaphore_t* _sem);

	/// Destroys a semaphore.
	///
	/// @param[in] _sem   : Semaphore pointer
	static inline void semaphore_destroy(semaphore_t* _sem);

	/// Semaphore post.
	///
	/// @param[in] _sem   : Semaphore pointer
	/// @param[in] _count : Semaphore count
	static inline void semaphore_post(semaphore_t* _sem, uint32_t _count);

	/// Waits on a semaphore.
	///
	/// @param[in] _sem   : Semaphore pointer
	/// @param[in] _ms    : Milliseconds to wait for
	///
	/// @returns true on success.
	static inline bool semaphore_wait(semaphore_t* _sem, int32_t _ms = -1);

} // namespace rtm

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	typedef HANDLE semaphore_t;

	static inline bool semaphore_init(semaphore_t* _sem) {
#if RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
		*_sem = CreateSemaphoreExW(NULL, 0, 0x7fffffff, NULL, 0, SEMAPHORE_ALL_ACCESS);
#else
		*_sem = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
#endif
		return *_sem != 0;
	}

	static inline void semaphore_destroy(semaphore_t* _sem) {
		CloseHandle(*_sem);
	}

	static inline void semaphore_post(semaphore_t* _sem, uint32_t _count) {
		ReleaseSemaphore(*_sem, _count, NULL);
	}
	
	static inline bool semaphore_wait(semaphore_t* _sem, int32_t _ms) {
		unsigned long ms = (0 > _ms) ? INFINITE : _ms;
		return WAIT_OBJECT_0 == WaitForSingleObjectEx(*_sem, ms, 0);
	}

#elif RTM_PLATFORM_POSIX

	struct semaphore_t
	{
		pthread_mutex_t	m_mutex;
		pthread_cond_t	m_cv;
		int32_t			m_count;
	};
	
	static inline bool semaphore_init(semaphore_t* _sem)
	{
	
		int result;
		result = pthread_mutex_init(&_sem->m_mutex, NULL);
		if (result != 0) return false;

		result = pthread_cond_init(&_sem->m_cv, NULL);
		if (result != 0) return false;
		_sem->m_count = 0;

		return true;
	}

	static inline void semaphore_destroy(semaphore_t* _sem)
	{
		pthread_cond_destroy(&_sem->m_cv);
		pthread_mutex_destroy(&_sem->m_mutex);
	}

	static inline void semaphore_post(semaphore_t* _sem, uint32_t _count)
	{
		pthread_mutex_lock(&_sem->m_mutex);

		for (uint32_t i=0; i<_count; ++i)
			pthread_cond_signal(&_sem->m_cv);

		_sem->m_count += _count;

		pthread_mutex_unlock(&_sem->m_mutex);
	}
	
	static inline bool semaphore_wait(semaphore_t* _sem, int32_t _ms)
	{
		int result = pthread_mutex_lock(&_sem->m_mutex);

	#if RTM_PLATFORM_ANDROID
		if (_ms != -1)
		{
			timespec ts;
			rtm::memSet(&ts, sizeof(timespec), 0);
			ts.tv_nsec = _ms * 1000;
			while (result == 0 && 0 >= _sem->m_count)
				result = pthread_cond_timedwait(&_sem->m_cv, &_sem->m_mutex, &ts);
		}
		else
	#endif
		{
			RTM_ASSERT(-1 == _ms, "NaCl, iOS and OSX don't support pthread_cond_timedwait at this moment.");
			while (result == 0 && 0 >= _sem->m_count)
				result = pthread_cond_wait(&_sem->m_cv, &_sem->m_mutex);
		}


		bool ok = result == 0;

		if (ok)
			--_sem->m_count;

		result = pthread_mutex_unlock(&_sem->m_mutex);
		return result == 0;
	}
	
#else
	#error "Unsupported platform/compiler!"
#endif

	class Semaphore
	{
		RTM_CLASS_NO_COPY(Semaphore)

		semaphore_t m_semaphore;

	public:
		inline Semaphore()
		{
			semaphore_init(&m_semaphore);
		}

		inline ~Semaphore()
		{
			semaphore_destroy(&m_semaphore);
		}

		inline void post(uint32_t _count = 1)
		{
			semaphore_post(&m_semaphore, _count);
		}

		inline bool wait(uint32_t _ms = -1)
		{
			return semaphore_wait(&m_semaphore, _ms);
		}
	};

} // namespace rtm

#endif // RTM_RBASE_SEMAPHORE_H
