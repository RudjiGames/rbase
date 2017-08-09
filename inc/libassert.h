//--------------------------------------------------------------------------//
/// Copyright (c) 2017 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RBASE_LIBASSERT_H__
#define __RTM_RBASE_LIBASSERT_H__

#ifndef RTM_LIBHANDLE_NAMESPACE
#error "Must define RTM_LIBHANDLE_NAMESPACE!"
#endif

#if !RTM_RETAIL

namespace RTM_LIBHANDLE_NAMESPACE {
	void ErrorHandler_fatal(const char* _file, int _line, const char* _format, ...);
	void ErrorHandler_warning(const char* _file, int _line, const char* _format, ...);
	void ErrorHandler_log(const char* _file, int _line, const char* _format, ...);
} // RTM_LIBHANDLE_NAMESPACE

#ifdef RTM_ASSERT
#undef RTM_ASSERT
#endif

#define RTM_ASSERT(_condition, _format, ...)																		\
				for(;;) {																							\
					RTM_DISABLE_CONST_EXPR_WARNING																	\
					if (!(_condition))																				\
					{																								\
						RTM_LIBHANDLE_NAMESPACE::ErrorHandler_fatal(__FILE__, __LINE__, _format, ##__VA_ARGS__);	\
						RTM_BREAK;																					\
					}																								\
					break;																							\
					RTM_ENABLE_CONST_EXPR_WARNING																	\
				}

#ifdef RTM_WARN
#undef RTM_WARN
#endif

#define RTM_WARN(_condition, _format, ...)																			\
				for(;;) {																							\
					RTM_DISABLE_CONST_EXPR_WARNING																	\
					if (!(_condition))																				\
					{																								\
						RTM_LIBHANDLE_NAMESPACE::ErrorHandler_warning(__FILE__, __LINE__, _format, ##__VA_ARGS__);	\
					}																								\
					break;																							\
					RTM_ENABLE_CONST_EXPR_WARNING																	\
				}

#ifdef RTM_LOG
#undef RTM_LOG
#endif

#define RTM_LOG(_format, ...)																						\
				for(;;) {																							\
					RTM_LIBHANDLE_NAMESPACE::ErrorHandler_log(__FILE__, __LINE__, _format, ##__VA_ARGS__);			\
					break;																							\
				}

#endif // !RTM_RETAIL

#endif // __RTM_RBASE_LIBASSERT_H__

