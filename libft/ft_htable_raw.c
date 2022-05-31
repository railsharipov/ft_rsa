/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hash.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static char	*__to_cstring(char *raw, size_t rawsize)
{
	char	*cskey;
	size_t	ix;

	LIBFT_ALLOC(cskey, rawsize + 1);
	ix = 0;
	while (ix < rawsize)
	{
		if (raw[ix] == 0)
		{
			cskey[ix] = '=';
		}
		else
		{
			cskey[ix] = raw[ix];
		}
		ix++;
	}
	cskey[rawsize] = 0;
	return (cskey);
}

void	ft_htbl_raw_add(
	t_htbl *htbl, const void *key, size_t keysize, void *content)
{
	char	*cskey;

	cskey = __to_cstring((char *)key, keysize);
	ft_htbl_add(htbl, (const char *)cskey, content);
	LIBFT_FREE(cskey);
}

void	*ft_htbl_raw_get(t_htbl *htbl, const void *key, size_t keysize)
{
	char	*cskey;
	void	*content;

	cskey = __to_cstring((char *)key, keysize);
	content = ft_htbl_get(htbl, (const char *)cskey);
	LIBFT_FREE(cskey);
	return (content);
}

void	ft_htbl_raw_assign(
	t_htbl *htbl, const void *key, size_t keysize, void *content)
{
	char	*cskey;

	cskey = __to_cstring((char *)key, keysize);
	ft_htbl_assign(htbl, (const char *)cskey, content);
	LIBFT_FREE(cskey);
}
