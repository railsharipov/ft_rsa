#include <util/bnum.h>

void	bnum_init_multi(t_num *num, ...)
{
	va_list	ap;

	bnum_init(num);
	va_start(ap, num);

	while (NULL != (num = va_arg(ap, t_num *)))
		bnum_init(num);

	va_end(ap);
}
