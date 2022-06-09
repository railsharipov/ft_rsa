#include <ft_ssl.h>
#include <ssl_der.h>

t_der *der_init(void)
{
	t_der *der;

	SSL_ALLOC(der, sizeof(t_der));
	return (der);
}
