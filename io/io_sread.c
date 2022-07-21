#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>

static ssize_t __sread_delim(t_iodes *iodes, char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	size_t		rbytes;
	uint32_t	delim;

	osbuf = iodes->osbuf;

	if (NULL == osbuf || NULL == osbuf->content)
		return (-1);

	nbytes = MIN(nbytes, MAX(0, osbuf->size - iodes->seek));
	delim = (uint32_t)iodes->delim;
	rbytes = 0;

	while ((rbytes < nbytes) && (iodes->seek < osbuf->size))
	{
		buf[rbytes] = osbuf->content[iodes->seek++];
		rbytes += IS_NONZERO_32(*buf ^ delim);
	}

	return (rbytes);
}

static ssize_t __sread(t_iodes *iodes, char *buf, size_t nbytes)
{
	t_ostring	*osbuf;
	size_t		rbytes;

	osbuf = iodes->osbuf;

	if (NULL == osbuf || NULL == osbuf->content)
		return (-1);

	nbytes = MIN(nbytes, MAX(0, osbuf->size - iodes->seek));
	rbytes = 0;

	while (nbytes-- > 0)
		buf[rbytes++] = osbuf->content[iodes->seek++];

	return (rbytes);
}

ssize_t	io_sread(t_iodes *iodes, char *buf, size_t nbytes)
{
	size_t	rbytes;

	if (NULL == iodes || NULL == buf)
		return (-1);

	if (nbytes == 0)
		return (0);

	if (iodes->delim)
		rbytes = __sread_delim(iodes, buf, nbytes);
	else
		rbytes = __sread(iodes, buf, nbytes);

	return (rbytes);
}
