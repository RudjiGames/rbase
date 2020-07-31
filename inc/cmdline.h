//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//
/// Taken from bx library: https://github.com/bkaradzic/bx                 ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_CMDLINE_H
#define RTM_RBASE_CMDLINE_H

#include <rbase/inc/platform.h>
#include <string.h>	// strlen
#include <stdlib.h>	// atoi

namespace rtm {

	class CommandLine
	{
		int					m_argc;
		const char* const*	m_argv;

	public:

		CommandLine(int _argc, const char* const* _argv) :
			m_argc(_argc), m_argv(_argv)
		{
		}

		CommandLine(int _argc, const char* _argv[]) :
			m_argc(_argc), m_argv(_argv)
		{
		}

		bool hasArg(char _optionLetter) const
		{
			return getArgString(_optionLetter) != 0;
		}

		bool hasArg(const char* _optionName) const
		{
			return getArgString(_optionName) != 0;
		}

		const char* getArg(int _argIndex) const
		{
			if (_argIndex >= m_argc)
				return 0;
			return m_argv[_argIndex];
		}

		bool getArg(char _optionLetter, const char*& _value) const
		{
			const char* arg = getArgString(_optionLetter);
			if (!arg)
				return false;
			_value = arg;
			return true;
		}

		bool getArg(const char* _optionName, const char*& _value) const
		{
			const char* arg = getArgString(_optionName);
			if (!arg)
				return false;
			_value = arg;
			return true;
		}

		bool getArg(char _optionLetter, int& _value) const
		{
			const char* arg = getArgString(_optionLetter);
			if (!arg)
				return false;
			_value = atoi(arg);
			return true;
		}

		bool getArg(const char* _optionName, int& _value) const
		{
			const char* arg = getArgString(_optionName);
			if (!arg)
				return false;
			_value = atoi(arg);
			return true;
		}

		bool getArg(char _optionLetter, float& _value) const
		{
			const char* arg = getArgString(_optionLetter);
			if (!arg)
				return false;
			_value = (float)atof(arg);
			return true;
		}

		bool getArg(const char* _optionName, float& _value) const
		{
			const char* arg = getArgString(_optionName);
			if (!arg)
				return false;
			_value = (float)atof(arg);
			return true;
		}

		bool getArg(char _optionLetter, bool& _value) const
		{
			const char* arg = getArgString(_optionLetter);
			if (!arg)
				return false;
			_value = (strcmp(arg, "true") == 0) || (strcmp(arg, "1") == 0);
			return true;
		}

		bool getArg(const char* _optionName, bool& _value) const
		{
			const char* arg = getArgString(_optionName);
			if (!arg)
				return false;
			_value = (strcmp(arg, "true") == 0) || (strcmp(arg, "1") == 0);
			return true;
		}

	private:
		const char* getArgString(char _optionLetter) const
		{
			for (int i=0; i<m_argc; ++i)
			{
				const char* argv = m_argv[i];
				if ((argv[0] == '-') && (argv[1] == _optionLetter))
				{
					const char* nextArg = (i == m_argc-1) ? 0 : m_argv[i+1];
					if (!nextArg)
						return "";
					if (nextArg[0] == '-')
						return "";
					return nextArg;
				}
			}
			return 0;
		}

		const char* getArgString(const char* _optionName) const
		{
			const size_t optionLen = strlen(_optionName);
			for (int i=0; i<m_argc; ++i)
			{
				const char* argv = m_argv[i];
				if (argv[0] == '-')
				{
					size_t len = strlen(&argv[1]);
					if (len != optionLen)
						continue;

					while ((argv[len] == ' ') || (argv[len] == '\t')) --len;
					bool match = strncmp(&argv[1], _optionName, (int32_t)len) == 0;
					if (!match)
						continue;

					const char* nextArg = (i == m_argc - 1) ? 0 : m_argv[i + 1];

					if (!nextArg)
						return "";
					if (nextArg[0] == '-')
						return "";
					return nextArg;
				}
			}
			return 0;
		}
	};

} // namespace rtm

#endif //  RTM_RBASE_CMDLINE_H
