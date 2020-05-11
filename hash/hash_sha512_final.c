/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha512.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/09 10:59:42 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/01 10:51:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_hash.h>

static const int	END_BYTE = 1 << 7;
static const int	LEN_SIZE = 16;

static void	__len_octets(t_uint128 msize_nbits, uint8_t *octets)
{
	octets[0] = (msize_nbits >> 120) & 0xFF;
	octets[1] = (msize_nbits >> 112) & 0xFF;
	octets[2] = (msize_nbits >> 104) & 0xFF;
	octets[3] = (msize_nbits >> 96) & 0xFF;
	octets[4] = (msize_nbits >> 88) & 0xFF;
	octets[5] = (msize_nbits >> 80) & 0xFF;
	octets[6] = (msize_nbits >> 72) & 0xFF;
	octets[7] = (msize_nbits >> 64) & 0xFF;
	octets[8] = (msize_nbits >> 56) & 0xFF;
	octets[9] = (msize_nbits >> 48) & 0xFF;
	octets[10] = (msize_nbits >> 40) & 0xFF;
	octets[11] = (msize_nbits >> 32) & 0xFF;
	octets[12] = (msize_nbits >> 24) & 0xFF;
	octets[13] = (msize_nbits >> 16) & 0xFF;
	octets[14] = (msize_nbits >> 8) & 0xFF;
	octets[15] = (msize_nbits) & 0xFF;
}

static void __swap_bytes_64(t_sha512_word *arr, size_t size)
{
	int ix;

	ix = 0;
	while (ix < size)
	{
		arr[ix] = util_bswap64(arr[ix]);
		ix++;
	}
}

void	hash_sha512_final(t_hash *sha512, const char *buf, size_t bufsize)
{
	char 		*pbuf;
	int			pbsize;
	t_uint128	msize_nbits;
	uint8_t		msize_octets[LEN_SIZE];

	if ((NULL == sha512) || (NULL == buf))
		return ;

	*(t_uint128 *)(sha512->msize) += bufsize;
	msize_nbits = *(t_uint128 *)(sha512->msize) * CHAR_BIT;
	__len_octets(msize_nbits, msize_octets);

	pbsize = CEIL(bufsize, SHA512_BLOCK_SIZE);

	if (pbsize-bufsize <= LEN_SIZE)
		pbsize += SHA512_BLOCK_SIZE;

	SSL_ALLOC(pbuf, pbsize);
	ft_memcpy(pbuf, buf, bufsize);
	pbuf[bufsize] = (char)END_BYTE;

	ft_memcpy(pbuf + pbsize-LEN_SIZE, msize_octets, LEN_SIZE);
	hash_sha512_update(sha512, pbuf, pbsize);
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_64(sha512->hash, SHA512_HASH_LEN);
# endif
}
