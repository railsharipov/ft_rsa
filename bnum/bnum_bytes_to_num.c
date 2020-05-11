#include <bnum.h>

void	bytes_to_num(t_num *num, const char *buf, int bufsize)
{
	unsigned char	*octets;

	octets = (unsigned char *)buf;
	init_num(num);

	if ((NULL == octets) || (0 == bufsize))
		return ;
	if (*octets == '-')
	{
		num->sign = BNUM_NEG;
		octets++;
		bufsize--;
	}
	while (bufsize-- > 0)
	{
		lsh_num_b_inpl(num, 8);
		*(num->val) |= *octets++;
	}
	skip_zeros(num);
}
