//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_CONSOLE_H__
#define __RTM_RBASE_CONSOLE_H__

#include <rbase/inc/platform.h>
#include <stdio.h>
#include <stdarg.h>

#if RTM_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace rtm {

	class Console
	{
		enum MsgColor
		{ 
			MSG_RED		= 0x1,
			MSG_GREEN	= 0x2,
			MSG_BLUE	= 0x4,
			MSG_LIGHT	= 0x8,

			MSG_WHITE = MSG_RED | MSG_GREEN | MSG_BLUE
		};

		static void vprintf(uint32_t _color, const char* _format, va_list& _args)
		{
			char buffer[8192];
			vsprintf(buffer, _format, _args);

#if RTM_PLATFORM_WINDOWS
			HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
			if (INVALID_HANDLE_VALUE != console)
			{
				DWORD att = 0;
				if (_color & MSG_RED)	att |= FOREGROUND_RED;
				if (_color & MSG_GREEN)	att |= FOREGROUND_GREEN;
				if (_color & MSG_BLUE)	att |= FOREGROUND_BLUE;
				if (_color & MSG_LIGHT)	att |= FOREGROUND_INTENSITY;

				SetConsoleTextAttribute(console, (WORD)att);	
				DWORD written;
				WriteFile(console, buffer, (DWORD)strlen(buffer), &written, NULL);
				att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				SetConsoleTextAttribute(console, (WORD)att);	
			}
#else
			RTM_UNUSED(_color);
			printf("%s\n",buffer);
#endif
		}

	public:

		static void	custom(int _r, int _g, int _b, int _hl, const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			uint32_t col = 0;
			col |= _r  ? MSG_RED   : 0;
			col |= _g  ? MSG_GREEN : 0;
			col |= _b  ? MSG_BLUE  : 0;
			col |= _hl ? MSG_LIGHT : 0;
			Console::vprintf(col, _format, args);
			va_end(args);
		}

		static void	info(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(MSG_RED | MSG_GREEN | MSG_BLUE, _format, args);
			va_end(args);
		}

		static void	debug(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(MSG_LIGHT | MSG_GREEN | MSG_BLUE, _format, args);
			va_end(args);
		}

		static void	warning(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(MSG_RED | MSG_GREEN | MSG_LIGHT, _format, args);
			va_end(args);
		}

		static void	error(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(MSG_RED | MSG_LIGHT, _format, args);
			va_end(args);
		}
	};

} // namespace rtm

#endif // __RTM_RBASE_CONSOLE_H__
