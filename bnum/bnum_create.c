#include <bnum.h>

t_num	*bnum_create(void)
{
	t_num	*num;

	LIBFT_ALLOC(num, sizeof(t_num));
	ft_bzero(num, sizeof(t_num));

	return (num);
}