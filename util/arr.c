#include "util/arr.h"

#include <string.h>

off_t seek_to_str(char const *buf, size_t *pos, const size_t buf_size,
		  const char *target, const size_t target_size)
{
seek_continue:
	while (*pos < buf_size && buf[*pos] != target[0]) {
		(*pos)++;
	}

	if ((*pos >= buf_size) || (*pos + target_size - 1 >= buf_size)) {
		return -1;
	}

	if (strncmp(buf + *pos, target, target_size) != 0) {
		(*pos)++;
		goto seek_continue;
	}

	return (*pos);
}

off_t find_str(char const *buf, const size_t pos, const size_t buf_size,
	       const char *target, const size_t target_size)
{
	size_t npos = pos;
	if (seek_to_str(buf, &npos, buf_size, target, target_size) < 0) {
		return -1;
	}
	return npos;
}

int seek_to_char(char const *buf, size_t *pos, size_t buf_size, char c)
{
	while (*pos < buf_size && buf[*pos] != c) {
		(*pos)++;
	}

	if (*pos >= buf_size) {
		return -1;
	}

	return 0;
}
