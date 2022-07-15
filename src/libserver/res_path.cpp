#include <unistd.h>
#include "common.h"
#include "res_path.h"
#include "util_string.h"
#include "log4_help.h"

ResPath::ResPath() {
    // 获取引擎环境配置
    char* envValue = getenv("GENGINE_RES_PATH");
    _resPath = envValue == nullptr ? "" : envValue;
    // 默认路径
    if (_resPath.empty()) {
        char name[PATH_MAX];
        int rslt = ::readlink("/proc/self/exe", name, PATH_MAX);
        if (rslt < 0 || rslt >= PATH_MAX) {
            LOG_WARN("ResPath::Initialize. res path failed.");
            return;
        }

        name[rslt] = 0;
        _resPath = name;

        for (int i = 0; i < 2; i++) {
            const auto index = _resPath.find_last_of("/");
            _resPath = _resPath.substr(0, index);
        }
    }

    _resPath += "/res";
    LOG_DEBUG("GENGINE_RES_PATH=" << _resPath);

}

std::string ResPath::FindResPath(const std::string& res) {
    return FindResPath(res.c_str());
}

std::string ResPath::FindResPath(const char* res) {
    auto fpath = _resPath + res;

    strutil::replace(fpath, "\\", "/");
    strutil::replace(fpath, "//", "/");

    return fpath;
}
