#include "app_type_mgr.h"

AppTypeMgr::AppTypeMgr() {
	_names.insert(std::make_pair(APP_TYPE::APP_LOGIN, "login"));
	_names.insert(std::make_pair(APP_TYPE::APP_GAMEWORLD, "gameworld"));
	_names.insert(std::make_pair(APP_TYPE::APP_DATABASE, "database"));
	_names.insert(std::make_pair(APP_TYPE::APP_ALL, "all"));
}

std::string AppTypeMgr::GetAppName(const APP_TYPE appType) {
	const auto iter = _names.find(appType);
	if (iter == _names.end())
		return "";

	return iter->second;
}

