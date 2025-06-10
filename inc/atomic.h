//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_ATOMIC_H
#define RTM_RBASE_ATOMIC_H

#include <rbase/inc/platform.h>

namespace rtm {

	/// Issues a read barrier
	static inline void readBarrier();

	/// Issues a write barrier
	static inline void writeBarrier();

	/// Issues a read/write memory barrier
	static inline void readWriteBarrier();

	/// Issues a full memory barrier
	static inline void memoryBarrier();

	/// Atomically increments a value.
	///
	/// @param[in,out] _value: Pointer to value to increment
	///
	/// @returns the resulting incremented value.
	static inline int32_t interlockedInc(int32_t volatile* _value);

	/// Atomically increments a value.
	///
	/// @param[in,out] _value: Pointer to value to increment
	///
	/// @returns the resulting incremented value.
	static inline int64_t interlockedInc(int64_t volatile* _value);

	/// Atomically decrements a value.
	///
	/// @param[in,out] _value: Pointer to value to decrement
	///
	/// @returns the resulting decremented value.
	static inline int32_t interlockedDec(int32_t volatile* _value);

	/// Atomically decrements a value.
	///
	/// @param[in,out] _value: Pointer to value to decrement
	///
	/// @returns the resulting decremented value.
	static inline int64_t interlockedDec(int64_t volatile* _value);

	/// Adds a value atomically.
	///
	/// @param[in,out] _value: Value to add to.
	/// @param[in] _add: Value to add.
	///
	/// @returns the initial value that was added to.
	///	---------------------------------------------------------------------
	static inline int32_t interlockedAdd(int32_t volatile* _value, int32_t _add);

	/// Adds a value atomically.
	///
	/// @param[in,out] _value: Value to add to.
	/// @param[in] _add: Value to add.
	///
	/// @returns the initial value that was added to.
	///	---------------------------------------------------------------------
	static inline int64_t interlockedAdd(int64_t volatile* _value, int64_t _add);

	/// Substracts a value atomically.
	///
	/// @param[in,out] _value: Value to substract from.
	/// @param[in] _sub: Value to substract.
	///
	/// @returns the initial value that was substracted from.
	static inline int32_t interlockedSub(int32_t volatile* _value, int32_t _sub);

	/// Substracts a value atomically.
	///
	/// @param[in,out] _value: Value to substract from.
	/// @param[in] _sub: Value to substract.
	///
	/// @returns the initial value that was substracted from.
	static inline int64_t interlockedSub(int64_t volatile* _value, int64_t _sub);

	/// Sets a value atomically.
	///
	/// @param[in,out] _value: Pointer to destination value.
	/// @param[in] _newValue: The value to set.
	///
	/// @returns the initial value before the new one was set.
	static inline int32_t interlockedSet(int32_t volatile* _value, int32_t _newValue);

	/// Sets a value atomically.
	///
	/// @param[in,out] _value: Pointer to destination value.
	/// @param[in] _newValue: The value to set.
	///
	/// @returns the initial value before the new one was set.
	static inline int64_t interlockedSet(int64_t volatile* _value, int64_t _newValue);

	/// Atomically compare-exchange-swap.
	///
	/// @param[in,out] _value: Pointer to value to compare against and set result to.
	/// @param[in] _oldValue: The expected original value.
	/// @param[in] _newValue: New value to set.
	///
	/// @returns the initial value before the new one was set.
	static inline int32_t interlockedCAS(int32_t volatile* _value, int32_t _oldValue, int32_t _newValue);

	/// Atomically compare-exchange-swap.
	///
	/// @param[in,out] _value: Pointer to value to compare against and set result to.
	/// @param[in] _oldValue: The expected original value.
	/// @param[in] _newValue: New value to set.
	///
	/// @returns the initial value before the new one was set.
	static inline int64_t interlockedCAS(int64_t volatile* _value, int64_t _oldValue, int64_t _newValue);

} // namespace rtm

#if RTM_PLATFORM_XBOXONE
#include <intrin.h>
#endif

#if RTM_PLATFORM_ANDROID || RTM_PLATFORM_LINUX || RTM_PLATFORM_OSX
#include <sched.h> // sched_yield 
#endif

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
	#ifdef __cplusplus
	extern "C" {
	#endif
		long		_InterlockedIncrement(long volatile*);
		long		_InterlockedDecrement(long volatile*);
		long		_InterlockedCompareExchange(long volatile*, long, long);
		long		_InterlockedExchange(long volatile*, long);
		long		_InterlockedExchangeAdd(long volatile*, long);
		long long	_InterlockedCompareExchange64(long long volatile*, long long, long long);
		long long	_InterlockedIncrement64(long long volatile*);
		long long	_InterlockedDecrement64(long long volatile*);
		long long	_InterlockedExchange64(long long volatile*, long long);
		long long	_InterlockedExchangeAdd64(long long volatile*, long long);
		void 		_ReadBarrier(void);
		void 		_WriteBarrier(void);
		void 		_ReadWriteBarrier(void);
		void		_mm_mfence(void);
	#ifdef __cplusplus
	} /* extern "C" */
	#endif
#if RTM_COMPILER_MSVC
		#pragma intrinsic (_InterlockedIncrement)
		#pragma intrinsic (_InterlockedDecrement)
		#pragma intrinsic (_InterlockedCompareExchange)
		#pragma intrinsic (_InterlockedExchange)
		#pragma intrinsic (_InterlockedExchangeAdd)
		#pragma intrinsic (_InterlockedCompareExchange64)
#if RTM_64BIT
		#pragma intrinsic (_InterlockedIncrement64)
		#pragma intrinsic (_InterlockedDecrement64)
		#pragma intrinsic (_InterlockedExchange64)
		#pragma intrinsic (_InterlockedExchangeAdd64)
#endif // RTM_64BIT
		#pragma intrinsic (_ReadBarrier)
		#pragma intrinsic (_WriteBarrier)
		#pragma intrinsic (_ReadWriteBarrier)
		#pragma intrinsic (_mm_mfence)
#endif // RTM_COMPILER_MSVC
#endif // RTM_PLATFORM_WINDOWS

namespace rtm {

	static inline void readBarrier()
	{
#if RTM_COMPILER_MSVC
		_ReadBarrier();
#elif RTM_PLATFORM_CHEERP
		__sync_synchronize();
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		__atomic_thread_fence(__ATOMIC_RELEASE);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline void writeBarrier()
	{
#if RTM_COMPILER_MSVC
		_WriteBarrier();
#elif RTM_PLATFORM_CHEERP
		__sync_synchronize();
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		__atomic_thread_fence(__ATOMIC_ACQUIRE);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline void readWriteBarrier()
	{
#if RTM_COMPILER_MSVC
		_ReadWriteBarrier();
#elif RTM_PLATFORM_CHEERP
		__sync_synchronize();
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		__atomic_thread_fence(__ATOMIC_ACQ_REL);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline void memoryBarrier()
	{
#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE || RTM_PLATFORM_WINRT
		_mm_mfence();
#elif RTM_PLATFORM_PS3
		__lwsync();
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		__sync_synchronize();
#else
		#error "Unsupported platform/compiler!"
#endif
	}

	static inline int32_t interlockedInc(int32_t volatile* _value)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_add(_value, 1);
#elif RTM_COMPILER_MSVC
		return _InterlockedIncrement((long volatile*)_value);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int64_t interlockedInc(int64_t volatile* _value)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_add(_value, 1);
#elif RTM_COMPILER_MSVC
		return _InterlockedIncrement64((long long volatile*)_value);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int32_t interlockedDec(int32_t volatile* _value)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_add(_value, -1);
#elif RTM_COMPILER_MSVC
		return _InterlockedDecrement((volatile long*)_value);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int64_t interlockedDec(int64_t volatile* _value)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_add(_value, -1);
#elif RTM_COMPILER_MSVC
		return _InterlockedDecrement64((volatile long long*)_value);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int32_t interlockedAdd(int32_t volatile* _value, int32_t _add)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_add(_value, _add);
#elif RTM_COMPILER_MSVC
		return _InterlockedExchangeAdd((volatile long*)_value, _add);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int64_t interlockedAdd(int64_t volatile* _value, int64_t _add)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_add(_value, _add);
#elif RTM_COMPILER_MSVC
		return _InterlockedExchangeAdd64((volatile long long*)_value, _add);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int32_t interlockedSub(int32_t volatile* _value, int32_t _sub)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_sub(_value, _sub);
#elif RTM_COMPILER_MSVC
		return _InterlockedExchangeAdd((volatile long*)_value, -_sub);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int64_t interlockedSub(int64_t volatile* _value, int64_t _sub)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_fetch_and_sub( _value, _sub );
#elif RTM_COMPILER_MSVC
		return _InterlockedExchangeAdd64((volatile long long*)_value, -_sub);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int32_t interlockedSet(int32_t volatile* _value, int32_t _newValue)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		__sync_synchronize();
		return __sync_lock_test_and_set(_value, _newValue);
#elif RTM_COMPILER_MSVC
		return _InterlockedExchange((volatile long*)_value, _newValue);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int64_t interlockedSet(int64_t volatile* _value, int64_t _newValue)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		__sync_synchronize();
		return __sync_lock_test_and_set(_value, _newValue);
#elif RTM_COMPILER_MSVC
		return _InterlockedExchange64((volatile long long*)_value, _newValue);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int32_t interlockedCAS(int32_t volatile* _value, int32_t _oldValue, int32_t _newValue)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_val_compare_and_swap(_value, _oldValue, _newValue);
#elif RTM_COMPILER_MSVC
		return _InterlockedCompareExchange((volatile long*)_value, _newValue, _oldValue);
#else
		#error "Unsupported compiler!"
#endif
	}

	static inline int64_t interlockedCAS(int64_t volatile* _value, int64_t _oldValue, int64_t _newValue)
	{
#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
		return __sync_val_compare_and_swap(_value, _oldValue, _newValue);
#elif RTM_COMPILER_MSVC
		return _InterlockedCompareExchange64((volatile long long*)_value, _newValue, _oldValue);
#else
		#error "Unsupported compiler!"
#endif
	}

} // namespace rtm

#endif // RTM_RBASE_ATOMIC_H
