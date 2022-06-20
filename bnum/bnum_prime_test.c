#include <bnum.h>

extern const uint64_t BNUM_PRIME_TAB[];

int		prime_test(const t_num *num, int bits, int prob, int verbal)
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
		divmod_num_d(num, BNUM_PRIME_TAB[idx], NULL, &rem);

		if (rem == 0)
			return (BNUM_FALSE);
	}
 	if (verbal)
		ft_printf("%@.");

	set_num_d(&rnd, 2);

	if (!miller_rabin(num, &rnd))
		return (BNUM_FALSE);

	for (int i = 0; i < prob; i++)
	{
		do {
			set_randnum(&rnd, 64);
			set_randnum(&rnd, rnd.val[0] % (uint64_t)bits + 1ull);
		} while ((rnd.len == 1) && (rnd.val[0] < 2));

		if (compare_num_u(&rnd, num) > 0)
			sub_num_u(&rnd, num, &rnd);
		if (!miller_rabin(num, &rnd))
			return (BNUM_FALSE);
		else if (verbal)
			ft_printf("%@+");
	}
	if (verbal)
		ft_printf("%@\n");

    return (BNUM_TRUE);
}
