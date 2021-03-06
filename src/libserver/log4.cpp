#include "common.h"
#include "log4.h"

#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/logger.h>

#include "util_string.h"
#include "thread_mgr.h"
#include "res_path.h"
#include "app_type_mgr.h"
#include "log4_help.h"

Log4::Log4(int appType) {
	_appType = static_cast<APP_TYPE>(appType);

    auto pResPath = ResPath::GetInstance();
    auto pAppTypeMgr = AppTypeMgr::GetInstance();

    const std::string filename = strutil::format("/log4/log4_%s.properties", pAppTypeMgr->GetAppName(_appType).c_str());
    std::string filePath = pResPath->FindResPath(filename);
    if (filePath.empty())
    {
        std::cout << " !!!!! log4 properties not found! filename:" << filename.c_str() << std::endl;
        return;
    }

    log4cplus::initialize();

    const log4cplus::tstring configFile = LOG4CPLUS_STRING_TO_TSTRING(filePath);

    log4cplus::PropertyConfigurator config(configFile);
    config.configure();

    DebugInfo(log4cplus::Logger::getRoot());
    DebugInfo(log4cplus::Logger::getInstance(LOG4CPLUS_C_STR_TO_TSTRING("GM")));
    LOG_DEBUG("Log4::Initialize is Ok.");
}

Log4::~Log4() {
	log4cplus::deinitialize();
}

void Log4::DebugInfo(log4cplus::Logger logger) const {
	log4cplus::SharedAppenderPtrList appenderList = logger.getAllAppenders();
	auto iter = appenderList.begin();
	while (iter != appenderList.end()) {
		log4cplus::Appender* pAppender = iter->get();
		std::cout << "[log4] " << LOG4CPLUS_TSTRING_TO_STRING(logger.getName()) << " appender name:" << LOG4CPLUS_TSTRING_TO_STRING(pAppender->getName()) << std::endl;
		++iter;
	}
}

/* std::string Log4::GetMsgIdName(const Proto::MsgId msgId) { */
/* 	const google::protobuf::EnumDescriptor *descriptor = Proto::MsgId_descriptor(); */
/* 	return descriptor->FindValueByNumber(msgId)->name(); */
/* } */

