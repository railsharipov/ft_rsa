#include <ft_ssl.h>
#include <ssl_pem.h>

t_pem	*pem_init(void)
{
 	t_pem *pem;

	SSL_ALLOC(pem, sizeof(t_pem));
 	return (pem);
}
