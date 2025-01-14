#include <ssl.h>
#include <der.h>

void  der_clean(t_der *der)
{
	if (NULL == der) {
		return ;
	}

	SSL_FREE(der->content);
	der->content = NULL;
	der->size = 0;
}
