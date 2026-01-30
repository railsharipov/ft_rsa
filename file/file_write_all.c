#include <file.h>
#include <unistd.h>
#include <sys/fcntl.h>

int file_write_all(const char *path, t_ostring *ostring)
{
	ssize_t	wbytes;
	int 	fd;

	if (NULL == path || NULL == ostring) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
		SSL_LOG(ERROR, "failed to open file %s", path);
		return (SSL_ERR);
	}
	wbytes = write(fd, ostring->content, ostring->size);

	if (close(fd) < 0) {
		SSL_LOG(WARN, "failed to close file %s after write", path);
	}
	if (wbytes < 0) {
		SSL_LOG(ERROR, "failed to write file %s", path);
		return (SSL_ERR);
	}
	SSL_LOG(DEBUG, "wrote %zd bytes to file %s", ostring->size, path);

	return (SSL_OK);
}