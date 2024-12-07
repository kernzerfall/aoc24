#include "util/parse.h"

uint64_t parse_u64(char const *__restrict__ buf, size_t *pos, size_t buf_len)
{
	if (!is_num(buf[*pos])) {
		return 0;
	}

	uint64_t res = 0;
	while (*pos < buf_len && is_num(buf[*pos])) {
		res = 10 * res + (buf[*pos] - '0');
		(*pos)++;
	}

	return res;
}
