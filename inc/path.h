//--------------------------------------------------------------------------//
/// Copyright (c) 2018 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_PATH_H
#define RTM_RBASE_PATH_H

#include <rbase/inc/platform.h>

namespace rtm {

	void pathRemoveRelative(char* _path);

	const char* pathGetFileName(const char* _path);

	const char* pathGetExt(const char* _path);

} // namespace rtm

#endif // RTM_RBASE_PATH_H
