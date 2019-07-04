//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_PATH_H
#define RTM_RBASE_PATH_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringview.h>

namespace rtm {

    /// Retrieves file name with extension if it is a file path
	const char* pathGetFileName(const char* _path);

    /// Retrieves file name with extension if it is a file path
	bool pathGetFileName(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves file name without extension if it is a file path
	bool pathGetFilenameNoExt(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves file extension if it is a file path
	const char* pathGetExt(const char* _path);

	/// Retrieves file extension if it is a file path
	bool pathGetExt(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves current working directory
	bool pathGetCurrentDirectory(char* _buffer, uint32_t _bufferSize);

	/// Retrieves path to data root directory
	bool pathGetDataDirectory(char* _buffer, uint32_t _bufferSize);

	/// Appends two paths and stores result to path referred by _path handle
	bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, uint32_t _bufferSize);

	/// Goes 'up' one level 
	bool pathUp(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Removes relative portions of the path
	bool pathCanonicalize(const char* _path, char* _buffer, uint32_t _bufferSize);

    /// Removes relative portions of the path In place 
    void pathCanonicalize(char* _path);

	/// Makes relative path absolute by appending it to base and canonicalizing it
	bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, uint32_t _bufferSize);

	/// Makes _path relative
	bool pathMakeRelative(const char* _pathFrom, const char* _pathTo, char* _buffer, uint32_t _bufferSize);
	
	/// Checks if path is absolute
	bool pathIsAbsolute(const char* _path);

	/// Checks if given path refers to a directory
	bool pathIsDirectory(const char* _path);

	/// Splits path and stores list of directories to _directories.
	bool pathSplit(const char* _path, uint32_t* _numDirectories, StringView* _dirList, uint32_t _maxDirs);

} // namespace rtm

#endif // RTM_RBASE_PATH_H
