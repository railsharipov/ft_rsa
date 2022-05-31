#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rand.h>

int	rand_useed(uint64_t *seed, int nbytes)
{
	int	fd;
	int	rbytes;

	if (nbytes <= 0)
		return (SSL_ERROR(INVALID_INPUT));

	if ((fd = open(RAND_DEV, O_RDONLY)) < 0)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if ((rbytes = read(fd, seed, nbytes)) < 0)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (rbytes != nbytes)
		return (SSL_ERROR(INVALID_NUMBER_OF_BYTES_READ));

	close(fd);

	return (SSL_OK);
}
