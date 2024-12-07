#include "util/sort.h"

bool sorted(void *arr, size_t arr_size,
	    int (*cmp)(void const *, size_t, size_t))
{
	for (size_t i = 0; i < arr_size - 1; ++i) {
		if (cmp(arr, i, i + 1) > 0) {
			return false;
		}
	}
	return true;
}

void sort(void *arr, size_t arr_size, sort_cmp_f cmp, sort_xchg_f xchg)
{
	bool changed = true;
	while (changed) {
		changed = false;
		for (size_t i = 0; i < arr_size - 1; ++i) {
			if (cmp(arr, i, i + 1) > 0) {
				xchg(arr, i, i + 1);
				changed = true;
			}
		}
	}
}
