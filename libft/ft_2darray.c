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

static void	__del_null_terminated(void **arr)
{
	while (NULL != *arr)
	{
		LIBFT_FREE(*arr);
		arr++;
	}
}

void	ft_2darray_del(void **arr, int size)
{
	if ((NULL == arr) || (size < -1))
	{
		return ;
	}
	if (size == -1)
	{
		__del_null_terminated(arr);
	}
	else
	{
		while (--size > 0)
		{
			LIBFT_FREE(arr[size]);
		}
	}
	LIBFT_FREE(arr);
}

int		ft_2darray_len(void **arr)
{
	int	len;

	if (NULL == arr)
	{
		return (0);
	}
	len = 0;
	while (NULL != *arr++)
	{
		len++;
	}
	return (len);
}

char	*ft_2darray_merge_cstr(char **arr, int size)
{
	char	*cstr;
	char	*ptr;
	int	cstr_size;
	int	ix;
	int	iy;

	if ((NULL == arr) || (size <= 0))
	{
		return (NULL);
	}
	cstr_size = 0;
	ix = 0;
	while (ix < size)
	{
		cstr_size += ft_strlen(arr[ix]);
		ix++;
	}
	LIBFT_ALLOC(cstr, cstr_size+1);
	ptr = cstr;
	ix = 0;
	while (ix < size)
	{
		iy = 0;
		while (arr[ix][iy] != '\0')
		{
			*ptr++ = arr[ix][iy++];
		}
		ix++;
	}
	return (cstr);
}
