#include "log4_help.h"
#include "common.h"

#include <list>

#if LOG_MSG_OPEN

bool IsLogShowMsg(const int msgId) {
	std::list<int> lists;

	//lists.push_back((int)Proto::MI_Ping);
	//lists.push_back((int)Proto::MI_AppInfoSync);
	//lists.push_back((int)Proto::MI_WorldInfoSyncToGather);
	//lists.push_back((int)Proto::MI_WorldProxyInfoSyncToGather);

	const auto iter = std::find(lists.begin(), lists.end(), msgId);
	if (iter != lists.end())
		return false;

	return true;
}

#endif
