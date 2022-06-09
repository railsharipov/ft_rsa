#include <ft_ssl.h>
#include <ssl_der.h>

void	der_append_content(t_der *der, void *content, size_t size)
{
	if (NULL == der || NULL == content)
		return ;

	if (size == 0)
		return ;

	SSL_REALLOC(der->content, der->size, der->size + size);
	ft_memcpy(der->content + der->size, content, size);
	der->size += size;
}
