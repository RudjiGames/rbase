//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------// 

#include <rbase_pch.h>
#include <rbase/inc/strings.h>

char toNoop(char _ch)
{
	return _ch;
}

bool isUpper(char _ch)
{
	return _ch >= 'A' && _ch <= 'Z';
}

char toLower(char _ch)
{
	return _ch + (isUpper(_ch) ? 0x20 : 0);
}

