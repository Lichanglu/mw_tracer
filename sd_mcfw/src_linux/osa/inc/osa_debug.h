
#ifndef _OSA_DEBUG_H_
#define _OSA_DEBUG_H_

#include <stdio.h>
#include <osa.h>

#include <osa_log.h>
#include <zlog.h>


#define  REACH_HOST_DEBUG_F  0
// printf wrapper that can be turned on and off by defining/undefining
#ifdef OSA_DEBUG_MODE

#if REACH_HOST_DEBUG_F
#define OSA_printf(...)\
	do {\
		if(g_osa_host_log != NULL){\
			zlog_info(g_osa_host_log, "\t [host] " __VA_ARGS__);\
		}\
		else {\
			printf("\n\r [host] " __VA_ARGS__);\
			fflush(stdout);\
		}\
	} while(0)

        
#define OSA_debug(...)\
	do {\
		if(g_osa_host_log != NULL){\
			zlog_debug(g_osa_host_log, "\t [host] " __VA_ARGS__);\
		}\
		else {\
			printf("\n\r [host] " __VA_ARGS__);\
			fflush(stdout);\
		}\
	} while(0)

       
#define OSA_error(...)\
	do {\
		if(g_osa_host_log != NULL){\
			zlog_error(g_osa_host_log, "\t [host] " __VA_ARGS__);\
		}\
		else {\
			printf("\n\r [host] " __VA_ARGS__);\
			fflush(stdout);\
		}\
	} while(0)
#else 
#define OSA_printf(...)	 do {;} while(0)
	
#define OSA_debug(...) 	do {;} while(0)
       
#define OSA_error(...) do {;} while(0)

#endif//end REACH_HOST_DEBUG_F




  
#if 0
 	#define OSA_printf(...)  do { printf("\n\r [host] " __VA_ARGS__); fflush(stdout); } while(0)
#endif

  #define OSA_assert(x)  \
  { \
    if( (x) == 0) { \
      fprintf(stderr, " ASSERT (%s|%s|%d)\r\n", __FILE__, __func__, __LINE__); \
      while (getchar()!='q')  \
        ; \
    } \
  } 

    #define UTILS_assert(x)   OSA_assert(x)
               
#define OSA_DEBUG \
  fprintf(stderr, " %s:%s:%d Press Any key to continue !!!", __FILE__, __func__, __LINE__); 


#define OSA_DEBUG_WAIT \
  OSA_DEBUG \
  getchar();

#define OSA_COMPILETIME_ASSERT(condition)                                       \
                   do {                                                         \
                       typedef char ErrorCheck[((condition) == TRUE) ? 1 : -1]; \
                   } while(0)


#else
  
  #define OSA_printf(...)
  #define OSA_assert(x)
  #define UTILS_assert(x)  
  #define OSA_DEBUG    
  #define OSA_DEBUG_WAIT  
  
  #define OSA_COMPILETIME_ASSERT(condition)                                       \
                   do {                                                         \
                       typedef char ErrorCheck[((condition) == TRUE) ? 1 : -1]; \
                   } while(0)
#endif

// printf wrapper that can be used to display errors. Prefixes all text with
// "ERROR" and inserts the file and line number of where in the code it was
// called
#define OSA_ERROR(...) \
  do \
  { \
  fprintf(stderr, " ERROR  (%s|%s|%d): ", __FILE__, __func__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  } \
  while(0);

#define OSA_assertSuccess(ret)  OSA_assert(ret==OSA_SOK)


#endif  //_OSA_DEBUG_H_



