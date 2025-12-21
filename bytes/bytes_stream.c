#include <io.h>
#include <bytes.h>
#include <libft/std.h>
#include <libft/alloc.h>

static ssize_t	__io_bytes_read(t_bytes *bytes, char *buf, size_t nbytes);
static ssize_t	__io_bytes_write(t_bytes *bytes, const char *buf, size_t nbytes);

static ssize_t	__read(t_bytes *bytes, char *buf, size_t nbytes);
static ssize_t	__write(t_bytes *bytes, const char *buf, size_t nbytes);

int	bytes_reader(t_io_v2_stream **stream, t_bytes *bytes)
{
	return (bytes_stream(stream, bytes, IO_V2_MODE_READ));
}

int	bytes_writer(t_io_v2_stream **stream, t_bytes *bytes)
{
	return (bytes_stream(stream, bytes, IO_V2_MODE_WRITE));
}

int	bytes_stream(t_io_v2_stream **stream, t_bytes *bytes, enum e_io_v2_mode mode)
{
	const t_io_v2_interface	io_interface = {
		.read = __io_bytes_read,
		.write = __io_bytes_write,
	};

	BYTES_LOG(TRACE, "bytes stream with mode=%#x, bytes=%p", mode, bytes);

	if (NULL == stream) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	if (NULL == bytes) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	SSL_ALLOC(*stream, sizeof(t_io_v2_stream));

	if (NULL == bytes->ostring) {
		BYTES_LOG(ERROR, "octet string is not specified");
		return (SSL_ERR);
	}
	BYTES_LOG(TRACE, "initializing bytes stream with ostring=%p", bytes->ostring);

	return (io_v2_stream(stream, bytes, io_interface, mode));
}

static ssize_t __io_bytes_read(t_bytes *bytes, char *buf, size_t nbytes)
{
	size_t	rbytes;

	BYTES_LOG(TRACE, "io bytes read with bytes=%p, buf=%p, nbytes=%zu", bytes, buf, nbytes);

	if (NULL == buf) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == bytes) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == bytes->ostring || NULL == bytes->ostring->content) {
		BYTES_LOG(ERROR, "octet string is not set or not initialized");
		return (-1);
	}
	if (nbytes == 0) {
		BYTES_LOG(TRACE, "buffer size is 0, nothing to read");
		return (0);
	}
	nbytes = MIN(nbytes, MAX(0, bytes->ostring->size - bytes->seek));
	BYTES_LOG(TRACE, "reading %zu bytes, ostring size=%zu, seek=%zd", nbytes, bytes->ostring->size, bytes->seek);

	rbytes = 0;
	while (nbytes-- > 0) {
		buf[rbytes++] = bytes->ostring->content[bytes->seek++];
	}
	BYTES_LOG(TRACE, "read %zd bytes", rbytes);

	return (rbytes);
}

static ssize_t	__io_bytes_write(t_bytes *bytes, const char *buf, size_t nbytes)
{
	BYTES_LOG(TRACE, "io bytes write with bytes=%p, buf=%p, nbytes=%zu", bytes, buf, nbytes);

	if (NULL == buf) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == bytes) {
		BYTES_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (NULL == bytes->ostring) {
		BYTES_LOG(ERROR, "octet string is not set");
		return (-1);
	}
	if (nbytes == 0) {
		BYTES_LOG(TRACE, "buffer size is 0, nothing to write");
		return (0);
	}

	BYTES_LOG(TRACE, "writing %zd bytes to ostring, current ostring size=%zu, seek=%zd", nbytes, bytes->ostring->size, bytes->seek);
	ft_ostr_append(bytes->ostring, (char *)buf, nbytes);
	bytes->seek += nbytes;
	BYTES_LOG(TRACE, "wrote %zd bytes, new ostring size=%zu, seek=%zd", nbytes, bytes->ostring->size, bytes->seek);

	return (nbytes);
}
