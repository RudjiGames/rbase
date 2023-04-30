//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>
#include <rbase/inc/console.h>

namespace rtm {

struct MemoryManagerCrt : public MemoryManager
{
	virtual void* alloc(size_t _size, size_t _alignment) override
	{
		return malloc(_size);
	}

	virtual void* realloc(void* _ptr, size_t _size, size_t _alignment) override
	{
		return ::realloc(_ptr, _size);
	}

	virtual void free(void* _ptr) override
	{
		::free(_ptr);
	}

} MemoryManagerCrtInstance;


struct ErrorHandlerStd : public ErrorHandler
{
	virtual void fatal(const char* _file, int _line, const char* _message) override
	{
		RTM_UNUSED_2(_file, _line);
		rtm::Console::error(_message);
	}

	virtual void warning(const char* _file, int _line, const char* _message) override
	{
		RTM_UNUSED_2(_file, _line);
		rtm::Console::warning(_message);
	}

	virtual void debug(const char* _file, int _line, const char* _message) override
	{
		RTM_UNUSED_2(_file, _line);
		rtm::Console::print(_message);
	}

} ErrorHandlerStdInstance;

struct MemoryManager* rbaseGetMemoryManager()
{
	return &MemoryManagerCrtInstance;
}

struct ErrorHandler* rbaseGetErrorHandler()
{
	return &ErrorHandlerStdInstance;
}

} // namespace rtm
