#include <bnum.h>

void	powmod_num(const t_num *b, const t_num *e, const t_num *m, t_num *res)
{
	t_num		*window;
	uint64_t	digit;
	int			wsize;
	int			thres[8] = { 0, 0, 36, 140, 450, 1303, 3529, INT_MAX };

	if (BNUM_ZERO(e))
		set_num_ud(res, 1);
	else if (BNUM_ONE(e))
		copy_num(b, res);
	else
	{
		uint64_t	bitcnt, bitbuf, bit;
		int			i, mode, ndigits, nbits;

		i = lmbit_num(e);
		for (wsize = 0; i > thres[wsize];)
			wsize++;

		LIBFT_ALLOC(window, (1 << wsize) * sizeof(t_num));

		for (i = 0; i < (1 << wsize); i++)
			init_num(window + i);

		set_num_ud(res, 1);
		set_num_ud(window, 1);
		divmod_num(b, m, NULL, window + 1);

		for (i = 2; i < (1 << wsize); i++)
		{
			mul_num(window + i-1, window + 1, window + i);
			divmod_num(window + i, m, NULL, window + i);
		}

		ndigits = e->len;
		nbits = 0;
		bitbuf = 0;
		bitcnt = 0;
		mode = 0;

		while (BNUM_TRUE)
		{
			if (nbits <= 0)
			{
				ndigits--;

				if (ndigits < 0)
					break ;

				digit = e->val[ndigits];
				nbits = BNUM_DIGIT_BIT;
			}

			nbits--;
			bit = (digit >> nbits) & 1ull;

			if (mode == 0 && bit == 0)
			{
				sqr_num(res, res);
				divmod_num(res, m, NULL, res);
				continue ;
			}

			bitcnt++;
			bitbuf |= bit << (wsize - bitcnt);
			mode = 1;

			if (bitcnt == wsize)
			{
				for (i = 0; i < wsize; i++)
				{
					sqr_num(res, res);
					divmod_num(res, m, NULL, res);
				}

				mul_num(res, window + bitbuf, res);
				divmod_num(res, m, NULL, res);

				bitcnt = 0;
				bitbuf = 0;
				mode = 0;
			}
		}

		if (mode == 1 && bitcnt > 0)
		{
			for (i = 0; i < bitcnt; i++)
			{
				sqr_num(res, res);
				divmod_num(res, m, NULL, res);

				bitbuf <<= 1;
				if (bitbuf & (1 << wsize))
				{
					mul_num(res, window + 1, res);
					divmod_num(res, m, NULL, res);
				}
			}

		}

		for (i = 0; i < (1 << wsize); i++)
			clear_num(window + i);

		LIBFT_FREE(window);
	}
}
