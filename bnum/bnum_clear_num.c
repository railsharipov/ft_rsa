#include <bnum.h>

void	clear_num(t_num *num)
{
	LIBFT_FREE(num->val);
	ft_bzero(num, sizeof(t_num));
}
