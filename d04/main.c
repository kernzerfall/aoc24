// #define DEBUG
#include "util/file.h"
#include "util/log.h"
#include "util/todo.h"
#include "types.h"

#include <sys/mman.h>
#include <sys/signal.h>

static const char xmas[] = "XMAS";

struct vec2 {
	off_t x;
	off_t y;
};

#define _V2_DIR_E ((struct vec2){ .x = 1, .y = 0 })
#define _V2_DIR_NE ((struct vec2){ .x = 1, .y = 1 })
#define _V2_DIR_N ((struct vec2){ .x = 0, .y = 1 })
#define _V2_DIR_NW ((struct vec2){ .x = -1, .y = 1 })
#define _V2_DIR_W ((struct vec2){ .x = -1, .y = 0 })
#define _V2_DIR_SW ((struct vec2){ .x = -1, .y = -1 })
#define _V2_DIR_S ((struct vec2){ .x = 0, .y = -1 })
#define _V2_DIR_SE ((struct vec2){ .x = 1, .y = -1 })

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

void vec2_add(struct vec2 *v1, const struct vec2 *v2)
{
	v1->x += v2->x;
	v1->y += v2->y;
}

int ray_advance(struct ray *ray, const struct vec2 *bounds)
{
	struct vec2 rpos = ray->pos;
	vec2_add(&rpos, &ray->dir);
	if ((0 <= rpos.x) && (rpos.x < bounds->x) && (0 <= rpos.y) &&
	    (rpos.y < bounds->y)) {
		ray->pos = rpos;
		return 0;
	}
	return -1;
}

size_t xy_to_offset(const struct vec2 *xy, const struct vec2 *bounds)
{
	return bounds->x * xy->y + xy->x;
}

void offset_to_xy(struct vec2 *res, size_t offset, const struct vec2 *bounds)
{
	res->x = offset % bounds->x;
	res->y = offset / bounds->x;
}

bool xmas_ray(const char *__restrict__ buf, const struct vec2 *origin,
	      const struct vec2 *bounds, enum dir2 dir)
{
	struct ray ray = { .pos = *origin, .dir = dirs[dir] };
	for (size_t i = 0; i < 3; ++i) {
		if (ray_advance(&ray, bounds) < 0 ||
		    buf[xy_to_offset(&ray.pos, bounds)] != xmas[i + 1]) {
			return false;
		}
	}

	return true;
}

size_t xmas_ray_scan(const char *__restrict__ buf, const struct vec2 *origin,
		     const struct vec2 *bounds)
{
	size_t res = 0;
	for (size_t i = 0; i < 8; ++i) {
		res += xmas_ray(buf, origin, bounds, i);
	}

	return res;
}

void part1(char const *buf, size_t buf_size)
{
	struct vec2 bounds = {
		.x = line_length(buf, buf_size),
		.y = count_lines(buf, buf_size),
	};

	pr_dbg_raw("bounds: %03zu - %03zu\n", bounds.x, bounds.y);

	size_t res = 0;
	for (size_t offset = 0; offset < buf_size; ++offset) {
		if (buf[offset] != 'X') {
			continue;
		}

		struct vec2 xy;
		offset_to_xy(&xy, offset, &bounds);

		pr_dbg_raw("found X at offset %03zu, xy = (%03zu, %03zu)\n",
			   offset, xy.x, xy.y);

		res += xmas_ray_scan(buf, &xy, &bounds);
	}
	pr_info("part1 result: %zu", res);
}

void part2(char const *buf, size_t buf_size)
{
	TODO();
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
