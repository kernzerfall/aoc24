#include "util/arr.h"
#include "util/file.h"
#include "util/log.h"
#include "util/parse.h"
#include "util/sort.h"
#include "types.h"

#include <sys/mman.h>
#include <sys/signal.h>

#define DONT "don't()"
#define DO "do()"
#define MUL "mul"
#define rsiz(x) (sizeof(x) - 1)

void part1(char const *buf, size_t buf_size)
{
	u64 res = 0;
	size_t pos = 0;

	while (pos < buf_size) {
		if (seek_to_str(buf, &pos, buf_size, MUL, rsiz(MUL)) < 0) {
			break;
		}

		pr_dbg_raw("mul found at %zu\n", pos);

		if (pos + 3 >= buf_size || buf[pos + 3] != '(') {
			pr_dbg_raw("EOF or no opening parens\n");
			pos += 3;
			continue;
		}
		pos = pos + 4;

		size_t after_parens = pos;
		if (seek_to_char(buf, &pos, buf_size, ')') < 0) {
			break;
			pr_dbg_raw("no closing parens\n");
		}

		pr_dbg_raw("closing parens at %zu\n", pos);

		off_t comma = 0;
		for (size_t npos = after_parens; npos < pos; ++npos) {
			if (is_num(buf[npos])) {
				continue;
			}

			if (buf[npos] == ',') {
				if (comma > 0) {
					comma = -1;
					break;
				}
				comma = npos;
				continue;
			}

			comma = -1;
			break;
		}

		if (comma <= 0 || comma == pos - 1) {
			pr_dbg_raw("parens content not OK\n");
			pos = after_parens;
			continue;
		}

		u64 a = parse_u64(buf, &after_parens, buf_size);
		after_parens++;
		u64 b = parse_u64(buf, &after_parens, buf_size);

		res += a * b;
	}

	pr_info("part1 result: %llu", res);
}

void part2(char const *buf, size_t buf_size)
{
	u64 res = 0;
	size_t pos = 0;
	bool do_state = true;

	while (pos < buf_size) {
		size_t pos_bf = pos;
		if (seek_to_str(buf, &pos, buf_size, MUL, rsiz(MUL)) < 0) {
			break;
		}

		off_t yidx = pos_bf;
		off_t nidx = pos_bf;
		do {
			pos_bf = max(nidx, yidx);
			yidx = find_str(buf, pos_bf, pos, DO, rsiz(DO));
			nidx = find_str(buf, pos_bf, pos, DONT, rsiz(DONT));
		} while (yidx >= 0 && nidx >= 0);

		if (nidx >= 0) {
			pr_dbg_raw("using dont at %zu\n", pos_bf);
			do_state = false;
		} else if (yidx >= 0) {
			pr_dbg_raw("using do at %zu\n", pos_bf);
			do_state = true;
		}

		if (do_state == false) {
			pos++;
			continue;
		}

		pr_dbg_raw("mul found at %zu\n", pos);

		if (pos + 3 >= buf_size || buf[pos + 3] != '(') {
			pr_dbg_raw("EOF or no opening parens\n");
			pos += 3;
			continue;
		}
		pos = pos + 4;

		size_t after_parens = pos;
		if (seek_to_char(buf, &pos, buf_size, ')') < 0) {
			break;
			pr_dbg_raw("no closing parens\n");
		}

		pr_dbg_raw("closing parens at %zu\n", pos);

		off_t comma = 0;
		for (size_t npos = after_parens; npos < pos; ++npos) {
			if (is_num(buf[npos])) {
				continue;
			}

			if (buf[npos] == ',') {
				if (comma > 0) {
					comma = -1;
					break;
				}
				comma = npos;
				continue;
			}

			comma = -1;
			break;
		}

		if (comma <= 0 || comma == pos - 1) {
			pr_dbg_raw("parens content not OK\n");
			pos = after_parens;
			continue;
		}

		u64 a = parse_u64(buf, &after_parens, buf_size);
		after_parens++;
		u64 b = parse_u64(buf, &after_parens, buf_size);

		res += a * b;
	}

	pr_info("part2 result: %llu", res);
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
	size_t buf_size;
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
