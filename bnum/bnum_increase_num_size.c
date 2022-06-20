#include <bnum.h>

void	increase_num_size(t_num *num, size_t newsize)
{
	if (newsize <= num->size)
		return ;

	if (newsize < 2 * num->size)
		newsize = 2 * num->size;

	LIBFT_REALLOC(
		num->val, num->size * sizeof(uint64_t), newsize * sizeof(uint64_t));
	num->size = newsize;
}
