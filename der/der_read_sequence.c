#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int  ASN_TAG       = (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int  ASN_CONSTRUCT = (ASN_ENCODE_CONSTRUCT | ASN_TAG);

int  der_read_sequence(t_iasn *item, char **derenc, size_t *dersize)
{
  unsigned char  *octets;
  size_t         osize;
  size_t         olen;

  SSL_CHECK(NULL != item);
  SSL_CHECK((NULL != derenc) && (NULL != *derenc));
  SSL_CHECK(NULL != dersize);

  octets = (unsigned char *)(*derenc);
  osize = *dersize;

  if ((ASN_CONSTRUCT | ASN_TAG_SEQUENCE) != *octets)
    return (DER_ERROR(INVALID_ASN_TYPE_TAG));

  octets++;
  osize--;

  if (SSL_OK != der_read_len(&octets, &osize, &olen))
    return (DER_ERROR(INVALID_ASN_LEN_TAG));

  if (olen > osize)
    return (DER_ERROR(INVALID_ASN_LEN_TAG));

  SSL_ALLOC(item->content, olen);
  ft_memcpy(item->content, octets, olen);
  item->__size = olen;

  *derenc = (char *)(octets) + olen;
  *dersize = osize - olen;

  return (SSL_OK);
}
