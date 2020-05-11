#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int  ASN_TAG       = (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int  ASN_PRIMITIVE = (ASN_ENCODE_PRIMITIVE | ASN_TAG);

int  der_read_bitstring(t_iasn *item, char **derenc, int *dersize)
{
  unsigned char  *octets;
  int            osize;
  int            olen;
  int            shift;

  SSL_CHECK(NULL != item);
  SSL_CHECK((NULL != derenc) && (NULL != *derenc));
  SSL_CHECK(NULL != dersize);

  octets = (unsigned char *)(*derenc);
  osize = *dersize;

  if ((ASN_PRIMITIVE | ASN_TAG_BIT_STRING) != *octets)
    return (SSL_ERROR("bad bit string asn tag"));

  octets++;
  osize--;

  if (SSL_OK != der_read_len(&octets, &osize, &olen))
    return (SSL_ERROR("bad asn len tag"));

  if ((olen > osize) || (olen < 1))
    return (SSL_ERROR("invalid bitstring len"));

  shift = *octets++;
  olen--;

  if (shift > CHAR_BIT-1)
    return (SSL_ERROR("invalid initial bitstring byte"));

	SSL_ALLOC(item->content, olen);
  ft_memcpy(item->content, octets, olen);
  util_rshift_bytes((unsigned char *)item->content, olen, shift);
  item->size = olen;

  *derenc = (char *)(octets) + olen;
  *dersize = osize - olen;

  return (SSL_OK);
}
