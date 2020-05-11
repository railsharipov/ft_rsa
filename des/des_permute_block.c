/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_encrypt_block.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/28 16:43:26 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:32:34 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_des.h>

static const uint8_t	PBA[48] = {
	32,	1,	2,	3,	4,	5,	4,	5,	6,	7,	8,	9,	8,	9,	10,	11,
	12,	13,	12,	13,	14,	15,	16,	17,	16,	17,	18,	19,	20,	21,	20,	21,
	22,	23,	24,	25,	24,	25,	26,	27,	28,	29,	28,	29,	30,	31,	32,	1
};

static const uint8_t	PBB[32] = {
	16,	7,	20,	21,	29,	12,	28,	17,	1,	15,	23,	26,	5,	18,	31,	10,
	2,	8,	24,	14,	32,	27,	3,	9,	19,	13,	30,	6,	22,	11,	4,	25
};

static const uint8_t	SB[8*4*16] = {
	14,	0,	4,	15,	13,	7,	1,	4,	2,	14,	15,	2,	11,	13,	8,	1,
	3,	10,	10,	6,	6,	12,	12,	11,	5,	9,	9,	5,	0,	3,	7,	8,
	4,	15,	1,	12,	14,	8,	8,	2,	13,	4,	6,	9,	2,	1,	11,	7,
	15,	5,	12,	11,	9,	3,	7,	14,	3,	10,	10,	0,	5,	6,	0,	13,

	15,	3,	1,	13,	8,	4,	14,	7,	6,	15,	11,	2,	3,	8,	4,	14,
	9,	12,	7,	0,	2,	1,	13,	10,	12,	6,	0,	9,	5,	11,	10,	5,
	0,	13,	14,	8,	7,	10,	11,	1,	10,	3,	4,	15,	13,	4,	1,	2,
	5,	11,	8,	6,	12,	7,	6,	12,	9,	0,	3,	5,	2,	14,	15,	9,

	10,	13,	0,	7,	9,	0,	14,	9,	6,	3,	3,	4,	15,	6,	5,	10,
	1,	2,	13,	8,	12,	5,	7,	14,	11,	12,	4,	11,	2,	15,	8,	1,
	13,	1,	6,	10,	4,	13,	9,	0,	8,	6,	15,	9,	3,	8,	0,	7,
	11,	4,	1,	15,	2,	14,	12,	3,	5,	11,	10,	5,	14,	2,	7,	12,

	7,	13,	13,	8,	14,	11,	3,	5,	0,	6,	6,	15,	9,	0,	10,	3,
	1,	4,	2,	7,	8,	2,	5,	12,	11,	1,	12,	10,	4,	14,	15,	9,
	10,	3,	6,	15,	9,	0,	0,	6,	12,	10,	11,	1,	7,	13,	13,	8,
	15,	9,	1,	4,	3,	5,	14,	11,	5,	12,	2,	7,	8,	2,	4,	14,

	2,	14,	12,	11,	4,	2,	1,	12,	7,	4,	10,	7,	11,	13,	6,	1,
	8,	5,	5,	0,	3,	15,	15,	10,	13,	3,	0,	9,	14,	8,	9,	6,
	4,	11,	2,	8,	1,	12,	11,	7,	10,	1,	13,	14,	7,	2,	8,	13,
	15,	6,	9,	15,	12,	0,	5,	9,	6,	10,	3,	4,	0,	5,	14,	3,

	12,	10,	1,	15,	10,	4,	15,	2,	9,	7,	2,	12,	6,	9,	8,	5,
	0,	6,	13,	1,	3,	13,	4,	14,	14,	0,	7,	11,	5,	3,	11,	8,
	9,	4,	14,	3,	15,	2,	5,	12,	2,	9,	8,	5,	12,	15,	3,	10,
	7,	11,	0,	14,	4,	1,	10,	7,	1,	6,	13,	0,	11,	8,	6,	13,

	4,	13,	11,	0,	2,	11,	14,	7,	15,	4,	0,	9,	8,	1,	13,	10,
	3,	14,	12,	3,	9,	5,	7,	12,	5,	2,	10,	15,	6,	8,	1,	6,
	1,	6,	4,	11,	11,	13,	13,	8,	12,	1,	3,	4,	7,	10,	14,	7,
	10,	9,	15,	5,	6,	0,	8,	15,	0,	14,	5,	2,	9,	3,	2,	12,

	13,	1,	2,	15,	8,	13,	4,	8,	6,	10,	15,	3,	11,	7,	1,	4,
	10,	12,	9,	5,	3,	6,	14,	11,	5,	0,	0,	14,	12,	9,	7,	2,
	7,	2,	11,	1,	4,	14,	1,	7,	9,	4,	12,	10,	14,	8,	2,	13,
	0,	15,	6,	12,	10,	9,	13,	0,	15,	3,	3,	5,	5,	6,	8,	11
};

void	__permute(uint32_t *pblock, uint32_t rblock, uint64_t key)
{
	uint64_t	tn;
	int			ix;

	*pblock = 0;
	tn = 0;

	for (ix = 0; ix < 48; ix++)
	{
		tn <<= 1;
		tn |= ((uint64_t)rblock >> (32 - PBA[ix])) & 1;
	}

	tn = tn ^ key;
	rblock = 0;

	for (ix = 0; ix < 8; ix++)
	{
		rblock = rblock << 4;
		rblock |= SB[ 64*ix + ((tn >> (42-6*ix)) & 0x3F) ];
	}

	for (ix = 0; ix < 32; ix++)
	{
		*pblock <<= 1;
		*pblock |= (rblock >> (32 - PBB[ix])) & 1;
	}
}

void	des_permute_block(uint64_t *block, uint64_t *ksched)
{
	uint32_t	lblock;
	uint32_t	rblock;
	uint32_t	tblock;
	uint32_t	pblock;
	int 		ix;

	lblock = *block >> 32;
	rblock = *block & ((1UL<<32)-1);

	for (ix = 0; ix < 16; ix++)
	{
		__permute(&pblock, rblock, ksched[ix]);

		tblock = lblock;
		lblock = rblock;
		rblock = tblock ^ pblock;
	}

	*block = (uint64_t)rblock << 32;
	*block |= lblock;
}
