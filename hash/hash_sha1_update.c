/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha1.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/09 10:59:42 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/01 10:51:29 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_hash.h>

static t_sha1_word		sched[80];
static t_sha1_word		*word;
static t_sha1_word		*var;
static t_sha1_word		*hash;

static const t_sha1_word	K[] = {
	[0 ... 19] = 0x5a827999,
	[20 ... 39] = 0x6ed9eba1,
	[40 ... 59] = 0x8f1bbcdc,
	[60 ... 79] = 0xca62c1d6
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
		sched[i] = util_bswap32(word[i]);
# endif
		i++;
	}
	while (i < 80)
	{
		sched[i] = LROT((sched[i-3]^sched[i-8]^sched[i-14]^sched[i-16]), 1);
		i++;
	}
}

static void	__rotate(t_sha1_word *t1, t_sha1_word *t2, t_sha1_word ix)
{
	*t2 = LROT(var[0], 5) + *t1 + var[4] + K[ix] + sched[ix];

	var[4] = var[3];
	var[3] = var[2];
	var[2] = LROT(var[1], 30);
	var[1] = var[0];
	var[0] = *t2;
}

static void	__rotate_hash(void)
{
	t_sha1_word	t1;
	t_sha1_word	t2;
	t_sha1_word	ix;

	ix = 0;
	while (ix < 20)
	{
		t1 = CH(var[1], var[2], var[3]);
		__rotate(&t1, &t2, ix++);
	}
	while (ix < 40)
	{
		t1 = H0(var[1], var[2], var[3]);
		__rotate(&t1, &t2, ix++);
	}
	while (ix < 60)
	{
		t1 = MAJ(var[1], var[2], var[3]);
		__rotate(&t1, &t2, ix++);
	}
	while (ix < 80)
	{
		t1 = H0(var[1], var[2], var[3]);
		__rotate(&t1, &t2, ix++);
	}
}

static void	__update_hash(void)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	hash[4] = var[4] + hash[4];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
	var[4] = hash[4];
}

void	hash_sha1_update(t_hash *sha1, const char *buf, size_t bufsize)
{
	if ((NULL == sha1) || (NULL == buf))
		return ;

	var = sha1->var;
	hash = sha1->hash;
	*(uint64_t *)sha1->msize += FLOOR(bufsize, SHA1_BLOCK_SIZE);

	while (bufsize >= SHA1_BLOCK_SIZE)
	{
		word = (t_sha1_word *)(buf);
		__update_sched();
		__rotate_hash();
		__update_hash();
		buf += SHA1_BLOCK_SIZE;
		bufsize -= SHA1_BLOCK_SIZE;
	}
}
