#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rand.h>
#include <ssl_io.h>

int	rand_fseed(uint64_t *seed, const char *files)
{
	char	**farr;
	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	int		fd;
	int		idx;

	if (NULL == files)
	{
		return (rand_useed(seed, 8));
	}
	farr = ft_strsplit(files, ':');
	rand_mseed(seed, 256);

	idx = 0;
	while ((NULL != farr) && (NULL != farr[idx]))
	{
		if ((fd = open(farr[idx], O_RDONLY)) < 0)
		{
			return (RAND_ERROR(UNSPECIFIED_ERROR));
		}
		if ((rbytes = read(fd, buf, IO_BUFSIZE)) < 0)
		{
			return (RAND_ERROR(UNSPECIFIED_ERROR));
		}
		while (rbytes > 0)
		{
			*seed *= ft_hash(buf, rbytes);
			if ((rbytes = read(fd, buf, IO_BUFSIZE)) < 0)
			{
				return (RAND_ERROR(UNSPECIFIED_ERROR));
			}
		}
		close(fd);
		idx++;
	}
	ft_2darray_del((void **)farr, -1);

	return (SSL_OK);
}
