#include <bnum.h>

void	powmod_num(const t_num *b, const t_num *e, const t_num *m, t_num *res)
{
	t_num	*window;
	int		wsize, bitblock;
	int		thres[8] = { 0, 0, 36, 140, 450, 1303, 3529, INT_MAX };

	if (BNUM_ZERO(e))
		set_num_d(res, 1);
	else if (BNUM_ONE(e))
		copy_num(b, res);
	else
	{
		int	i, bits, bitbuf, bitcnt;

		bits = lmbit_num(e);
		for (wsize = 0; bits > thres[wsize];)
			wsize++;

		LIBFT_ALLOC(window, (1 << wsize) * sizeof(t_num));
		set_num_d(res, 1);
		set_num_d(window, 1);
		divmod_num(b, m, NULL, window + 1);

		for (i = 2; i < (1 << wsize); i++)
		{
			mul_num(window + i-1, window + 1, window + i);
			divmod_num(window + i, m, NULL, window + i);
		}
		bits = lmbit_num(e);
		while (bits >= wsize)
		{
			bitbuf = 0;
			bitcnt = 0;
			while (bitcnt++ < wsize)
			{
				bitblock = e->val[(bits-1) / BNUM_DIGIT_BIT];
				bitbuf |= (int)((bitblock >> ((bits-1)%BNUM_DIGIT_BIT)) & 1ull);
				bitbuf = (bitbuf << 1);
				--bits;
			}
			while (--bitcnt)
			{
				sqr_num(res, res);
				divmod_num(res, m, NULL, res);
			}
			mul_num(res, window + bitbuf, res);
			divmod_num(res, m, NULL, res);
		}
		for (; bits > 0; bits--)
		{
			sqr_num(res, res);
			divmod_num(res, m, NULL, res);

			bitblock = e->val[(bits-1) / BNUM_DIGIT_BIT];

			if ((bitblock >> ((bits-1) % BNUM_DIGIT_BIT)) & 1ull)
			{
				mul_num(res, window + 1, res);
				divmod_num(res, m, NULL, res);
			}
		}
		LIBFT_FREE(window);
	}
}
