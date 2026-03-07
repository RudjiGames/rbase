//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_PATH_H
#define RTM_RBASE_PATH_H

#include <rbase/inc/platform.h>
#include <rbase/inc/stringview.h>

namespace rtm {

	/// Retrieves file name with extension from a file path.
	///
	/// @param[in] _path       : File system path.
	///
	/// @returns pointer to file name portion of the path, or nullptr if the path is a directory.
	const char* pathGetFileName(const char* _path);

	/// Retrieves file name with extension from a file path and copies it into a buffer.
	///
	/// @param[in] _path       : File system path.
	/// @param[out] _buffer    : Buffer to store the file name.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if the path has no file name or the buffer is too small.
	bool pathGetFileName(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves file name without extension from a file path.
	///
	/// @param[in] _path       : File system path.
	/// @param[out] _buffer    : Buffer to store the file name (without extension).
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if the path has no file name or the buffer is too small.
	bool pathGetFilenameNoExt(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves the file extension from a file path.
	///
	/// @param[in] _path       : File system path.
	///
	/// @returns pointer to the extension (including the dot), or nullptr if none.
	const char* pathGetExt(const char* _path);

	/// Retrieves the file extension from a file path and copies it into a buffer.
	///
	/// @param[in] _path       : File system path.
	/// @param[out] _buffer    : Buffer to store the extension.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if there is no extension or the buffer is too small.
	bool pathGetExt(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Retrieves the current working directory.
	///
	/// @param[out] _buffer    : Buffer to store the directory path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success.
	bool pathGetCurrentDirectory(char* _buffer, uint32_t _bufferSize);

	/// Retrieves the path to the data root directory.
	///
	/// @param[out] _buffer    : Buffer to store the directory path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success.
	bool pathGetDataDirectory(char* _buffer, uint32_t _bufferSize);

	/// Appends a sub-path to a base path.
	///
	/// @param[in] _path       : Base file system path.
	/// @param[in] _appendPath : Path segment to append.
	/// @param[out] _buffer    : Buffer to store the resulting combined path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if the buffer is too small.
	bool pathAppend(const char* _path, const char* _appendPath, char* _buffer, uint32_t _bufferSize);

	/// Navigates one directory level up from the given path.
	///
	/// @param[in] _path       : File system path.
	/// @param[out] _buffer    : Buffer to store the parent directory path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if already at the root or the buffer is too small.
	bool pathUp(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Resolves '.' and '..' segments and produces a canonical path.
	///
	/// @param[in] _path       : File system path to canonicalize.
	/// @param[out] _buffer    : Buffer to store the resulting canonical path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if the buffer is too small.
	bool pathCanonicalize(const char* _path, char* _buffer, uint32_t _bufferSize);

	/// Resolves '.' and '..' segments in place.
	///
	/// @param[in,out] _path   : File system path to canonicalize (modified in place).
	void pathCanonicalize(char* _path);

	/// Converts a relative path to an absolute path by combining it with a base and canonicalizing.
	///
	/// @param[in] _relative   : Relative path to make absolute.
	/// @param[in] _base       : Base directory path.
	/// @param[out] _buffer    : Buffer to store the resulting absolute path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if the buffer is too small.
	bool pathMakeAbsolute(const char* _relative, const char* _base, char* _buffer, uint32_t _bufferSize);

	/// Computes a relative path from one location to another.
	///
	/// @param[in] _pathFrom   : Starting directory path.
	/// @param[in] _pathTo     : Target path to reach.
	/// @param[out] _buffer    : Buffer to store the resulting relative path.
	/// @param[in] _bufferSize : Size of the provided buffer, in bytes.
	///
	/// @returns true on success, false if the buffer is too small.
	bool pathMakeRelative(const char* _pathFrom, const char* _pathTo, char* _buffer, uint32_t _bufferSize);
	
	/// Checks if a path is absolute.
	///
	/// @param[in] _path       : File system path.
	///
	/// @returns true if the path is absolute.
	bool pathIsAbsolute(const char* _path);

	/// Checks if path is a directory.
	///
	/// @param[in] _path       : File system path.
	///
	/// @returns true if path is a directory.
	bool pathIsDirectory(const char* _path);

	/// Splits a path into its individual directory components.
	///
	/// @param[in] _path       : File system path to split.
	/// @param[out] _numDirs   : Receives the number of directory components found.
	/// @param[out] _dirList   : Array of StringView to receive each path component.
	/// @param[in] _maxDirs    : Maximum number of entries in _dirList.
	///
	/// @returns true on success, false if _maxDirs is too small to hold all components.
	bool pathSplit(const char* _path, uint32_t* _numDirs, StringView* _dirList, uint32_t _maxDirs);

} // namespace rtm

#endif // RTM_RBASE_PATH_H
