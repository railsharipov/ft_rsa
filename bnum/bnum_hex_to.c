#include <bnum.h>
#include <printnl.h>

static const int	A[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	48,	48,	48,	48,	48,	48,	48,	48,	48,	48,	0,	0,	0,	0,	0,	0,
	0,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,
	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	55,	0,	0,	0,	0,	0,
	0,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,
	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	87,	0,	0,	0,	0,	0,
};

void	bnum_from_hex(t_num *num, const char *hex)
{
	size_t		hexsize;
	int			nbits;
	int			nwords;
	uint64_t	bitblock;
	int			idx;

	if (NULL == hex || NULL == num)
		return ;

	hexsize = ft_strlen(hex);
	nbits = CHAR_BIT * hexsize;
	nwords = NBITS_TO_NWORDS(nbits, BNUM_DIGIT_BIT);

	if (num->size < nwords)
		bnum_increase_size(num, nwords);

	bnum_reset(num);
	idx = 0;

	while (idx < hexsize)
	{
		bnum_lsh_bit_inpl(num, 4);
		bitblock = (uint64_t)(hex[idx] - A[hex[idx] & 0x7F]);
		num->val[0] |= bitblock;
		idx++;
	}

	num->len = nwords;
	num->sign = BNUM_POS;
	bnum_skip_zeros(num);
}
