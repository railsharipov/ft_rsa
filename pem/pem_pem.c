#include <common.h>
#include <pem.h>

void	pem_init(t_pem	*pem)
{
	if (NULL == pem) {
		return ;
	}
	ft_bzero(pem, sizeof(t_pem));
}
