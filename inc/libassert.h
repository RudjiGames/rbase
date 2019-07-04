//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RBASE_LIBASSERT_H
#define RTM_RBASE_LIBASSERT_H

#ifndef RBASE_NAMESPACE
#error "Must define RBASE_NAMESPACE!"
#endif

#if !RTM_RETAIL

namespace RBASE_NAMESPACE {
	void ErrorHandler_fatal(const char* _file, int _line, const char* _format, ...);
	void ErrorHandler_warning(const char* _file, int _line, const char* _format, ...);
	void ErrorHandler_log(const char* _file, int _line, const char* _format, ...);
} // RBASE_NAMESPACE

#ifdef RTM_ASSERT
#undef RTM_ASSERT
#endif

#define RTM_ASSERT(_condition, _format, ...)																		\
				for(;;) {																							\
					RTM_DISABLE_CONST_EXPR_WARNING																	\
					if (!(_condition))																				\
					{																								\
						RBASE_NAMESPACE::ErrorHandler_fatal(__FILE__, __LINE__, _format, ##__VA_ARGS__);	\
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
						RBASE_NAMESPACE::ErrorHandler_warning(__FILE__, __LINE__, _format, ##__VA_ARGS__);	\
					}																								\
					break;																							\
					RTM_ENABLE_CONST_EXPR_WARNING																	\
				}

#ifdef RTM_LOG
#undef RTM_LOG
#endif

#define RTM_LOG(_format, ...)																						\
				for(;;) {																							\
					RBASE_NAMESPACE::ErrorHandler_log(__FILE__, __LINE__, _format, ##__VA_ARGS__);			\
					break;																							\
				}

#endif // !RTM_RETAIL

#endif // RTM_RBASE_LIBASSERT_H

