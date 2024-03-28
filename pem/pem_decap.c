#include <ssl/ssl.h>
#include <ssl/pem.h>
#include <util/parser.h>
#include <unistd.h>

int	pem_decap(t_pem *pem, const char *type, char **content, size_t *consize)
{
	char	*preen;
	char	*posten;
	int		presize;
	int		postsize;
	int		bidx;
	int		eidx;

	if (NULL == pem || NULL == content || NULL == consize) {
		return (PEM_ERROR(INVALID_INPUT_ERROR));
	}

	presize = ft_sprintf(&preen, "%s%s%s", "-----BEGIN ", type, "-----");
	postsize = ft_sprintf(&posten, "%s%s%s", "-----END ", type, "-----");

	if (NULL == preen || NULL == posten) {
		return (PEM_ERROR(UNEXPECTED_ERROR));
	}
	*content = NULL;

	bidx = parser_find(pem->content, pem->size, preen, presize);
	eidx = parser_find(pem->content, pem->size, posten, postsize);

	SSL_FREE(preen);
	SSL_FREE(posten);

	if (bidx < 0 || eidx < 0 || bidx > eidx) {
		return (PEM_ERROR("invalid pem encapsulation"));
	}
	*consize = eidx - (bidx + presize);
	*content = ft_memdup(pem->content + (bidx + presize), *consize);

	return (SSL_OK);
}
