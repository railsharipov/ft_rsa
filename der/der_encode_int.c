#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/der.h>

int der_encode_int(t_ostring *osbuf, void *content, size_t size)
{
	return (der_encode_ostring(osbuf, content, size));
}
