//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_URI_H
#define RTM_RBASE_URI_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringview.h>

namespace rtm {

	struct UriPart
	{
		enum Enum
		{
			Scheme,
			Authority,
			User,
			UserName,
			Password,
			Host,
			Port,
			Path,
			Query,
			Fragment,

			Count
		};
	};

	class UriView
	{
	protected:
		StringView	m_parts[UriPart::Count];
		uint32_t	m_length;

	public:
		UriView();
		UriView(const char* _str, uint32_t _len = UINT32_MAX);
		UriView(const StringView& _str);

		void				clear();
		void				parse(const StringView& _str);
		void				parse(const char* _str, uint32_t _len = UINT32_MAX);
		const StringView&	get(UriPart::Enum _part) const;
		uint32_t			length() const;
		uint32_t			write(char* _buffer, uint32_t _bufferSize, bool _skipFragment = false) const;
		operator const char* () const { return m_parts[UriPart::Scheme].data(); } 

	private:
		uint32_t			writePart(UriPart::Enum _part, char* _buffer, uint32_t _bufferSize) const;
		uint32_t			writeString(const StringView& _str, char* _buffer, uint32_t _bufferSize) const;
	};

	class Uri : public UriView
	{
		String	m_uri;

	public:
		Uri();
		Uri(const char* _str, uint32_t _len = UINT32_MAX);
		Uri(const StringView& _str);

		void setPart(UriPart::Enum _part, const StringView& _str);

		const String&	getUri() const { return m_uri; }
	};

	/// Returns size of buffer needed to store encoded version of input URI
	/// Includes 0 terminator

	uint32_t uriEncodedSize(const StringView& _str);
	uint32_t uriEncodedSize(const char* _uri, uint32_t _maxUriChars = UINT32_MAX);

	/// URI encoding
	/// Returns UINT32_MAX if not enough space in destination buffer or
	/// number of written characters on success.

	uint32_t uriEncode(const StringView& _str, char* _buffer, uint32_t _bufferSize);
	uint32_t uriEncode(const char* _uri, char* _buffer, uint32_t _bufferSize, uint32_t _maxUriChars = UINT32_MAX);

	/// Returns size of buffer needed to store decoded version of input encoded URI
	/// Returns UINT32_MAX for invalid URIs
	/// Includes 0 terminator

	uint32_t uriDecodedSize(const StringView& _str);
	uint32_t uriDecodedSize(const char* _uri, uint32_t _maxUriChars = UINT32_MAX);

	/// URI decoding
	/// Returns UINT32_MAX if not enough space in destination buffer or 0 on error/
	/// Returns number of written characters on success.

	uint32_t uriDecode(const StringView& _str, char* _buffer, uint32_t _bufferSize);
	uint32_t uriDecode(const char* _uri, char* _buffer, uint32_t _bufferSize, uint32_t _maxUriChars = UINT32_MAX);

	/// URI nesting
	/// Stores nested URI in a query component with the form 'vfs=URI'

	uint32_t uriNest(const UriView& _uri, const UriView& _nestedUri, char* _buffer, uint32_t _bufferSize, uint32_t* _neededBufferSize = 0);
	uint32_t uriNestArr(const UriView* _uris, uint32_t _numUris, char* _buffer, uint32_t _bufferSize, uint32_t* _neededBufferSize = 0);

	/// Parse URI query and retrieve key value pairs

	uint32_t uriParseQuery(const StringView& _uri, StringView* _strs, uint32_t _numStrs, uint32_t* _numStrsNeeded = 0);
	uint32_t uriParseQuery(const UriView& _uri, StringView* _strs, uint32_t _numStrs, uint32_t* _numStrsNeeded = 0);

} // namespace rtm

#endif // RTM_RBASE_URI_H
