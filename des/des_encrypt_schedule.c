/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_create_schedule.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/06 14:36:29 by rsharipo          #+#    #+#             */
/*   Updated: 2019/04/06 14:36:32 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <ssl/des.h>

static const unsigned char	CHA[16] = {
	1,  1,  2,  2,  2,  2,  2,  2,
	1,  2,  2,  2,  2,  2,  2,  1
};

static const unsigned char	CSA[48] = {
	14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
	23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

static const uint64_t	MASK28 = ((uint64_t)1 << 28) - 1;
static const uint64_t	MASK56 = ((uint64_t)1 << 56) - 1;

void	des_encrypt_schedule(uint64_t *ksched, uint64_t *pkey)
{
	uint64_t	lblock;
	uint64_t	rblock;
	uint64_t	tn;
	int			ix;
	int			iy;

	lblock = *pkey >> 28;
	rblock = *pkey & MASK28;

	ix = 0;
	while (ix < 16)
	{
		lblock = ((lblock << CHA[ix]) & MASK28) | (lblock >> (28 - CHA[ix]));
		rblock = ((rblock << CHA[ix]) & MASK28) | (rblock >> (28 - CHA[ix]));

		tn = ((lblock << 28) & MASK56) | rblock;

		iy = 0;
		while (iy < 48)
		{
			ksched[ix] <<= 1;
			ksched[ix] |= (tn >> (56 - CSA[iy])) & 1;
			iy++;
		}

		ix++;
	}
}
