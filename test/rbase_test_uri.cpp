//--------------------------------------------------------------------------//
/// Copyright (c) 2018 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_test_pch.h>
#include <rbase/inc/uri.h>

using namespace rtm;

SUITE(rbase)
{
	TEST(uri)
	{
		char buffer[2048];

		CHECK(14 == uriEncode("Hello World", buffer, 2048));
		CHECK(0 == strCmp(buffer, "Hello%20World"));

		CHECK(12 == uriDecode("Hello%20World", buffer, 2048));
		CHECK(0 == strCmp(buffer, "Hello World"));


		CHECK(67 == uriEncode("https://www.google.ca/?gws_rd=ssl#q=url+decoding", buffer, 2048));
		CHECK(0 == strCmp(buffer, "https%3A%2F%2Fwww.google.ca%2F%3Fgws_rd%3Dssl%23q%3Durl%2Bdecoding"));

		CHECK(49 == uriDecode("https%3A%2F%2Fwww.google.ca%2F%3Fgws_rd%3Dssl%23q%3Durl%2Bdecoding", buffer, 2048));
		CHECK(0 == strCmp(buffer, "https://www.google.ca/?gws_rd=ssl#q=url+decoding"));


		CHECK(54 == uriEncode("http://hu.wikipedia.org/wiki/São_Paulo", buffer, 2048));
		CHECK(0 == strCmp(buffer, "http%3A%2F%2Fhu.wikipedia.org%2Fwiki%2FS%C3%A3o_Paulo"));

		CHECK(40 == uriDecode("http%3A%2F%2Fhu.wikipedia.org%2Fwiki%2FS%C3%A3o_Paulo", buffer, 2048));
		CHECK(0 == strCmp(buffer, "http://hu.wikipedia.org/wiki/São_Paulo"));
	}
}
