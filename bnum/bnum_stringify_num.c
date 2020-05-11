#include <bnum.h>

char	*stringify_num(const t_num *num)
{
	ssize_t	bufsize, bits;
	char	*buf, *bptr;
	t_num	copy;

	bits = lmbit_num(num);
	bufsize = bits/8 + (bits%8 != 0) + (num->sign == BNUM_NEG);
	LIBFT_ALLOC(buf, bufsize + 1);

	bptr = buf + bufsize;
	copy_num(num, &copy, 0, num->len);

	while (!(BNUM_ZERO(&copy)))
	{
		*--bptr = *(copy.val) & 0xFF;
		rsh_num_b_inpl(&copy, 8);
	}

	if (num->sign == BNUM_NEG)
		*--bptr = '-';

	return (buf);
}
