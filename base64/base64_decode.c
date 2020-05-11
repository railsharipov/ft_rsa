/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_decode.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/28 17:44:36 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/12 10:45:14 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <bnum.h>
#include <ssl_base64.h>

static const uint8_t	MES_BLOCK_SIZE = 3;
static const uint8_t	B64_BLOCK_SIZE = 4;

static const uint8_t	UB64[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	62,	0,	0,	0,	63,
	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	0,	0,	0,	0,	0,	0,

	0,	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,
	15,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	0,	0,	0,	0,	0,
	0,	26,	27,	28,	29,	30,	31,	32,	33,	34,	35,	36,	37,	38,	39,	40,
	41,	42,	43,	44,	45,	46,	47,	48,	49,	50,	51,	0,	0,	0,	0,	0
};

int	base64_decode(const char *enc, size_t encsize, char **mes, size_t *messize)
{
	uint8_t	*omes;
	uint8_t	*oenc;

	if ((NULL == enc) || (NULL == mes) || (NULL == messize))
	{
		return (SSL_ERROR("invalid base64 input"));
	}
	*mes = NULL;

	if ((encsize % B64_BLOCK_SIZE != 0)
		|| (SSL_OK != base64_check(enc, encsize)))
	{
		return (SSL_ERROR("invalid base64 encoding"));
	}

	oenc = (uint8_t *)(enc);
	*messize = CEIL(encsize, B64_BLOCK_SIZE)/B64_BLOCK_SIZE * MES_BLOCK_SIZE;
	SSL_ALLOC(*mes, *messize);
	omes = (uint8_t *)(*mes);

	while (encsize >= B64_BLOCK_SIZE)
	{
		*omes++ = ( UB64[oenc[0]]<<2 )|( UB64[oenc[1]]>>4 );
		*omes++ = ( UB64[oenc[1]]<<4 )|( UB64[oenc[2]]>>2 );
		*omes++ = ( UB64[oenc[2]]<<6 )|( UB64[oenc[3]] );

		oenc += B64_BLOCK_SIZE;
		encsize -= B64_BLOCK_SIZE;
	}

	while (*--oenc == '=')
		if (*--omes == 0)
			*messize -= 1;

	return (SSL_OK);
}
