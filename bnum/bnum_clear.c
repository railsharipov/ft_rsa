#include <bnum.h>

void	bnum_clear(t_num *num)
{
	LIBFT_FREE(num->val);
	ft_bzero(num, sizeof(t_num));
}
