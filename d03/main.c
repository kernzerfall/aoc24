#include "util/file.h"
#include "util/log.h"
#include "util/todo.h"
#include "util/parse.h"
#include "util/sort.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void part1(char const *buf, size_t buf_size)
{
	TODO();
}

void part2(char const *buf, size_t buf_size)
{
	TODO();
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		pr_err("Usage: %s 1|2|b <file>", argv[0]);
		return -1;
	}

	size_t part = argv[1][0] - '0';
	switch (part) {
	case 1:
		break;
	case 2:
		break;
	case 'b' - '0': {
		part = 3;
	}; break;
	default: {
		pr_err("unknown part: %zu", part);
	}; break;
	}

	int fd;
	char *buf;
	off_t buf_size;
	if (mmap_file_ro(&fd, (void **)&buf, &buf_size, argv[2]) < 0) {
		abort();
	}

	if (part & 0b01) {
		part1(buf, buf_size);
	}

	if (part & 0b10) {
		part2(buf, buf_size);
	}

cleanup_file:
	if (buf)
		munmap(buf, buf_size);
	close(fd);
}
