//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_URI_H
#define RTM_RBASE_URI_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringview.h>

namespace rtm {

	/// URI encoding
	/// Returns UINT32_MAX if not enough space in destination buffer or
	/// number of written characters on success.

	uint32_t uriEncode(const StringView& _str, char* _buffer, uint32_t _bufferSize);
	uint32_t uriEncode(const char* _uri, char* _buffer, uint32_t _bufferSize, uint32_t _maxUriChars = UINT32_MAX);

	/// URI decoding
	/// Returns UINT32_MAX if not enough space in destination buffer or 0 on error/
	/// Returns number of written characters on success.

	uint32_t uriDecode(const StringView& _str, char* _buffer, uint32_t _bufferSize);
	uint32_t uriDecode(const char* _uri, char* _buffer, uint32_t _bufferSize, uint32_t _maxUriChars = UINT32_MAX);

} // namespace rtm

#endif // RTM_RBASE_URI_H
