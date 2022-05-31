#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>

static ssize_t __sread_delim(
	t_io *const io, char *buf, size_t nbytes, uint32_t delim)
{
	size_t	bytes;

	bytes = 0;
	delim = ~delim;

	nbytes = MIN(nbytes, MAX(0, io->insize - io->seek));

	while ((bytes < nbytes) && (io->seek < io->insize))
	{
		buf[bytes] = io->input[io->seek++];
		bytes += IS_NONZERO_32(*buf & delim);
	}

	return (bytes);
}

ssize_t	io_sread(t_io *const io, char *const buf, size_t nbytes)
{
	size_t	bytes;
	char	*bufptr;

	if (NULL == buf)
		return (-1);
	else if (nbytes == 0)
		return (0);
	else if (NULL == io->input)
	{
		IO_ERROR(INVALID_INPUT);
		return (-1);
	}
	else if (io->delim)
		return (__sread_delim(io, buf, nbytes, io->delim));

	bufptr = buf;
	bytes = 0;
	nbytes = MIN(nbytes, MAX(0, io->insize - io->seek));

	while (nbytes-- > 0)
	{
		*bufptr++ = io->input[io->seek++];
		bytes++;
	}

	return (bytes);
}
