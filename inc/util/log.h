#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define __pr_esc "\x1b"
#define __pr_red __pr_esc "[31m"
#define __pr_green __pr_esc "[32m"
#define __pr_yellow __pr_esc "[33m"
#define __pr_reset __pr_esc "[0m"

#define _pr_red(x) __pr_red x __pr_reset
#define _pr_green(x) __pr_green x __pr_reset
#define _pr_yellow(x) __pr_yellow x __pr_reset

#ifndef __pr_fmt
#define __pr_fmt(fmt, ...)                                            \
	__FILE__ ":" STRINGIFY(__LINE__) ": " fmt "\n" __VA_OPT__(, ) \
		__VA_ARGS__
#endif

#define __STR(x) #x
#define STRINGIFY(x) __STR(x)
#define pr_err(fmt, ...) \
	fprintf(stderr,  \
		_pr_red(" ERR ") __pr_fmt(fmt) __VA_OPT__(, ) __VA_ARGS__)

#define pr_info(fmt, ...) \
	fprintf(stderr,   \
		_pr_green(" INF ") __pr_fmt(fmt) __VA_OPT__(, ) __VA_ARGS__)

#ifdef TRACE
#ifndef DEBUG
#define DEBUG
#endif
#define pr_trace_raw(...) fprintf(stderr, _pr_yellow(" TRC ") __VA_ARGS__)
#else
#define pr_trace_raw(...)
#endif

#ifdef DEBUG
#define pr_dbg_raw(...) fprintf(stderr, _pr_yellow(" DBG ") __VA_ARGS__)
#else
#define pr_dbg_raw(...)
#endif

#define pr_dbg(fmt, ...) pr_dbg_raw(__pr_fmt(fmt) __VA_OPT__(, ) __VA_ARGS__)

#define pr_trace(fmt, ...) \
	pr_trace_raw(__pr_fmt(fmt) __VA_OPT__(, ) __VA_ARGS__)

#endif // __LOG_H__
