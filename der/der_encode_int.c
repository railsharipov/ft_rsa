#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_der.h>

int der_encode_int(t_ostring *osbuf, void *content, size_t size)
{
	return (der_encode_ostring(osbuf, content, size));
}
