#include <ssl/ssl.h>
#include <ssl/pem.h>
#include <util/parser.h>

static void	__concat(t_pem *pem, char *content, size_t consize)
{
	ft_memcpy(pem->content + pem->size, content, consize);
	pem->size += consize;
}

void	pem_encap(t_pem **pem, const char *type, char *content, size_t consize)
{
	char	*preen;
	char	*posten;
	int		presize;
	int		postsize;

	if (NULL == pem) {
		return ;
	}

	presize = ft_sprintf(&preen, "%s%s%s", "-----BEGIN ", type, "-----");
	postsize = ft_sprintf(&posten, "%s%s%s", "-----END ", type, "-----");

	*pem = pem_init();
	SSL_ALLOC((*pem)->content, consize + presize + postsize + 3);

	__concat(*pem, preen, presize);
	__concat(*pem, "\n", 1);
	__concat(*pem, content, consize);
	__concat(*pem, posten, postsize);
	__concat(*pem, "\n", 1);

	SSL_FREE(preen);
	SSL_FREE(posten);
}