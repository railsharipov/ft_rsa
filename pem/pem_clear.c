#include <ssl/ssl.h>
#include <ssl/pem.h>

void	pem_clear(t_pem *pem)
{
	if (NULL == pem)
	{
		return ;
	}
	ft_bzero(pem->content, pem->size);
	pem->size = 0;
	SSL_FREE(pem->content);
}
