#include <file.h>
#include <unistd.h>
#include <sys/fcntl.h>

int file_read_all(const char *path, t_ostring *ostring)
{
	char	buf[IO_BUFSIZE];
	ssize_t	nbytes;
	int 	fd;

	if (NULL == path || NULL == ostring) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if ((fd = open(path, O_RDONLY)) < 0) {
		FILE_LOG(ERROR, "failed to open file %s", path);
		return (SSL_ERR);
	}
	ostring->content = NULL;
	ostring->size = 0;

	while ((nbytes = read(fd, buf, IO_BUFSIZE)) > 0) {
		SSL_REALLOC(ostring->content, ostring->size, ostring->size + nbytes);
		ft_memcpy(ostring->content + ostring->size, buf, nbytes);
		ostring->size += nbytes;
	}
	if (close(fd) < 0) {
		FILE_LOG(WARN, "failed to close file %s after read", path);
	}
	if (nbytes < 0) {
		FILE_LOG(ERROR, "failed to read file %s", path);
		return (SSL_ERR);
	}
	FILE_LOG(DEBUG, "read %zd bytes from file %s", ostring->size, path);

	return (SSL_OK);
}