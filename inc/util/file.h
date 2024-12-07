#ifndef __FILE_H__
#define __FILE_H__

#include <unistd.h>

inline static size_t count_lines(char const *__restrict__ buf, const size_t len)
{
	if (len == 0) {
		return 0;
	}

	size_t res = 1;
	for (size_t i = 0; i < len; ++i) {
		if (buf[i] == '\n') {
			res++;
		}
	}

	return res;
}

int mmap_file_ro(int *__restrict__ fd, void **__restrict__ ptr,
		 off_t *__restrict__ fsize, char const *__restrict__ path);

#endif // __FILE_H__
