//--------------------------------------------------------------------------//
/// Copyright (c) 2022 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_FILE_H
#define RTM_RBASE_FILE_H

#include <rbase/inc/platform.h>

namespace rtm {

	struct File
	{
		enum Seek
		{
			Seek_CUR	= 1,
			Seek_END	= 2,
			Seek_SET	= 0
		};

		enum Status
		{
			Open		= 1,
			Closed,
			Fail,
			Downloading
		};

		enum Enum
		{
			LocalStorage,
			HTTP
		};
	};

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
		{}
	};

	struct FileReaderHandle { uint32_t idx; };
	struct FileWriterHandle { uint32_t idx; };

	// ------------------------------------------------
	/// File reader functions
	// ------------------------------------------------

	///
	FileReaderHandle fileReaderCreate(File::Enum _type = File::LocalStorage, FileCallBacks* _callBacks = 0);

	///
	void fileReaderDestroy(FileReaderHandle _handle);

	///
	File::Status fileReaderOpen(FileReaderHandle _handle, const char* _path);

	///
	void fileReaderClose(FileReaderHandle _handle);

	///
	File::Status fileReaderGetStatus(FileReaderHandle _handle);

	///
	int64_t	fileReaderSeek(FileReaderHandle _handle, int64_t _offset, uint32_t _origin = File::Seek_CUR);

	///
	int32_t	fileReaderRead(FileReaderHandle _handle, void* _dest, uint32_t _size);

	// ------------------------------------------------
	/// File writer functions
	// ------------------------------------------------

	///
	FileWriterHandle fileWriterCreate(File::Enum _type = File::LocalStorage, FileCallBacks* _callBacks = 0);

	///
	void fileWriterDestroy(FileWriterHandle _handle);

	///
	File::Status fileWriterOpen(FileWriterHandle _handle, const char* _path);

	///
	void fileWriterClose(FileWriterHandle _handle);

	///
	File::Status fileWriterGetStatus(FileReaderHandle _handle);

	///
	int64_t	fileWriterSeek(FileWriterHandle _handle, int64_t _offset, uint32_t _origin = File::Seek_CUR);

	///
	int32_t	fileWriterRead(FileWriterHandle _handle, void* _dest, uint32_t _size);

} // namespace rtm

#endif // RTM_RBASE_FILE_H
