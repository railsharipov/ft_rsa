/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/09 10:59:42 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/01 10:51:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <common.h>
#include <logger.h>
#include <hash.h>

static const t_sha256_word	HASH_INIT_VECT[] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

void	hash_sha224_init(t_hash *ctx)
{
	ft_bzero(ctx, sizeof(t_hash));
	ft_memcpy(ctx->var, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ft_memcpy(ctx->hash, HASH_INIT_VECT, sizeof(HASH_INIT_VECT));
	ctx->blocksize = SHA256_BLOCK_SIZE;
	ctx->hashsize = SHA224_HASH_SIZE;
}

void	hash_sha224_update(t_hash *ctx, const unsigned char *mes, size_t messize)
{
	hash_sha256_update(ctx, mes, messize);
}

void	hash_sha224_final(t_hash *ctx)
{
	hash_sha256_final(ctx);
}
