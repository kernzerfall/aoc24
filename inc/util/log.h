#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define __STR(x) #x
#define STRINGIFY(x) __STR(x)
#define pr_err(fmt, ...)                                                      \
	fprintf(stderr,                                                       \
		__FILE__ ":" STRINGIFY(__LINE__) ": " fmt "\n" __VA_OPT__(, ) \
			__VA_ARGS__)

#define pr_info(fmt, ...)                                                     \
	fprintf(stderr,                                                       \
		__FILE__ ":" STRINGIFY(__LINE__) ": " fmt "\n" __VA_OPT__(, ) \
			__VA_ARGS__)

#endif // __LOG_H__
