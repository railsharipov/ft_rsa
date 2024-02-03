#include <sys/fcntl.h>
#include <unistd.h>
#include <ssl/ssl.h>
#include <ssl/rand.h>

int	rand_useed(uint64_t *seed, int nbytes)
{
	int	fd;
	int	rbytes;

	if (nbytes <= 0)
		return (RAND_ERROR(INVALID_INPUT_ERROR));

	if ((fd = open(RAND_DEV, O_RDONLY)) < 0)
		return (RAND_ERROR(UNSPECIFIED_ERROR));

	if ((rbytes = read(fd, seed, nbytes)) < 0)
		return (RAND_ERROR(UNSPECIFIED_ERROR));

	if (rbytes != nbytes)
		return (RAND_ERROR("invalid number of bytes read"));

	close(fd);

	return (SSL_OK);
}
