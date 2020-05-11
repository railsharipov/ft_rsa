/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_ecb_encode.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/28 17:44:36 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/12 10:45:14 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_rand.h>
#include <ssl_base64.h>
#include <ssl_des.h>

static int		__is_salted;

static uint8_t	*__salt;
static uint8_t	*__key;
static uint8_t	*__vect;

static uint64_t	__permut_key;
static uint64_t	__ksched[16];

static int	__vectors(uint32_t vflag)
{
	uint64_t	rand_seed;

	__is_salted = 0;

	if (!SSL_FLAG(DES_K, vflag))
	{
		if (!SSL_FLAG(DES_S, vflag))
		{
			if (SSL_OK != rand_useed(&rand_seed, sizeof(uint64_t)))
				return (SSL_ERROR("rand error"));
			if (SSL_OK != rand_bytes(rand_seed, __salt, sizeof(__salt)))
				return (SSL_ERROR("rand error"));
			__is_salted = 1;
		}
		if (SSL_OK != rand_pbkdf2(
			__key, __salt, (SSL_FLAG(DES_V, vflag)) ? (NULL):(__vect)))
		{
			return (SSL_ERROR("pbkdf2 error"));
		}
	}
	else if (!SSL_FLAG(DES_V, vflag))
	{
		return (SSL_ERROR("iv vector required"));
	}
	return (SSL_OK);
}

static void	__encrypt(
	const uint8_t *mes, size_t messize, uint8_t **ciph, size_t *ciphsize)
{
	size_t	ix;
	uint8_t	padsize;
	uint8_t	vectbuf[8];

	ft_memcpy(vectbuf, __vect, 8);
	padsize = 8 - (messize % 8);
	*ciphsize = messize + padsize + (16 * __is_salted);
	SSL_ALLOC(*ciph, *ciphsize);

	ix = 0;
	if (__is_salted)
	{
		ft_memcpy(*ciph, "Salted__", 8);
		ft_memcpy(*ciph + 8, __salt, 8);
		ix += 16;
	}
	while (messize-- > 0)
	{
		(*ciph)[ix++] = *mes++;
	}
	while (ix < *ciphsize)
	{
		(*ciph)[ix++] = padsize;
	}
	ix = 0 + (16 * __is_salted);
	while (ix < *ciphsize)
	{
		*(uint64_t *)(*ciph + ix) ^= *(uint64_t *)(vectbuf);

		des_permute_block_init(*ciph + ix);
		des_permute_block(*ciph + ix, __ksched);
		des_permute_block_final(*ciph + ix);

		*(uint64_t *)(*ciph + ix) = util_bswap64(*(uint64_t *)(*ciph + ix));
		*(uint64_t *)(vectbuf) = *(uint64_t *)(*ciph + ix);

		ix += 8;
	}
}

int	des_cbc_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes))
	{
		return (SSL_ERROR("invalid des-cbc input"));
	}
	ciph->content = NULL;
	__salt = des->salt;
	__key = des->key;
	__vect = des->vect;

	if (SSL_OK != __vectors(des->vflag))
	{
		return (SSL_ERROR("invalid des-cbc input"));
	}
	des_permute_key(&__permut_key, __key);
	des_encrypt_schedule(__ksched, &__permut_key);

	__encrypt(
		(uint8_t *)(mes->content), mes->size, &ciph->content, &ciph->size);

	return (SSL_OK);
}
