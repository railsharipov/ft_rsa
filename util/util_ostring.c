#include <ft_ssl.h>

void	util_ostrinit(t_ostring *ostring)
{
	if (NULL == ostring)
	{
		return ;
	}
	ostring->content = NULL;
	ostring->size = 0;
}

t_ostring	*util_ostrnew(void *content, size_t consize)
{
	t_ostring	*ostring;

	SSL_ALLOC(ostring, sizeof(t_ostring));
	ostring->content = content;
	ostring->size = consize;

	return (ostring);
}

t_ostring	*util_ostrdup(t_ostring *src)
{
	t_ostring	*dup;

	if (NULL == src)
	{
		return (NULL);
	}
	SSL_ALLOC(dup, sizeof(t_ostring));
	dup->content = ft_memdup(src->content, src->size);
	dup->size = src->size;

	return (dup);
}

void	util_ostrdel(t_ostring *ostring)
{
	if (NULL == ostring)
	{
		return ;
	}
	SSL_FREE(ostring->content);
	SSL_FREE(ostring);
}
