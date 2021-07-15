#ifndef __SYSLOG_DIAG_H__
#define __SYSLOG_DIAG_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <syslog.h>

#define LVL_EMERG 0  /* system is unusable */
#define LVL_ALERT 1  /* action must be taken immediately */
#define LVL_CRIT 2   /* critical conditions */
#define LVL_ERR 3    /* error conditions */
#define LVL_WARN 4   /* warning conditions */
#define LVL_NOTICE 5 /* normal but significant condition */
#define LVL_INFO 6   /* informational */
#define LVL_DEBUG 7  /* debug-level messages */

extern char MODULE_TAG[16];

#ifdef DEBUG
#define PRINTF_DEBUG(format, ...) printf(format, ##__VA_ARGS__)
#define DIAG_DEBUG(format, ...) \
    ecarx_syslog(LVL_DEBUG, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#else
#define PRINTF_DEBUG(format, ...)
#define DIAG_DEBUG(format, ...)
#endif

void ecarx_openlog(const char *ident, int option, int facility);
int ecarx_setlogmask(int maskpri);
void ecarx_syslog(unsigned int level, const char *tag, const char *file, const char *func, unsigned int line, const char *format, ...);
void ecarx_closelog();

/* Open the connection between debugging process and syslog process,
 * and configure the syslog property at the same time.
 * If debug process does not call the 'ecarx_openlog',
 * first calling ecarx_syslog would execute ecarx_openlog defaultly.*/
#define DIAG_OPENLOG(ident, option, facility) \
    ecarx_openlog(ident, option, facility)

/* Set syslog recording mask. */
#define DIAG_SETLOGMASK(maskpri) \
    ecarx_setlogmask(maskpri)

/* Close the connection between debugging process and syslog process. */
#define DIAG_CLOSELOG() \
    ecarx_closelog()

#define DIAG_EMERG(format, ...) \
    ecarx_syslog(LVL_EMERG, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define DIAG_ALERT(format, ...) \
    ecarx_syslog(LVL_ALERT, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define DIAG_CRIT(format, ...) \
    ecarx_syslog(LVL_CRIT, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define DIAG_ERR(format, ...) \
    ecarx_syslog(LVL_ERR, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define DIAG_WARN(format, ...) \
    ecarx_syslog(LVL_WARN, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define DIAG_NOTICE(format, ...) \
    ecarx_syslog(LVL_NOTICE, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define DIAG_INFO(format, ...) \
    ecarx_syslog(LVL_INFO, MODULE_TAG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYSLOG_DIAG_H__ */
