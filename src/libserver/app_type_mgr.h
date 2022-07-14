#pragma once

#include <map>
#include "singleton.h"

enum APP_TYPE
{
    // APP_Global �� APP_None ֵ��ͬ�� APP_Global ֻ���ڶ�ȡ����
    APP_None = 0,

    APP_DATABASE = 1,
    APP_LOGIN = 1 << 2,
    APP_GAMEWORLD = 1 << 3,

    APP_ALL =  APP_GAMEWORLD | APP_DATABASE | APP_LOGIN,
};

class AppTypeMgr : public Singleton<AppTypeMgr>
{
public:
    AppTypeMgr();
	std::string GetAppName(APP_TYPE appType);
	
private:
	std::map<APP_TYPE, std::string> _names;
};



