#include "console_cmd_pool.h"
#include "common.h"
#include "log4_help.h"

void ConsoleCmdPool::RegisterHandler() {
	OnRegisterHandler("-help", BindFunP1(this, &ConsoleCmdPool::HandleHelp));
}

void ConsoleCmdPool::HandleHelp(std::vector<std::string>& params) {
	LOG_DEBUG("-show name.\t\tshow connect pool info.");
}

