#include <ft_ssl.h>
#include <ssl_io.h>
#include <ssl_error.h>

int	io_set_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf)
{
	if (NULL == iodes || NULL == osbuf)
		return (IO_ERROR(INVALID_INPUT));

	ft_bzero(iodes, sizeof(t_iodes));

	if (!SSL_FLAG(IO_OSBUF, flags))
		return (IO_ERROR(INVALID_INPUT));

	if (!(SSL_FLAG(IO_READ, flags) || SSL_FLAG(IO_WRITE, flags)))
		return (IO_ERROR(INVALID_INPUT));

	if (SSL_FLAG(IO_READ, flags))
		if (NULL == osbuf->content)
			return (IO_ERROR(INVALID_INPUT));

	iodes->osbuf = osbuf;
	iodes->mode = IO_MODE_OSBUF;

	return (SSL_OK);
}
