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

#include <ssl/ssl.h>
#include <ssl/hash.h>
#include <libft/bytes.h>

static t_sha256_word		sched[64];
static t_sha256_word		*word;
static t_sha256_word		*var;
static t_sha256_word		*hash;

static const t_sha256_word	K[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void	__update_sched(void)
{
	int	i;

	i = 0;
	while (i < 16)
	{
# if BYTE_ORDER == BIG_ENDIAN
		sched[i] = word[i];
# else
		sched[i] = ft_uint_bswap32(word[i]);
# endif
		i++;
	}
	while (i < 64)
	{
		sched[i] = S1(sched[i-2]) + sched[i-7] + S0(sched[i-15]) + sched[i-16];
		i++;
	}
}

static void	__rotate_hash(void)
{
	t_sha256_word	t1;
	t_sha256_word	t2;
	t_sha256_word	ix;

	ix = 0;
	while (ix < 64)
	{
		t1 = var[7] + E1(var[4]) + CH(var[4],var[5],var[6]) + K[ix] + sched[ix];
		t2 = E0(var[0]) + MAJ(var[0],var[1],var[2]);
		var[7] = var[6];
		var[6] = var[5];
		var[5] = var[4];
		var[4] = var[3] + t1;
		var[3] = var[2];
		var[2] = var[1];
		var[1] = var[0];
		var[0] = t1 + t2;
		ix++;
	}
}

static void	__update_hash(void)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	hash[4] = var[4] + hash[4];
	hash[5] = var[5] + hash[5];
	hash[6] = var[6] + hash[6];
	hash[7] = var[7] + hash[7];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
	var[4] = hash[4];
	var[5] = hash[5];
	var[6] = hash[6];
	var[7] = hash[7];
}

void	hash_sha256_update(t_hash *sha256, const char *buf, size_t bufsize)
{
	if ((NULL == sha256) || (NULL == buf))
		return ;

	var = sha256->var;
	hash = sha256->hash;
	*(uint64_t *)sha256->msize += FLOOR(bufsize, SHA256_BLOCK_SIZE);

	while (bufsize >= SHA256_BLOCK_SIZE)
	{
		word = (t_sha256_word *)(buf);
		__update_sched();
		__rotate_hash();
		__update_hash();
		buf += SHA256_BLOCK_SIZE;
		bufsize -= SHA256_BLOCK_SIZE;
	}
}
