#include <ft_ssl.h>
#include <ssl_io.h>

static ssize_t __fread_delim(
	t_io *const io, char *buf, size_t nbytes, uint32_t delim)
{
	ssize_t	bytes;
	size_t	idx;
	char	*tbuf;
	char 	*tbufptr;

	SSL_ALLOC(tbuf, nbytes);
	idx = 0;
	while (idx < nbytes)
	{
		tbufptr = tbuf;
		bytes = read(io->fd, tbuf, nbytes-idx);

		if (bytes <= 0)
			break ;

		io->seek += bytes;

		while (bytes > 0)
		{
			buf[idx] = *tbufptr;
			idx += IS_NONZERO_32(*tbufptr ^ delim);
			tbufptr++;
			bytes--;
		}
	}
	SSL_FREE(tbuf);
	return ((bytes >= 0) ? (idx) : (-1));
}

ssize_t io_fread(t_io *const io, char *buf, size_t nbytes)
{
	ssize_t	bytes;

	if (NULL == buf)
		return (-1);
	else if (nbytes == 0)
		return (0);
	else if (io->delim)
		return (__fread_delim(io, buf, nbytes, io->delim));

	bytes = read(io->fd, buf, nbytes);

	if (bytes > 0)
		io->seek += bytes;

	return (bytes);
}
