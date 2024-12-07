#ifndef __SORT_H__
#define __SORT_H__

#include <stdbool.h>
#include <unistd.h>

#define min(a, b) ((a > b) ? b : a)
#define max(a, b) ((a < b) ? b : a)

typedef int (*sort_cmp_f)(void const *, size_t, size_t);

bool sorted(void *arr, size_t arr_size,
	    int (*cmp)(void const *, size_t, size_t));

void sort(void *arr, size_t arr_size, sort_cmp_f cmp,
	  void (*xchg)(void *, size_t, size_t));

#endif // __SORT_H__
