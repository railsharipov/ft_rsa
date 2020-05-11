/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_encode.c                                    :+:      :+:    :+:   */
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

static const char	B64[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

static void	__last_block(uint8_t *omes, size_t messize, uint8_t *oenc)
{
	uint8_t	olast[B64_BLOCK_SIZE];
	int		ix;

	if (messize == 0)
		return ;

	ft_memzcpy(olast, omes, MES_BLOCK_SIZE, messize);

	oenc[0] = B64[( ( olast[0]>>2 )&0x3F )];
	oenc[1] = B64[( ( olast[0]<<4 )&0x30 )|( ( olast[1]>>4 )&0xF )];
	oenc[2] = B64[( ( olast[1]<<2 )&0x3C )|( ( olast[2]>>6 )&0x3 )];
	oenc[3] = B64[( olast[2]&0x3F )];

	ix = (CHAR_BIT * messize)/6 + 1;
	while (ix < B64_BLOCK_SIZE)
		oenc[ix++] = '=';

}

int	base64_encode(const char *mes, size_t messize, char **enc, size_t *encsize)
{
	uint8_t	*omes;
	uint8_t	*oenc;

	if ((NULL == mes) || (NULL == enc) || (NULL == encsize))
	{
		return (SSL_ERROR("invalid base64 input"));
	}
	omes = (uint8_t *)(mes);
	*encsize = CEIL(messize, MES_BLOCK_SIZE)/MES_BLOCK_SIZE * B64_BLOCK_SIZE;
	SSL_ALLOC(*enc, *encsize);
	oenc = (uint8_t *)(*enc);

	while (messize >= MES_BLOCK_SIZE)
	{
		*oenc++ = B64[( ( omes[0]>>2 )&0x3F )];
		*oenc++ = B64[( ( omes[0]<<4 )&0x30 )|( ( omes[1]>>4 )&0xF )];
		*oenc++ = B64[( ( omes[1]<<2 )&0x3C )|( ( omes[2]>>6 )&0x3 )];
		*oenc++ = B64[( omes[2]&0x3F )];

		omes += MES_BLOCK_SIZE;
		messize -= MES_BLOCK_SIZE;
	}
	__last_block(omes, messize, oenc);

	return (SSL_OK);
}
