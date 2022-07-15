#include <iostream>
#include <thread>
#include "console.h"
#include "thread.h"
#include "util_string.h"
#include "console_cmd_pool.h"
#include "log4_help.h"

void ConsoleCmd::OnRegisterHandler(std::string key, HandleConsole handler) {
    _handles[key] = handler;
}

bool ConsoleCmd::CheckParamCnt(std::vector<std::string>& params, const size_t count) {
    if (params.size() == count)
        return true;

    LOG_WARN("input param size is error. see: -help");

    return false;
}

void ConsoleCmd::Dispose() {
    _handles.clear();
}

void ConsoleCmd::Process(std::vector<std::string>& params) {
    if (params.size() <= 1)
        return;

    const std::string key = params[1];
    const auto iter = _handles.find(key);

    if (iter == _handles.end()) {
        LOG_WARN("input error. can't find cmd:" << key.c_str());
        return;
    }

    params.erase(params.begin(), params.begin() + 2);
    iter->second(params);
}

bool Console::Init() {
    _thread = std::thread([this]() {
        char _buffer[ConsoleMaxBuffer];
        do {
            std::cin.getline(_buffer, ConsoleMaxBuffer);

            _lock.lock();
            _commands.push(std::string(_buffer));
            _lock.unlock();

            std::string cmdMsg = _buffer;
            auto iter = cmdMsg.find("exit");
            if (iter != std::string::npos) {                
                _isRun = false;
                Global::GetInstance()->IsStop = true;
            }

        } while (_isRun);
    });

    Register<ConsoleCmdPool>("pool");
    return true;
}

void Console::Update() {
    _lock.lock();
    if (_commands.size() <= 0) {
        _lock.unlock();
        return;
    }

    const std::string cmd = _commands.front();
    _commands.pop();
    _lock.unlock();

    std::vector<std::string> params;
    strutil::split(cmd, ' ', params);
    if (params.size() <= 0)
        return;

    const std::string key = params[0];
    const auto iter = _handles.find(key);

    if (iter == _handles.end()) {
        LOG_DEBUG("input error. cmd:" << cmd.c_str());
        return;
    }

    iter->second->Process(params);
}

void Console::Dispose() {
    for (auto &pair : _handles) {
        pair.second->Dispose();
    }

    _handles.clear();

    if (_isRun)
        _thread.detach();
    else
        _thread.join();

    ThreadObject::Dispose();
}
