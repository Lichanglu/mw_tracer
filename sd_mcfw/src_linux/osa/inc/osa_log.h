#ifndef _OSA_LOG_H_
#define _OSA_LOG_H_

#include <zlog.h>

extern zlog_category_t *g_osa_host_log;


zlog_category_t *OSA_logInit(const char *config_file, const char *app_log_cat,
								const char *host_log_cat);

int OSA_logDeinit();



#endif

