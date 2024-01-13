#include <util/bnum.h>

void	bnum_rsh_dig_inpl(t_num *num, int shifts)
{
	int	i;

	if (shifts <= 0)
		return ;

	if (shifts >= num->len)
	{
		bnum_set_dig_u(num, 0);
		return ;
	}

	for (i = 0; i < num->len - shifts; i++)
		num->val[i] = num->val[i + shifts];

	while (i < num->len)
		num->val[i++] = 0;

	bnum_skip_zeros(num);
}
