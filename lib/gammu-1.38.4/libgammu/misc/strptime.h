#ifndef __helper__strptime_h__
#define __helper__strptime_h__

#include <gammu-config.h>

#ifdef HAVE_STRPTIME
#define _XOPEN_SOURCE
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <time.h>
#else
#include <time.h>
char * strptime(const char *buf, const char *fmt, struct tm *tm);
#endif

#endif
