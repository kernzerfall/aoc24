#include "util/file.h"
#include "util/parse.h"
#include "util/sort.h"
#include "util/log.h"
#include "types.h"

#include <sys/mman.h>
#include <sys/signal.h>

#define BIT_ONE ((u128)(1))
#define DEPMAP_SIZE 128
#define DEPMAP_BYTES (DEPMAP_SIZE * sizeof(u128))

#define SCRATCHPAD_BYTES (sizeof(u8) * 512)

// Bitmap function to set n-th bit
inline static void bm_set(u128 *bm, size_t n)
{
	*bm |= (BIT_ONE << n);
}

// Bitmap function to clear n-th bit
inline static void bm_clear(u128 *bm, size_t n)
{
	*bm &= ~(BIT_ONE << n);
}

// Bitmap function to get n-th bit
inline static bool bm_get(u128 const *bm, size_t n)
{
	return !!(*bm & (BIT_ONE << n));
}

static size_t bm_count(u128 const *bm)
{
	return __builtin_popcountll(*bm) + __builtin_popcountll(*bm >> 64);
}

/**
 * Calculates the dependency map from the rules at the start of `buf` and puts
 * it in `depmap`. Advances `pos` to right after the rules.
 */
void build_depmap(u128 **depmap, char const *buf, size_t *pos, size_t buf_size)
{
#ifndef __linux__
	// Linux mmap calls return already zeroed-out memory regions,
	// this may not work on other systems
#pragma message("This part relies on Linux internals! It may not work for you!")
#endif

	// first allocate a map of 128x128 bits
	// all numbers are < 100, so this should work out
	*depmap = mmap(NULL, DEPMAP_BYTES, PROT_READ | PROT_WRITE,
		       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	u128 *dm = *depmap;

	while (*pos + 1 < buf_size && buf[*pos] != '\n') {
		u64 dependency = parse_u64(buf, pos, buf_size);
		(*pos)++;
		u64 dependant = parse_u64(buf, pos, buf_size);
		(*pos)++;
		pr_dbg("dep %lu --> %lu", dependant, dependency);

		bm_set(dm + dependant, dependency);
	}
	(*pos)++;
}

/**
 * Checks whether the list starting at `buf`+`pos` and ending at the
 * first newl afterwards is safe.
 *
 * The elements of the list get saved in the scratchpad, the size in list_size.
 *
 * Depmap contains the dependency map (array of bitmaps) calculated by
 * `build_depmap`.
 */
u64 list_safe(u128 const *depmap, char const *buf, size_t *pos, size_t buf_size,
	      u8 *scratchpad, size_t *list_size)
{
	u128 seen = 0;
	u128 forbidden = 0;
	*list_size = 0;

	bool reject = false;
	while (*pos < buf_size) {
		u64 curr = parse_u64(buf, pos, buf_size);
		scratchpad[*list_size] = curr;
		(*list_size)++;

		if (bm_get(&forbidden, curr)) {
			pr_dbg("list rejected due to forbidden %lu\n", curr);
			// we don't break out here, since we want to have parsed
			// the whole list for part2
			reject = true;
		}

		// Comparison -> where `seen` and the dependencies of `curr`
		//               differ
		u128 comp = seen ^ depmap[curr];

		pr_trace("curr = %lu", curr);
		pr_trace("seen = %016lx%016lx", (u64)(seen >> 64), (u64)(seen));
		pr_trace("curr = %016lx%016lx", (u64)(depmap[curr] >> 64),
			 (u64)(depmap[curr]));
		pr_trace("xor  = %016lx%016lx\n", (u64)(comp >> 64),
			 (u64)(comp));

		// If we see a number that has an unmet dependency, that
		// dependency is from now on forbidden, since seeing it
		// afterwards would break a rule
		if (comp) {
			forbidden |= comp;
		}

		// Note the current number as "seen"
		bm_set(&seen, curr);

		if (buf[*pos] == '\n') {
			break;
		}

		(*pos)++;
	}

	if (reject) {
		return 0;
	}

	// if we're here, the list is accepted -> find middle
	size_t middle = *list_size / 2;
	pr_dbg("list done, middle@%zu -> %d\n", middle, scratchpad[middle]);
	return scratchpad[middle];
}

/* ================================================ *\
 * Define interface functions to use library sorter *
\* ================================================ */

u8 u8_get(void const *ptr, size_t n)
{
	return *((u8 *)ptr + n);
}

void u8_xchg(void *ptr, size_t i, size_t j)
{
	u8 *lptr = ptr;
	u8 t = lptr[i];
	lptr[i] = lptr[j];
	lptr[j] = t;
}

/**
 * Comparison based on the depmap.
 *     i depends on j => j < i,
 *     j depends on i => i < j,
 *     i nodep on j AND j nodep on j => i, j incomparable (poor man's "equal")
 *
 *  `knowl` ("knowledge") is the depmap.
 */
static int depmap_u8_cmp(void const *knowl, void const *ptr, size_t i, size_t j)
{
	const u128 *depmap = knowl;
	u8 a = u8_get(ptr, i);
	u8 b = u8_get(ptr, j);

	if (bm_get(depmap + a, b)) {
		return -1;
	}

	if (bm_get(depmap + b, a)) {
		return 1;
	}

	return 0;
}

/**
 * Reorders the list based on `depmap` and returns its middle element.
 */
u64 reorder_list(const u128 *depmap, u8 *scratchpad, size_t list_size)
{
	sort_knowl((void *)depmap, (void *)scratchpad, list_size, depmap_u8_cmp,
		   u8_xchg);

#if DEBUG
	pr_dbg("reordered list:");
	pr_dbg_raw("\t");
	for (size_t i = 0; i < list_size; ++i) {
		pr_dbg_raw("%d ", scratchpad[i]);
	}
	pr_dbg_raw("\n");
#endif
	size_t middle = list_size / 2;
	pr_dbg("list reorder, middle@%zu -> %d\n", middle, scratchpad[middle]);
	return scratchpad[middle];
}

/**
 * Computes both parts
 */
void parts(char const *buf, size_t buf_size)
{
	u128 *depmap;
	size_t pos = 0;
	build_depmap(&depmap, buf, &pos, buf_size);

	// allocate list_safe's scratchpad ONCE to avoid unneeded syscalls
	u8 *scratchpad = mmap(NULL, SCRATCHPAD_BYTES, PROT_READ | PROT_WRITE,
			      MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	size_t res1 = 0;
	size_t res2 = 0;
	size_t list_size = 0;
	while (pos < buf_size) {
		size_t curr = list_safe(depmap, buf, &pos, buf_size, scratchpad,
					&list_size);
		res1 += curr;

		if (!curr) {
			res2 += reorder_list(depmap, scratchpad, list_size);
		}

		pos++;
	}

	munmap(scratchpad, SCRATCHPAD_BYTES);
	munmap(depmap, DEPMAP_BYTES);

	pr_info("part1 result: %lu", res1);
	pr_info("part2 result: %lu", res2);
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
	case 2: {
		pr_err("on this one, both parts must be run at once.\n");
		raise(SIGABRT);
	}; break;
	case 'b' - '0': {
		part = 3;
	}; break;
	default: {
		pr_err("unknown part: %zu", part);
		raise(SIGABRT);
	}; break;
	}

	int fd;
	char *buf;
	size_t buf_size;
	if (mmap_file_ro(&fd, (void **)&buf, &buf_size, argv[2]) < 0) {
		raise(SIGABRT);
	}

	parts(buf, buf_size);

	if (buf)
		munmap(buf, buf_size);
	close(fd);
}
