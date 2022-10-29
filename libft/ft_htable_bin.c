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

#include <stddef.h>
#include <libft.h>

static char		*__to_cstring(char *raw, size_t rawsize);

void	ft_htbl_bin_add(
	t_htbl *htbl, void *content, const void *bin, size_t binsize)
{
	char	*cskey;

	cskey = __to_cstring((char *)bin, binsize);
	(ft_htbl_add)(htbl, content, (const char *)cskey);
	LIBFT_FREE(cskey);
}

void	*ft_htbl_bin_get(t_htbl *htbl, const void *bin, size_t binsize)
{
	char	*cskey;
	void	*content;

	cskey = __to_cstring((char *)bin, binsize);
	content = (ft_htbl_get)(htbl, (const char *)cskey);
	LIBFT_FREE(cskey);
	
	return (content);
}

void	ft_htbl_bin_assign(
	t_htbl *htbl, void *content, const void *bin, size_t binsize)
{
	char	*cskey;

	cskey = __to_cstring((char *)bin, binsize);
	(ft_htbl_assign)(htbl, content, (const char *)cskey);
	LIBFT_FREE(cskey);
}

static char	*__to_cstring(char *raw, size_t rawsize)
{
	char *cskey;
	size_t ix;

	LIBFT_ALLOC(cskey, rawsize + 1);

	ix = 0;
	while (ix < rawsize)
	{
		if (raw[ix] == 0)
			cskey[ix] = '=';
		else
			cskey[ix] = raw[ix];

		ix++;
	}
	cskey[rawsize] = 0;

	return (cskey);
}