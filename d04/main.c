#define TRACE
#include "util/sort.h"
#include "util/file.h"
#include "util/log.h"
#include "types.h"

#include <sys/mman.h>
#include <sys/signal.h>

#define MASK_DIM 3
#define MASK_LEN (MASK_DIM * MASK_DIM)

#define mask_sq3(_0, _1, _2) (STRINGIFY(_0) STRINGIFY(_1) STRINGIFY(_2))

static const char xmas[] = "XMAS";

struct vec2 {
	s64 x;
	s64 y;
};

#define _V2_DIR_E ((struct vec2){ .x = 1, .y = 0 })
#define _V2_DIR_NE ((struct vec2){ .x = 1, .y = -1 })
#define _V2_DIR_N ((struct vec2){ .x = 0, .y = -1 })
#define _V2_DIR_NW ((struct vec2){ .x = -1, .y = -1 })
#define _V2_DIR_W ((struct vec2){ .x = -1, .y = 0 })
#define _V2_DIR_SW ((struct vec2){ .x = -1, .y = 1 })
#define _V2_DIR_S ((struct vec2){ .x = 0, .y = 1 })
#define _V2_DIR_SE ((struct vec2){ .x = 1, .y = 1 })

static const struct vec2 dirs[8] = { _V2_DIR_E,	 _V2_DIR_NE, _V2_DIR_N,
				     _V2_DIR_NW, _V2_DIR_W,  _V2_DIR_SW,
				     _V2_DIR_S,	 _V2_DIR_SE };

enum dir2 : u8 {
	DIR_E = 0,
	DIR_NE = 1,
	DIR_N = 2,
	DIR_NW = 3,
	DIR_W = 4,
	DIR_SW = 5,
	DIR_S = 6,
	DIR_SE = 7
};

struct ray {
	struct vec2 pos;
	struct vec2 dir;
};

char p2_masks[][MASK_LEN] = {
	// clang-format off
	mask_sq3(M.S,
	         .A.,
	         M.S),
	mask_sq3(S.S,
	         .A.,
	         M.M),
	mask_sq3(M.M,
	         .A.,
	         S.S),
	mask_sq3(S.M,
	         .A.,
	         S.M),
	// clang-format on
};

const struct vec2 msk_bounds = { .x = MASK_DIM, .y = MASK_DIM };

inline void vec2_add(struct vec2 *v1, const struct vec2 *v2)
{
	v1->x += v2->x;
	v1->y += v2->y;
}

inline bool inside_bounds(const struct vec2 *pos, const struct vec2 *bounds)
{
	return (0 <= pos->x) && (pos->x < bounds->x) && (0 <= pos->y) &&
	       (pos->y < bounds->y);
}

inline int ray_advance(struct ray *ray, const struct vec2 *bounds)
{
	struct vec2 rpos = ray->pos;
	pr_trace("ra (%03lld,%03lld) + (%03lld,%03lld) -> ", rpos.x, rpos.y,
		 ray->dir.x, ray->dir.y);
	vec2_add(&rpos, &ray->dir);
	pr_trace_raw("(%03lld,%03lld)\n", rpos.x, rpos.y);
	if (inside_bounds(&rpos, bounds)) {
		ray->pos = rpos;
		return 0;
	}
	return -1;
}

inline size_t buf_pos_to_offset(const struct vec2 *xy,
				const struct vec2 *bounds)
{
	return bounds->x * xy->y + xy->x;
}

inline void buf_offset_to_pos(struct vec2 *res, size_t offset,
			      const struct vec2 *bounds)
{
	res->x = offset % bounds->x;
	res->y = offset / bounds->x;
}

inline char buf_pos(const char *__restrict__ buf, const struct vec2 *xy,
		    const struct vec2 *bounds)
{
	return buf[buf_pos_to_offset(xy, bounds)];
}

bool xmas_ray(const char *__restrict__ buf, const struct vec2 *origin,
	      const struct vec2 *bounds, enum dir2 dir)
{
	struct ray ray = { .pos = *origin, .dir = dirs[dir] };
	for (size_t i = 0; i < 3; ++i) {
		if (ray_advance(&ray, bounds) < 0 ||
		    buf_pos(buf, &ray.pos, bounds) != xmas[i + 1]) {
			return false;
		}
	}

	return true;
}

inline size_t xmas_ray_scan(const char *__restrict__ buf,
			    const struct vec2 *origin,
			    const struct vec2 *bounds)
{
	size_t res = 0;
	for (size_t i = 0; i < 8; ++i) {
		res += xmas_ray(buf, origin, bounds, i);
	}

	return res;
}

bool xmask_cmp(const char *__restrict__ buf, const struct vec2 *origin,
	       const struct vec2 *bounds, const char msk[MASK_LEN])
{
	// check that we can fit the mask inside the slice of buf first
	struct vec2 low_pos = *origin;
	struct vec2 diff = { .x = msk_bounds.x - 1, .y = msk_bounds.y - 1 };

	vec2_add(&low_pos, &diff);
	if (!inside_bounds(&low_pos, bounds) ||
	    buf_pos(buf, &low_pos, bounds) == '\n') {
		pr_dbg_raw("\tpattern does not fit in buf slice\n");
		return false;
	}

	// move low_pos to lower left position in buf
	low_pos.x = origin->x;
	low_pos.y = origin->y + msk_bounds.y - 1;

	// Rays to check SE from origin
	struct ray br_se = { .pos = *origin, .dir = _V2_DIR_SE };
	struct ray mr_se = { .pos = { 0 }, .dir = _V2_DIR_SE };

	// Rays to NE from origin+(0,2)
	struct ray br_ne = { .pos = low_pos, .dir = _V2_DIR_NE };
	struct ray mr_ne = { .pos = { .x = 0, .y = msk_bounds.y - 1 },
			     .dir = _V2_DIR_NE };

	// Check both NE and SE in one loop
	// Done by checking that the ray pairs (br_ne, mr_ne) and (br_se, mr_se)
	// traverse the same chars
	for (size_t i = 0; i < min(msk_bounds.x, msk_bounds.y); ++i) {
		const char curr_buf_se = buf_pos(buf, &br_se.pos, bounds);
		const char curr_buf_ne = buf_pos(buf, &br_ne.pos, bounds);
		const char curr_msk_se = buf_pos(msk, &mr_se.pos, &msk_bounds);
		const char curr_msk_ne = buf_pos(msk, &mr_ne.pos, &msk_bounds);

		const bool mismatch_se = curr_buf_se != curr_msk_se;
		const bool mismatch_ne = curr_buf_ne != curr_msk_ne;

		pr_dbg_raw(
			"\tSE <%03lld, %03lld> %c - <%03lld, %03lld> %c : %b\n",
			br_se.pos.x, br_se.pos.y, curr_buf_se, mr_se.pos.x,
			mr_se.pos.y, curr_msk_se, mismatch_se);

		pr_dbg_raw(
			"\tNE <%03lld, %03lld> %c - <%03lld, %03lld> %c : %b\n",
			br_ne.pos.x, br_ne.pos.y, curr_buf_ne, mr_ne.pos.x,
			mr_ne.pos.y, curr_msk_ne, mismatch_ne);

		if (mismatch_se || mismatch_ne) {
			return false;
		}

		// advance all rays
		if (ray_advance(&br_se, bounds) < 0) {
			pr_trace_raw("\t\t>> SE bray OOB\n");
		}
		if (ray_advance(&mr_se, &msk_bounds) < 0) {
			pr_trace_raw("\t\t>> SE mray OOB\n");
		}
		if (ray_advance(&br_ne, bounds) < 0) {
			pr_trace_raw("\t\t>> NE bray OOB\n");
		}
		if (ray_advance(&mr_ne, &msk_bounds) < 0) {
			pr_trace_raw("\t\t>> NE mray OOB\n");
		}
	}

	pr_dbg("xmask HIT");
	return true;
}

void part1(char const *buf, size_t buf_size)
{
	struct vec2 bounds = {
		.x = line_length(buf, buf_size),
		.y = count_lines(buf, buf_size),
	};

	pr_dbg_raw("bounds: %03lld - %03lld\n", bounds.x, bounds.y);

	size_t res = 0;
	for (size_t offset = 0; offset < buf_size; ++offset) {
		if (buf[offset] != 'X') {
			continue;
		}

		struct vec2 xy;
		buf_offset_to_pos(&xy, offset, &bounds);

		pr_dbg("found X at offset %03zu, xy = (%03lld, %03lld)", offset,
		       xy.x, xy.y);

		res += xmas_ray_scan(buf, &xy, &bounds);
	}
	pr_info("part1 result: %zu", res);
}

void part2(char const *buf, size_t buf_size)
{
	struct vec2 bounds = {
		.x = line_length(buf, buf_size),
		.y = count_lines(buf, buf_size),
	};

	pr_dbg_raw("bounds: %03lld - %03lld\n", bounds.x, bounds.y);

	size_t res = 0;
	for (size_t offset = 0; offset < buf_size; ++offset) {
		if (buf[offset] != 'M' && buf[offset] != 'S') {
			continue;
		}

		// 'M' or 'S' found:

		struct vec2 xy;
		buf_offset_to_pos(&xy, offset, &bounds);

		pr_dbg("found %c at offset %03zu, xy = (%03lld, %03lld)",
		       buf[offset], offset, xy.x, xy.y);

		if (buf[offset] == 'M') {
			res += xmask_cmp(buf, &xy, &bounds, p2_masks[0]) ||
			       xmask_cmp(buf, &xy, &bounds, p2_masks[2]);
		} else if (buf[offset] == 'S') {
			res += xmask_cmp(buf, &xy, &bounds, p2_masks[1]) ||
			       xmask_cmp(buf, &xy, &bounds, p2_masks[3]);
		}
	}
	pr_info("part2 result: %zu", res);
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
		raise(SIGABRT);
	}

	if (part & 0b01) {
		part1(buf, buf_size);
	}

	if (part & 0b10) {
		part2(buf, buf_size);
	}

	if (buf)
		munmap(buf, buf_size);
	close(fd);
}
