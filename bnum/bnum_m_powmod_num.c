#include <bnum.h>

void	m_powmod_num(const t_num *b, const t_num *e, const t_num *m, t_num *res)
{
#ifdef BNUM_HEAP_WINDOW
	t_num		*window, r;
#else
	t_num		window[BNUM_MAX_DIG], r;
#endif
	uint64_t	rho;
	int			wsize;
	int			thres[8] = { 0, 0, 36, 140, 450, 1303, 3529, INT_MAX };

	if (BNUM_ZERO(e))
		set_num(res, 1);
	else if (BNUM_ONE(e))
		copy_num(b, res, 0, b->len);
	else
	{
		int	i, bits, bitbuf, bitcnt;

		bits = lmbit_num(e);
		for (wsize = 0; bits > thres[wsize];)
			wsize++;

#ifdef BNUM_HEAP_WINDOW
		LIBFT_ALLOC(window, (1 << wsize) * sizeof(t_num));
#endif
		montgomery_setup(m, &rho);
		montgomery_norm(m, res);
		copy_num(res, window, 0, res->len);
		mul_num(b, res, window + 1);
		divmod_num(window + 1, m, NULL, window + 1);

		for (i = 2; i < (1 << wsize); i++)
		{
			mul_num(window + i-1, window + 1, window + i);
			montgomery_reduce(window + i, m, rho);
		}
		bits = lmbit_num(e);
		while (bits >= wsize)
		{
			bitbuf = 0;
			bitcnt = 0;
			while (bitcnt++ < wsize)
			{
				bitbuf = (bitbuf << 1) | (int)((e->val[(bits-1) / BNUM_DIGIT_BIT]
				>> ((bits-1) % BNUM_DIGIT_BIT)) & 1ull);
				--bits;
			}
			while (--bitcnt)
			{
				sqr_num(res, res);
				montgomery_reduce(res, m, rho);
			}
			mul_num(res, window + bitbuf, res);
			montgomery_reduce(res, m, rho);
		}
		for (; bits > 0; bits--)
		{
			sqr_num(res, res);
			montgomery_reduce(res, m, rho);
			if (
				((e->val[(bits-1) / BNUM_DIGIT_BIT]
				>> ((bits-1) % BNUM_DIGIT_BIT)) & 1ull)
			)
			{
				mul_num(res, window + 1, res);
				montgomery_reduce(res, m, rho);
			}
		}
		montgomery_reduce(res, m, rho);

#ifdef BNUM_HEAP_WINDOW
		LIBFT_FREE(window);
#endif
	}
}
