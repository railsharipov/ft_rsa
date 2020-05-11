#include <ft_ssl.h>
#include <ssl_pem.h>

void  pem_del(t_pem *pem)
{
	if (NULL == pem)
	{
		return ;
	}
	if (pem->content)
	{
		ft_bzero(pem->content, pem->size);
		SSL_FREE(pem->content);
	}
	pem->size = 0;
	SSL_FREE(pem);
}
