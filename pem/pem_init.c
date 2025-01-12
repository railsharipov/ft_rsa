#include <ssl.h>
#include <pem.h>

t_pem	*pem_init(void)
{
 	t_pem *pem;

	SSL_ALLOC(pem, sizeof(t_pem));
 	return (pem);
}
