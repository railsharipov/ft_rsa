#include <util/bnum.h>
#include <libft/string.h>

t_num	*bnum_from_dec(const char *dec)
{
	t_num	*num;
	char	*decptr;
	size_t	decsize;
	size_t	idx;
	int		sign;

	if (NULL == dec) {
		return (NULL);
	}
	if (*dec == '-') {
		sign = BNUM_NEG;
		dec++;
	} else {
		sign = BNUM_POS;
	}

	decsize = ft_strlen(dec);

	num = bnum_create();
	bnum_set_dig_u(num, 0u);

	idx = 0;
	while (idx < decsize) {
		bnum_mul_dig(num, 10u, num);
		bnum_add_dig(num, ((uint64_t)(dec[idx] - 48)) % 10u, num);
		idx++;
	}
	num->sign = sign;

	return (num);
}
