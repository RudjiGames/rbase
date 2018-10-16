//--------------------------------------------------------------------------//
/// Copyright (c) 2018 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_test_pch.h>
#include <rbase/inc/path.h>

using namespace rtm;

SUITE(rbase)
{
	TEST(path)
	{
		const char* pathFile	= "/some/dir/file.txt";
		const char* pathDir		= "/some/dir/";

		// const char* pathGetFileName(const char* _path);

		CHECK(0 == strCmp(pathGetFileName(pathFile), "file.txt"));
		CHECK(0 == strCmp(pathGetFileName(pathDir),	""));

		char buffer[1024];

		// bool pathGetFileName(const char* _path, char* _buffer, size_t _bufferSize);

		CHECK(true == pathGetFileName(pathFile, buffer, 1024));
		CHECK(0 == strCmp(buffer, "file.txt"));

		CHECK(true == pathGetFileName(pathDir, buffer, 1024));
		CHECK(0 == strCmp(buffer, ""));

		// bool pathGetFilenameNoExt(const char* _path, char* _buffer, size_t _bufferSize);

		CHECK(true == pathGetFilenameNoExt(pathFile, buffer, 1024));
		CHECK(0 == strCmp(buffer, "file"));

		CHECK(true == pathGetFilenameNoExt(pathDir, buffer, 1024));
		CHECK(0 == strCmp(buffer, ""));

		// const char* pathGetExt(const char* _path);

		CHECK(0 == strCmp(pathGetExt(pathFile), "txt"));
		CHECK(0 == strCmp(pathGetExt(pathDir), ""));
		
		// bool pathGetExt(const char* _path, char* _buffer, size_t _bufferSize);

		CHECK(true == pathGetExt(pathFile, buffer, 1024));
		CHECK(0 == strCmp(buffer, "txt"));

		CHECK(true == pathGetExt(pathDir, buffer, 1024));
		CHECK(0 == strCmp(buffer, ""));

		// bool pathGetCurrentDirectory(char* _buffer, size_t _bufferSize);

		pathGetCurrentDirectory(buffer, 1024);
		Console::debug("Current working dir : ");
		Console::info("%s\n", buffer);

		// bool pathGetDataDirectory(char* _buffer, size_t _bufferSize);

		pathGetDataDirectory(buffer, 1024);
		Console::debug("Current data dir    : ");
		Console::info("%s\n", buffer);

		// bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, size_t _bufferSize);

		CHECK(true == pathAppend(pathDir, "file.txt", buffer, 1024));
		CHECK(0 == strCmp(buffer, pathFile));

		CHECK(false == pathAppend(pathFile, "file.txt", buffer, 1024));

		// bool pathUp(const char* _path, char* _buffer, size_t _bufferSize);

		CHECK(true == pathUp(pathDir, buffer, 1024));
		CHECK(0 == strCmp(buffer, "/some/"));

		CHECK(true == pathUp(pathFile, buffer, 1024));
		CHECK(0 == strCmp(buffer, "/some/dir/"));

		// bool pathCanonicalize(const char* _path, char* _buffer, size_t _bufferSize);

		CHECK(true == pathCanonicalize("/some/dir/../and/up/test.txt", buffer, 1024));
		CHECK(0 == strCmp(buffer, "/some/and/up/test.txt"));

		// void pathCanonicalize(char* _path);

		strlCpy(buffer, 1024, "/some/dir/../and/up/test.txt");
		pathCanonicalize(buffer);
		CHECK(0 == strCmp(buffer, "/some/and/up/test.txt"));

		// bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, size_t _bufferSize);

		CHECK(false == pathMakeAbsolute("../dir2/file2.txt", "/root/dir1/file3.txt", buffer, 1024));
		CHECK(true  == pathMakeAbsolute("../dir2/file2.txt", "/root/dir1/", buffer, 1024));
		CHECK(0		== strCmp(buffer, "/root/dir2/file2.txt"));

		//bool pathMakeRelative(const char* _pathFrom, const char* _pathTo, char* _buffer, size_t _bufferSize);

		CHECK(true	== pathMakeRelative("/usr/tosa/test/dir/text.file", "/usr/tosa/some/other/dir/", buffer, 1024));
		CHECK(0		== strCmp(buffer, "../../some/other/dir/"));

		CHECK(true	== pathMakeRelative("/usr/tosa/some/other/dir/", "/usr/tosa/test/dir/text.file", buffer, 1024));
		CHECK(0		== strCmp(buffer, "../../../test/dir/text.file"));

		CHECK(false	== pathMakeRelative("/usr/tosa/test/dir/text.file", "/user/tosa/some/other/dir/", buffer, 1024));
		CHECK(false	== pathMakeRelative("/usr/tosa/test/dir/text.file", "../usr/tosa/some/other/dir/", buffer, 1024));

		//bool pathIsAbsolute(const char* _path);

		CHECK(true  == pathIsAbsolute(pathFile));
		CHECK(false == pathIsAbsolute("../test.txt"));

		// bool pathIsDirectory(const char* _path);

		CHECK(true  == pathIsDirectory(pathDir));
		CHECK(false == pathIsDirectory(pathFile));

		// bool pathSplit(const char* _path, uint32_t* _numDirectories, StringView* _stringViews, uint32_t _maxViews);

		uint32_t numDirs;
		StringView strs[16];
		CHECK(true == pathSplit("/some/and/up/test.txt", &numDirs, strs, 16));
		CHECK(3 == numDirs);
		CHECK(0 == strCmp(strs[0], "some",	strs[0].length()));
		CHECK(0 == strCmp(strs[1], "and",	strs[1].length()));
		CHECK(0 == strCmp(strs[2], "up",	strs[2].length()));
	}
}
