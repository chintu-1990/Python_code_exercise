
#ifndef COMPONENTSINTERFACE_H_
#define COMPONENTSINTERFACE_H_

#include <string>

class BaseAbstraction
{
public:
	//Pure virtual functions 
	virtual eUserErrorCode Initialize() = 0;
	virtual eUserErrorCode UnInitialize() = 0;
	virtual eUserErrorCode GetItemByName(std::string ItemName, std::string &OutputData) = 0;
	virtual eUserErrorCode GetNextItem(std::string &OutputData) = 0;
	//Virtual functions
	virtual ~BaseAbstraction ()
	{
		;
	}
	virtual eUserErrorCode GetNextSubMod(std::string &strSubModName)
	{
		return eFailure;
	}
	virtual eUserErrorCode GetNextTab(std::string &strTabName, std::string &strHeader)
	{
		return eFailure;
	}
};


#endif //COMPONENTSINTERFACE_H_


