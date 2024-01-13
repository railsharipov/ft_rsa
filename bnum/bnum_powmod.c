#include <util/bnum.h>
#include <limits.h>

void	bnum_powmod(const t_num *b, const t_num *e, const t_num *m, t_num *res)
{
	t_num		*window;
	uint64_t	digit;
	int			wsize;
	int			thres[8] = { 0, 0, 36, 140, 450, 1303, 3529, INT_MAX };

	if (BNUM_ZERO(e))
		bnum_set_dig_u(res, 1);
	else if (BNUM_ONE(e))
		bnum_copy(b, res);
	else
	{
		uint64_t	bitcnt, bitbuf, bit;
		int			i, mode, ndigits, nbits;

		i = bnum_lmbit(e);
		for (wsize = 0; i > thres[wsize];)
			wsize++;

		BNUM_ALLOC(window, (1 << wsize) * sizeof(t_num));

		for (i = 0; i < (1 << wsize); i++)
			bnum_init(window + i);

		bnum_set_dig_u(res, 1);
		bnum_set_dig_u(window, 1);
		bnum_divmod(b, m, NULL, window + 1);

		for (i = 2; i < (1 << wsize); i++)
		{
			bnum_mul(window + i-1, window + 1, window + i);
			bnum_divmod(window + i, m, NULL, window + i);
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
				bnum_sqr(res, res);
				bnum_divmod(res, m, NULL, res);
				continue ;
			}

			bitcnt++;
			bitbuf |= bit << (wsize - bitcnt);
			mode = 1;

			if (bitcnt == wsize)
			{
				for (i = 0; i < wsize; i++)
				{
					bnum_sqr(res, res);
					bnum_divmod(res, m, NULL, res);
				}

				bnum_mul(res, window + bitbuf, res);
				bnum_divmod(res, m, NULL, res);

				bitcnt = 0;
				bitbuf = 0;
				mode = 0;
			}
		}

		if (mode == 1 && bitcnt > 0)
		{
			for (i = 0; i < bitcnt; i++)
			{
				bnum_sqr(res, res);
				bnum_divmod(res, m, NULL, res);

				bitbuf <<= 1;
				if (bitbuf & (1 << wsize))
				{
					bnum_mul(res, window + 1, res);
					bnum_divmod(res, m, NULL, res);
				}
			}

		}

		for (i = 0; i < (1 << wsize); i++)
			bnum_clear(window + i);

		BNUM_FREE(window);
	}
}
