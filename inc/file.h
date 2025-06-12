//--------------------------------------------------------------------------//
/// Copyright 2025 by Milos Tosic. All Rights Reserved.                    ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_FILE_H
#define RTM_RBASE_FILE_H

#include <rbase/inc/platform.h>

namespace rtm {

	enum FileSeek {
		CUR		= 1,
		END		= 2,
		SET		= 0
	};

	enum FileStatus {
		OPEN		= 1,
		CLOSED		= 2,
		FAIL		= 3,
		DOWNLOADING	= 4
	};

	enum FileStorage {
		Local = 1,
		HTTP  = 2
	};

	/// 
	struct FileCallBacks;

	struct FileReaderHandle { uint32_t idx; };
	struct FileWriterHandle { uint32_t idx; };

	/// Checks if file reader handle is valid
	///
	/// @param[in] _handle      : File handle Handle to check
	///
	/// @returns true if handle is valid
	static inline bool fileHandleIsValid(FileReaderHandle _handle) { return _handle.idx != UINT32_MAX; }

	/// Checks if file writer handle is valid
	///
	/// @param[in] _handle      : File handle Handle to check
	///
	/// @returns true if handle is valid
	static inline bool fileHandleIsValid(FileWriterHandle _handle) { return _handle.idx != UINT32_MAX; }

	// ------------------------------------------------
	/// File reader functions
	// ------------------------------------------------

	/// Create file reader.
	///
	/// @param[in] _type        : File storage type
	/// @param[in] _callBacks   : Optional pointer to file call backs structure
	///
	/// @returns handle to created file reader
	FileReaderHandle fileReaderCreate(FileStorage _type = FileStorage::Local, struct FileCallBacks* _callBacks = 0);

	/// Destroys file reader and closes related handles. 
	///
	/// @param[in] _handle      : File handle
	void fileReaderDestroy(FileReaderHandle _handle);

	/// Opens file for reading.
	///
	/// @param[in] _handle      : File handle
	/// @param[in] _path        : File path
	///
	/// @returns status of the file after open operation.
	FileStatus fileReaderOpen(FileReaderHandle _handle, const char* _path);

	/// Closes file reader. 
	///
	/// @param[in] _handle      : File handle
	void fileReaderClose(FileReaderHandle _handle);

	/// Gets current status of file reader.
	///
	/// @param[in] _handle      : File handle
	///
	/// @returns status of file reader.
	FileStatus fileReaderGetStatus(FileReaderHandle _handle);

	/// Sets the position for the file reader stream.
	///
	/// @param[in] _handle      : File handle
	/// @param[in] _offset      : Offset to seek by
	/// @param[in] _origin      : Origin to seek from
	///
	/// @returns current file position.
	int64_t	fileReaderSeek(FileReaderHandle _handle, int64_t _offset, uint64_t _origin = FileSeek::CUR);

	/// Reads data from file.
	///
	/// @param[in] _handle      : File handle
	/// @param[in] _dest        : Destination buffer
	/// @param[in] _size        : Destination buffer size
	///
	/// @returns number of bytes read.
	int64_t	fileReaderRead(FileReaderHandle _handle, void* _dest, int64_t _size);

	/// Returns file size.
	///
	/// @param[in] _handle      : File handle
	///
	/// @returns size of file, in bytes.
	int64_t	fileReaderGetSize(FileReaderHandle _handle);

	// ------------------------------------------------
	/// File writer functions
	// ------------------------------------------------

	/// Create file writer.
	///
	/// @param[in] _type        : File storage type
	/// @param[in] _callBacks   : Optional pointer to file call backs structure
	///
	/// @returns handle to created file writer
	FileWriterHandle fileWriterCreate(FileStorage _type = FileStorage::Local, struct FileCallBacks* _callBacks = 0);

	/// Destroys file writer and closes related handles.
	///
	/// @param[in] _handle      : File handle
	void fileWriterDestroy(FileWriterHandle _handle);

	/// Opens file for writing.
	///
	/// @param[in] _handle      : File handle
	/// @param[in] _path        : File path
	///
	/// @returns status of the file after open operation.
	FileStatus fileWriterOpen(FileWriterHandle _handle, const char* _path);

	/// Closes file writer.
	///
	/// @param[in] _handle      : File handle
	void fileWriterClose(FileWriterHandle _handle);

	/// Gets current status of file writer.
	///
	/// @param[in] _handle      : File handle
	///
	/// @returns status of the underlying file.
	FileStatus fileWriterGetStatus(FileReaderHandle _handle);

	/// Sets the position for the file writer stream.
	///
	/// @param[in] _handle      : File handle
	/// @param[in] _offset      : Offset to seek by
	/// @param[in] _origin      : Origin to seek from
	///
	/// @returns current file position.
	int64_t	fileWriterSeek(FileWriterHandle _handle, int64_t _offset, uint64_t _origin = FileSeek::CUR);

	/// Writes data to file.
	///
	/// @param[in] _handle      : File handle
	/// @param[in] _src         : Source buffer
	/// @param[in] _size        : Source buffer size
	///
	/// @returns number of bytes written.
	int64_t	fileWriterWrite(FileWriterHandle _handle, const void* _src, int64_t _size);

	/// Returns file size.
	///
	/// @param[in] _handle      : File handle
	///
	/// @returns size of file, in bytes.
	int64_t	fileWriterGetSize(FileWriterHandle _handle);

	// ------------------------------------------------
	/// Utility functions
	// ------------------------------------------------

	/// Returns file size, in bytes.
	///
	/// @param[in] _type        : File storage type
	/// @param[in] _path        : File path
	///
	/// @returns size of file, in bytes.
	int64_t	fileGetSize(FileStorage _type, const char* _path);

	/// Reads file contents and stores them to destination buffer.
	///
	/// @param[in] _type        : File storage type
	/// @param[in] _path        : File path
	/// @param[in] _data        : Data buffer.
	/// @param[in] _size        : Data buffer size.
	///
	/// @returns number of bytes read.
	int64_t	fileRead(FileStorage _type, const char* _path, void* _data, int64_t _size);

	/// Writes data from source buffer to destination file.
	///
	/// @param[in] _type        : File storage type
	/// @param[in] _path        : File path
	/// @param[in] _data        : Data buffer.
	/// @param[in] _size        : Data buffer size.
	///
	/// @returns number of bytes written.
	int64_t	fileWrite(FileStorage _type, const char* _path, const void* _data, int64_t _size);

	/// Writes file to storage if contents of the buffer are different from contents of the file
	///
	/// @param[in] _type        : File storage type
	/// @param[in] _path        : File path
	/// @param[in] _data        : Data buffer.
	/// @param[in] _size        : Data buffer size.
	/// @param[in,out] _written : Optional pointer to store success result to
	///
	/// @returns number of bytes written.
	int64_t	fileWriteIfDifferent(FileStorage _type, const char* _path, const void* _data, int64_t _size, bool* _written = 0);

} // namespace rtm

/// ---------------------------------------------------------------------- ///
///  Implementation                                                        ///
/// ---------------------------------------------------------------------- ///

namespace rtm {

	struct FileCallBacks
	{
		typedef void (*progCb)(float _progress);
		typedef void (*doneCb)(const char* _path);
		typedef void (*failCb)(const char* _errorMessage);

		progCb	m_progCb;
		doneCb	m_doneCb;
		failCb	m_failCb;

		FileCallBacks()
			: m_progCb(0)
			, m_doneCb(0)
			, m_failCb(0)
		{
		}
	};

} // namespace rtm

#endif // RTM_RBASE_FILE_H
