//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CONSOLE_H
#define RTM_RBASE_CONSOLE_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringfn.h>
#include <stdio.h> /* vsnprintf */
#include <stdarg.h>

#if RTM_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#define RTM_CONSOLE_TEMP_BUFFER_SIZE	8192+128
#define RTM_CONSOLE_ENABLE_ANSI			1

#if RTM_PLATFORM_WINDOWS
#if (_WIN32_WINNT < 0x0603)
#undef	RTM_WINDOWS_CONSOLE
#define	RTM_WINDOWS_CONSOLE 1
#endif
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

		static inline char* setColor(char* _buffer, uint32_t _buffSize, uint8_t _r, uint8_t _g, uint8_t _b)
		{
#if !RTM_CONSOLE_ENABLE_ANSI
			RTM_UNUSED_4(_buffSize, _r, _g, _b);
			return _buffer;
#else
			RTM_ASSERT(_buffSize >= 32, "");
			// 256 color ANSI mode
			// "\x1b[38;2;" r;g;bm
			const char* ANSI_256 = "\x1b[38;2;";
			rtm::strlCat(_buffer, _buffSize, ANSI_256);
			char* b = itoaf(&_buffer[rtm::strLen(_buffer)], _r);
			b = itoaf(b, _g);
			b = itoaf(b, _b);
			b[-1] = 'm';
			b[0] = '\0';
			return b;
#endif
		}

		static inline void restoreColor(char* _buffer, uint32_t _buffSize)
		{
#if !RTM_CONSOLE_ENABLE_ANSI
			RTM_UNUSED_2(_buffer, _buffSize);
#else
			rtm::strlCat(_buffer, _buffSize, "\x1b[0m");
#endif
		}

		static inline void printf(const char* _str)
		{
#if RTM_PLATFORM_WINDOWS
			HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
			static bool ansiModeSet = false;
			if (!ansiModeSet)
			{
				ansiModeSet = true;
				DWORD dwMode = 0;
				GetConsoleMode(console, &dwMode);
				dwMode |= 0x0004;//ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(console, dwMode);
			}
			DWORD written = 0;
			if (INVALID_HANDLE_VALUE != console)
				WriteFile(console, _str, (DWORD)rtm::strLen(_str), &written, NULL);
#else
			::printf("%s", _str);
#endif
		}

		static inline void vprintf(const char* _format, va_list& _args)
		{
			char buffer[8192];
			vsnprintf(buffer, 8192, _format, _args);
			Console::printf(buffer);
		}

		static void	rgbInternal(uint8_t _r, uint8_t _g, uint8_t _b, const char* _prepend, const char* _format, va_list& _args)
		{
			char buffer[RTM_CONSOLE_TEMP_BUFFER_SIZE];
			char* append = buffer;
			buffer[0] = '\0';
			uint32_t remainder = RTM_CONSOLE_TEMP_BUFFER_SIZE - uint32_t(append - buffer);
			append = setColor(append, remainder, _r, _g, _b);
			remainder = RTM_CONSOLE_TEMP_BUFFER_SIZE - uint32_t(append - buffer);
			rtm::strlCpy(append, remainder, _prepend);
			rtm::strlCat(append, remainder, _format);
			restoreColor(append, remainder);
			Console::vprintf(buffer, _args);
		}

	public:

		static void	rgb(uint8_t _r, uint8_t _g, uint8_t _b, const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			rgbInternal(_r, _g, _b, "", _format, args);
			va_end(args);
		}

		static void	print(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			rgbInternal(192, 192, 192, "", _format, args);
			va_end(args);
		}

		static void	info(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			rgbInternal(0, 192, 0, "", _format, args);
			va_end(args);
		}

		static void	debug(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			rgbInternal(0, 192, 192, "", _format, args);
			va_end(args);
		}

		static void	warning(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			rgbInternal(192, 192, 0, "", _format, args);
			va_end(args);
		}

		static void	error(const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			rgbInternal(255, 0, 0, "ERROR: ", _format, args);
			va_end(args);
		}
	};

} // namespace rtm

#endif // RTM_RBASE_CONSOLE_H
