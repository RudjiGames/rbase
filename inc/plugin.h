/*--------------------------------------------------------------------------
  	Copyright (c) 2019 Milos Tosic. All Rights Reserved.
	License: http://www.opensource.org/licenses/BSD-2-Clause
--------------------------------------------------------------------------*/

#ifndef RTM_RBASE_PLUGIN_H
#define RTM_RBASE_PLUGIN_H

/* SDK version  */

#define	RTM_PLUGIN_VERSION_HIGH				1
#define	RTM_PLUGIN_VERSION_LOW				0
#define RTM_PLUGIN_VERSION					((RTM_PLUGIN_VERSION_HIGH << 16) | RTM_PLUGIN_VERSION_LOW)
#define RTM_PLUGIN_MAKE_VERSION(_h, _l)		((_h << 16) | _l)
#define RTM_PLUGIN_GET_VERSION_LOW(_v)		(_v & 0x0000ffff)
#define RTM_PLUGIN_GET_VERSION_HIGH(_v)		((_v >> 16) & 0x0000ffff)

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
#define RTM_PROPERTY_TYPE_UNSIGNED_MASK		0x80000000
#define RTM_PROPERTY_TYPE_MAKE_UNSIGNED(_x)	(_x | RTM_PROPERTY_TYPE_UNSIGNED_MASK)
#define RTM_PROPERTY_TYPE_IS_UNSIGNED(_x)	(_x & RTM_PROPERTY_TYPE_UNSIGNED_MASK)

/* Returns version of the plugin */
typedef void* (*rtmPluginRealloc)(void* _ptr, size_t _size);

/* 
 * Common plugin function prototypes.
 * These functions are implemented by all plugins.
 */

/* Initializes the plugin. */
typedef uint32_t		(*rtmPluginInit)(rtmPluginRealloc _realloc);

/* Initializes the plugin. */
typedef uint32_t		(*rtmPluginShutDown)();

/* Returns name of the plugin */
typedef const char*		(*rtmPluginGetName)();

/* Returns version of the plugin */
typedef uint32_t		(*rtmPluginGetVersion)();

/* Returns name of the plugin */
typedef const char*		(*rtmPluginGetAuthor)();

/* Returns type of the plugin */
typedef uint32_t		(*rtmPluginGetType)();

/* Returns plugin identifier. Should be an unique 64bit number. */
typedef uint64_t		(*rtmPluginGetID)();

/* Returns plugin property value pointer. Returnz 0 for end of list. Size is used to return length of strings. */
typedef void*			(*rtmPluginGetProperty)(uint32_t _index, uint32_t* _propertyType, const char** _name);

/* Sets value of a property. */
typedef uint32_t		(*rtmPluginSetProperty)(uint32_t _index, void* _propertyData);

/* Creates an instance of the plugin. */
typedef uint32_t		(*rtmPluginCreate)(void** _pluginInstance, void* _userData);

/* Destroys an instance of the plugin. */
typedef uint32_t		(*rtmPluginDestroy)(void* _pluginInstance);

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
typedef uint32_t (*PluginGetDescriptor)(rtmPluginDescriptor* _plugin);

#endif /* RTM_RBASE_PLUGIN_H */
