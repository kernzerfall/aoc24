#ifndef __FILE_H__
#define __FILE_H__

#include <unistd.h>

static inline size_t count_lines(char const *__restrict__ buf,
				 const size_t buf_size)
{
	if (buf_size == 0) {
		return 0;
	}

	size_t res = 1;
	for (size_t i = 0; i < buf_size; ++i) {
		if (buf[i] == '\n') {
			res++;
		}
	}

	// ignore final newline
	if (buf[buf_size - 1] == '\n') {
		res--;
	}

	return res;
}

static inline size_t line_length(char const *__restrict__ buf,
				 const size_t buf_size)
{
	if (buf_size == 0) {
		return 0;
	}

	size_t len = 0;
	while (len < buf_size && buf[len] != '\n') {
		len++;
	}

	return len + 1;
}

int mmap_file_ro(int *__restrict__ fd, void **__restrict__ ptr,
		 off_t *__restrict__ fsize, char const *__restrict__ path);

#endif // __FILE_H__
