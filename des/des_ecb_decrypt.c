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

static uint64_t	__permut_key;
static uint64_t	__ksched[16];

static int	__remove_pad(uint8_t **mes, size_t *messize)
{
	uint8_t	padsize;
	uint8_t	ix;

	if (*messize == 0)
	{
		return (SSL_ERR);
	}
	if ((padsize = (*mes)[*messize-1]) > 8)
	{
		return (SSL_ERROR("bad cipher pad"));
	}

	ix = 0;
	while (ix++ < padsize)
	{
		*messize -= 1;
		if ((*mes)[*messize] != padsize)
		{
			return (SSL_ERROR("bad cipher pad"));
		}
	}

	return (SSL_OK);
}

static int	__vectors(const uint8_t *ciph, size_t ciphsize, uint32_t vflag)
{
	__is_salted = 0;

	if (!SSL_FLAG(DES_K, vflag))
	{
		if (!SSL_FLAG(DES_S, vflag))
		{
			if (ciphsize < 16)
				return (SSL_ERROR("bad cipher size"));

			if (ft_strncmp((char *)ciph, "Salted__", 8))
				return (SSL_ERROR("bad cipher"));

			ft_memcpy(__salt, ciph + 8, 8);
			__is_salted = 1;
		}
		if (SSL_OK != rand_pbkdf2(__key, __salt, NULL))
		{
			return (SSL_ERROR("pbkdf2 error"));
		}
	}
	return (SSL_OK);
}

static int	__decrypt(
	const uint8_t *ciph, size_t ciphsize, uint8_t **mes, size_t *messize)
{
	size_t	ix;

	*messize = ciphsize - (__is_salted*16);
	SSL_ALLOC(*mes, *messize);

	if (__is_salted)
	{
		ciph += 16;
		ciphsize -= 16;
	}
	ix = 0;
	while (ix < ciphsize)
	{
		(*mes)[ix] = *ciph++;
		ix++;
	}
	ix = 0;
	while (ix < *messize)
	{
		des_permute_block_init(*mes + ix);
		des_permute_block(*mes + ix, __ksched);
		des_permute_block_final(*mes + ix);

		*(uint64_t *)(*mes + ix) = util_bswap64(*(uint64_t *)(*mes + ix));
		ix += 8;
	}
	if (SSL_OK != __remove_pad(mes, messize))
	{
		return (SSL_ERROR("bad cipher"));
	}

	return (SSL_OK);
}

int	des_ecb_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes))
	{
		return (SSL_ERROR("invalid des-ecb input"));
	}
	mes->content = NULL;

	if (ciph->size % DES_MES_BLOCK_SIZE != 0)
	{
		return (SSL_ERROR("bad des cipher"));
	}
	__salt = des->salt;
	__key = des->key;

	if (SSL_OK != __vectors(
		(uint8_t *)(ciph->content), ciph->size, des->vflag))
	{
		return (SSL_ERROR("invalid des-ecb input"));
	}

	des_permute_key(&__permut_key, __key);
	des_decrypt_schedule(__ksched, &__permut_key);

	return (__decrypt(
		(uint8_t *)(ciph->content), ciph->size, &(mes->content), &(mes->size)));
}
