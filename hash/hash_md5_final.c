/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   md5.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/09 10:59:42 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/01 10:51:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <ssl/hash.h>

static const int	END_BYTE = 1 << 7;
static const int	LEN_SIZE = 8;

void	hash_md5_final(t_hash *md5, const char *buf, size_t bufsize)
{
	char 		*pbuf;
	int			pbsize;
	uint64_t	msize_nbits;
	unsigned char		msize_octets[8];

	if ((NULL == md5) || (NULL == buf))
		return ;

	*(uint64_t *)(md5->msize) += bufsize;
	msize_nbits = *(uint64_t *)(md5->msize) * CHAR_BIT;
	ft_memcpy(msize_octets, &msize_nbits, LEN_SIZE);

	pbsize = CEIL(bufsize, MD5_BLOCK_SIZE);

	if (pbsize-bufsize <= LEN_SIZE)
		pbsize += MD5_BLOCK_SIZE;

	SSL_ALLOC(pbuf, pbsize);
	ft_memcpy(pbuf, buf, bufsize);
	pbuf[bufsize] = (char)END_BYTE;

	ft_memcpy(pbuf + pbsize-LEN_SIZE, msize_octets, LEN_SIZE);
	hash_md5_update(md5, pbuf, pbsize);
}
