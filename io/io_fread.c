#include <unistd.h>
#include <util/io.h>

static ssize_t __fread_delim(t_iodes *iodes, char *buf, size_t nbytes)
{
	uint32_t	delim;
	ssize_t		rbytes;
	ssize_t		tbytes;
	char		tbuf[nbytes];
	char 		*tbufptr;

	delim = (uint32_t)iodes->delim;

	tbytes = 0;
	while (tbytes < nbytes)
	{
		tbufptr = tbuf;
		rbytes = read(iodes->fd, tbuf, nbytes-tbytes);

		if (rbytes <= 0)
			break ;

		iodes->seek += rbytes;

		while (rbytes > 0)
		{
			buf[tbytes] = *tbufptr;
			// if character is delimiter, do not increment
			tbytes += (size_t)IS_NONZERO_INT32(*tbufptr ^ delim);
			tbufptr++;
			rbytes--;
		}
	}

	return ((rbytes >= 0) ? (tbytes) : (-1));
}

static ssize_t __fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	ssize_t	rbytes;

	rbytes = read(iodes->fd, buf, nbytes);

	if (rbytes > 0)
		iodes->seek += rbytes;

	return (rbytes);
}

ssize_t io_fread(t_iodes *iodes, char *buf, size_t nbytes)
{
	ssize_t	rbytes;

	if (NULL == buf || NULL == iodes)
		return (-1);

	if (nbytes == 0)
		return (0);

	if (iodes->delim)
		rbytes = __fread_delim(iodes, (char *)buf, nbytes);
	else
		rbytes = __fread(iodes, (char *)buf, nbytes);

	return (rbytes);
}
