#include <ft_ssl.h>
#include <ssl_der.h>
#include <ssl_asn.h>

int der_append_bool(t_der *der, void *content, int cont_nbytes)
{
  int   id_nbytes, len_nbytes, enc_nbytes;
  char  *precontent;

  if (NULL == der)
    return (SSL_ERROR("invalid input"));

  id_nbytes = 1; // since simple tag expected

	SSL_ALLOC(precontent, (1+sizeof(cont_nbytes))+id_nbytes);

  *precontent = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE;
  *precontent |= ASN_TAG_BOOLEAN;

  len_nbytes = der_append_len(precontent + id_nbytes, cont_nbytes);
  enc_nbytes = id_nbytes + len_nbytes + cont_nbytes;

  SSL_REALLOC(der->content, der->size, der->size + enc_nbytes);
  ft_memcpy(der->content + der->size, precontent, id_nbytes + len_nbytes);
  der->size += id_nbytes + len_nbytes;
  ft_memcpy(der->content + der->size, content, cont_nbytes);
  der->size += cont_nbytes;

  SSL_FREE(precontent);

  return (SSL_OK);
}
