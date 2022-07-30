#include <ft_ssl.h>
#include <ssl_der.h>

void  der_del(t_der *der)
{
	if (NULL == der)
		return ;

	SSL_FREE(der->content);
	der->content = NULL;
	der->size = 0;
	SSL_FREE(der);
}
