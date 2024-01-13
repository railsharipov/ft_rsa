#include <util/bnum.h>

void	bnum_del(t_num *num)
{
	if (NULL == num)
		return ;

	if (NULL != num->val)
	{
		ft_bzero(num->val, num->size * sizeof(uint64_t));
		BNUM_FREE(num->val);
	}

	ft_bzero(num, sizeof(t_num));
	BNUM_FREE(num);
}
