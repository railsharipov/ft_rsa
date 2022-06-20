#include <bnum.h>

void	rsh_num_d_inpl(t_num *num, int shifts)
{
	int	i;

	if (shifts <= 0)
		return ;

	if (shifts >= num->len)
	{
		set_num_d(num, 0);
		return ;
	}

	for (i = 0; i < num->len - shifts; i++)
		num->val[i] = num->val[i + shifts];

	while (i < num->len)
		num->val[i++] = 0;

	skip_zeros(num);
}
