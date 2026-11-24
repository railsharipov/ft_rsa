#include <bnum.h>

t_num	*bnum_create(void)
{
	t_num	*num;

	BNUM_ALLOCZ(num, sizeof(t_num));

	return (num);
}
