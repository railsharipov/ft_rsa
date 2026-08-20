#include <common.h>
#include <logger.h>
#include <der.h>

t_ostring *der_init(void)
{
    t_ostring *der;

    SSL_LOG(TRACE, "initializing DER structure");

    SSL_ALLOC(der, sizeof(t_ostring));
    der->content = NULL;
    der->size = 0;

    SSL_LOG(TRACE, "DER structure initialized successfully");

    return (der);
}

void  der_clear(t_ostring *der)
{
	if (NULL == der) {
		return ;
	}

	SSL_LOG(TRACE, "clearing DER structure");

	SSL_FREE(der->content);
	der->content = NULL;
	der->size = 0;

	SSL_LOG(TRACE, "DER structure cleared");
}

void  der_del(t_ostring *der)
{
	if (NULL == der) {
		return ;
	}

	SSL_LOG(TRACE, "deleting DER structure");

	SSL_FREE(der->content);
	der->content = NULL;
	der->size = 0;
	SSL_FREE(der);

	SSL_LOG(TRACE, "DER structure deleted");
}
