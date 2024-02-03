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

#include <ssl/ssl.h>
#include <ssl/rand.h>
#include <ssl/base64.h>
#include <ssl/des.h>
#include <libft/bytes.h>

static int		__is_salted;

static unsigned char	*__salt;
static unsigned char	*__key;
static unsigned char	*__vect;

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
				return (DES_ERROR(UNSPECIFIED_ERROR));
			if (SSL_OK != rand_bytes(rand_seed, __salt, sizeof(__salt)))
				return (DES_ERROR(UNSPECIFIED_ERROR));
			__is_salted = 1;
		}
		if (SSL_OK != rand_pbkdf2(
			__key, __salt, (SSL_FLAG(DES_V, vflag)) ? (NULL):(__vect)))
		{
			return (DES_ERROR(UNSPECIFIED_ERROR));
		}
	}
	else if (!SSL_FLAG(DES_V, vflag))
	{
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static void	__encrypt(
	const unsigned char *mes, size_t messize, char **ciph, size_t *ciphsize)
{
	size_t	ix;
	unsigned char	padsize;
	unsigned char	vectbuf[8];
	unsigned char	**ciph_ptr;

	ciph_ptr = (unsigned char **)ciph;
	ft_memcpy(vectbuf, __vect, 8);
	padsize = 8 - (messize % 8);
	*ciphsize = messize + padsize + (16 * __is_salted);
	SSL_ALLOC(*ciph_ptr, *ciphsize);

	ix = 0;
	if (__is_salted)
	{
		ft_memcpy(*ciph_ptr, "Salted__", 8);
		ft_memcpy(*ciph_ptr + 8, __salt, 8);
		ix += 16;
	}
	while (messize-- > 0)
	{
		(*ciph_ptr)[ix++] = *mes++;
	}
	while (ix < *ciphsize)
	{
		(*ciph_ptr)[ix++] = padsize;
	}
	ix = 0 + (16 * __is_salted);
	while (ix < *ciphsize)
	{
		*(uint64_t *)(*ciph_ptr + ix) ^= *(uint64_t *)(vectbuf);

		des_permute_block_init((uint64_t *)*ciph_ptr + ix);
		des_permute_block((uint64_t *)*ciph_ptr + ix, __ksched);
		des_permute_block_final((uint64_t *)*ciph_ptr + ix);

		*(uint64_t *)(*ciph_ptr + ix) = ft_uint_bswap64(*(uint64_t *)(*ciph_ptr + ix));
		*(uint64_t *)(vectbuf) = *(uint64_t *)(*ciph_ptr + ix);

		ix += 8;
	}
}

int	des_cbc_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph)
{
	if ((NULL == des) || (NULL == ciph) || (NULL == mes))
	{
		return (DES_ERROR(INVALID_INPUT_ERROR));
	}
	ciph->content = NULL;
	__salt = des->salt;
	__key = des->key;
	__vect = des->vect;

	if (SSL_OK != __vectors(des->vflag))
	{
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}
	des_permute_key(&__permut_key, __key);
	des_encrypt_schedule(__ksched, &__permut_key);

	__encrypt(
		(unsigned char *)(mes->content), mes->size, &ciph->content, &ciph->size);

	return (SSL_OK);
}
