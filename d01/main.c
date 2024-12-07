#include "util/arr.h"
#include "util/file.h"
#include "util/log.h"
#include "util/sort.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

int parse_input(uint64_t **arr1, uint64_t **arr2, off_t *arr_size,
		char const *__restrict__ path)
{
	int ret = 0;
	int fd;
	char const *buf;
	if (mmap_file_ro(&fd, (void **)&buf, arr_size, path) < 0) {
		pr_err("mmap failed");
		ret = -1;
		goto exit;
	}

	size_t lines = count_lines(buf, *arr_size);

	*arr1 = malloc(lines * sizeof(uint64_t));
	*arr2 = malloc(lines * sizeof(uint64_t));

	if (!*arr1 || !*arr2) {
		ret = -1;
		goto free_arr;
	}

	off_t buf_offset = 0;
	for (size_t i = 0; i < lines; ++i) {
		uint64_t l1 = 0;
		uint64_t l2 = 0;
		while (buf_offset < *arr_size && buf[buf_offset] != ' ') {
			int n = buf[buf_offset] - '0';
			l1 = 10 * l1 + n;
			buf_offset++;
		}

		buf_offset += 3;

		while (buf_offset < *arr_size && buf[buf_offset] != '\n') {
			int n = buf[buf_offset] - '0';
			l2 = 10 * l2 + n;
			buf_offset++;
		}

		buf_offset += 1;

		(*arr1)[i] = l1;
		(*arr2)[i] = l2;
	}

	ret = 0;
	*arr_size = lines;
	goto file_cleanup;

free_arr:
	if (arr1)
		free(arr1);
	if (arr2)
		free(arr2);
file_cleanup:
	munmap((void *)buf, *arr_size);
	close(fd);
exit:
	return ret;
}

void part1(uint64_t const *arr1, uint64_t const *arr2, size_t arr_size)
{
	uint64_t result = 0;
	for (size_t i = 0; i < arr_size; ++i) {
		result += max(arr1[i], arr2[i]) - min(arr1[i], arr2[i]);
	}

	pr_info("part1 result: %zu", result);
}

uint64_t simil_partial(uint64_t left, uint64_t const *__restrict__ arr2,
		       size_t arr_size)
{
	return left * arr_count(arr2, arr_size, &left, u64_get_idx_ptr, u64_eq);
}

uint64_t simil(uint64_t const *__restrict__ arr1,
	       uint64_t const *__restrict__ arr2, size_t arr_size)
{
	uint64_t res = 0;
	for (size_t i = 0; i < arr_size; ++i) {
		res += simil_partial(arr1[i], arr2, arr_size);
	}

	return res;
}

void part2(uint64_t const *arr1, uint64_t const *arr2, size_t arr_size)
{
	uint64_t res = simil(arr1, arr2, arr_size);
	pr_info("part2 result: %zu", res);
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		pr_err("Usage: %s 0|1|b <file>", argv[0]);
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

	uint64_t *arr1, *arr2;
	off_t arr_size;
	if (parse_input(&arr1, &arr2, &arr_size, argv[2]) < 0) {
		pr_err("parsing failed.");
		abort();
	}

	if (part & 0b01) {
		sort(arr1, arr_size, u64_cmp, u64_xchg);
		sort(arr2, arr_size, u64_cmp, u64_xchg);
		part1(arr1, arr2, arr_size);
	}

	if (part & 0b10) {
		part2(arr1, arr2, arr_size);
	}
}
