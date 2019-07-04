//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/uri.h>

namespace rtm {

UriView::UriView()
	: m_length(0)
{
}

UriView::UriView(const char* _str, uint32_t _len)
	: m_length(0)
{
	parse(StringView(_str, _len));
}

UriView::UriView(const StringView& _str)
	: m_length(0)
{
	parse(_str);
}

void UriView::clear()
{
	for (int i=0; i<UriPart::Count; ++i)
		m_parts[i].clear();
	m_length = 0;
}

void UriView::parse(const StringView& _str)
{
	clear();

	const char* schemeEnd = strChr(_str, ':');
	if (!schemeEnd)
		return clear();

	m_parts[UriPart::Scheme] = StringView(_str.data(), schemeEnd);

	const char* authority		= strStr(_str, "//");
	const char* authorityEnd	= 0;
	const char* strEnd			= _str.data() + _str.length();

	if (!authority)
		authority = schemeEnd + 1;
	else
		authority += 2;

	const char* userInfoEnd = strChr(authority, '@');
	authorityEnd = strChr(authority, '/');

	if (!authorityEnd)
		authorityEnd = strEnd;

	m_parts[UriPart::Authority] = StringView(authority, authorityEnd);

	const char* hostStart = authority;

	if (userInfoEnd)
	{
		hostStart = userInfoEnd + 1;

		StringView user = StringView(authority, userInfoEnd);
		m_parts[UriPart::User] = user;
		if (const char* pwd = strChr(user, ':'))
		{
			m_parts[UriPart::UserName] = StringView(authority, pwd);
			m_parts[UriPart::Password] = StringView(pwd + 1, userInfoEnd);
		}
		else
			m_parts[UriPart::UserName] = m_parts[UriPart::User];
	}

	StringView hostAndPort(hostStart, authorityEnd);
	if (const char* port = strChr(hostAndPort, ':'))
	{
		m_parts[UriPart::Host] = StringView(hostStart, port);
		m_parts[UriPart::Port] = StringView(port + 1, authorityEnd);
	}
	else
		m_parts[UriPart::Host] = StringView(hostStart, authorityEnd);

	if (*authorityEnd == '/')
	{
		const char* path		= strChr(authorityEnd, '/');
		const char* query		= strChr(authorityEnd, '?');
		const char* fragment	= strChr(authorityEnd, '#');

		const char* pathEnd		= query ? query : fragment;
		if (!pathEnd)
			pathEnd = strEnd;

		m_parts[UriPart::Path]	= StringView(path, pathEnd);

		if (query)
		{
			if (fragment)
				m_parts[UriPart::Query] = StringView(query + 1, fragment);
			else
				m_parts[UriPart::Query] = StringView(query + 1, strEnd);
		}

		if (fragment)
			m_parts[UriPart::Fragment]	= StringView(fragment + 1, strEnd);
	}

	m_length = _str.length();
}

void UriView::parse(const char* _str, uint32_t _len)
{
	uint32_t len = _len == UINT32_MAX ? strLen(_str) : _len;
	parse(StringView(_str, len));
}

const StringView& UriView::get(UriPart::Enum _part) const
{
	return m_parts[_part];
}

uint32_t UriView::length() const
{
	return m_length;
}

uint32_t UriView::write(char* _buffer, uint32_t _bufferSize, bool _skipFragment) const
{
	RTM_ASSERT(_buffer, "");

	uint32_t len = 0;

	len += writePart(UriPart::Scheme,			&_buffer[len], _bufferSize - len);
	len += writeString(StringView(":"),			&_buffer[len], _bufferSize - len);

	if (m_parts[UriPart::Authority].length())
	{
		len += writeString(StringView("//"),	&_buffer[len], _bufferSize - len);

		if (m_parts[UriPart::User].length())
		{
			len += writePart(UriPart::UserName,	&_buffer[len], _bufferSize - len);
			len += writeString(StringView(":"),	&_buffer[len], _bufferSize - len);
			len += writePart(UriPart::Password,	&_buffer[len], _bufferSize - len);
			len += writeString(StringView("@"),	&_buffer[len], _bufferSize - len);
		}
		len += writePart(UriPart::Host,		&_buffer[len], _bufferSize - len);

		if (m_parts[UriPart::Port].length())
		{
			len += writeString(StringView(":"),	&_buffer[len], _bufferSize - len);
			len += writePart(UriPart::Port,		&_buffer[len], _bufferSize - len);
		}
	}

	len += writePart(UriPart::Path,		&_buffer[len], _bufferSize - len);

	if (m_parts[UriPart::Query].length())
	{
		len += writeString(StringView("?"),	&_buffer[len], _bufferSize - len);
		len += writePart(UriPart::Query,	&_buffer[len], _bufferSize - len);

	}

	if (m_parts[UriPart::Fragment].length() && !_skipFragment)
	{
		len += writeString(StringView("#"),	&_buffer[len], _bufferSize - len);
		len += writePart(UriPart::Fragment,	&_buffer[len], _bufferSize - len);
	}

	return len;
}

uint32_t UriView::writePart(UriPart::Enum _part, char* _buffer, uint32_t _bufferSize) const
{
	RTM_ASSERT(_buffer, "");
	return writeString(m_parts[_part], _buffer, _bufferSize);
}

uint32_t UriView::writeString(const StringView& _str, char* _buffer, uint32_t _bufferSize) const
{
	return strlCpy(_buffer, _bufferSize, _str.data(), _str.length());
}

Uri::Uri()
{
}

Uri::Uri(const char* _str, uint32_t _len)
{
	m_uri.set(_str, _len);
	parse(m_uri.data(), m_uri.length());
}

Uri::Uri(const StringView& _str)
{
	m_uri.set(_str.data(), _str.length());
	parse((StringView)m_uri);
}

void Uri::setPart(UriPart::Enum _part, const StringView& _str)
{
	m_parts[_part] = _str;

	char  buffer[4096];
	char* store = buffer;

	uint32_t len = length();
	if (len >= 4096)
		store = new char[len+1];
	store[0] = 0;

	write(store, len);
	m_uri = store;

	parse(m_uri, len);

	if (len >= 4096)
		delete[] store;
}

static inline int shouldEncode(char ch)
{
	return !(false
		|| isAlphaNum(ch)
		|| '-' == ch || '_' == ch
		|| '.' == ch || '!' == ch
		|| '~' == ch || '*' == ch
		|| '(' == ch || ')' == ch);
}

uint32_t uriEncodedSize(const StringView& _str)
{
	return uriEncodedSize(_str.data(), _str.length());
}

uint32_t uriEncodedSize(const char* _uri, uint32_t _maxUriChars)
{
	RTM_ASSERT(_uri, "");

	uint32_t len = _maxUriChars == UINT32_MAX ? strLen(_uri) : _maxUriChars;
	const char* uriEnd = _uri + len;

	uint32_t dSize = 0;
	while (_uri < uriEnd)
	{
		char ch = *_uri++;

		if (shouldEncode(ch))
			dSize += 3;
		else
			dSize++;
	}

	return dSize;
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

	_buffer[dSize] = '\0';
	return dSize;
}

uint32_t uriDecodedSize(const StringView& _str)
{
	return uriDecodedSize(_str.data(), _str.length());
}

uint32_t uriDecodedSize(const char* _uri, uint32_t _maxUriChars)
{
	RTM_ASSERT(_uri, "");

	uint32_t len = _maxUriChars == UINT32_MAX ? strLen(_uri) : _maxUriChars;
	const char* uriEnd = _uri + len;

	uint32_t dSize = 0;
	while (_uri < uriEnd)
	{
		char ch = *_uri++;

		if (ch == '%')
		{
			if (_uri > uriEnd - 2)
				return UINT32_MAX;

			if (isHexNum(_uri[0]) && isHexNum(_uri[1]))
			{
				_uri += 2;
				dSize++;
			}
			else
				return UINT32_MAX;
		}
		else
			dSize++;
	}

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

	_buffer[dSize] = '\0';
	return dSize;
}

uint32_t uriNest(const UriView& _uri, const UriView& _nestedUri, char* _buffer, uint32_t _bufferSize, uint32_t* _neededBufferSize)
{
	const uint32_t keyLen = 4; // 4 for strLen("vfs=") or strLen("vfs=")
	uint32_t encNestLen = uriEncodedSize(_nestedUri, _nestedUri.length()) + keyLen;

	if (_uri.length() + encNestLen >= _bufferSize)
	{
		if (_neededBufferSize)
			*_neededBufferSize = _uri.length() + encNestLen;
		return 0;
	}

	uint32_t written = _uri.write(_buffer, _bufferSize, true);
	StringView query	= _uri.get(UriPart::Query);
	StringView fragment	= _uri.get(UriPart::Fragment);

	if (!query.length())
		written += strlCpy(&_buffer[written], _bufferSize - written, "?");
	else
		written += strlCpy(&_buffer[written], _bufferSize - written, "&");

	written += strlCpy(&_buffer[written], _bufferSize - written, "vfs=");
	written += uriEncode(_nestedUri, &_buffer[written], _bufferSize - written, _nestedUri.length());

	if (fragment.length())
	{
		written += strlCpy(&_buffer[written], _bufferSize - written, "#");
		written += strlCpy(&_buffer[written], _bufferSize - written, fragment.data(), fragment.length());
	}

	_buffer[written] = 0;

	return written;
}

uint32_t uriNestArr(const UriView* _uris, uint32_t _numUris, char* _buffer, uint32_t _bufferSize, uint32_t* _neededBufferSize)
{
	const uint32_t keyLen = 4; // 4 for strLen("vfs=") or strLen("vfs=")
	uint32_t uri0Len	= _uris[0].length();
	uint32_t totalLen	= 0;

	for (uint32_t i=1; i<_numUris; ++i)
		totalLen += keyLen + uriEncodedSize(_uris[i]);

	totalLen += uri0Len;

	if (totalLen >= _bufferSize)
	{
		if (_neededBufferSize)
			*_neededBufferSize = totalLen;
		return 0;
	}

	uint32_t written = _uris[0].write(_buffer, _bufferSize, true);
	StringView query	= _uris[0].get(UriPart::Query);
	StringView fragment = _uris[0].get(UriPart::Fragment);

	if (!query.length())
		written += strlCpy(&_buffer[written], _bufferSize - written, "?");
	else
		written += strlCpy(&_buffer[written], _bufferSize - written, "&");

	for (uint32_t i=1; i<_numUris; ++i)
	{
		written += strlCpy(&_buffer[written], _bufferSize - written, "vfs=");
		written += uriEncode(_uris[i], &_buffer[written], _bufferSize - written, _uris[i].length());
		if (i < _numUris - 1)
		written += strlCpy(&_buffer[written], _bufferSize - written, "&");
	}

	if (fragment.length())
	{
		written += strlCpy(&_buffer[written], _bufferSize - written, "#");
		written += strlCpy(&_buffer[written], _bufferSize - written, fragment.data(), fragment.length());
	}

	_buffer[written] = 0;

	return written;
}

uint32_t uriParseQuery(const StringView& _uri, StringView* _strs, uint32_t _numStrs, uint32_t* _numStrsNeeded)
{
	const char* ptr		= _uri.data();
	const char* ptrEnd	= ptr + _uri.length();
	const char* kvEnd	= 0;

	uint32_t storeIndex = 0;
	uint32_t kvPairs	= 0;

	while (kvEnd < ptrEnd)
	{
		kvEnd	= strChr(ptr, '&');
		if (!kvEnd) kvEnd = ptrEnd;

		const char* eq = strChr(ptr, '=', (uint32_t)(kvEnd - ptr));
		StringView key(ptr, eq);
		StringView value(eq+1, kvEnd);

		if (storeIndex < _numStrs)	_strs[storeIndex++] = key;
		if (storeIndex < _numStrs)	_strs[storeIndex++] = value;

		ptr = kvEnd + 1;
		++kvPairs;
	}

	if (kvPairs * 2 == storeIndex)
		return storeIndex;

	if (*_numStrsNeeded)
		*_numStrsNeeded = kvPairs * 2;

	return 0;
}

uint32_t uriParseQuery(const UriView& _uri, StringView* _strs, uint32_t _numStrs, uint32_t* _numStrsNeeded)
{
	StringView query = _uri.get(UriPart::Query);
	return uriParseQuery(query, _strs, _numStrs, _numStrsNeeded);
}

} // namespace rtm
