#include "util/arr.h"
#include "util/file.h"
#include "util/log.h"
#include "util/sort.h"
#include "types.h"

#include <sys/mman.h>
#include <sys/signal.h>

int parse_input(u64 **arr1, u64 **arr2, size_t *arr_size,
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

	*arr1 = mmap(NULL, lines * sizeof(u64), PROT_READ | PROT_WRITE,
		     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*arr2 = mmap(NULL, lines * sizeof(u64), PROT_READ | PROT_WRITE,
		     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (!*arr1 || !*arr2) {
		ret = -1;
		goto free_arr;
	}

	size_t buf_offset = 0;
	for (size_t i = 0; i < lines; ++i) {
		u64 l1 = 0;
		u64 l2 = 0;
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
		munmap(arr1, lines * sizeof(u64));
	if (arr2)
		munmap(arr2, lines * sizeof(u64));
file_cleanup:
	munmap((void *)buf, *arr_size);
	close(fd);
exit:
	return ret;
}

void part1(u64 const *arr1, u64 const *arr2, size_t arr_size)
{
	u64 result = 0;
	for (size_t i = 0; i < arr_size; ++i) {
		result += max(arr1[i], arr2[i]) - min(arr1[i], arr2[i]);
	}

	pr_info("part1 result: %llu", result);
}

u64 simil_partial(u64 left, u64 const *__restrict__ arr2, size_t arr_size)
{
	return left * arr_count(arr2, arr_size, &left, u64_get_idx_ptr, u64_eq);
}

u64 simil(u64 const *__restrict__ arr1, u64 const *__restrict__ arr2,
	  size_t arr_size)
{
	u64 res = 0;
	for (size_t i = 0; i < arr_size; ++i) {
		res += simil_partial(arr1[i], arr2, arr_size);
	}

	return res;
}

void part2(u64 const *arr1, u64 const *arr2, size_t arr_size)
{
	u64 res = simil(arr1, arr2, arr_size);
	pr_info("part2 result: %llu", res);
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

	u64 *arr1, *arr2;
	size_t arr_size;
	if (parse_input(&arr1, &arr2, &arr_size, argv[2]) < 0) {
		pr_err("parsing failed.");
		raise(SIGABRT);
	}

	if (part & 0b01) {
		sort(arr1, arr_size, u64_cmp, u64_xchg);
		sort(arr2, arr_size, u64_cmp, u64_xchg);
		part1(arr1, arr2, arr_size);
	}

	if (part & 0b10) {
		part2(arr1, arr2, arr_size);
	}

	if (arr1)
		munmap(arr1, arr_size * sizeof(u64));
	if (arr2)
		munmap(arr2, arr_size * sizeof(u64));
}
