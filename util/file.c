#include "util/file.h"
#include "util/log.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int mmap_file_ro(int *__restrict__ fd, void **__restrict__ ptr,
		 off_t *__restrict__ fsize, char const *__restrict__ path)
{
	if (!fd || !ptr || !fsize) {
		pr_err("got a null pointer");
	}

	*fd = open(path, O_RDONLY);
	if (fd < 0) {
		pr_err("open failed");
	}

	struct stat fd_stat;
	if (fstat(*fd, &fd_stat) < 0) {
		pr_err("stat failed");
		goto close;
	}

	*fsize = fd_stat.st_size;

	void *m_ptr = mmap(NULL, *fsize, PROT_READ, MAP_SHARED, *fd, 0);
	if (!m_ptr) {
		pr_err("mmap failed");
		goto close;
	}

	*ptr = m_ptr;

	return 0;
close:
	close(*fd);
	return -EPERM;
}
