#include <ft_ssl.h>
#include <ssl_rand.h>

int	rand_useed(uint64_t *seed, int nbytes)
{
	int	fd;
	int	rbytes;

	if (nbytes <= 0)
		return (SSL_ERROR("invalid seed size"));

	if ((fd = open(RAND_DEV, O_RDONLY)) < 0)
		return (SSL_ERROR(NULL));

	if ((rbytes = read(fd, seed, nbytes)) < 0)
		return (SSL_ERROR(NULL));

	if (rbytes != nbytes)
		return (SSL_ERROR("bad read size"));

	close(fd);

	return (SSL_OK);
}
