//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_SEMAPHORE_H
#define RTM_RBASE_SEMAPHORE_H

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	#include <windows.h>
	#include <winbase.h>
#elif RTM_PLATFORM_POSIX
	//#include <errno.h>
	#include <semaphore.h>
	#include <time.h>
	#include <pthread.h>
//sceKernelXxxSema
#else
	#error "Unsupported platform/compiler!"
#endif

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
	
	static inline bool semaphore_wait(semaphore_t* _sem, int32_t _ms = -1) {
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
	
	static inline bool semaphore_wait(semaphore_t* _sem, int32_t _ms = -1)
	{
		int result = pthread_mutex_lock(&_sem->m_mutex);

	#if RTM_PLATFORM_ANDROID
		if (_ms != -1)
		{
			timespec ts;
			memset(&ts, sizeof(timespec), 0);
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
	
#elif RTM_PLATFORM_POSIX
	typedef sem_t semaphore_t;

	static inline bool semaphore_init(semaphore_t* _sem) {
		int32_t result = sem_init(_sem, 0, 0);
		return result == 0;
	}

	static inline void semaphore_destroy(semaphore_t* _sem) {
		sem_destroy(_sem);
	}

	static inline void semaphore_post(semaphore_t* _sem, uint32_t _count) {
		for (uint32_t i=0; i<_count; ++i)
			sem_post(_sem);
	}
	
	static inline bool semaphore_wait(semaphore_t* _sem, int32_t _ms = -1) {
	{
		if (0 > _ms)
			return (0 == sem_wait(_sem));
	
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += _ms/1000;
		ts.tv_nsec += (_ms % 1000)*1000;
		return 0 == semaphore_timedwait(_sem, &ts);
	}

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
