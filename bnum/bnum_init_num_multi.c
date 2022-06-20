#include <bnum.h>

void	init_num_multi(t_num *num, ...)
{
	va_list	ap;

	init_num(num);
	va_start(ap, num);

	while (NULL != (num = va_arg(ap, t_num *)))
		init_num(num);

	va_end(ap);
}
