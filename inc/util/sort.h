#ifndef __SORT_H__
#define __SORT_H__

#include "types.h"
#include <unistd.h>

#define min(a, b) ((a > b) ? b : a)
#define max(a, b) ((a < b) ? b : a)

typedef int (*sort_cmp_f)(void const *, size_t, size_t);
typedef void (*sort_xchg_f)(void *, size_t, size_t);

bool sorted(void const *__restrict__ arr, size_t arr_size, sort_cmp_f cmp);

void sort(void *arr, size_t arr_size, sort_cmp_f cmp, sort_xchg_f xchg);

#endif // __SORT_H__
