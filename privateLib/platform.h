#ifndef PLATFORM_H
#define PLATFORM_H


#define PLATFORM_WINDOWS  0x01   /*window*/
#define PLATFORM_LINUX    0x02   /*linux*/

/* Windows */
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
    #define PLATFORM_VALUE  PLATFORM_WINDOWS
/* Linux */
#else
    #define PLATFORM_VALUE  PLATFORM_LINUX
#endif

#define IS_PLATFORM_WINDOWS  (PLATFORM_VALUE == PLATFORM_WINDOWS)
#define IS_PLATFORM_LINUX    (PLATFORM_VALUE == PLATFORM_LINUX)
#endif
