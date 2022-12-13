//--------------------------------------------------------------------------//
/// Copyright (c) 2022 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_FILE_H
#define RTM_RBASE_FILE_H

#include <rbase/inc/platform.h>

namespace rtm {

	constexpr uint32_t SEEK_CUR = 1;
	constexpr uint32_t SEEK_END = 2;
	constexpr uint32_t SEEK_SET = 0;

	struct FileIO
	{
		enum Enum
		{
			LocalStorage,
			HTTP
		};

		typedef void (*prog)(float _progress);
		typedef void (*done)(void* _data, int32_t _numBytes, void* _userData);
		typedef void (*fail)(const char* _errorMessage);

		prog	progCb;
		done	doneCb;
		fail	failCb;

		FileIO() : progCb(0), doneCb(0), failCb(0) {}
		virtual ~FileIO() {}
	};

	struct FileReader : public FileIO
	{
		virtual ~FileReader() {}
		virtual bool	open(const char* _file) = 0;
		virtual void	close() = 0;
		virtual int64_t	seek(int64_t _offset, uint32_t _origin = SEEK_CUR) = 0;
		virtual int32_t	read(void* _dest, uint32_t _size, void* _userData = 0) = 0;
	};

	struct FileWriter : public FileIO
	{
		virtual ~FileWriter() {}
		virtual bool	open(const char* _file) = 0;
		virtual void	close() = 0;
		virtual int64_t	seek(int64_t _offset, uint32_t _origin = SEEK_CUR) = 0;
		virtual int32_t	write(void* _src, uint32_t _size, void* _userData = 0) = 0;
	};

	FileReader*	createFileReader(FileIO::Enum _type);
	void		deleteFileReader(FileReader*);
	FileWriter*	createFileWriter(FileIO::Enum _type);
	void		deleteFileWriter(FileWriter*);

} // namespace rtm

#endif // RTM_RBASE_FILE_H
