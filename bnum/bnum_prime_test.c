#include <bnum.h>

extern const uint64_t BNUM_PRIME_TAB[];

int		bnum_prime_test(const t_num *num, int bits, int prob, int verbal)
{
	t_num		rnd;
	uint64_t	rem;
	int			idx;

	if (num->len == 1)
	{
		if (num->val[0] <= 1)
			return (BNUM_FALSE);
		if (num->val[0] <= 3)
			return (BNUM_TRUE);
	}
	if (!(num->val[0] & 1ull))
		return (BNUM_FALSE);

	for (idx = 0; idx < BNUM_PTAB_SIZE; idx++)
	{
		bnum_divmod_dig(num, BNUM_PRIME_TAB[idx], NULL, &rem);

		if (rem == 0)
			return (BNUM_FALSE);
	}
 	if (verbal)
		ft_printf("%@.");

	bnum_set_dig_u(&rnd, 2);

	if (!bnum_miller_rabin(num, &rnd))
		return (BNUM_FALSE);

	for (int i = 0; i < prob; i++)
	{
		do {
			bnum_set_rand(&rnd, 64);
			bnum_set_rand(&rnd, rnd.val[0] % (uint64_t)bits + 1ull);
		} while ((rnd.len == 1) && (rnd.val[0] < 2));

		if (bnum_cmp_u(&rnd, num) > 0)
			bnum_sub_u(&rnd, num, &rnd);
		if (!bnum_miller_rabin(num, &rnd))
			return (BNUM_FALSE);
		else if (verbal)
			ft_printf("%@+");
	}
	if (verbal)
		ft_printf("%@\n");

    return (BNUM_TRUE);
}
