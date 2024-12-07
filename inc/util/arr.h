#ifndef __ARR_H__
#define __ARR_H__

#include "util/sort.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef void *(arr_get_f)(void const *, size_t);
typedef bool (*eq_f)(void const *, void const *);

static inline uint64_t u64_get(void const *arr, size_t idx)
{
	return ((uint64_t *)arr)[idx];
}

static inline void *u64_get_idx_ptr(void const *arr, size_t idx)
{
	return (uint64_t *)arr + idx;
}

static inline void u64_xchg(void *arr, size_t i1, size_t i2)
{
	uint64_t *carr = arr;
	uint64_t t = carr[i1];
	carr[i1] = carr[i2];
	carr[i2] = t;
}

static inline int u64_cmp(void const *arr, size_t i1, size_t i2)
{
	uint64_t a = u64_get(arr, i1);
	uint64_t b = u64_get(arr, i2);

	if (a < b)
		return -1;
	else if (a > b)
		return 1;
	else
		return 0;
}

static inline void u64_print(void const *arr, size_t arr_size)
{
	for (size_t i = 0; i < arr_size; ++i) {
		printf("%zu\n", u64_get(arr, i));
	}
}

static inline bool u64_eq(void const *a, void const *b)
{
	return u64_get(a, 0) == u64_get(b, 0);
}

static inline size_t arr_count(void const *__restrict__ arr, size_t arr_size,
			       void *target, arr_get_f get, eq_f eq)
{
	size_t count = 0;
	for (size_t i = 0; i < arr_size; ++i) {
		if (eq(target, get(arr, i))) {
			count++;
		}
	}

	return count;
}

int seek_to_char(char const *buf, size_t *pos, size_t buf_size, char c);

off_t seek_to_str(char const *buf, size_t *pos, const size_t buf_size,
		  const char *target, const size_t target_size);

off_t find_str(char const *buf, const size_t pos, const size_t buf_size,
	       const char *target, const size_t target_size);

#endif // __ARR_H__
