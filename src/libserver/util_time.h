#pragma once

#include "common.h"
#include "util_string.h"

#include <chrono>

namespace timeutil {
	typedef uint64 Time;  // milliseconds from 1971

	inline Time AddSeconds(Time timeValue, int second) {
		return timeValue + (uint64)second * 1000;
	}

	inline Time AddMilliseconds(Time timeValue, int milliseconds) {
		return timeValue + milliseconds;
	}

	inline std::string NowToString() {
		auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		auto tt = std::chrono::system_clock::to_time_t(timeValue);
		struct tm* ptm = localtime(&tt);
		std::string rs = strutil::format("%d-%02d-%02d %02d:%02d:%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

		return rs;
	}
}
