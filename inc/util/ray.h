/*
 * Utils for 2D vectors and rays
 */

#ifndef __RAY_H__
#define __RAY_H__

#include "types.h"
#include "util/log.h"

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

static inline void vec2_add(struct vec2 *v1, const struct vec2 *v2)
{
	v1->x += v2->x;
	v1->y += v2->y;
}

static inline bool inside_bounds(const struct vec2 *pos,
				 const struct vec2 *bounds)
{
	return (0 <= pos->x) && (pos->x < bounds->x) && (0 <= pos->y) &&
	       (pos->y < bounds->y);
}

static inline int ray_advance(struct ray *ray, const struct vec2 *bounds)
{
	struct vec2 rpos = ray->pos;
	pr_trace("ra (%03ld,%03ld) + (%03ld,%03ld) -> ", rpos.x, rpos.y,
		 ray->dir.x, ray->dir.y);
	vec2_add(&rpos, &ray->dir);
	pr_trace_raw("(%03ld,%03ld)\n", rpos.x, rpos.y);
	if (inside_bounds(&rpos, bounds)) {
		ray->pos = rpos;
		return 0;
	}
	return -1;
}

// Utils to use 1D buffers as linear 2D buffers

static inline size_t buf_pos_to_offset(const struct vec2 *xy,
				       const struct vec2 *bounds)
{
	return bounds->x * xy->y + xy->x;
}

static inline void buf_offset_to_pos(struct vec2 *res, size_t offset,
				     const struct vec2 *bounds)
{
	res->x = offset % bounds->x;
	res->y = offset / bounds->x;
}

static inline char buf_pos(const char *__restrict__ buf, const struct vec2 *xy,
			   const struct vec2 *bounds)
{
	return buf[buf_pos_to_offset(xy, bounds)];
}

#endif // __RAY_H__
