#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>

static ssize_t	__swrite_delim(t_iodes *iodes, const char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	char		*obufptr;
	ssize_t		wbytes;
	ssize_t		offset;
	ssize_t		width;
	ssize_t		ix;

	osbuf = iodes->osbuf;

	if (NULL == osbuf)
		return (-1);

	if (iodes->lwidth <= 0)
		iodes->lwidth = 64;

	width = (size_t)iodes->lwidth;
	offset = MIN(nbytes, (iodes->seek % width));
	wbytes = 0;

	SSL_REALLOC(osbuf->content, osbuf->size, osbuf->size + 2*nbytes);
	obufptr = osbuf->content + osbuf->size;

	/* Finish last line if it's not [width] bytes long */
	if (offset)
	{
		for (; wbytes < (width - offset); nbytes--)
			obufptr[wbytes++] = *buf++;
		if (nbytes > 0)
			obufptr[wbytes++] = iodes->delim;
	}

	while (nbytes/width > 0)
	{
		for (ix = 0; ix < width; ix++, nbytes--)
			obufptr[wbytes++] = *buf++;
		obufptr[wbytes++] = iodes->delim;
	}

	while (nbytes-- > 0)
		obufptr[wbytes++] = *buf++;

	osbuf->size += wbytes;
	iodes->seek += wbytes;

	return (wbytes);
}

static ssize_t __swrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	char		*obufptr;
	ssize_t		wbytes;

	osbuf = iodes->osbuf;

	if (NULL == osbuf)
		return (-1);

	SSL_REALLOC(osbuf->content, nbytes, nbytes + osbuf->size);
	obufptr = osbuf->content + osbuf->size;

	wbytes = 0;
	while (wbytes < nbytes)
		obufptr[wbytes++] = *buf++;

	osbuf->size += wbytes;
	iodes->seek += wbytes;

	return (wbytes);
}

ssize_t	io_swrite(t_iodes *iodes, const char *buf, size_t nbytes)
{
	ssize_t	wbytes;

	if (NULL == buf || NULL == iodes)
		return (-1);

	if (nbytes == 0)
		return (0);

	if (iodes->delim)
		wbytes = __swrite_delim(iodes, (char *)buf, nbytes);
	else
		wbytes = __swrite(iodes, (char *)buf, nbytes);

	return (wbytes);
}
