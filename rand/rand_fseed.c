#include <ft_ssl.h>
#include <ssl_rand.h>
#include <ssl_io.h>

int	rand_fseed(uint64_t *seed, const char *files)
{
	char	**farr;
	char	buf[RD_BUF];
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
			return (SSL_ERROR(NULL));
		}
		if ((rbytes = read(fd, buf, RD_BUF)) < 0)
		{
			return (SSL_ERROR(NULL));
		}
		while (rbytes > 0)
		{
			*seed *= ft_hash(buf, rbytes);
			if ((rbytes = read(fd, buf, RD_BUF)) < 0)
			{
				return (SSL_ERROR(NULL));
			}
		}
		close(fd);
		idx++;
	}
	ft_2darray_del(farr, -1);

	return (SSL_OK);
}
