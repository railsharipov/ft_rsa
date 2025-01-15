#include <sys/fcntl.h>
#include <unistd.h>
#include <common.h>
#include <rand.h>

int	rand_useed(uint64_t *seed, int nbytes)
{
	int	fd;
	int	rbytes;

	if (nbytes <= 0) {
		RAND_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if ((fd = open(RAND_DEV, O_RDONLY)) < 0) {
		RAND_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	if ((rbytes = read(fd, seed, nbytes)) < 0) {
		RAND_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	if (rbytes != nbytes) {
		RAND_LOG(ERROR, "invalid number of bytes read");
		return (SSL_ERR);
	}

	close(fd);

	return (SSL_OK);
}
