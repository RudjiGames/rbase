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


		UriView uri1("https://user:password@www.google.ca:23/?gws_rd=ssl#q=url+decoding");

		CHECK(0 == strCmp(uri1.get(UriPart::Scheme),	"https"));
		CHECK(0 == strCmp(uri1.get(UriPart::Authority),	"user:password@www.google.ca:23"));
		CHECK(0 == strCmp(uri1.get(UriPart::User),		"user:password"));
		CHECK(0 == strCmp(uri1.get(UriPart::UserName),	"user"));
		CHECK(0 == strCmp(uri1.get(UriPart::Password),	"password"));
		CHECK(0 == strCmp(uri1.get(UriPart::Host),		"www.google.ca"));
		CHECK(0 == strCmp(uri1.get(UriPart::Port),		"23"));
		CHECK(0 == strCmp(uri1.get(UriPart::Path),		"/"));
		CHECK(0 == strCmp(uri1.get(UriPart::Query),		"gws_rd=ssl"));
		CHECK(0 == strCmp(uri1.get(UriPart::Fragment),	"q=url+decoding"));

		UriView uri2("https://user:password@www.google.ca:23");

		CHECK(0 == strCmp(uri2.get(UriPart::Scheme),	"https"));
		CHECK(0 == strCmp(uri2.get(UriPart::Authority), "user:password@www.google.ca:23"));
		CHECK(0 == strCmp(uri2.get(UriPart::User),		"user:password"));
		CHECK(0 == strCmp(uri2.get(UriPart::UserName),	"user"));
		CHECK(0 == strCmp(uri2.get(UriPart::Password),	"password"));
		CHECK(0 == strCmp(uri2.get(UriPart::Host),		"www.google.ca"));
		CHECK(0 == strCmp(uri2.get(UriPart::Port),		"23"));
		CHECK(0 == uri2.get(UriPart::Path).length());
		CHECK(0 == uri2.get(UriPart::Query).length());
		CHECK(0 == uri2.get(UriPart::Fragment).length());

		const char* URIs[7] = {
			"ftp://ftp.is.co.za/rfc/rfc1808.txt",
			"http://www.ietf.org/rfc/rfc2396.txt",
			"ldap://[2001:db8::7]/c=GB?objectClass?one",
			"mailto:John.Doe@example.com",
			"news:comp.infosystems.www.servers.unix",
			"telnet://192.0.2.16:80/",
			"https://www.google.ca/?gws_rd=ssl#q=url+decoding"
		};

		const char* HOSTs[7] = {
			"ftp.is.co.za",
			"www.ietf.org",
			"[2001:db8::7]",
			"example.com",
			"comp.infosystems.www.servers.unix",
			"192.0.2.16:80",
			"www.google.ca"
		};

		for (int i=0; i<7; ++i)
		{
			UriView uv(URIs[i]);
			CHECK(0 == strCmp(uv.get(UriPart::Host), HOSTs[i]));
		}
	}
}
