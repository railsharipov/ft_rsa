#include <common.h>
#include <pem.h>

void	pem_init(t_pem	*pem)
{
	if (NULL == pem) {
		return ;
	}
	ft_bzero(pem, sizeof(t_pem));
}

void	pem_clear(t_ostring *pem)
{
	if (NULL == pem) {
		return ;
	}
	ft_bzero(pem->content, pem->size);
	pem->size = 0;
	SSL_FREE(pem->content);
}

void  pem_del(t_ostring *pem)
{
	if (NULL == pem) {
		return ;
	}
	if (pem->content) {
		ft_bzero(pem->content, pem->size);
		SSL_FREE(pem->content);
	}
	pem->size = 0;
	SSL_FREE(pem);
}

