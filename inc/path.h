//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_PATH_H
#define RTM_RBASE_PATH_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringview.h>

namespace rtm {

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path       : File system path
	///
	/// @returns the pointer to file name portion of the path, nullptr if dir.
	const char* pathGetFileName(const char* _path);

	/// Retrieves file name with extension if it is a file path.
	///
	/// @param[in] _path       : File system path
	/// @param[out] _buffer    : Buffer to store file name to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathGetFileName(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves file name without extension if it is a file path.
	///
	/// @param[in] _path       : File system path
	/// @param[out] _buffer    : Buffer to store file name to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathGetFilenameNoExt(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves file extension if it is a file path.
	///
	/// @param[in] _path       : File system path
	///
	/// @returns pointer to extension part of the file name.
	const char* pathGetExt(const char* _path);

	/// Retrieves file extension if it is a file path.
	///
	/// @param[in] _path       : File system path
	/// @param[out] _buffer    : Buffer to store file name to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathGetExt(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves current working directory.
	///
	/// @param[out] _buffer    : Buffer to store file name to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathGetCurrentDirectory(char* _buffer, uint32_t _bufferSize);

	/// Retrieves path to data root directory
	///
	/// @param[out] _buffer    : Buffer to store file name to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathGetDataDirectory(char* _buffer, uint32_t _bufferSize);

	/// Retrieves path to data root directory.
	///
	/// @param[in] _path       : File system path to be appended to
	/// @param[in] _appendPath : File system path to be appended
	/// @param[out] _buffer    : Buffer to store resulting path to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, uint32_t _bufferSize);

	/// Goes 'up' one level 
	///
	/// @param[in] _path       : File system path
	/// @param[out] _buffer    : Buffer to store resulting path to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathUp(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Removes relative portions of the path.
	///
	/// @param[in] _path       : File system path
	/// @param[out] _buffer    : Buffer to store resulting path to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathCanonicalize(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Removes relative portions of the path
	///
	/// @param[in] _path       : File system path
    void pathCanonicalize(char* _path);

	/// Makes relative path absolute by appending it to base and canonicalizing it.
	///
	/// @param[in] _relative   : Relative path to make absolute
	/// @param[in] _base       : Base path
	/// @param[out] _buffer    : Buffer to store resulting path to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, uint32_t _bufferSize);

	/// Makes path relative.
	///
	/// @param[in] _pathFrom   : Path to be relative from
	/// @param[in] _pathTo     : Path to be relative to
	/// @param[out] _buffer    : Buffer to store resulting path to
	/// @param[in] _bufferSize : Size of the provided buffer
	///
	/// @returns true on success.
	bool pathMakeRelative(const char* _pathFrom, const char* _pathTo, char* _buffer, uint32_t _bufferSize);
	
	/// Checks if path is absolute.
	///
	/// @param[in] _path       : File system path
	///
	/// @returns true if path is absolue.
	bool pathIsAbsolute(const char* _path);

	/// Checks if path is a directory.
	///
	/// @param[in] _path       : File system path.
	///
	/// @returns true if path is a directory.
	bool pathIsDirectory(const char* _path);

	/// Splits path and stores list of directories to _dirList.
	///
	/// @param[in] _path       : File system path to split into list of directories
	/// @param[out] _numDirs   : Pointer to store number of directories in the path
	/// @param[out] _dirList   : Buffer to store resulting path to
	/// @param[in] _maxDirs    : Maximum number of directories to retrieve
	///
	/// @returns true on success.
	bool pathSplit(const char* _path, uint32_t* _numDirs, StringView* _dirList, uint32_t _maxDirs);

} // namespace rtm

#endif // RTM_RBASE_PATH_H
