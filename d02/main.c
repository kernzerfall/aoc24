#include "util/file.h"
#include "util/log.h"
#include "util/parse.h"
#include "util/sort.h"
#include "types.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>

enum state : u8 {
	UNKNOWN,
	PARSED_FIRST,
	INFERRED_DECREASING,
	INFERRED_INCREASING,
	UNSAFE,
};

int line_level_safe(char const *buf, size_t *pos, size_t buf_size, off_t ignore)
{
	size_t curr_elem_idx = 0;
	u64 curr = 0;
	u64 last = 0;

	enum state state = UNKNOWN;
	while (*pos < buf_size && buf[*pos] != '\n') {
		last = curr;

		if (ignore == curr_elem_idx) {
			while (*pos < buf_size && is_num(buf[*pos])) {
				(*pos)++;
			}
			goto loop_skip_elem;
		}

		curr = parse_u64(buf, pos, buf_size);
		pr_dbg_raw("%zu\t", curr);
		switch (state) {
		case UNKNOWN: {
			state = PARSED_FIRST;
		}; break;
		case PARSED_FIRST: {
			state = UNSAFE;
			if (last < curr && last + 3 >= curr) {
				state = INFERRED_INCREASING;
			} else if (curr < last && curr + 3 >= last) {
				state = INFERRED_DECREASING;
			}
		}; break;
		case INFERRED_DECREASING: {
			if (last <= curr || curr + 3 < last) {
				state = UNSAFE;
			}
		}; break;
		case INFERRED_INCREASING: {
			if (last >= curr || last + 3 < curr) {
				state = UNSAFE;
			}
		}; break;
		case UNSAFE:
			break;
		}

		// if (state == UNSAFE) {
		// 	break;
		// }

loop_skip_elem:
		curr_elem_idx++;
		while (*pos < buf_size && !is_num(buf[*pos]) &&
		       buf[*pos] != '\n') {
			(*pos)++;
		}
	}

	while (*pos < buf_size && buf[*pos] != '\n') {
		(*pos)++;
	}

	(*pos)++;

	if (state != UNSAFE && state != UNKNOWN) {
		pr_dbg_raw("\x1b[32msafe\x1b[0m\n");
		return 1;
	}

	pr_dbg_raw("\x1b[31munsafe\x1b[0m\n");
	return -curr_elem_idx;
}

void part1(char const *buf, size_t buf_size)
{
	size_t pos = 0;
	size_t safe_levels = 0;

	while (pos < buf_size) {
		safe_levels +=
			(line_level_safe(buf, &pos, buf_size, -1) > 0) ? 1 : 0;
	}

	pr_info("part1 result: %zu", safe_levels);
}

bool bruteforce_relaxed(char const *buf, size_t *pos, size_t buf_size,
			size_t elem_num_unsafe)
{
	const size_t orig_pos = *pos;
	size_t max_pos = *pos;
	for (size_t i = 0; i <= elem_num_unsafe; ++i) {
		if (line_level_safe(buf, pos, buf_size, i) > 0) {
			return true;
		}

		max_pos = max(max_pos, *pos);
		*pos = orig_pos;
	}

	*pos = max_pos;
	return false;
}

void part2(char const *buf, size_t buf_size)
{
	size_t pos = 0;
	size_t safe_levels = 0;

	while (pos < buf_size) {
		size_t pos_saved = pos;
		int safe_val = line_level_safe(buf, &pos, buf_size, -1);
		if (safe_val > 0) {
			safe_levels++;
			continue;
		}

		pos = pos_saved;
		safe_levels +=
			bruteforce_relaxed(buf, &pos, buf_size, -safe_val);
	}

	pr_info("part2 result: %zu", safe_levels);
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

cleanup_file:
	if (buf)
		munmap(buf, buf_size);
	close(fd);
}
