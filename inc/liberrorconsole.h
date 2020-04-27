//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_LIBERRORCONSOLE_H
#define RTM_RBASE_LIBERRORCONSOLE_H

#include <rbase/inc/console.h>

struct ConsoleErrorHandler : public rtm::ErrorHandler
{
	virtual void fatal(const char* _file, int _line, const char* _message)
	{
		rtm::Console::error("%s:%d  %s", _file, _line, _message);
	}
		
	virtual void warning(const char* _file, int _line, const char* _message)
	{
		rtm::Console::warning("%s:%d  %s", _file, _line, _message);
	}

	virtual void debug(const char* _file, int _line, const char* _message)
	{
		rtm::Console::debug("%s:%d  %s", _file, _line, _message);
	}
};

struct UnitTestLibInterface : public rtmLibInterface
{
	ConsoleErrorHandler m_eh;

	UnitTestLibInterface()
	{
		m_memory	= 0;
		m_error		= &m_eh;
	}
};

UnitTestLibInterface g_libInterface;

#endif // RTM_RBASE_LIBERRORCONSOLE_H
