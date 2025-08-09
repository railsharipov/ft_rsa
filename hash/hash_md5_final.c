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

#include <common.h>
#include <hash.h>

void	hash_md5_final(t_hash *md5, const unsigned char *buf, size_t bufsize)
{
	uint64_t		total_bits;
	uint32_t		rem_len;
	uint32_t		pad_len;
	unsigned char	pbuf[MD5_BLOCK_SIZE * 2];

	if ((NULL == md5) || (NULL == buf)) {
		return ;
	}

	*(uint64_t *)(md5->msize) += bufsize;
	total_bits = *(uint64_t *)(md5->msize) * 8;
	rem_len = bufsize % MD5_BLOCK_SIZE;

	hash_md5_update(md5, buf, bufsize - rem_len);
	buf += bufsize - rem_len;

	ft_memcpy(pbuf, buf, rem_len);
	pbuf[rem_len] = 0x80;
	pad_len = rem_len + 1;

	if (pad_len > MD5_BLOCK_SIZE - 8) {
		ft_bzero(pbuf + pad_len, MD5_BLOCK_SIZE - pad_len);
		hash_md5_update(md5, pbuf, MD5_BLOCK_SIZE);
		pad_len = 0;
	}

	ft_bzero(pbuf + pad_len, MD5_BLOCK_SIZE - 8 - pad_len);
	ft_memcpy(pbuf + MD5_BLOCK_SIZE - 8, &total_bits, sizeof(total_bits));
	hash_md5_update(md5, pbuf, MD5_BLOCK_SIZE);
}
