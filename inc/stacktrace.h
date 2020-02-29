//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_STACKTRACE_H
#define RTM_RBASE_STACKTRACE_H

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#elif RTM_PLATFORM_LINUX || RTM_PLATFORM_OSX
#include <execinfo.h>
#endif

namespace rtm {

#if RTM_PLATFORM_WINDOWS || RTM_PLATFORM_XBOXONE

#if RTM_COMPILER_MSVC
	static inline uint32_t getStackTrace(void* _traces[], uint32_t _numFrames, uint32_t _skip)
	{
		return (uint32_t)CaptureStackBackTrace( (ULONG)_skip, (ULONG)_numFrames, _traces, NULL );
	}
#elif RTM_COMPILER_GCC
	static inline uint32_t getStackTrace(void* _traces[], uint32_t _numFrames, uint32_t _skip)
	{
		return (uint32_t)RtlCaptureStackBackTrace((ULONG)_skip, (ULONG)_numFrames, (PVOID*)_traces, NULL);
	}
#else
#error "Unsupported compiler!"
#endif

#elif RTM_PLATFORM_PS4

	static inline uint32_t getStackTrace(void* _traces[], uint32_t _numFrames, uint32_t _skip)
	{
		uint32_t num = 0;
		void** ptr = (void**)__builtin_frame_address(0);
		while (_skip)
		{
			ptr = (void**)(*ptr);
			--_skip;
		}
		while (ptr && num < _numFrames)
		{
			_traces[num++] = (*(ptr + 1));
			ptr = (void**)(*ptr);
		}
		return num;
	}

#elif RTM_PLATFORM_ANDROID
	struct unwindArg
	{
		int			m_tracesToSkip;
		int			m_numTraces;
		int			m_framesSize;
		uintptr_t*	m_frames;
	};
	static _Unwind_Reason_Code unwindTraceFunc(struct _Unwind_Context* _context, void* _arg) 
	{
		unwindArg& arg = *(unwindArg*)_arg;
		void* ip = (void*)_Unwind_GetIP(_context);
		if (0 == ip) 
			return _URC_END_OF_STACK;
		else
		{
			if (arg.m_tracesToSkip)
			{
				--arg.m_tracesToSkip;
				return _URC_NO_REASON;
			}
			if (arg.m_numTraces < arg.m_framesSize)
			{
				arg.m_frames[arg.m_numTraces++] = (uintptr_t)ip;
				return _URC_NO_REASON;
			}
			return _URC_END_OF_STACK;
		}
	}
	static inline uint32_t getStackTrace(void* _traces[], uint32_t _numFrames, uint32_t _skip)
	{
		unwindArg arg;
		arg.m_tracesToSkip	= _skip;
		arg.m_numTraces		= 0;
		arg.m_framesSize	= _numFrames;
		arg.m_frames		= _traces;
		_Unwind_Backtrace(unwindTraceFunc, &arg);
		return arg.m_numTraces;
	}

#elif RTM_PLATFORM_LINUX || RTM_PLATFORM_OSX

	static inline uint32_t getStackTrace(void* _traces[], uint32_t _numFrames, uint32_t _skip)
	{
		void* trace[256];
		uint32_t numTraces = (uint32_t)backtrace(trace, 256);
		if (_skip >= numTraces)
			return 0;
		const uint32_t retTraces = numTraces - _skip;
		for (uint32_t i=0; (i<retTraces) && (i<_numFrames); ++i)
			_traces[i] = trace[i+_skip];
		return retTraces;
	}

#else
	static inline uint32_t getStackTrace(void* _traces[], uint32_t _numFrames, uint32_t _skip)
	{
		RTM_UNUSED_3(_traces, _numFrames, _skip);
		return 0;
	}
#endif

} // namespace rtm

#endif // RTM_RBASE_STACKTRACE_H

