//--------------------------------------------------------------------------//
/// Copyright 2023 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_TLS_H
#define RTM_RBASE_TLS_H

#include <rbase/inc/platform.h>

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE

	#define WINDOWS_LEAN_AND_MEAN
	#include <windows.h>

	static inline uint32_t tlsAllocate()
	{
		return (uint32_t)TlsAlloc();
	}

	static inline void tlsSetValue(uint32_t _handle, void* _value)
	{
		TlsSetValue(_handle, _value);
	}

	static inline void* tlsGetValue(uint32_t _handle)
	{
		return TlsGetValue(_handle);
	}

	static inline void tlsFree(uint32_t _handle)
	{
		TlsFree(_handle);
	}

#elif RTM_PLATFORM_PS4

	static inline uint32_t tlsAllocate()
	{
		ScePthreadKey handle;
		scePthreadKeyCreate(&handle, 0);
		return handle;
	}

	static inline void tlsSetValue(uint32_t _handle, void* _value)
	{
		scePthreadSetspecific(_handle, _value);
	}

	static inline void* tlsGetValue(uint32_t _handle)
	{
		return scePthreadGetspecific(_handle);
	}

	static inline void tlsFree(uint32_t _handle)
	{
		scePthreadKeyDelete(_handle);
	}

#elif RTM_PLATFORM_POSIX

	#include <pthread.h>

	static inline uint32_t tlsAllocate()
	{
		uint32_t handle;
		pthread_key_create((pthread_key_t*)&handle, 0);
		return handle;
	}

	static inline void tlsSetValue(uint32_t _handle, void* _value)
	{
		pthread_setspecific(_handle, _value);
	}

	static inline void* tlsGetValue(uint32_t _handle)
	{
		return pthread_getspecific(_handle);
	}

	static inline void tlsFree(uint32_t _handle)
	{
		pthread_key_delete(_handle);
	}

#else
	#error "Unsupported platform!"
#endif

} // namespace rtm

#endif // RTM_RBASE_TLS_H
