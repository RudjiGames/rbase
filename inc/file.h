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
	/// @param[in] _handle: Handle to check
	///
	/// @returns true if handle is valid
	static inline bool fileHandleIsValid(FileReaderHandle _handle) { return _handle.idx != UINT32_MAX; }

	/// Checks if file writer handle is valid
	///
	/// @param[in] _handle: Handle to check
	///
	/// @returns true if handle is valid
	static inline bool fileHandleIsValid(FileWriterHandle _handle) { return _handle.idx != UINT32_MAX; }

	// ------------------------------------------------
	/// File reader functions
	// ------------------------------------------------

	/// 
	///
	/// @param[in] _type:
	/// @param[in] _callBacks:
	///
	/// @returns handle to created file reader
	FileReaderHandle fileReaderCreate(FileStorage _type = FileStorage::Local, struct FileCallBacks* _callBacks = 0);

	/// 
	///
	/// @param[in] _handle:
	void fileReaderDestroy(FileReaderHandle _handle);

	/// 
	///
	/// @param[in] _handle:
	/// @param[in] _path:
	///
	/// @returns 
	FileStatus fileReaderOpen(FileReaderHandle _handle, const char* _path);

	/// 
	///
	/// @param[in] _handle:
	void fileReaderClose(FileReaderHandle _handle);

	/// 
	///
	/// @param[in] _handle:
	///
	/// @returns 
	FileStatus fileReaderGetStatus(FileReaderHandle _handle);

	/// 
	///
	/// @param[in] _handle:
	/// @param[in] _offset:
	/// @param[in] _origin:
	///
	/// @returns 
	int64_t	fileReaderSeek(FileReaderHandle _handle, int64_t _offset, uint64_t _origin = FileSeek::CUR);

	/// 
	///
	/// @param[in] _handle:
	/// @param[in] _dest:
	/// @param[in] _size:
	///
	/// @returns 
	int64_t	fileReaderRead(FileReaderHandle _handle, void* _dest, int64_t _size);

	/// 
	///
	/// @param[in] _handle:
	///
	/// @returns 
	int64_t	fileReaderGetSize(FileReaderHandle _handle);

	// ------------------------------------------------
	/// File writer functions
	// ------------------------------------------------

	/// 
	///
	/// @param[in] _type:
	/// @param[in] _callBacks:
	///
	/// @returns handle to created file writer
	FileWriterHandle fileWriterCreate(FileStorage _type = FileStorage::Local, struct FileCallBacks* _callBacks = 0);

	/// 
	///
	/// @param[in] _handle:
	void fileWriterDestroy(FileWriterHandle _handle);

	/// 
	///
	/// @param[in] _handle:
	/// @param[in] _path:
	///
	/// @returns 
	FileStatus fileWriterOpen(FileWriterHandle _handle, const char* _path);

	/// 
	///
	/// @param[in] _handle:
	void fileWriterClose(FileWriterHandle _handle);

	/// 
	///
	/// @param[in] _handle:
	///
	/// @returns 
	FileStatus fileWriterGetStatus(FileReaderHandle _handle);

	/// 
	///
	/// @param[in] _handle:
	/// @param[in] _offset:
	/// @param[in] _origin:
	///
	/// @returns 
	int64_t	fileWriterSeek(FileWriterHandle _handle, int64_t _offset, uint64_t _origin = FileSeek::CUR);

	/// 
	///
	/// @param[in] _handle:
	/// @param[in] _src:
	/// @param[in] _size:
	///
	/// @returns 
	int64_t	fileWriterWrite(FileWriterHandle _handle, const void* _src, int64_t _size);

	/// 
	///
	/// @param[in] _handle:
	///
	/// @returns 
	int64_t	fileWriterGetSize(FileWriterHandle _handle);

	// ------------------------------------------------
	/// Utility functions
	// ------------------------------------------------

	/// 
	///
	/// @param[in] _type:
	/// @param[in] _path:
	///
	/// @returns 
	int64_t	fileGetSize(FileStorage _type, const char* _path);

	/// 
	///
	/// @param[in] _type:
	/// @param[in] _path:
	/// @param[in] _data:
	/// @param[in] _size:
	///
	/// @returns 
	int64_t	fileRead(FileStorage _type, const char* _path, void* _data, int64_t _size);

	/// 
	///
	/// @param[in] _type:
	/// @param[in] _path:
	/// @param[in] _data:
	/// @param[in] _size:
	///
	/// @returns 
	int64_t	fileWrite(FileStorage _type, const char* _path, const void* _data, int64_t _size);

	/// 
	///
	/// @param[in] _type:
	/// @param[in] _path:
	/// @param[in] _data:
	/// @param[in] _dataSize:
	/// @param[in] _written:
	///
	/// @returns 
	int64_t	fileWriteIfDifferent(FileStorage _type, const char* _path, const void* _data, int64_t _dataSize, bool* _written = 0);

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
