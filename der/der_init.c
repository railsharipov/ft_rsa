#include <ssl/ssl.h>
#include <ssl/der.h>

t_der *der_init(void)
{
	t_der *der;

	SSL_ALLOC(der, sizeof(t_der));
	der->content = NULL;
	der->size = 0;
	return (der);
}
