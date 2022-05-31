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
#include <ssl_error.h>
#include <ssl_rand.h>
#include <ssl_base64.h>
#include <ssl_des.h>

static int		__is_salted;

static unsigned char	*__salt;
static unsigned char	*__key;

static uint64_t	__permut_key;
static uint64_t	__ksched[16];

static int	__remove_pad(unsigned char **mes, size_t *messize)
{
	unsigned char	padsize;
	unsigned char	ix;

	if (*messize == 0)
	{
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}
	if ((padsize = (*mes)[*messize-1]) > 8)
	{
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}

	ix = 0;
	while (ix++ < padsize)
	{
		*messize -= 1;
		if ((*mes)[*messize] != padsize)
		{
			return (DES_ERROR(UNSPECIFIED_ERROR));
		}
	}

	return (SSL_OK);
}

static int	__vectors(const unsigned char *ciph, size_t ciphsize, uint32_t vflag)
{
	__is_salted = 0;

	if (!SSL_FLAG(DES_K, vflag))
	{
		if (!SSL_FLAG(DES_S, vflag))
		{
			if (ciphsize < 16)
				return (DES_ERROR(UNSPECIFIED_ERROR));

			if (ft_strncmp((char *)ciph, "Salted__", 8))
				return (DES_ERROR(UNSPECIFIED_ERROR));

			ft_memcpy(__salt, ciph + 8, 8);
			__is_salted = 1;
		}
		if (SSL_OK != rand_pbkdf2(__key, __salt, NULL))
		{
			return (DES_ERROR(UNSPECIFIED_ERROR));
		}
	}
	return (SSL_OK);
}

static int	__decrypt(
	const unsigned char *ciph, size_t ciphsize, char **mes, size_t *messize)
{
	size_t	ix;
	unsigned char **mes_ptr;

	mes_ptr = (unsigned char **)mes;
	*messize = ciphsize - (__is_salted*16);
	SSL_ALLOC(*mes_ptr, *messize);

	if (__is_salted)
	{
		ciph += 16;
		ciphsize -= 16;
	}
	ix = 0;
	while (ix < ciphsize)
	{
		(*mes_ptr)[ix] = *ciph++;
		ix++;
	}
	ix = 0;
	while (ix < *messize)
	{
		des_permute_block_init((uint64_t *)*mes_ptr + ix);
		des_permute_block((uint64_t *)*mes_ptr + ix, __ksched);
		des_permute_block_final((uint64_t *)*mes_ptr + ix);

		*(uint64_t *)(*mes_ptr + ix) = util_bswap64(*(uint64_t *)(*mes_ptr + ix));
		ix += 8;
	}
	if (SSL_OK != __remove_pad(mes_ptr, messize))
	{
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}

	return (SSL_OK);
}

int	des_ecb_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes))
	{
		return (DES_ERROR(INVALID_INPUT));
	}
	mes->content = NULL;

	if (ciph->size % DES_MES_BLOCK_SIZE != 0)
	{
		return (DES_ERROR(INVALID_DES_ENCODING));
	}
	__salt = des->salt;
	__key = des->key;

	if (SSL_OK != __vectors(
		(unsigned char *)(ciph->content), ciph->size, des->vflag))
	{
		return (DES_ERROR(INVALID_DES_ENCODING));
	}

	des_permute_key(&__permut_key, __key);
	des_decrypt_schedule(__ksched, &__permut_key);

	return (__decrypt(
		(unsigned char *)(ciph->content), ciph->size,
		&(mes->content), &(mes->size)));
}
