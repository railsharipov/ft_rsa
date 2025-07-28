#include <libft/string.h>
#include <libft/alloc.h>
#include <libft/std.h>

void 	ft_ostr_init(t_ostring *ostring)
{
	if (NULL == ostring) {
		return ;
	}

	ostring->content = NULL;
	ostring->size = 0;
}

void 	ft_ostr_init_with_size(t_ostring *ostring, size_t size)
{
	if (NULL == ostring) {
		return ;
	}

	LIBFT_ALLOC(ostring->content, size);
	ostring->size = size;
}

t_ostring	*ft_ostr_dup(t_ostring *src)
{
	t_ostring	*dup;

	if (NULL == src) {
		return (NULL);
	}

	LIBFT_ALLOC(dup, sizeof(t_ostring));
	dup->content = ft_memdup(src->content, src->size);
	dup->size = src->size;

	return (dup);
}

void	ft_ostr_del(t_ostring *ostring)
{
	if (NULL == ostring) {
		return ;
	}

	LIBFT_FREE(ostring->content);
	LIBFT_FREE(ostring);
}

void	ft_ostr_clear(t_ostring *ostring)
{
	if (NULL == ostring) {
		return ;
	}

	LIBFT_FREE(ostring->content);
	ostring->content = NULL;
	ostring->size = 0;
}

void	ft_ostr_append(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring) {
		return ;
	}

	LIBFT_REALLOC(ostring->content, ostring->size, ostring->size + size);
	ft_memcpy(ostring->content + ostring->size, content, size);
	ostring->size += size;
}

void	ft_ostr_prepend(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring) {
		return ;
	}

	LIBFT_REALLOC(ostring->content, ostring->size, ostring->size + size);
	ft_memcpy(ostring->content + size, ostring->content, ostring->size);
	ft_memcpy(ostring->content, content, size);
	ostring->size += size;
}

char 	*ft_ostr_to_cstr(t_ostring *ostring, size_t idx, size_t len)
{
	char	*str;

	if (NULL == ostring) {
		return NULL;
	}

	if (idx >= ostring->size) {
		return NULL;
	}

	len = MIN(len, ostring->size - idx);

	LIBFT_ALLOC(str, len + 1);
	ft_memcpy(str, ostring->content + idx, len);
	str[len] = 0;

	return str;
}
