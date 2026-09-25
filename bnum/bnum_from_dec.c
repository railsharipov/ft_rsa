#include <bnum.h>
#include <libft.h>

void	bnum_init_from_dec(t_num *num, const char *dec)
{
	size_t	decsize;
	size_t	idx;
	int		sign;

	if (NULL == num || NULL == dec) {
		return ;
	}

	if (*dec == '-') {
		sign = BNUM_NEG;
		dec++;
	} else {
		sign = BNUM_POS;
	}
	decsize = ft_strlen(dec);
	bnum_set_dig_u(num, 0u);

	idx = 0;
	while (idx < decsize) {
		bnum_mul_dig(num, 10u, num);
		bnum_add_dig(num, ((uint64_t)(dec[idx] - 48)) % 10u, num);
		idx++;
	}
	num->sign = sign;
}

t_num	*bnum_from_dec(const char *dec)
{
	t_num *num = bnum_create();
	bnum_init_from_dec(num, dec);
	return (num);
}
