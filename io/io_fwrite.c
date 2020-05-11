#include <ft_ssl.h>
#include <ssl_io.h>

static ssize_t	__fwrite_delim(
	t_io *const io, char *buf, size_t nbytes, char **s)
{
	int	idx;
	int	idw;
	int	offset;
	int	width;

	if (io->lwidth <= 0)
		io->lwidth = 64;

	SSL_ALLOC((*s), 2*nbytes);
	width = io->lwidth;
	offset = MIN(nbytes, (io->seek % width));
	idw = 0;

	/* Finish last line if it's not [width] bytes long */
	if (offset)
	{
		for (; idw < (width - offset); idw++, nbytes--)
			(*s)[idw] = *buf++;
		if (nbytes > 0)
			(*s)[idw++] = io->delim;
	}
	while (nbytes/width > 0)
	{
		for (idx = 0; idx < width; idx++, nbytes--)
			(*s)[idw++] = *buf++;
		(*s)[idw++] = io->delim;
	}
	while (nbytes-- > 0)
		(*s)[idw++] = *buf++;

	return (idw);
}

ssize_t	io_fwrite(t_io *const io, char *buf, size_t nbytes)
{
	char	*s;
	ssize_t	sbytes;
	ssize_t	wbytes;

	if (NULL == buf)
		return (-1);

	if (nbytes == 0)
		return (0);

	s = NULL;

	if (io->delim)
	{
		sbytes = __fwrite_delim(io, buf, nbytes, &s);
		wbytes = write(io->fd, s, sbytes);
		SSL_FREE(s);
	}
	else
	{
		wbytes = write(io->fd, buf, nbytes);
	}
	io->seek += nbytes;

	if (wbytes < 0)
		SSL_ERROR("write error");

	return (wbytes);
}
