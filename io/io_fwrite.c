#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>

static ssize_t	__fwrite_delim(t_iodes *iodes, const char *buf, size_t nbytes)
{
	char	tbuf[2*nbytes];
	ssize_t	wbytes;
	ssize_t	offset;
	ssize_t	width;
	ssize_t	ix;

	if (iodes->lwidth <= 0)
		iodes->lwidth = 64;

	width = (size_t)iodes->lwidth;
	offset = MIN(nbytes, (iodes->seek % width));
	wbytes = 0;

	/* Finish last line if it's not [width] bytes long */
	if (offset)
	{
		for (; wbytes < (width - offset); nbytes--)
			tbuf[wbytes++] = *buf++;
		if (nbytes > 0)
			tbuf[wbytes++] = iodes->delim;
	}

	while (nbytes/width > 0)
	{
		for (ix = 0; ix < width; ix++, nbytes--)
			tbuf[wbytes++] = *buf++;
		tbuf[wbytes++] = iodes->delim;
	}

	while (nbytes-- > 0)
		tbuf[wbytes++] = *buf++;

	wbytes = write(iodes->fd, tbuf, wbytes);

	if (wbytes > 0)
		iodes->seek += wbytes;

	return (wbytes);
}

static ssize_t __fwrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;

	wbytes = write(iodes->fd, buf, nbytes);

	if (wbytes > 0)
		iodes->seek += wbytes;

	return (wbytes);
}

ssize_t	io_fwrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;

	if (NULL == buf || NULL == iodes)
		return (-1);

	if (nbytes == 0)
		return (0);

	if (iodes->delim)
		wbytes = __fwrite_delim(iodes, (char *)buf, nbytes);
	else
		wbytes = __fwrite(iodes, (char *)buf, nbytes);

	return (wbytes);
}
