//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CONSOLE_H
#define RTM_RBASE_CONSOLE_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringfn.h>
#include <stdio.h>
#include <stdarg.h>

#if RTM_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace rtm {

	class Console
	{
		static char* itoaf(char* _buffer, uint8_t _val)
		{
			int numDigits = 0;
			char digits[3];

			while (_val)
			{
				digits[numDigits++] = '0' + (_val % 10);
				_val /= 10;
			}

			if (numDigits)
			{
				for (int i = numDigits - 1; i >= 0; --i)
					*_buffer++ = digits[i];
			}
			else
				*_buffer++ = '0';
			*_buffer++ = ';';
			return _buffer;
		}

		static void vprintf(uint8_t _r, uint8_t _g, uint8_t _b, const char* _format, va_list& _args)
		{
			char buffer[8192];

#if RTM_PLATFORM_WINDOWS
			vsprintf(buffer, _format, _args);
			HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
			if (INVALID_HANDLE_VALUE != console)
			{
				DWORD att = 0;
				if (_r)	att |= FOREGROUND_RED;
				if (_g)	att |= FOREGROUND_GREEN;
				if (_b)	att |= FOREGROUND_BLUE;
				if (false
					|| (_r > 192)
					|| (_g > 192)
					|| (_b > 192))
					att |= FOREGROUND_INTENSITY;

				SetConsoleTextAttribute(console, (WORD)att);
				DWORD written;
				WriteFile(console, buffer, (DWORD)strlen(buffer), &written, NULL);
				att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				SetConsoleTextAttribute(console, (WORD)att);
			}
#else
			strlCpy(buffer, 8192, "\x1b[38;2;");
			char* b = itoaf(&buffer[7], _r);
			b = itoaf(b, _g);
			b = itoaf(b, _b);
			b[-1] = 'm';
			vsprintf(b, _format, _args);
			strlCat(buffer, 8192, "\x1b[0m");
			printf("%s", buffer);
#endif
		}

	public:

		static void	rgb(uint8_t _r, uint8_t _g, uint8_t _b, const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(_r, _g, _b, _format, args);
			va_end(args);
		}

		static void	info(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(192, 192, 192, _format, args);
			va_end(args);
		}

		static void	debug(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(0, 255, 255, _format, args);
			va_end(args);
		}

		static void	warning(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(255, 255, 0, _format, args);
			va_end(args);
		}

		static void	error(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			Console::vprintf(255, 0, 0, _format, args);
			va_end(args);
		}
	};

} // namespace rtm

#endif // RTM_RBASE_CONSOLE_H
