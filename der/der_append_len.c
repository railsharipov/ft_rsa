#include <ft_ssl.h>
#include <ssl_asn.h>

int  der_append_len(char *encoding, int len)
{
  int len_nbits;
  int len_nbytes;
  int idx;

  len_nbits = util_lmbit(len, 8*sizeof(len));
  len_nbits = CEIL(len_nbits, 8);
  len_nbytes = (len >= 0x7F) * (len_nbits/8);

  idx = 0;

  if (len_nbytes == 0)
  {
    encoding[idx] = ASN_LEN_SHORT;
    encoding[idx++] |= len;
  }
  else
  {
    encoding[idx] = ASN_LEN_LONG;
    encoding[idx++] |= len_nbytes;

    while (len_nbits > 0)
    {
      encoding[idx++] = len >> (len_nbits-8);
      len_nbits -= 8;
    }
  }
  return (idx);
}
