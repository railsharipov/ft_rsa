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

#include <ssl/ssl.h>
#include <ssl/des.h>
#include <libft/bytes.h>

static t_des			__des;
static unsigned char	__key[8];
static unsigned char	__salt[8];
static unsigned char	__vect[8];

static void	__init_bufs(void);

t_des	*des_init(
	const unsigned char *key, const unsigned char *salt, const unsigned char *vect)
{
	__init_bufs();

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
	__init_bufs();

	if (NULL != keyhex)
	{
		ft_hex_to_bytes(__key, keyhex, MIN(16, ft_strlen(keyhex)));
		__des.vflag |= DES_K;
	}
	if (NULL != salthex)
	{
		ft_hex_to_bytes(__salt, salthex, MIN(16, ft_strlen(salthex)));
		__des.vflag |= DES_S;
	}
	if (NULL != vecthex)
	{
		ft_hex_to_bytes(__vect, vecthex, MIN(16, ft_strlen(vecthex)));
		__des.vflag |= DES_V;
	}
	__des.key = __key;
	__des.salt = __salt;
	__des.vect = __vect;

	return (&__des);
}

static void	__init_bufs(void)
{
	ft_bzero(__key, sizeof(__key));
	ft_bzero(__salt, sizeof(__salt));
	ft_bzero(__vect, sizeof(__vect));
	ft_bzero(&__des, sizeof(t_des));
}
