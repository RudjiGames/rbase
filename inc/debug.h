//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_DEBUG_H
#define RTM_RBASE_DEBUG_H

#include <rbase/inc/platform.h>

#if RTM_PLATFORM_WINDOWS
	extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char* _str);
#elif RTM_PLATFORM_ANDROID
	#include <android/log.h>
#elif RTM_PLATFORM_EMSCRIPTEN
	#include <emscripten/emscripten.h>
#else
	#include <stdio.h>
#endif

namespace rtm {

	static inline void debugBreak()
	{
#if RTM_PLATFORM_EMSCRIPTEN
		emscripten_log(EM_LOG_CONSOLE | EM_LOG_ERROR | EM_LOG_C_STACK | EM_LOG_JS_STACK | EM_LOG_DEMANGLE, "debugBreak!");
		// Doing emscripten_debugger() disables asm.js validation due to an emscripten bug
		//emscripten_debugger();
		EM_ASM({ debugger; });
#else
		RTM_BREAK;
#endif
	}

	static inline void debugOutput(const char* _out)
	{
#if RTM_PLATFORM_WINDOWS
		OutputDebugStringA(_out);
#elif RTM_PLATFORM_ANDROID
		__android_log_write(ANDROID_LOG_DEBUG, "", _out);
#elif RTM_PLATFORM_EMSCRIPTEN
		emscripten_log(EM_LOG_CONSOLE, "%s", _out);
#else
		fputs(_out, stderr);
		fflush(stderr);
#endif
	}

} // namespace rtm

#endif // RTM_RBASE_DEBUG_H

