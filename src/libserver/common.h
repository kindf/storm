#pragma once

#include <climits>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdint.h>
#define engine_stricmp strcasecmp
#define engine_access access
#define engine_strncpy(dest, destsz, src, srcsz) strncpy(dest, src, srcsz)

#define SOCKET int
#define INVALID_SOCKET -1

typedef uint64_t	uint64;
typedef uint32_t	uint32;

enum THREAD_OBJECT_TYPE {
    TOT_DEFAULT = 0,
    TOT_CLIENT,
    TOT_WORLD,
    TOT_LOGIN,
    TOT_DATABASE,
    TOT_CONSOLE,
    TOT_TEST,
};
