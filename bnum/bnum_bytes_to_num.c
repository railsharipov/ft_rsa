#include <bnum.h>

/* Convert bytes representing unsigned integer to bignum */

void	bytes_to_num(t_num *num, const char *buf, int bufsize)
{
	unsigned char	*octets;
	size_t			ndigits;
	size_t			nbits;
	int				idx;

	if (NULL == num || NULL == buf)
		return ;

	init_num(num);

	if (bufsize == 0)
		return ;

	nbits = bufsize * CHAR_BIT;
	ndigits = NBITS_TO_NWORDS(nbits, BNUM_DIGIT_BIT);

	if (ndigits > num->size)
		increase_num_size(num, ndigits);

	octets = (unsigned char *)buf;

	idx = 0;
	while (idx < bufsize)
	{
		lsh_num_b_inpl(num, 8);
		*(num->val) |= octets[idx++];
	}
	skip_zeros(num);
}
