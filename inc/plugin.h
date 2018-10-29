/*--------------------------------------------------------------------------
  	Copyright (c) 2018 Milos Tosic. All Rights Reserved.
	License: http://www.opensource.org/licenses/BSD-2-Clause
--------------------------------------------------------------------------*/

#ifndef RTM_RBASE_PLUGIN_H
#define RTM_RBASE_PLUGIN_H

/* SDK version  */

#define	RTM_PLUGIN_VERSION_HIGH				1
#define	RTM_PLUGIN_VERSION_LOW				0
#define RTM_PLUGIN_VERSION					((RTM_PLUGIN_VERSION_HIGH << 16) | RTM_PLUGIN_VERSION_LOW)

/* Return values */

#define	RTM_RESULT_OK						0x00000000
#define	RTM_RESULT_ERROR					0x80000000
#define	RTM_RESULT_MAKE_ERROR(x)			(RTM_ERROR | x)
#define	RTM_RESULT_GET_ERROR(x)				(x & (~RTM_ERROR))

/* Constants */

#define	RTM_TRUE							1
#define	RTM_FALSE							0

/* 
 * Plugins can expose custom properies to be edited by users.
 */

#define RTM_PROPERTY_TYPE_END				 0
#define RTM_PROPERTY_TYPE_STRUCT_BEGIN		 1
#define RTM_PROPERTY_TYPE_STRUCT_END		 2
#define RTM_PROPERTY_TYPE_BOOL				 3
#define RTM_PROPERTY_TYPE_BYTE				 4
#define RTM_PROPERTY_TYPE_INT16				 6
#define RTM_PROPERTY_TYPE_INT32				 8
#define RTM_PROPERTY_TYPE_INT64				10
#define RTM_PROPERTY_TYPE_DOUBLE			11
#define RTM_PROPERTY_TYPE_STRING			12
#define RTM_PROPERTY_TYPE_TIME				13
#define RTM_PROPERTY_TYPE_UNSIGNED_MAXK		0x80000000
#define RTM_PROPERTY_TYPE_IS_UNSIGNED(x)	(x & RTM_PROPERTY_TYPE_UNSIGNED_MAXK)

/* Returns version of the plugin */
typedef void* (__stdcall *rtmPluginRealloc)(void* _ptr, size_t _size);

/* 
 * Common plugin function prototypes.
 * These functions are implemented by all plugins.
 */

/* Initializes the plugin. */
typedef uint32_t		(__stdcall *rtmPluginInit)(rtmPluginRealloc _realloc);

/* Initializes the plugin. */
typedef uint32_t		(__stdcall *rtmPluginShutDown)();

/* Returns name of the plugin */
typedef const char*		(__stdcall *rtmPluginGetName)();

/* Returns version of the plugin */
typedef uint32_t		(__stdcall *rtmPluginGetVersion)();

/* Returns name of the plugin */
typedef const char*		(__stdcall *rtmPluginGetAuthor)();

/* Returns type of the plugin */
typedef uint32_t		(__stdcall *rtmPluginGetType)();

/* Returns plugin identifier. Should be an unique 64bit number. */
typedef uint64_t		(__stdcall *rtmPluginGetID)();

/* Returns plugin property value pointer. Returnz 0 for end of list. Size is used to return length of strings. */
typedef void*			(__stdcall *rtmPluginGetProperty)(uint32_t _index, uint32_t* _propertyType);

/* Sets value of a property. */
typedef uint32_t		(__stdcall *rtmPluginSetProperty)(uint32_t _index, void* _propertyData);

/* Creates an instance of the plugin. */
typedef uint32_t		(__stdcall *rtmPluginCreate)(void** _pluginInstance);

/* Destroys an instance of the plugin. */
typedef uint32_t		(__stdcall *rtmPluginDestroy)(void* _pluginInstance);

/* Plugin description and utility functions */
typedef struct _rtmPluginDescriptor
{
	rtmPluginInit			pluginInit;
	rtmPluginShutDown		pluginShutDown;
	rtmPluginGetName		pluginGetName;
	rtmPluginGetVersion		pluginGetVersion;
	rtmPluginGetAuthor		pluginGetAuthor;
	rtmPluginGetType		pluginGetType;
	rtmPluginGetID			pluginGetID;
	rtmPluginGetProperty	pluginGetProperty;
	rtmPluginSetProperty	pluginSetProperty;
	rtmPluginCreate			pluginCreateInstance;
	rtmPluginDestroy		pluginDestroyInstance;

} rtmPluginDescriptor;

/* Every plugin must implement PluginGetDescriptor function */
typedef uint32_t (__stdcall *PluginGetDescriptor)(rtmPluginDescriptor* _plugin);

#endif /* RTM_RBASE_PLUGIN_H */
