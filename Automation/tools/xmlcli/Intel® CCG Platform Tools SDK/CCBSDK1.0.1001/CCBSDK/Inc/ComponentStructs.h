
#ifndef COMPONENTSTRUCTS_H_
#define COMPONENTSTRUCTS_H_

#include <string>
#include <vector>

#define COMMAND_NAME_STR_LEN     100
#define COMMAND_DESCR_STR_LEN    512
#define FILE_PATH_STR_LEN        512


#define STRINGS_ARE_EQUAL          0

#define TERMINATE_NULL_CHAR_LEN    1

#define SST_MAX_STR_LEN          512
#define MAX_INT_VALUE_STR_LEN     15

#define STR_GROUP_NAME_TAG       "#GROUPNAME"

#define STR_ITEM_START_TAG       "\""
#define STR_ITEM_SEPERATOR_TAG   "\", \""
#define STR_ITEM_END_TAG         "\""

//GetArchitecture Specific
#define CPUID_MASK 0x000F0FFF
#define CPUID_MASK_STEPPING 0x000F0FF0


typedef enum
{
	UI_XML_FILE_TYPE = 0,
	UI_XML_MEMORY_TYPE,
	LOG_XML_FILE_TYPE,
	LOG_XML_MEMORY_TYPE,
	LOG_HTML_FILE_TYPE,
	LOG_HTML_MEMORY_TYPE,
}DATATYPE;

typedef struct
{
	std::vector<std::string> lsCommands;
	std::string Description;
}CAPABS, *TCAPABS;

typedef struct
{
	std::vector<TCAPABS> lsCapabs;
	UInt32 GroupID;
}GROUPCAPABS, *TGROUPCAPABS;

typedef struct
{
	UInt32 length;
	Int8* buff;
}DATA, *TDATA;

typedef struct
{
	Int8* Name;
	UInt32 Index;
	bool isSupported;
}LIBDETAILS, *TLIBDETAILS;

typedef struct
{
	std::string strTabName;
	std::string strHeader;
	std::vector<std::string> lsRowData;
}TABDATA, *TTABDATA;

typedef struct
{
	std::string strSubModName;
	std::vector<TTABDATA> lspTabData;
}SUBMODDATA, *TSUBMODDATA;

typedef struct
{
	bool PluginEnabled;
	std::vector<bool> lsModuleEnabled;
}PLUGINMODLIST, *TPLUGINMODLIST;

#endif //COMPONENTSTRUCTS_H_

