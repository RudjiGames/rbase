//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/uri.h>

namespace rtm {

static inline int shouldEncode(char ch)
{
	return !(false
		|| isAlphaNum(ch)
		|| '-' == ch || '_' == ch
		|| '.' == ch || '!' == ch
		|| '~' == ch || '*' == ch
		|| '(' == ch || ')' == ch);
}

uint32_t uriEncode(const StringView& _str, char* _buffer, uint32_t _bufferSize)
{
	return uriEncode(_str.data(), _buffer, _bufferSize, _str.length());
}

uint32_t uriEncode(const char* _uri, char* _buffer, uint32_t _bufferSize, uint32_t _maxUriChars)
{
	RTM_ASSERT(_uri, "");
	RTM_ASSERT(_buffer, "");

	uint32_t len = _maxUriChars == UINT32_MAX ? strLen(_uri) : _maxUriChars;
	const char* uriEnd = _uri + len;

	uint32_t dSize = 0;
	while (_uri < uriEnd)
	{
		if (dSize >= _bufferSize)
		{
			_buffer[0] = '\0';
			return UINT32_MAX;
		}

		char ch = *_uri++;

		if (shouldEncode(ch))
		{
			if (dSize < _bufferSize - 3)
			{
				_buffer[dSize++] = '%';
				_buffer[dSize++] = toHexNum(ch >> 4);
				_buffer[dSize++] = toHexNum(ch);
			}
			else
			{
				_buffer[0] = '\0';
				return UINT32_MAX;
			}
		}
		else
			_buffer[dSize++] = ch;
	}

	_buffer[dSize++] = '\0';

	return dSize;
}

uint32_t uriDecode(const StringView& _str, char* _buffer, uint32_t _bufferSize)
{
	return uriDecode(_str.data(), _buffer, _bufferSize, _str.length());
}

uint32_t uriDecode(const char* _uri, char* _buffer, uint32_t _bufferSize, uint32_t _maxUriChars)
{
	RTM_ASSERT(_uri, "");
	RTM_ASSERT(_buffer, "");

	uint32_t len = _maxUriChars == UINT32_MAX ? strLen(_uri) : _maxUriChars;
	const char* uriEnd = _uri + len;

	uint32_t dSize = 0;
	while (_uri < uriEnd)
	{
		if (dSize >= _bufferSize)
		{
			_buffer[0] = '\0';
			return UINT32_MAX;
		}

		char ch = *_uri++;

		if (ch == '%')
		{
			if (_uri > uriEnd - 2)
			{
				_buffer[0] = '\0';
				return UINT32_MAX;
			}

			if (isHexNum(_uri[0]) && isHexNum(_uri[1]))
			{
				char c1 = fromHexNum(*_uri++);
				char c2 = fromHexNum(*_uri++);
				char c = ((c1 << 4) | c2);

				_buffer[dSize++] = c;
			}
			else
			{
				_buffer[0] = '\0';	// invalid format or not enough space in destination buffer
				return 0;
			}
		}
		else
			_buffer[dSize++] = ch;
	}

	_buffer[dSize++] = '\0';

	return dSize;
}

} // namespace rtm
