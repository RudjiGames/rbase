//--------------------------------------------------------------------------//
/// Copyright (c) 2023 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/file.h>
#include <rbase/inc/datastore.h>
#include <rbase/inc/stringfn.h>
#include <rbase/inc/hash.h>
#include <rbase/inc/thread.h>
#include <rbase/inc/console.h>
#include <rbase/inc/uint32_t.h>

#if RTM_PLATFORM_EMSCRIPTEN
#include <emscripten/wget.h>
#endif // RTM_PLATFORM_EMSCRIPTEN

#include <stdio.h>

namespace rtm {

constexpr int RTM_MAX_FILES = 64;

struct FileReader
{
	RTM_ALIGN(16) uint8_t	m_data[64];
	FileCallBacks			m_callBacks;

	void			(*construct)(FileReader*);
	void			(*destruct)(FileReader*);
	File::Status	(*open)(FileReader*, const char* _file);
	File::Status	(*getstatus)(FileReader*);
	void			(*close)(FileReader*);
	int64_t			(*seek)(FileReader*, int64_t _offset, uint32_t _origin);
	int32_t			(*read)(FileReader*, void* _dest, uint32_t _size);
};

struct FileWriter
{
	RTM_ALIGN(16) uint8_t	m_data[64];
	FileCallBacks			m_callBacks;

	void			(*construct)(FileWriter*);
	void			(*destruct)(FileWriter*);
	File::Status	(*open)(FileWriter*, const char* _file);
	File::Status	(*getstatus)(FileWriter*);
	void			(*close)(FileWriter*);
	int64_t			(*seek)(FileWriter*, int64_t _offset, uint32_t _origin);
	int32_t			(*write)(FileWriter*, void* _dest, uint32_t _size);
};

rtm::Data<FileReader, RTM_MAX_FILES, rtm::Storage::Dense>	s_readers;
rtm::Data<FileWriter, RTM_MAX_FILES, rtm::Storage::Dense>	s_writers;

// ------------------------------------------------
/// Noop reader/writer
// ------------------------------------------------

File::Status	noopReadOpen(FileReader*, const char* _path) { RTM_UNUSED(_path); return File::Fail; }
void			noopReadClose(FileReader*) {}
File::Status	noopReadGetStatus(FileReader*) { return File::Closed; }
int64_t			noopReadSeek(FileReader*, int64_t _offset, uint32_t _origin) { RTM_UNUSED_2(_offset, _origin); return 0; }
int32_t			noopReadRead(FileReader*, void* _dest, uint32_t _size) { RTM_UNUSED_2(_dest, _size); return 0; }

File::Status	noopWriteOpen(FileWriter*, const char* _path) { RTM_UNUSED(_path); return File::Fail; }
void			noopWriteClose(FileWriter*) {}
File::Status	noopWriteGetStatus(FileWriter*) { return File::Closed; }
int64_t			noopWriteSeek(FileWriter*, int64_t _offset, uint32_t _origin) { RTM_UNUSED_2(_offset, _origin); return 0; }
int32_t			noopWriteWrite(FileWriter*, void* _dest, uint32_t _size) { RTM_UNUSED_2(_dest, _size); return 0; }

void fileReaderSetNoop(FileReader* _reader)
{
	_reader->construct	= noopReadClose;	// same fn declaration
	_reader->destruct	= noopReadClose;	// same fn declaration
	_reader->open		= noopReadOpen;
	_reader->getstatus	= noopReadGetStatus;
	_reader->close		= noopReadClose;
	_reader->seek		= noopReadSeek;
	_reader->read		= noopReadRead;
}

void fileWriterSetNoop(FileWriter* _writer)
{
	_writer->construct	= noopWriteClose;	// same fn declaration
	_writer->destruct	= noopWriteClose;	// same fn declaration
	_writer->open		= noopWriteOpen;
	_writer->getstatus	= noopWriteGetStatus;
	_writer->close		= noopWriteClose;
	_writer->seek		= noopWriteSeek;
	_writer->write		= noopWriteWrite;
}

// ------------------------------------------------
/// Local storage reader/writer
// ------------------------------------------------

struct membersLocal
{
	FILE* m_file;
};

#define LOCAL(_file) (*(membersLocal*)_file->m_data)

void localReadConstruct(FileReader* _file)
{
	LOCAL(_file).m_file = 0;
}

File::Status localReadOpen(FileReader* _file, const char* _path)
{
	LOCAL(_file).m_file = fopen(_path, "rb");
	if (LOCAL(_file).m_file)
	{
		if (_file->m_callBacks.m_doneCb)
			_file->m_callBacks.m_doneCb(_path);
		return File::Open;
	}

	return File::Fail;
}

File::Status localReadGetStatus(FileReader* _file)
{
	if (LOCAL(_file).m_file)
		return File::Open;
	return File::Closed;
}

void localReadClose(FileReader* _file)
{
	if (LOCAL(_file).m_file)
	{
		fclose(LOCAL(_file).m_file);
		LOCAL(_file).m_file = 0;
	}
}

void localReadDestruct(FileReader* _file)
{
	localReadClose(_file);
}

int64_t	localReadSeek(FileReader* _file, int64_t _offset, uint32_t _origin)
{
	if (!LOCAL(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot seek. File is not open!");
		return 0;
	}

	fseek(LOCAL(_file).m_file, (long)_offset, _origin);
	return ftell(LOCAL(_file).m_file);
}

int32_t	localReadRead(FileReader* _file, void* _dest, uint32_t _size)
{
	if (!LOCAL(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot read. File is not open!");
		return 0;
	}

	return (int32_t)fread(_dest, 1, _size, LOCAL(_file).m_file);
}

void localWriteConstruct(FileWriter* _file)
{
	LOCAL(_file).m_file = 0;
}

File::Status localWriteOpen(FileWriter* _file, const char* _path)
{
	LOCAL(_file).m_file = fopen(_path, "wb");
	if (LOCAL(_file).m_file != 0)
		return File::Open;
	return File::Fail;
}

File::Status localWriteGetStatus(FileWriter* _file)
{
	if (LOCAL(_file).m_file)
		return File::Open;
	return File::Closed;
}

void localWriteClose(FileWriter* _file)
{
	if (LOCAL(_file).m_file)
	{
		fclose(LOCAL(_file).m_file);
		LOCAL(_file).m_file = 0;
	}
}

void localWriteDestruct(FileWriter* _file)
{
	localWriteClose(_file);
}

int64_t	localWriteSeek(FileWriter* _file, int64_t _offset, uint32_t _origin)
{
	if (!LOCAL(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot seek. File is not open!");
		return 0;
	}

	fseek(LOCAL(_file).m_file, (long)_offset, _origin);
	return ftell(LOCAL(_file).m_file);
}

int32_t	localWriteWrite(FileWriter* _file, void* _dest, uint32_t _size)
{
	if (!LOCAL(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot write. File is not open!");
		return 0;
	}

	return (int32_t)fwrite(_dest, 1, _size, LOCAL(_file).m_file);
}

void fileReaderSetLocal(FileReader* _reader)
{
	_reader->construct	= localReadConstruct;
	_reader->destruct	= localReadDestruct;
	_reader->open		= localReadOpen;
	_reader->getstatus	= localReadGetStatus;
	_reader->close		= localReadClose;
	_reader->seek		= localReadSeek;
	_reader->read		= localReadRead;
}

void fileWriterSetLocal(FileWriter* _writer)
{
	_writer->construct	= localWriteConstruct;
	_writer->destruct	= localWriteDestruct;
	_writer->open		= localWriteOpen;
	_writer->getstatus	= localWriteGetStatus;
	_writer->close		= localWriteClose;
	_writer->seek		= localWriteSeek;
	_writer->write		= localWriteWrite;
}

// ------------------------------------------------
/// HTTP reader/writer
// ------------------------------------------------

#if RTM_PLATFORM_WINDOWS

#pragma comment(lib, "urlmon.lib")

#include <urlmon.h>

struct membersHTTP
{
	FILE*		m_file;
	rtm::Thread m_thread;
	char*		m_url;
};

#define HTTP(_file) (*(membersHTTP*)_file->m_data)

class DownloadProgress : public IBindStatusCallback {
public:
	FileReader* m_reader;

    HRESULT __stdcall QueryInterface(const IID &,void **) { 
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
    ULONG STDMETHODCALLTYPE Release(void) { return 1; }
    HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD /*dwReserved*/, IBinding* /*pib*/) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG* /*pnPriority*/) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD /*reserved*/) { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT /*hresult*/, LPCWSTR /*szError*/) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* /*grfBINDF*/, BINDINFO* /*pbindinfo*/) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD /*grfBSCF*/, DWORD /*dwSize*/, FORMATETC* /*pformatetc*/, STGMEDIUM* /*pstgmed*/) { return E_NOTIMPL; }        
    virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID /*riid*/, IUnknown* /*punk*/) { return E_NOTIMPL; }

    virtual HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG /*ulStatusCode*/, LPCWSTR /*szStatusText*/)
    {
		if (m_reader->m_callBacks.m_progCb)
			m_reader->m_callBacks.m_progCb((float)ulProgress/(float)ulProgressMax);
        return S_OK;
    }
};

struct DownloadThread
{
	static int32_t threadFunc(void* _userData)
	{
		FileReader* file = (FileReader*)_userData;

		uint8_t digest[16];
		char	hash[33];
		rtm::md5_calculate(HTTP(file).m_url, rtm::strLen(HTTP(file).m_url), digest);
		rtm::md5_to_string(digest, hash);
		RTM_LOG("Downloading %s to %s\n", _path, hash);

		DownloadProgress progress;
		progress.m_reader = file;
		if (S_OK == URLDownloadToFile(0, HTTP(file).m_url, hash, 0, static_cast<IBindStatusCallback*>(&progress)))
		{
			HTTP(file).m_file = fopen(hash, "rb");
			if (file->m_callBacks.m_doneCb)
				file->m_callBacks.m_doneCb(hash);
		}
		else
		{
			if (file->m_callBacks.m_failCb)
				file->m_callBacks.m_failCb("Downloading failed!");
		}

		delete[] HTTP(file).m_url;
		HTTP(file).m_url = 0;

		return HTTP(file).m_file != 0 ? File::Open : File::Fail;
	}
};

void httpReadConstruct(FileReader* _file)
{
	HTTP(_file).m_file = 0;
}

File::Status httpReadOpen(FileReader* _file, const char* _path)
{
	uint32_t len = rtm::strLen(_path);
	HTTP(_file).m_url = new char[len+1];
	rtm::strlCpy(HTTP(_file).m_url, len+1, _path);
	HTTP(_file).m_thread.start(DownloadThread::threadFunc, _file);
	return File::Downloading;
}

File::Status httpReadGetStatus(FileReader* _file)
{
	if (HTTP(_file).m_file)
		return File::Open;

	if (HTTP(_file).m_url)
	{
		uint32_t exitCode = HTTP(_file).m_thread.getExitCode();
		if (exitCode == 0) return File::Downloading;
		if (exitCode == 1) return File::Open;
	}
	return File::Fail;
}

void httpReadClose(FileReader* _file)
{
	if (HTTP(_file).m_file)
	{
		fclose(HTTP(_file).m_file);
		HTTP(_file).m_file = 0;
		delete[] HTTP(_file).m_url;
		HTTP(_file).m_url = 0;
	}
}

void httpReadDestruct(FileReader* _file)
{
	httpReadClose(_file);
}

int64_t	httpReadSeek(FileReader* _file, int64_t _offset, uint32_t _origin)
{
	RTM_ASSERT(HTTP(_file).m_file != 0, "");
	if (!HTTP(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot seek. File is not open!");
		return 0;
	}
	fseek(HTTP(_file).m_file, (long)_offset, _origin);
	return ftell(HTTP(_file).m_file);
}

int32_t	httpReadRead(FileReader* _file, void* _dest, uint32_t _size)
{
	if (!HTTP(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot read. File is not open!");
		return 0;
	}
	return (int32_t)fread(_dest, 1, _size, HTTP(_file).m_file);
}
#elif RTM_PLATFORM_EMSCRIPTEN

struct membersHTTP
{
	int		m_reqHandle;
	FILE*	m_file;
};

#define HTTP(_file) (*(membersHTTP*)_file->m_data)

static void onLoad(unsigned _handle, void* _userData, const char* _path)
{
	FileReader* _file = (FileReader*)_userData;
	RTM_ASSERT(HTTP(_file).m_reqHandle == _handle, "");
	RTM_UNUSED(_handle);
	HTTP(_file).m_reqHandle		= 0;
	HTTP(_file).m_file			= fopen(_path, "rb");

	if ((!HTTP(_file).m_file) && (_file->m_callBacks.m_failCb))
		_file->m_callBacks.m_failCb("Failed to open file!");
	else
		if (_file->m_callBacks.m_doneCb)
			_file->m_callBacks.m_doneCb(_path);
}

static void onError(unsigned _handle, void* _userData, int /*_httpErrorCode*/)
{
	FileReader* _file = (FileReader*)_userData;
	RTM_ASSERT(HTTP(_file).m_reqHandle == _handle, "");
	RTM_UNUSED(_handle);
	HTTP(_file).m_reqHandle	= 0;

	if (_file->m_callBacks.m_failCb)
		_file->m_callBacks.m_failCb("Downloading failed!");
}

static void onProgress(unsigned _handle, void* _userData, int _percent)
{
	FileReader* _file = (FileReader*)_userData;
	RTM_ASSERT(HTTP(_file).m_reqHandle == _handle, "");
	RTM_UNUSED(_handle);

	float progress = (float)_percent / (float)100.0f;
	if (progress > 1.0f)
		progress = 1.0f;

	if (_file->m_callBacks.m_progCb)
		_file->m_callBacks.m_progCb(progress);
}

void httpReadConstruct(FileReader* _file)
{
	HTTP(_file).m_reqHandle		= 0;
	HTTP(_file).m_file			= 0;
}

File::Status httpReadOpen(FileReader* _file, const char* _path)
{
	uint8_t digest[16];
	char	hash[33];
	rtm::md5_calculate(_path, rtm::strLen(_path), digest);
	rtm::md5_to_string(digest, hash);
	RTM_LOG("Downloading %s to %s\n", _path, hash);

	if (!HTTP(_file).m_file)
		HTTP(_file).m_reqHandle = emscripten_async_wget2(_path, hash, "GET", 0, _file, onLoad, onError, onProgress);

	if (HTTP(_file).m_reqHandle != 0)
		return File::Downloading;

	return File::Fail;
}

File::Status httpReadGetStatus(FileReader* _file)
{
	if (HTTP(_file).m_file)
		return File::Open;

	if (HTTP(_file).m_reqHandle)
		return File::Downloading;

	return File::Fail;
}

void httpReadClose(FileReader* _file)
{
	if (HTTP(_file).m_reqHandle)
		emscripten_async_wget2_abort(HTTP(_file).m_reqHandle);
	HTTP(_file).m_reqHandle = 0;

	if (HTTP(_file).m_file)
	{
		fclose(HTTP(_file).m_file);
		HTTP(_file).m_file = 0;
	}
}

void httpReadDestruct(FileReader* _file)
{
	httpReadClose(_file);
}

int64_t	httpReadSeek(FileReader* _file, int64_t _offset, uint32_t _origin)
{
	RTM_ASSERT(HTTP(_file).m_file != 0, "");
	if (!HTTP(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot seek. File is not open!");
		return 0;
	}
	fseek(HTTP(_file).m_file, (long)_offset, _origin);
	return ftell(HTTP(_file).m_file);
}

int32_t	httpReadRead(FileReader* _file, void* _dest, uint32_t _size)
{
	if (!HTTP(_file).m_file)
	{
		if (_file->m_callBacks.m_failCb)
			_file->m_callBacks.m_failCb("Cannot read. File is not open!");
		return 0;
	}
	return fread(_dest, 1, _size, HTTP(_file).m_file);
}

#else // RTM_PLATFORM_EMSCRIPTEN
	#define httpReadConstruct	noopReadClose
	#define httpReadDestruct	noopReadClose
	#define httpReadOpen		noopReadOpen
	#define httpReadGetStatus	noopReadGetStatus
	#define httpReadClose		noopReadClose
	#define httpReadSeek		noopReadSeek
	#define httpReadRead		noopReadRead
#endif // RTM_PLATFORM_EMSCRIPTEN

void httpWriteConstruct(FileWriter* _file)
{
	RTM_UNUSED(_file);
}

File::Status httpWriteOpen(FileWriter* _file, const char* _path)
{
	RTM_UNUSED_2(_file, _path);
	return File::Fail;
}

File::Status httpWriteIsGetStatus(FileWriter*)
{
	return File::Closed;
}

void httpWriteClose(FileWriter* _file)
{
	RTM_UNUSED(_file);
}

void httpWriteDestruct(FileWriter* _file)
{
	httpWriteClose(_file);
}

int64_t	httpWriteSeek(FileWriter* _file, int64_t _offset, uint32_t _origin)
{
	RTM_UNUSED_3(_file, _offset, _origin);
	return 0;
}

int32_t	httpWriteWrite(FileWriter* _file, void* _dest, uint32_t _size)
{
	RTM_UNUSED_3(_file, _dest, _size);
	return 0;
}

void fileReaderSetHTTP(FileReader* _reader)
{
	_reader->construct	= httpReadConstruct;
	_reader->destruct	= httpReadDestruct;
	_reader->open		= httpReadOpen;
	_reader->getstatus	= httpReadGetStatus;
	_reader->close		= httpReadClose;
	_reader->seek		= httpReadSeek;
	_reader->read		= httpReadRead;
}

void fileWriterSetHTTP(FileWriter* _writer)
{
	_writer->construct	= httpWriteConstruct;
	_writer->destruct	= httpWriteDestruct;
	_writer->open		= httpWriteOpen;
	_writer->getstatus	= httpWriteIsGetStatus;
	_writer->close		= httpWriteClose;
	_writer->seek		= httpWriteSeek;
	_writer->write		= httpWriteWrite;
}

// ------------------------------------------------
/// API
// ------------------------------------------------

FileReaderHandle fileReaderCreate(File::Enum _type, FileCallBacks* _callBacks)
{
	FileReader* reader = 0;
	uint32_t idx = s_readers.allocate(reader);

	if (s_readers.isValid(idx))
	{
		switch (_type)
		{
			case File::LocalStorage:	fileReaderSetLocal(reader); break;
			case File::HTTP:			fileReaderSetHTTP(reader); break;
			default:					fileReaderSetNoop(reader); break;
		};

		if (_callBacks)
			reader->m_callBacks = *_callBacks;

		reader->construct(reader);

		return { idx };
	}

	return { UINT32_MAX };
}

void fileReaderDestroy(FileReaderHandle _handle)
{
	if (!s_readers.isValid(_handle.idx))
		return;

	FileReader* reader = s_readers.getDataPtr(_handle.idx);
	reader->destruct(reader);

	s_readers.free(_handle.idx);
}

File::Status fileReaderOpen(FileReaderHandle _handle, const char* _path)
{
	if (!s_readers.isValid(_handle.idx))
		return File::Fail;

	FileReader* reader = s_readers.getDataPtr(_handle.idx);
	return reader->open(reader, _path);
}

void fileReaderClose(FileReaderHandle _handle)
{
	if (!s_readers.isValid(_handle.idx))
		return;

	FileReader* reader = s_readers.getDataPtr(_handle.idx);
	return reader->close(reader);
}

File::Status fileReaderGetStatus(FileReaderHandle _handle)
{
	if (!s_readers.isValid(_handle.idx))
		return File::Fail;

	FileReader* reader = s_readers.getDataPtr(_handle.idx);
	return reader->getstatus(reader);
}

int64_t	fileReaderSeek(FileReaderHandle _handle, int64_t _offset, uint32_t _origin)
{
	if (!s_readers.isValid(_handle.idx))
		return 0;

	FileReader* reader = s_readers.getDataPtr(_handle.idx);
	return reader->seek(reader, _offset, _origin);
}

int32_t	fileReaderRead(FileReaderHandle _handle, void* _dest, uint32_t _size)
{
	if (!s_readers.isValid(_handle.idx))
		return 0;

	FileReader* reader = s_readers.getDataPtr(_handle.idx);
	return reader->read(reader, _dest, _size);
}

FileWriterHandle fileWriterCreate(File::Enum _type, FileCallBacks* _callBacks)
{
	FileWriter* writer = 0;
	uint32_t idx = s_writers.allocate(writer);

	if (s_writers.isValid(idx))
	{
		switch (_type)
		{
			case File::LocalStorage:	fileWriterSetLocal(writer); break;
			case File::HTTP:			fileWriterSetHTTP(writer); break;
			default:					fileWriterSetNoop(writer); break;
		};

		if (_callBacks)
			writer->m_callBacks = *_callBacks;

		writer->construct(writer);

		return { idx };
	}

	return { UINT32_MAX };
}

void fileWriterDestroy(FileWriterHandle _handle)
{
	if (!s_writers.isValid(_handle.idx))
		return;

	FileWriter* writer = s_writers.getDataPtr(_handle.idx);
	writer->destruct(writer);

	s_writers.free(_handle.idx);
}

File::Status fileWriterOpen(FileWriterHandle _handle, const char* _path)
{
	if (!s_writers.isValid(_handle.idx))
		return File::Fail;

	FileWriter* writer = s_writers.getDataPtr(_handle.idx);
	return writer->open(writer, _path);
}

void fileWriterClose(FileWriterHandle _handle)
{
	if (!s_writers.isValid(_handle.idx))
		return;

	FileWriter* writer = s_writers.getDataPtr(_handle.idx);
	return writer->close(writer);
}

File::Status fileWriterGetStatus(FileReaderHandle _handle)
{
	if (!s_writers.isValid(_handle.idx))
		return File::Fail;

	FileWriter* writer = s_writers.getDataPtr(_handle.idx);
	return writer->getstatus(writer);
}

int64_t	fileWriterSeek(FileWriterHandle _handle, int64_t _offset, uint32_t _origin)
{
	if (!s_writers.isValid(_handle.idx))
		return 0;

	FileWriter* writer = s_writers.getDataPtr(_handle.idx);
	return writer->seek(writer, _offset, _origin);
}

int32_t	fileWriterRead(FileWriterHandle _handle, void* _dest, uint32_t _size)
{
	if (!s_writers.isValid(_handle.idx))
		return 0;

	FileWriter* writer = s_writers.getDataPtr(_handle.idx);
	return writer->write(writer, _dest, _size);
}

} // namespace rtm
