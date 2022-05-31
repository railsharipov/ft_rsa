/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.c                                           :+:      :+:    :+:   */
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
static const int	LEN_SIZE = 8;

static void	__len_octets(uint64_t msize_nbits, unsigned char *octets)
{
	octets[0] = (msize_nbits >> 56) & 0xFF;
	octets[1] = (msize_nbits >> 48) & 0xFF;
	octets[2] = (msize_nbits >> 40) & 0xFF;
	octets[3] = (msize_nbits >> 32) & 0xFF;
	octets[4] = (msize_nbits >> 24) & 0xFF;
	octets[5] = (msize_nbits >> 16) & 0xFF;
	octets[6] = (msize_nbits >> 8) & 0xFF;
	octets[7] = (msize_nbits) & 0xFF;
}

static void __swap_bytes_32(uint32_t *arr, size_t size)
{
	int ix;

	ix = 0;
	while (ix < size)
	{
		arr[ix] = util_bswap32(arr[ix]);
		ix++;
	}
}

void	hash_sha256_final(t_hash *sha256, const char *buf, size_t bufsize)
{
	char 		*pbuf;
	int			pbsize;
	uint64_t	msize_nbits;
	unsigned char		msize_octets[LEN_SIZE];

	if ((NULL == sha256) || (NULL == buf))
		return ;

	*(uint64_t *)(sha256->msize) += bufsize;
	msize_nbits = *(uint64_t *)(sha256->msize) * CHAR_BIT;
	__len_octets(msize_nbits, msize_octets);

	pbsize = CEIL(bufsize, SHA256_BLOCK_SIZE);

	if (pbsize-bufsize <= LEN_SIZE)
		pbsize += SHA256_BLOCK_SIZE;

	SSL_ALLOC(pbuf, pbsize);
	ft_memcpy(pbuf, buf, bufsize);
	pbuf[bufsize] = (char)END_BYTE;

	ft_memcpy(pbuf + pbsize-LEN_SIZE, msize_octets, LEN_SIZE);
	hash_sha256_update(sha256, pbuf, pbsize);
# if BYTE_ORDER == LITTLE_ENDIAN
	__swap_bytes_32(sha256->hash, SHA256_HASH_LEN);
# endif
}
