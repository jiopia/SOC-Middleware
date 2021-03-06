#ifndef VIEW_UTIL_H
#define VIEW_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
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
#include "syslog_diag.h"

using namespace std;

#define EOK 0

extern std::string strComponentName;
extern bool g_debugFlag;

#define GET_BIT(data, pos) (((data) >> (pos)) & 1)

#define NONE "\033[m\n"
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

#define DebugPrint(fmt, ...)                                                                                                                                                      \
    if (g_debugFlag)                                                                                                                                                              \
    {                                                                                                                                                                             \
        time_t logTime = time(NULL);                                                                                                                                              \
        struct tm *currTime = localtime(&logTime);                                                                                                                                \
        printf("[DEBUG][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
    }

#define InfoPrint(fmt, ...)                                                                                                                                                               \
    if (g_debugFlag)                                                                                                                                                                      \
    {                                                                                                                                                                                     \
        time_t logTime = time(NULL);                                                                                                                                                      \
        struct tm *currTime = localtime(&logTime);                                                                                                                                        \
        printf("%s[INFO][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, GREEN, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                                     \
    }

#define WarnPrint(fmt, ...)                                                                                                                                                                   \
    if (g_debugFlag)                                                                                                                                                                          \
    {                                                                                                                                                                                         \
        time_t logTime = time(NULL);                                                                                                                                                          \
        struct tm *currTime = localtime(&logTime);                                                                                                                                            \
        printf("%s[WARN][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, DARY_GRAY, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                                         \
    }

#define ErrPrint(fmt, ...)                                                                                                                                                               \
    if (g_debugFlag)                                                                                                                                                                     \
    {                                                                                                                                                                                    \
        time_t logTime = time(NULL);                                                                                                                                                     \
        struct tm *currTime = localtime(&logTime);                                                                                                                                       \
        printf("%s[ERROR][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, RED, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                                    \
    }

#define CriticalPrint(fmt, ...)                                                                                                                                                                   \
    if (g_debugFlag)                                                                                                                                                                              \
    {                                                                                                                                                                                             \
        time_t logTime = time(NULL);                                                                                                                                                              \
        struct tm *currTime = localtime(&logTime);                                                                                                                                                \
        printf("%s[CRITICAL][%s]:[%02d:%02d:%02d]:[%s]:[%d]" fmt, LIGHT_RED, strComponentName.c_str(), currTime->tm_hour, currTime->tm_min, currTime->tm_sec, __func__, __LINE__, ##__VA_ARGS__); \
        printf(NONE);                                                                                                                                                                             \
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

inline void DebugPrintMsg(const unsigned char *ucMsgData, int iDataLen)
{
    std::string strMsgPrint;
    strMsgPrint += std::string("Message:");
    for (int index = 0; index < iDataLen; index++)
    {
        char byteDataBuff[4] = {0};
        sprintf(byteDataBuff, " [0x%02X]", ucMsgData[index]);
        strMsgPrint += std::string(byteDataBuff);
    }
    DebugPrint("%s", strMsgPrint.c_str());
    printf(NONE);
}

inline void HexToStr(char *dstStr, const unsigned char *srcUch, const int len)
{
    for (int i = 0; i < len; i++)
    {
        sprintf(dstStr, "%02X", srcUch[i]);
        dstStr += 2;
    }
}

inline std::string HexToStr(const unsigned char uchData)
{
    char buff[4] = {0};
    HexToStr(buff, &uchData, 1);
    return std::string(buff);
}

inline std::string UIntToHexStr(unsigned int iSrcValue)
{
    char tmpBuff[32] = {0};
    snprintf(tmpBuff, sizeof(tmpBuff), "%d", iSrcValue);
    std::string strHexValue = tmpBuff;
    if (strHexValue.length() % 2 == 1)
    {
        if (strHexValue.length() == 1)
        {
            strHexValue = std::string("0") +
                          strHexValue.substr(strHexValue.length() - 1);
        }
        else if (strHexValue.length() > 1)
        {
            strHexValue = strHexValue.substr(0, strHexValue.length() - 2) +
                          std::string("0") +
                          strHexValue.substr(strHexValue.length() - 1);
        }
    }

    return strHexValue;
}

inline std::string UCharArrToStr(const unsigned char *chArr, const int len)
{
    std::string strData;
    for (int index = 0; index < len; index++)
    {
        unsigned char chByteTmp = chArr[index];
        strData += HexToStr(chByteTmp);
    }

    return strData;
}

inline int StrToUCharArr(unsigned char *dstUChArr, std::string strSrc)
{
    int iDataLen = 0;
    if (strSrc.length() % 2 != 0)
    {
        return iDataLen;
    }

    for (int index = 0; index < strSrc.length(); index += 2)
    {
        dstUChArr[iDataLen] = strtol(strSrc.substr(index, 2).c_str(), 0, 16);
        iDataLen++;
    }

    return iDataLen;
}

// 41 28 00 00 => 10.5
inline float IEE754_HexToFloat(const unsigned char *ucSrcHex) // ucSrcHex[0] ucSrcHex[1] ucSrcHex[2] ucSrcHex[3]: 00 00 28 41
{
    unsigned long ucSrcValue = ucSrcHex[3] << 24 |
                               ucSrcHex[2] << 16 |
                               ucSrcHex[1] << 8 |
                               ucSrcHex[0];

    int sign = (ucSrcValue & 0x80000000) ? -1 : 1;                    //???????????????
    int iPower = ((ucSrcValue >> 23) & 0xff);                         //???????????????
    int iFinalPower = iPower - 127;                                   //??????????????????????????????????????????????????????30???23????????????e
    float mantissa = 1 + ((float)(ucSrcValue & 0x7fffff) / 0x7fffff); //???22~0?????????10????????????????????????x
    return sign * mantissa * pow(2, iFinalPower);
}

inline std::string FloatToStr(const float fSrc)
{
    std::string strResult;
    char strBuff[32] = {0};
    sprintf(strBuff, "%.4f", fSrc);
    strResult = strBuff;
    return strResult;
}

#endif // !VIEW_UTIL_H
