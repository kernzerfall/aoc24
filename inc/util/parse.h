#ifndef __PARSE_H__
#define __PARSE_H__

#include "types.h"
#include <unistd.h>

static inline bool is_num(char c)
{
	return c >= '0' && c <= '9';
}

u64 parse_u64(char const *__restrict__ buf, size_t *pos, size_t buf_len);

#endif // __PARSE_H__
