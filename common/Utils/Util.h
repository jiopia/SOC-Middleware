#ifndef VIEW_UTIL_H
#define VIEW_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <thread>
#include <mutex>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <chrono>
#include <sys/time.h>
#include <time.h>

using namespace std;

#define EOK 0
#define IS_DEBUG_ON 1

extern std::string strComponentName;

#define GET_BIT(data, pos) (((data) >> (pos)) & 1)

#define NONE "\033[m\r\n"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

#define DebugPrint(fmt, ...)                                                                                                                                            \
    if (IS_DEBUG_ON)                                                                                                                                                    \
    {                                                                                                                                                                   \
        time_t logTime = time(NULL);                                                                                                                                    \
        struct tm *currTime = localtime(&logTime);                                                                                                                      \
        printf("[DEBUG][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
    }

#define InfoPrint(fmt, ...)                                                                                                                                                     \
    if (IS_DEBUG_ON)                                                                                                                                                            \
    {                                                                                                                                                                           \
        time_t logTime = time(NULL);                                                                                                                                            \
        struct tm *currTime = localtime(&logTime);                                                                                                                              \
        printf("%s[INFO][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, GREEN, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                           \
    }

#define WarnPrint(fmt, ...)                                                                                                                                                         \
    if (IS_DEBUG_ON)                                                                                                                                                                \
    {                                                                                                                                                                               \
        time_t logTime = time(NULL);                                                                                                                                                \
        struct tm *currTime = localtime(&logTime);                                                                                                                                  \
        printf("%s[WARN][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, DARY_GRAY, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                               \
    }

#define ErrPrint(fmt, ...)                                                                                                                                                     \
    if (IS_DEBUG_ON)                                                                                                                                                           \
    {                                                                                                                                                                          \
        time_t logTime = time(NULL);                                                                                                                                           \
        struct tm *currTime = localtime(&logTime);                                                                                                                             \
        printf("%s[ERROR][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, RED, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                          \
    }

#define CriticalPrint(fmt, ...)                                                                                                                                                         \
    if (IS_DEBUG_ON)                                                                                                                                                                    \
    {                                                                                                                                                                                   \
        time_t logTime = time(NULL);                                                                                                                                                    \
        struct tm *currTime = localtime(&logTime);                                                                                                                                      \
        printf("%s[CRITICAL][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, LIGHT_RED, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                                   \
    }

#define BDSTAR_MALLOC(num, type) \
        (type *)malloc(num * sizeof(type)))

#define BDSTAR_FREE(data) \
    {                     \
        if (data != NULL) \
        {                 \
            free(data);   \
            data = NULL;  \
        }                 \
    }

#define GET_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#endif // !VIEW_UTIL_H
