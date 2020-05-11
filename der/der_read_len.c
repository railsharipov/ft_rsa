#include <ft_ssl.h>
#include <ssl_asn.h>

int	__len_long(unsigned char **octets, int *osize, int *len)
{
  int   len_nbytes;

  len_nbytes = **octets & (~(1<<7));
  *octets = *octets + 1;
  *osize = *osize - 1;

  while ((len_nbytes-- > 0) && (*osize > 0))
  {
    *len <<= 8;
    *len |= **octets;
    *octets = *octets + 1;
    *osize = *osize - 1;
  }

  if (len_nbytes > 0)
    return (SSL_ERR);
  return (SSL_OK);
}

int der_read_len(char **derenc, int *dersize, int *len)
{
  unsigned char *octets;
  int           osize;

  SSL_CHECK((NULL != derenc) && (NULL != *derenc));
  SSL_CHECK(NULL != dersize);
  SSL_CHECK(NULL != len);

  if (*dersize <= 0)
    return (SSL_ERROR("bad der encoding: unexpected bytes stop"));

  octets = (unsigned char *)(*derenc);
  osize = *dersize;
  *len = 0;

  // asn_print_len_tag(octets);

  if (ASN_LEN_SHORT == (*octets & (1<<7)))
  {
    *len = *octets & 0x7F;
    octets++;
    osize--;
  }
  else
  {
    if (SSL_OK != __len_long(&octets, &osize, len))
      return (SSL_ERROR("bad der length bytes: unexpected bytes stop"));
  }

  *derenc = (char *)octets;
  *dersize = osize;

  return (SSL_OK);
}
