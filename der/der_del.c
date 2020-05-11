#include <ft_ssl.h>
#include <ssl_der.h>

void  der_del(t_der *der)
{
  if (NULL == der)
    return ;
  ft_bzero(der->content, der->size);
  der->size = 0;
  SSL_FREE(der->content);
  SSL_FREE(der);
}
