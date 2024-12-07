#ifndef __TODO_H__
#define __TODO_H__

#include <signal.h>
#include "util/log.h"

#define TODO()                            \
	do {                              \
		pr_err("unimplemented!"); \
		raise(SIGABRT);           \
	} while (0);

#endif // __LOG_H__
