#include <sys/fcntl.h>
#include <unistd.h>
#include <common.h>
#include <rand.h>
#include <io.h>
#include <libft/2darray.h>
#include <libft/htable.h>

int	rand_fseed(uint64_t *seed, const char *files)
{
	char	**farr;
	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	int		fd;
	int		idx;

	if (NULL == files) {
		return (rand_useed(seed, 8));
	}

	farr = ft_strsplit(files, ':');
	rand_mseed(seed, 256);

	idx = 0;
	while ((NULL != farr) && (NULL != farr[idx])) {
		if ((fd = open(farr[idx], O_RDONLY)) < 0) {
			RAND_LOG(ERROR, UNSPECIFIED_ERROR);
			return (SSL_ERR);
		}

		if ((rbytes = read(fd, buf, IO_BUFSIZE)) < 0) {
			RAND_LOG(ERROR, UNSPECIFIED_ERROR);
			return (SSL_ERR);
		}

		while (rbytes > 0) {
			*seed *= ft_hash((unsigned char *)buf, rbytes);

			if ((rbytes = read(fd, buf, IO_BUFSIZE)) < 0) {
				RAND_LOG(ERROR, UNSPECIFIED_ERROR);
				return (SSL_ERR);
			}
		}
		close(fd);
		idx++;
	}
	ft_2darray_del_null_terminated((void **)farr);

	return (SSL_OK);
}
