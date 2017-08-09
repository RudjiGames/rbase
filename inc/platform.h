//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_PLATFORM_H__
#define __RTM_RBASE_PLATFORM_H__

#include <stdint.h>

//--------------------------------------------------------------------------
/// Platforms
//--------------------------------------------------------------------------
#define RTM_PLATFORM_WINDOWS	0
#define RTM_PLATFORM_WINPHONE	0
#define RTM_PLATFORM_WINSTORE	0
#define RTM_PLATFORM_LINUX		0
#define RTM_PLATFORM_IOS		0
#define RTM_PLATFORM_OSX		0
#define RTM_PLATFORM_PS3		0
#define RTM_PLATFORM_PS4		0
#define RTM_PLATFORM_XB1		0
#define RTM_PLATFORM_ANDROID	0
#define RTM_PLATFORM_NACL		0

//--------------------------------------------------------------------------
/// Compilers
//--------------------------------------------------------------------------
#define RTM_COMPILER_MSVC		0
#define RTM_COMPILER_GCC		0
#define RTM_COMPILER_CLANG		0
#define RTM_COMPILER_SNC		0

//--------------------------------------------------------------------------
/// CPUs
//--------------------------------------------------------------------------
#define RTM_CPU_X86				0
#define RTM_CPU_PPC				0
#define RTM_CPU_ARM				0
#define RTM_CPU_MIPS			0

//--------------------------------------------------------------------------
/// Endianess
//--------------------------------------------------------------------------
#define RTM_LITTLE_ENDIAN		0
#define RTM_BIG_ENDIAN			0

//--------------------------------------------------------------------------
/// Word size
//--------------------------------------------------------------------------
#define RTM_32BIT				0
#define RTM_64BIT				0

//--------------------------------------------------------------------------
/// Detect compiler
//--------------------------------------------------------------------------
#if defined(__SNC__)
#undef RTM_COMPILER_SNC
#define RTM_COMPILER_SNC		1

#elif defined(__GNUC__)
#undef RTM_COMPILER_GCC
#define RTM_COMPILER_GCC		1

// check for clang before GCC as clang defines GNU macros as well
#elif defined(__clang__)
#undef RTM_COMPILER_CLANG
#define RTM_COMPILER_CLANG		1

#elif defined(_MSC_VER)
#undef RTM_COMPILER_MSVC
#define RTM_COMPILER_MSVC		1

#else
#error "Compiler not supported!"
#endif

//--------------------------------------------------------------------------
/// Detect platform
//--------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#undef RTM_PLATFORM_WINDOWS
#define RTM_PLATFORM_WINDOWS	1
#elif defined(__ANDROID__)
#undef RTM_PLATFORM_ANDROID
#define RTM_PLATFORM_ANDROID	1
#elif defined(__linux__) || defined(linux)
#undef RTM_PLATFORM_LINUX
#define RTM_PLATFORM_LINUX		1
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#undef  RTM_PLATFORM_IOS
#define RTM_PLATFORM_IOS		1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#undef  RTM_PLATFORM_OSX
#define RTM_PLATFORM_OSX		1
#elif defined(__ORBIS__)
#undef RTM_PLATFORM_PS4
#define RTM_PLATFORM_PS4		1
#else
#error "Platform not supported!"
#endif

#define RTM_PLATFORM_POSIX (RTM_PLATFORM_LINUX		||	\
							RTM_PLATFORM_OSX		||	\
							RTM_PLATFORM_ANDROID	||	\
							RTM_PLATFORM_IOS		||	\
							0)

//--------------------------------------------------------------------------
/// Detect CPU
//--------------------------------------------------------------------------
#if defined(__arm__)
#undef RTM_CPU_ARM
#define RTM_CPU_ARM				1
#define RTM_CACHE_LINE_SIZE		64
#elif defined(__MIPSEL__) || defined(__mips_isa_rev)
#undef RTM_CPU_MIPS
#define RTM_CPU_MIPS			1
#define RTM_CACHE_LINE_SIZE		64
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__) || defined(__PPU__)
#undef RTM_CPU_PPC
#define RTM_CPU_PPC				1
#define RTM_CACHE_LINE_SIZE		128
#elif defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#undef RTM_CPU_X86
#define RTM_CPU_X86				1
#define RTM_CACHE_LINE_SIZE		64
#else
#error "Platform not supported!"
#endif

//--------------------------------------------------------------------------
/// Detect endianess
//--------------------------------------------------------------------------
#if RTM_CPU_PPC
#undef RTM_BIG_ENDIAN
#define RTM_BIG_ENDIAN			1
#else
#undef RTM_LITTLE_ENDIAN
#define RTM_LITTLE_ENDIAN		1
#endif

//--------------------------------------------------------------------------
/// 32bit or 64bit
//--------------------------------------------------------------------------

#if (defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(__ppc64__) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) )
#undef RTM_64BIT
#define RTM_64BIT 1
#else
#undef RTM_32BIT
#define RTM_32BIT 1
#endif

//--------------------------------------------------------------------------
/// Alignment macros
//--------------------------------------------------------------------------
#if RTM_COMPILER_MSVC
#define RTM_ALIGN(_x)	__declspec(align(_x))
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
#define RTM_ALIGN(_x)	__attribute__((aligned(_x)))
#endif

#if RTM_COMPILER_GCC || RTM_COMPILER_CLANG
#define RTM_ALIGNOF(_type) __alignof__(_type)
#else
#define RTM_ALIGNOF(_type) __alignof(_type)
#endif
#define RTM_ALIGNTO(_value, _a)	(((_value)+(_a-1)) & ((~0)&(~(_a-1))))

//--------------------------------------------------------------------------
/// Inlining macros
//--------------------------------------------------------------------------
#if RTM_COMPILER_MSVC
#define RFORCEINLINE	__forceinline
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG || RTM_COMPILER_SNC
#define RFORCEINLINE	__attribute__((always_inline))
#else
#error "Unsupported compiler!"
#endif

#if RTM_COMPILER_MSVC
#define RNOINLINE	__declspec(noinline)
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG || RTM_COMPILER_SNC
#define RNOINLINE	__attribute__((no_inline))
#else
#error "Unsupported compiler!"
#endif


//--------------------------------------------------------------------------
/// Asserts
//--------------------------------------------------------------------------

#if RTM_COMPILER_MSVC
#define RTM_BREAK	__debugbreak()
#elif RTM_CPU_ARM
#define RTM_BREAK	asm("bkpt 0")
#elif !RTM_PLATFORM_NACL && RTM_CPU_X86 && (RTM_COMPILER_GCC || RTM_COMPILER_CLANG)
#define RTM_BREAK	__asm__ ("int $3")
#else
#define RTM_BREAK	{ int* int3 = (int*)3L; *int3 = 3; }
#endif

#ifndef RTM_ASSERT
#define RTM_ASSERT(_condition, _format, ...) for(;;) { (void)sizeof(_condition); break; }
#endif // RTM_ASSERT

#ifndef RTM_WARN
#define RTM_WARN(_format, ...) for(;;) { break; }
#endif // RTM_WARN

#ifndef RTM_LOG
#define RTM_LOG(_format, ...)  for(;;) { break; }
#endif // RTM_LOG

#if RTM_COMPILER_MSVC
#define RTM_DISABLE_CONST_EXPR_WARNING	\
	__pragma(warning(push))				\
	__pragma(warning(disable:4127))

#define RTM_ENABLE_CONST_EXPR_WARNING	\
	__pragma(warning(pop))
#else
#define RTM_DISABLE_CONST_EXPR_WARNING
#define RTM_ENABLE_CONST_EXPR_WARNING
#endif

#define RTM_ALIGNVERIFY(_addr, _alignment)	\
	RTM_ASSERT((((uintptr_t)_addr)%_alignment == 0), "Bad alignment!")

namespace rtm {

	struct MemoryManager
	{
		virtual void* alloc(size_t _size, size_t _alignment) = 0;
		virtual void* realloc(void* _ptr, size_t _size, size_t _alignment) = 0;
		virtual void  free(void* _ptr) = 0;
	};

	struct ErrorHandler
	{
		virtual void fatal(const char* _file, int _line, const char* _message) = 0;
		virtual void warning(const char* _file, int _line, const char* _message) = 0;
		virtual void debug(const char* _file, int _line, const char* _message) = 0;
	};

	struct LibInterface
	{
		MemoryManager*	m_memory;
		ErrorHandler*	m_error;

		LibInterface()
			: m_memory(0)
			, m_error(0)
		{}
	};

} // namespace rtm

//--------------------------------------------------------------------------
/// Utility
//--------------------------------------------------------------------------

template<typename T, size_t N> char (&RTM_NUM_ELEMENTS_REQUIRES_ARRAY_ARGUMENT(const T(&)[N]) )[N];
#define RTM_NUM_ELEMENTS(_x) sizeof(RTM_NUM_ELEMENTS_REQUIRES_ARRAY_ARGUMENT(_x) )

#define RTM_CLASS_NO_COPY(_class)					\
	private:_class(const _class&);					\
			_class& operator = (const _class&);


#define RTM_CLASS_NO_DEFAULT_CONSTRUCTOR(_class)	\
	private: _class();

#define RTM_UNUSED(_x) ((void)_x)

#if RTM_COMPILER_MSVC
#define RTM_ALIGN_TYPE(_align, _type) __declspec(align(_align)) _type
#elif RTM_COMPILER_GCC || RTM_COMPILER_CLANG
#define RTM_ALIGN_TYPE(_align, _type) _type __attribute__((aligned(_align)))
#else
#error "Unsupported compiler!"
#endif

#endif // __RTM_RBASE_PLATFORM_H__

