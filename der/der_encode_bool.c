#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/der.h>
#include <ssl/asn.h>

int	der_encode_bool(t_ostring *osbuf, void *content, size_t size)
{
	if (NULL == content || size != 1)
		return (DER_ERROR(INVALID_INPUT));

	return (der_encode_ostring(osbuf, content, size));
}
