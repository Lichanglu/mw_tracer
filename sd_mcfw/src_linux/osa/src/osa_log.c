#include <stdio.h>

#include <zlog.h>
#include <osa_log.h>


zlog_category_t *g_osa_app_log = NULL;
zlog_category_t *g_osa_host_log = NULL;
zlog_category_t *g_osa_bios_log = NULL;



zlog_category_t *OSA_logInit(const char *config_file, const char *app_log_cat,
								const char *host_log_cat)
{
	int ret = 0;

	if(config_file == NULL){
		printf("osa zlog config file error!\n");
		return NULL;
	}

	ret = zlog_init(config_file);
	if(ret){
		printf("zlog init failed\n");
		return NULL;
	}

	if(app_log_cat != NULL){
		g_osa_app_log = zlog_get_category(app_log_cat);
		if(g_osa_app_log == NULL){
			ret = -1;
			printf("get g_osa_app_log fail\n");
		}
	}

	if(host_log_cat != NULL){
		g_osa_host_log = zlog_get_category(host_log_cat);
		if(g_osa_host_log == NULL){
			ret = -1;
			printf("get g_osa_host_log fail\n");
		}
	}

	if(ret == -1){
		zlog_fini();
		return NULL;
	}
	
	return g_osa_app_log;
}

int OSA_logDeinit()
{
	zlog_fini();

	return 0;
}


