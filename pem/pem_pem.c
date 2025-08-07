#include <common.h>
#include <pem.h>

t_pem	*pem_create(const char *label, t_pem_proc proc, t_pem_cipher cipher)
{
	t_pem	*pem;

	LIBFT_ALLOC(pem, sizeof(t_pem));
	ft_bzero(pem, sizeof(t_pem));
	pem->label = ft_strdup(label);
	pem->proc = proc;
	pem->cipher = cipher;

	return (pem);
}

void	pem_init(t_pem	*pem)
{
	if (NULL == pem) {
		return ;
	}
	ft_bzero(pem, sizeof(t_pem));
}

void	pem_del(t_pem *pem)
{
	if (NULL == pem) {
		return ;
	}
	SSL_FREE(pem->label);
	SSL_FREE(pem);
}

void 	pem_clear(t_pem *pem)
{
	if (NULL == pem) {
		return ;
	}
	SSL_FREE(pem->label);
	ft_bzero(pem, sizeof(t_pem));
}