//--------------------------------------------------------------------------//
/// Copyright (c) 2018 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_test_pch.h>
#include <rbase/inc/stringview.h>

using namespace rtm;

SUITE(rbase)
{
	TEST(string)
	{
		char buffer[128];
		const char* testString = "This is the test string";

		CHECK(23 == strLen(testString));

		strlCpy(buffer, 128, testString);

		CHECK(0 == strCmp(buffer, testString));

		strToUpper(buffer);
		CHECK(0 == strCmp(buffer, "THIS IS THE TEST STRING"));
		CHECK(0 == striCmp(buffer, testString));

		strToLower(buffer);
		CHECK(0 == strCmp(buffer, "this is the test string"));
		CHECK(0 == striCmp(buffer, testString));

		CHECK(0 == striCmp(strStr(buffer, "test"), "test string"));


		StringTemp<> fs(testString);
		String ds = fs;
		StringView vs(ds.data(), ds.length());

		CHECK(0 == strCmp(vs, testString));

		StringView blank;
		CHECK(0 != strCmp("", "notblank"));
		CHECK(0 != strCmp(blank, "notblank"));
		CHECK(0 != strCmp(blank, ""));

		CHECK(true);
	}
}
