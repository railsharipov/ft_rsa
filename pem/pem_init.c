#include <ssl/ssl.h>
#include <ssl/pem.h>

t_pem	*pem_init(void)
{
 	t_pem *pem;

	SSL_ALLOC(pem, sizeof(t_pem));
 	return (pem);
}
