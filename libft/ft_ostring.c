#include <libft/string.h>
#include <libft/alloc.h>
#include <libft/std.h>

t_ostring	*ft_ostr_create(const char *cstr)
{
	t_ostring	*ostring;

	LIBFT_ALLOC(ostring, sizeof(t_ostring));
	ft_ostr_init(ostring);
	ft_ostr_append_cstr(ostring, cstr);

	return (ostring);
}

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

t_ostring	*ft_ostr_dup(const t_ostring *ostring)
{
	t_ostring	*dup;

	LIBFT_ALLOC(dup, sizeof(t_ostring));
	ft_ostr_init(dup);
	dup->content = ft_memdup(ostring->content, ostring->size);
	dup->size = ostring->size;

	return (dup);
}

void 	ft_ostr_appendf(t_ostring *ostring, const char *fmt, ...)
{
	char	*cstr;
	va_list	ap;

	if (NULL == ostring) {
		return ;
	}
	cstr = NULL;
	va_start(ap, fmt);
	ft_vsprintf(&cstr, fmt, ap);
	va_end(ap);

	ft_ostr_append_cstr(ostring, cstr);
	LIBFT_FREE(cstr);
}

void 	ft_ostr_append_cstr(t_ostring *ostring, const char *cstr)
{
	if (NULL == ostring) {
		return ;
	}
	ft_ostr_append(ostring, (void *)cstr, ft_strlen(cstr));
}

void 	ft_ostr_append_ostr(t_ostring *ostring, const t_ostring *ostr)
{
	if (NULL == ostring || NULL == ostr) {
		return ;
	}
	ft_ostr_append(ostring, (void *)ostr->content, ostr->size);
}

void	ft_ostr_append(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring) {
		return ;
	}

	LIBFT_REALLOC(ostring->content, ostring->size, ostring->size + size);
	if (NULL != content) {
		ft_memcpy(ostring->content + ostring->size, content, size);
	}
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

char 	*ft_ostr_to_cstr(const t_ostring *ostring, size_t idx, size_t len)
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
