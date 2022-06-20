#include <bnum.h>

void	clear_num_multi(t_num *num, ...)
{
	va_list	ap;

	init_num(num);
	va_start(ap, num);

	while (NULL != (num = va_arg(ap, t_num *)))
	{
		LIBFT_FREE(num->val);
		ft_bzero(num, sizeof(t_num));
	}

	va_end(ap);
}
