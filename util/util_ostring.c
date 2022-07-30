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
