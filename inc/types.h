#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#define true 1
#define false 0

#if defined(__unix__) && defined(__x86_64__)
#define CHAR_BIT 8
#endif

#if defined(__x86_64__)
typedef __uint128_t u128;
typedef __int128_t s128;
#endif

typedef _Bool bool;

typedef uint_least64_t u64;
typedef int_least64_t s64;

typedef unsigned u8;
typedef signed char s8;

#endif // __TYPES_H__
