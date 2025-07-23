#include <common.h>
#include <der.h>

t_der *der_init(void)
{
    t_der *der;

    DER_LOG(TRACE, "initializing DER structure");

    SSL_ALLOC(der, sizeof(t_der));
    der->content = NULL;
    der->size = 0;

    DER_LOG(TRACE, "DER structure initialized successfully");

    return (der);
}

void  der_clear(t_der *der)
{
	if (NULL == der) {
		return ;
	}

	DER_LOG(TRACE, "clearing DER structure");

	SSL_FREE(der->content);
	der->content = NULL;
	der->size = 0;

	DER_LOG(TRACE, "DER structure cleared");
}

void  der_del(t_der *der)
{
	if (NULL == der) {
		return ;
	}

	DER_LOG(TRACE, "deleting DER structure");

	SSL_FREE(der->content);
	der->content = NULL;
	der->size = 0;
	SSL_FREE(der);

	DER_LOG(TRACE, "DER structure deleted");
}
