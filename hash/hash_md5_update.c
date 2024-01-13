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

static t_md5_word		*word;
static t_md5_word		*var;
static t_md5_word		*hash;

static const t_md5_word	SCHED[] = {
	0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
	0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
	0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
	0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
	0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
	0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
	0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
	0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15
};

static const t_md5_word	K[] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static void	__rotate(t_md5_word *t1, t_md5_word *t2, t_md5_word *i)
{
	*t1 = *t1 + var[0] + K[*i] + word[*t2];
	var[0] = var[3];
	var[3] = var[2];
	var[2] = var[1];
	var[1] = var[1] + LROT(*t1, SCHED[*i]);
}

static void	__rotate_hash(void)
{
	t_md5_word	t1;
	t_md5_word	t2;
	t_md5_word	ix;

	for (ix = 0; ix < 16; ix++)
	{
		t1 = F0(var[1], var[2], var[3]);
		t2 = ix;
		__rotate(&t1, &t2, &ix);
	}
	for (; ix < 32; ix++)
	{
		t1 = G0(var[1], var[2], var[3]);
		t2 = (5 * ix + 1) % 16;
		__rotate(&t1, &t2, &ix);
	}
	for (; ix < 48; ix++)
	{
		t1 = H0(var[1], var[2], var[3]);
		t2 = (3 * ix + 5) % 16;
		__rotate(&t1, &t2, &ix);
	}
	for (; ix < 64; ix++)
	{
		t1 = I0(var[1], var[2], var[3]);
		t2 = (7 * ix) % 16;
		__rotate(&t1, &t2, &ix);
	}
}

static void	__update_hash(void)
{
	hash[0] = var[0] + hash[0];
	hash[1] = var[1] + hash[1];
	hash[2] = var[2] + hash[2];
	hash[3] = var[3] + hash[3];
	var[0] = hash[0];
	var[1] = hash[1];
	var[2] = hash[2];
	var[3] = hash[3];
}

void	hash_md5_update(t_hash *md5, const char *buf, size_t bufsize)
{
	if ((NULL == md5) || (NULL == buf))
		return ;

	var = md5->var;
	hash = md5->hash;
	*(uint64_t *)md5->msize += FLOOR(bufsize, MD5_BLOCK_SIZE);

	while (bufsize >= MD5_BLOCK_SIZE)
	{
		word = (t_md5_word *)(buf);
		__rotate_hash();
		__update_hash();
		buf += MD5_BLOCK_SIZE;
		bufsize -= MD5_BLOCK_SIZE;
	}
}
