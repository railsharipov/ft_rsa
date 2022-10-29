/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_2d_array_del.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:35:41 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:18:24 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

void	ft_2darray_del(void **arr, int size)
{
	if (NULL == arr)
		return ;
	
	while (size > 0)
		LIBFT_FREE(arr[size--]);
	
	LIBFT_FREE(arr);
}

void 	ft_2darray_del_null_terminated(void **arr)
{
	if (NULL == arr)
		return ;

	while (NULL != *arr)
	{
		LIBFT_FREE(*arr);
		arr++;
	}

	LIBFT_FREE(arr);
}

int 	ft_2darray_len_null_terminated(void **arr)
{
	int	len;

	if (NULL == arr)
		return (0);
	
	len = 0;
	while (NULL != *arr++)
		len++;
	
	return (len);
}

char	*ft_2darray_strjoin(char **arr, int size, const char *delim)
{
	char	*joined_str;
	int		joined_size;
	int		delim_size;
	int		ix, iy;
	char	*p;

	if (NULL == arr || size <= 0)
		return (NULL);
	
	joined_size = 0;
	delim_size = ft_strlen(delim);

	ix = 0;
	while (ix < size)
	{
		joined_size += ft_strlen(arr[ix]);
		ix++;
	}
	joined_size += delim_size;

	LIBFT_ALLOC(joined_str, joined_size+1);

	p = joined_str;

	ix = 0;
	while (ix < size-1)
	{
		iy = 0;
		while (arr[ix][iy])
			*p++ = arr[ix][iy++];
		
		ft_memcpy(p, delim, delim_size);
		p += delim_size;

		ix++;
	}

	iy = 0;
	while (arr[ix][iy])
		*p++ = arr[ix][iy++];

	*p = 0;

	return (joined_str);
}
