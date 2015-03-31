#include "reach_system_priv.h"
#include "reach_host_priv.h"
#include "git_version.h"
#include "build_info.h"

Int32 host_system_init()
{
	printf("This mcfw is build at [%s],the git version is [%s]\n", g_make_build_date, _VERSION);
	return System_init();
}

Int32 host_system_deinit()
{
	return System_deInit();
}


