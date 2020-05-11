#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int  ASN_TAG       = (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int  ASN_PRIMITIVE = (ASN_ENCODE_PRIMITIVE | ASN_TAG);

int  der_read_int(t_iasn *item, char **derenc, int *dersize)
{
  unsigned char  *octets;
  int            osize;
  int            olen;
  int            sign;
  t_num          *num;

  SSL_CHECK(NULL != item);
  SSL_CHECK((NULL != derenc) && (NULL != *derenc));
  SSL_CHECK(NULL != dersize);

  octets = (unsigned char *)(*derenc);
  osize = *dersize;
  num = item->content;

  if ((ASN_PRIMITIVE | ASN_TAG_INT) != *octets)
    return (SSL_ERROR("bad integer asn tag"));

  octets++;
  osize--;

  if (SSL_OK != der_read_len(&octets, &osize, &olen))
    return (SSL_ERROR("bad asn len tag"));

  if (olen > osize)
    return (SSL_ERROR("invalid asn len"));

  if (olen * CHAR_BIT > BNUM_MAX_DIG * BNUM_DIGIT_BIT)
    return (SSL_ERROR("asn integer len exceeds bnum size limit"));

  if (*octets & (1u<<7))
  {
    sign = BNUM_NEG;
    *octets &= ~(1u<<7);
    if (*octets == 0)
    {
      octets++;
      osize--;
    }
  }
  else
  {
    sign = BNUM_POS;
  }

  bytes_to_num(num, (char *)octets, olen);
  num->sign = sign;

  *derenc = (char *)(octets) + olen;
  *dersize = osize - olen;

  return (SSL_OK);
}
