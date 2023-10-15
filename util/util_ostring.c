#include <ft_ssl.h>

t_ostring	*util_ostr_init(void)
{
	t_ostring	*ostring;

	SSL_ALLOC(ostring, sizeof(t_ostring));

	ostring->content = NULL;
	ostring->size = 0;

	return (ostring);
}

t_ostring	*util_ostr_new(size_t size)
{
	t_ostring	*ostring;

	SSL_ALLOC(ostring, sizeof(t_ostring));
	SSL_ALLOC(ostring->content, size);

	ostring->size = size;

	return (ostring);
}

t_ostring	*util_ostr_dup(t_ostring *src)
{
	t_ostring	*dup;

	if (NULL == src)
		return (NULL);

	SSL_ALLOC(dup, sizeof(t_ostring));
	dup->content = ft_memdup(src->content, src->size);
	dup->size = src->size;

	return (dup);
}

void	util_ostr_del(t_ostring *ostring)
{
	if (NULL == ostring)
		return ;

	SSL_FREE(ostring->content);
	SSL_FREE(ostring);
}

void	util_ostr_clean(t_ostring *ostring)
{
	if (NULL == ostring)
		return ;

	SSL_FREE(ostring->content);
	ostring->content = NULL;
	ostring->size = 0;
}

void	util_ostr_append(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring)
		return ;

	SSL_REALLOC(ostring->content, ostring->size, ostring->size + size);
	ft_memcpy(ostring->content + ostring->size, content, size);
	ostring->size += size;
}

void	util_ostr_prepend(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring)
		return ;

	SSL_REALLOC(ostring->content, ostring->size, ostring->size + size);
	ft_memcpy(ostring->content + size, ostring->content, ostring->size);
	ft_memcpy(ostring->content, content, size);
	ostring->size += size;
}

char 	*util_ostr_to_str(t_ostring *ostring, size_t idx, size_t len)
{
	char	*str;

	if (NULL == ostring)
		return NULL;

	if (idx >= ostring->size)
		return NULL;
	
	if (idx + len + 1 > ostring->size)
		len = ostring->size - idx - 1;

	SSL_ALLOC(str, len);
	ft_memcpy(str, ostring->content + idx, len);
	
	return str;
}

void	util_ostr_set_content(t_ostring *ostring, void *content, size_t size)
{
	if (NULL == ostring)
		return ;

	ostring->content = content;
	ostring->size = size;
}

void	*util_ostr_get_content(t_ostring *ostring)
{
	if (NULL == ostring)
		return NULL;

	return ostring->content;
}

size_t	util_ostr_get_size(t_ostring *ostring)
{
	if (NULL == ostring)
		return 0;

	return ostring->size;
}