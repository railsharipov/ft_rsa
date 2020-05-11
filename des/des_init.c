/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_func.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/13 09:49:33 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:36:39 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_des.h>

static t_des	__des;
static uint8_t	__key[8];
static uint8_t	__salt[8];
static uint8_t	__vect[8];

t_des	*des_init(
	const uint8_t *key, const uint8_t *salt, const uint8_t *vect)
{
	ft_bzero(__key, sizeof(__key));
	ft_bzero(__salt, sizeof(__salt));
	ft_bzero(__vect, sizeof(__vect));
	ft_bzero(&__des, sizeof(t_des));

	if (NULL != key)
	{
		ft_memcpy(__key, key, 8);
		__des.vflag |= DES_K;
	}
	if (NULL != salt)
	{
		ft_memcpy(__salt, salt, 8);
		__des.vflag |= DES_S;
	}
	if (NULL != vect)
	{
		ft_memcpy(__vect, vect, 8);
		__des.vflag |= DES_V;
	}
	__des.key = __key;
	__des.salt = __salt;
	__des.vect = __vect;

	return (&__des);
}

t_des	*des_hexinit(
	const char *keyhex, const char *salthex, const char *vecthex)
{
	ft_bzero(__key, sizeof(__key));
	ft_bzero(__salt, sizeof(__salt));
	ft_bzero(__vect, sizeof(__vect));
	ft_bzero(&__des, sizeof(t_des));

	if (NULL != keyhex)
	{
		ft_hexbin(__key, keyhex, MIN(16, ft_strlen(keyhex)));
		__des.vflag |= DES_K;
	}
	if (NULL != salthex)
	{
		ft_hexbin(__salt, salthex, MIN(16, ft_strlen(salthex)));
		__des.vflag |= DES_S;
	}
	if (NULL != vecthex)
	{
		ft_hexbin(__vect, vecthex, MIN(16, ft_strlen(vecthex)));
		__des.vflag |= DES_V;
	}
	__des.key = __key;
	__des.salt = __salt;
	__des.vect = __vect;

	return (&__des);
}
