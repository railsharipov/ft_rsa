#include <unistd.h>
#include <sys/fcntl.h>
#include <io.h>
#include <file.h>
#include <libft/std.h>
#include <libft/alloc.h>

static ssize_t	__io_file_read(t_file *file, char *buf, size_t nbytes);
static ssize_t	__io_file_write(t_file *file, const char *buf, size_t nbytes);
static int		__io_file_close(t_file *file);

int	file_reader(t_io_stream **stream, t_file *file)
{
	return (file_stream(stream, file, IO_MODE_READ));
}

int	file_writer(t_io_stream **stream, t_file *file)
{
	return (file_stream(stream, file, IO_MODE_WRITE));
}

int	file_stream(t_io_stream **stream, t_file *file, enum e_io_mode mode)
{
	const t_io_interface	io_interface = {
		.read = __io_file_read,
		.write = __io_file_write,
		.close = __io_file_close,
	};

	FILE_LOG(TRACE, "file stream with mode=%d, file=%p", mode, file);

	if (NULL == stream) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(*stream, sizeof(t_io_stream));

	if (NULL == file->path) {
		FILE_LOG(ERROR, "file path is not specified");
		return (SSL_ERR);
	}
	FILE_LOG(TRACE, "opening file '%s'", file->path);
	file->fd = open(file->path, O_TRUNC|O_RDWR|O_CREAT, 0644);

	if (file->fd < 0) {
		FILE_LOG(ERROR, "file open failed: %s", strerror(errno));
		return (SSL_ERR);
	}

	return (io_stream(stream, file, io_interface, mode));
}

static int		__io_file_close(t_file *file)
{
	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (file->fd > 2) {
		FILE_LOG(TRACE, "closing file descriptor %d", file->fd);
		close(file->fd);
		FILE_LOG(TRACE, "file descriptor %d closed", file->fd);
	}
	else {
		FILE_LOG(TRACE, "skipping close for file descriptor: %d", file->fd);
	}
	return (SSL_OK);
}

static ssize_t __io_file_read(t_file *file, char *buf, size_t nbytes)
{
	ssize_t	rbytes;
	int		err;

	FILE_LOG(TRACE, "io file read with file=%p, buf=%p, nbytes=%zu", file, buf, nbytes);

	if (NULL == buf) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (nbytes == 0) {
		FILE_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}

	rbytes = read(file->fd, buf, nbytes);

	if (rbytes < 0) {
		err = errno;
		FILE_LOG(ERROR, "read error: %s (errno=%d)", strerror(err), err);
		return (-1);
	}
	FILE_LOG(TRACE, "read %zd bytes from fd %d", rbytes, file->fd);

	file->seek += rbytes;
	FILE_LOG(TRACE, "updated seek to %zd", file->seek);

	return (rbytes);
}

static ssize_t	__io_file_write(t_file *file, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;
	int		err;

	FILE_LOG(TRACE, "io file write with file=%p, buf=%p, nbytes=%zu", file, buf, nbytes);

	if (NULL == buf) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == file) {
		FILE_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (nbytes == 0) {
		FILE_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}

	wbytes = write(file->fd, buf, nbytes);

	if (wbytes < 0) {
		err = errno;
		FILE_LOG(ERROR, "write error: %s (errno=%d)", strerror(err), err);
		return (-1);
	}
	FILE_LOG(TRACE, "write %zd bytes to fd %d", wbytes, file->fd);

	file->seek += wbytes;
	FILE_LOG(TRACE, "updated seek to %zd", file->seek);

	return (wbytes);
}
