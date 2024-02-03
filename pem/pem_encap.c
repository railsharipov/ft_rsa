#include <ssl/ssl.h>
#include <ssl/pem.h>
#include <util/parser.h>

static char	*__preen;
static char	*__posten;
static int	__presize;
static int	__postsize;

static void	__pre_encap(const char *type)
{
	__presize = ft_sprintf(&__preen, "%s%s%s", "-----BEGIN ", type, "-----");
}

static void	__post_encap(const char *type)
{
	__postsize = ft_sprintf(&__posten, "%s%s%s", "-----END ", type, "-----");
}

static void	__concat(t_pem *pem, char *content, size_t consize)
{
	ft_memcpy(pem->content + pem->size, content, consize);
	pem->size += consize;
}

void	pem_encap(t_pem **pem, const char *type, char *content, size_t consize)
{
	if (NULL == pem) {
		return ;
	}
	__pre_encap(type);
	__post_encap(type);

	*pem = pem_init();
	SSL_ALLOC((*pem)->content, consize + __presize + __postsize + 3);

	__concat(*pem, __preen, __presize);
	__concat(*pem, "\n", 1);
	__concat(*pem, content, consize);
	__concat(*pem, __posten, __postsize);
	__concat(*pem, "\n", 1);

	if (NULL != __preen)
		free(__preen);
	if (NULL != __posten)
		free(__posten);
}

int	pem_remove_encap(
	t_pem *pem, const char *type, char **content, size_t *consize)
{
	int		bidx;
	int		eidx;

	if (NULL == pem || NULL == content || NULL == consize) {
		return (PEM_ERROR(INVALID_INPUT_ERROR));
	}
	__pre_encap(type);
	__post_encap(type);
	*content = NULL;

	bidx = parser_find(pem->content, pem->size, __preen, __presize);
	eidx = parser_find(pem->content, pem->size, __posten, __postsize);

	if (NULL != __preen) {
		free(__preen);
	}
	if (NULL != __posten) {
		free(__posten);
	}
	if ((bidx < 0) || (eidx < 0) || (bidx > eidx)) {
		return (PEM_ERROR("invalid pem encapsulation"));
	}
	*consize = eidx - (bidx + __presize);
	*content = ft_memdup(pem->content + (bidx + __presize), *consize);

	return (SSL_OK);
}
