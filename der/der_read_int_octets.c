#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

int  der_read_int_octets(t_iasn *item, unsigned char *derenc, size_t dersize)
{
	return (der_read_ostring_octets(item, derenc, dersize));
}
