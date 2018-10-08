//--------------------------------------------------------------------------//
/// Copyright (c) 2018 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_test_pch.h>

struct rbaseUnitTestApp : public rapp::App
{
	RAPP_CLASS(rbaseUnitTestApp)

		virtual ~rbaseUnitTestApp() {}

	int init(int32_t, const char* const*, rtmLibInterface*)
	{
		return 0;
	}

	void suspend() {}
	void resume() {}

	void update(float)
	{
		UnitTest::RunAllTests();
		quit();
	}

	void draw() {}

	void shutDown()
	{
	}
};

RAPP_REGISTER(rbaseUnitTestApp, "rbaseUnitTest", "rbase Unit testing");
