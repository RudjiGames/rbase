//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/file.h>
#include <rbase/inc/stringfn.h>
#include <rbase/inc/hash.h>
#include <rbase/inc/console.h>
#include <rbase/inc/uint32_t.h>

#if RTM_PLATFORM_EMSCRIPTEN
#include <emscripten/wget.h>
#endif // RTM_PLATFORM_EMSCRIPTEN

#include <stdio.h>

namespace rtm {

struct FileReaderNoop : public FileReader
{
	virtual ~FileReaderNoop() {}
	bool open(const char* /*_file*/) { return false; }
	void close() {}
	int64_t	seek(int64_t /*_offset*/, uint32_t /*_origin*/) { return 0; }
	int32_t	read(void* /*_dest*/, uint32_t /*_size*/, void* /*_userData*/) { return 0; }
};

struct FileWriterNoop : public FileWriter
{
	virtual ~FileWriterNoop() {}
	bool open(const char* /*_file*/) { return false; }
	void close() {}
	int64_t	seek(int64_t /*_offset*/, uint32_t /*_origin*/) { return 0; }
	int32_t	write(void* /*_src*/, uint32_t /*_size*/, void* /*_userData*/) { return 0; }
};

struct FileReaderLocal : public FileReader
{
	FILE*	m_file;

	FileReaderLocal()
		: m_file(0)
	{
	}

	virtual ~FileReaderLocal()
	{
		close();
	}

	bool open(const char* _file) override
	{
		m_file = fopen(_file, "rb");
		if ((!m_file) && failCb)
			failCb("Failed to open file!");
		return m_file != 0;
	}

	void close() override
	{
		if (m_file)
			fclose(m_file);
		else
			if (failCb)
				failCb("Cannot close file that is not open!");

		m_file = 0;
	}

	int64_t	seek(int64_t _offset, uint32_t _origin) override
	{
		RTM_ASSERT(m_file != 0, "");
		if (!m_file)
		{
			if (failCb) failCb("Cannot seek file that is not open!");
			return 0;
		}
		fseek(m_file, (long)_offset, _origin);
		return ftell(m_file);
	}

	int32_t	read(void* _dest, uint32_t _size, void* _userData) override
	{
		RTM_ASSERT(m_file != 0, "");
		if (!m_file)
		{
			if (failCb) failCb("Cannot read file that is not open!");
			return 0;
		}
		int32_t bytesRead = fread(_dest, 1, _size, m_file);
		if (doneCb)
			doneCb(_dest, bytesRead, _userData);
		return bytesRead;
	}
};

struct FileWriterLocal : public FileWriter
{
	FILE*	m_file;

	FileWriterLocal()
		: m_file(0)
	{
	}

	virtual ~FileWriterLocal()
	{
		close();
	}

	bool open(const char* _file) override
	{
		m_file = fopen(_file, "wb");
		return m_file != 0;
	}

	void close() override
	{
		if (m_file)
			fclose(m_file);
		else
			if (failCb)
				failCb("Cannot close file that is not open!");

		m_file = 0;
	}

	int64_t	seek(int64_t _offset, uint32_t _origin) override
	{
		RTM_ASSERT(m_file != 0, "");
		if (!m_file)
		{
			if (failCb) failCb("Cannot seek file that is not open!");
			return 0;
		}
		fseek(m_file, (long)_offset, _origin);
		return ftell(m_file);
	}

	int32_t	write(void* _src, uint32_t _size, void* _userData) override
	{
		RTM_ASSERT(m_file != 0, "");
		if (!m_file)
		{
			if (failCb) failCb("Cannot write file that is not open!");
			return 0;
		}
		int32_t bytesWritten = fwrite(_src, 1, _size, m_file);
		if (doneCb)
			doneCb(_src, bytesWritten, _userData);
		return bytesWritten;
	}
};

#if RTM_PLATFORM_EMSCRIPTEN
struct FileReaderHTTP : public FileReaderLocal
{
	int			m_reqHandle;
	uint8_t*	m_fileData;
	int32_t		m_fileSize;
	int32_t		m_filePtr;

	FileReaderHTTP()
		: m_reqHandle(0)
		, m_fileData(0)
		, m_fileSize(0)
		, m_filePtr(0)
	{
	}

	virtual ~FileReaderHTTP()
	{
		close();
	}

	bool open(const char* _file) override
	{
		m_reqHandle = emscripten_async_wget2_data(_file, "GET", 0, this, 0, onLoad, onError, onProgress);
		return true;
	}

	void close() override
	{
		if (m_fileData)
		{
			free(m_fileData);
			m_fileData	= 0;
			m_fileSize	= 0;
			m_filePtr	= 0;

		}

		if (m_reqHandle)
			emscripten_async_wget2_abort(m_reqHandle);
		m_reqHandle = 0;
	}

	int64_t	seek(int64_t _offset, uint32_t _origin) override
	{
		switch (_origin)
		{
			case SEEK_CUR:
				_offset += (int64_t)m_filePtr;
				break;

			case SEEK_END:
				_offset += (int64_t)m_fileSize;
				break;
			
			case SEEK_SET:
				_offset += (int64_t)0;
				break;
		};

		if (_offset < 0)			_offset = 0;
		if (_offset > m_fileSize)	_offset = m_fileSize;

		int64_t ret = _offset - (int64_t)(m_filePtr);
		m_filePtr = (uint32_t)_offset;
		return ret;
	}

	int32_t	read(void* _dest, uint32_t _size, void* _userData) override
	{
		if (!m_fileData)
			return 0;

		uint32_t size = rtm::uint32_min(_size, (uint32_t)(m_fileSize - m_filePtr));

		if (size)
		{
			memcpy(_dest, &m_fileData[m_filePtr], _size);
			m_filePtr += _size;
		}

		if (doneCb)
			doneCb(_dest, size, _userData);

		return size;
	}

	static void onLoad(unsigned _handle, void* _userData, void* _buffer, unsigned _bufferSize)
	{
		FileReaderHTTP* reader = (FileReaderHTTP*)_userData;
		RTM_ASSERT(reader->m_reqHandle == _handle, "");
		reader->m_reqHandle	= 0;
		reader->m_fileData	= (uint8_t*)_buffer;
		reader->m_fileSize	= (int32_t)_bufferSize;

		reader->doneCb(_buffer, (int32_t)_bufferSize, _userData);
		memcpy(reader->m_fileData, _buffer, _bufferSize);
	}

	static void onError(unsigned _handle, void* _userData, int /*_httpErrorCode*/, const char* _statusMessage)
	{
		FileReaderHTTP* reader = (FileReaderHTTP*)_userData;
		RTM_ASSERT(reader->m_reqHandle == _handle, "");
		reader->m_reqHandle	= 0;
		reader->failCb(_statusMessage);
	}

	static void onProgress(unsigned _handle, void* _userData, int _loaded, int _totalSizeOrZeroIfUnknown)
	{
		FileReaderHTTP* reader = (FileReaderHTTP*)_userData;
		RTM_ASSERT(reader->m_reqHandle == _handle, "");

		// just pick some size and clamp progress to 1
		if (_totalSizeOrZeroIfUnknown == 0)
			_totalSizeOrZeroIfUnknown = 64 * 1024;

		float progress = (float)_loaded / (float)_totalSizeOrZeroIfUnknown;
		if (progress > 1.0f)
			progress = 1.0f;

		reader->progCb(progress);
	}
};

//#elif RTM_PLATFORM_WINDOWS
//struct FileReaderHTTP : public FileReaderLocal
//{
//};
//
#else
typedef FileReaderNoop FileReaderHTTP;
#endif

FileReader*	createFileReader(FileReader::Enum _type)
{
	switch (_type)
	{
	case FileReader::LocalStorage:	return new FileReaderLocal();
	case FileReader::HTTP:			return new FileReaderHTTP();
	};

	return 0;
}

void deleteFileReader(FileReader* _instance)
{
	delete _instance;
}

FileWriter*	createFileWriter(FileWriter::Enum _type)
{
	switch (_type)
	{
	case FileWriter::LocalStorage:	return new FileWriterLocal();
	case FileWriter::HTTP:			return 0;
	};

	return 0;
}

void deleteFileWriter(FileWriter* _instance)
{
	delete _instance;
}

} // namespace rtm
