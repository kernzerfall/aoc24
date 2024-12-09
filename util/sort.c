#include "util/sort.h"

bool sorted(void const *__restrict__ arr, size_t arr_size, sort_cmp_f cmp)
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

void sort_knowl(void *knowl, void *arr, size_t arr_size, sort_cmp_knowl_f cmp,
		sort_xchg_f xchg)
{
	bool changed = true;
	while (changed) {
		changed = false;
		for (size_t i = 0; i < arr_size - 1; ++i) {
			if (cmp(knowl, arr, i, i + 1) > 0) {
				xchg(arr, i, i + 1);
				changed = true;
			}
		}
	}
}
