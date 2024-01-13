#include <util/bnum.h>

void	bnum_clear_multi(t_num *num, ...)
{
	va_list	ap;

	bnum_init(num);
	va_start(ap, num);

	while (NULL != (num = va_arg(ap, t_num *)))
	{
		BNUM_FREE(num->val);
		ft_bzero(num, sizeof(t_num));
	}

	va_end(ap);
}
