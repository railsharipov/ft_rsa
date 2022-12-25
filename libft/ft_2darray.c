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

void	ft_2darray_del(void **arr, int arr_len)
{
	if (NULL == arr)
		return ;
	
	while (arr_len > 0)
		LIBFT_FREE(arr[arr_len--]);
	
	LIBFT_FREE(arr);
}

void 	ft_2darray_del_null_terminated(void **arr)
{
	size_t	idx;

	if (NULL == arr)
		return ;

	idx = 0;
	while (NULL != arr[idx])
	{
		LIBFT_FREE(arr[idx]);
		idx++;
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

char	*ft_2darray_strjoin(char **str_arr, size_t str_arr_len, const char *delim)
{
	char	*joined_str;
	int		joined_size;
	int		delim_size;
	int		ix;
	size_t	slen;
	char	*p;

	if (NULL == str_arr || str_arr_len == 0)
		return (NULL);
	
	joined_size = 0;
	delim_size = ft_strlen(delim);

	ix = 0;
	while (ix < str_arr_len)
		joined_size += ft_strlen(str_arr[ix++]);
	
	joined_size += delim_size * (str_arr_len-1);

	LIBFT_ALLOC(joined_str, joined_size+1);
	p = joined_str;

	ix = 0;
	while (ix < str_arr_len-1)
	{
		slen = ft_strlen(str_arr[ix]);
		ft_memcpy(p, str_arr[ix], slen);
		ft_memcpy(p + slen, delim, delim_size);

		p += slen + delim_size;
		ix++;
	}

	ft_strcpy(p, str_arr[str_arr_len-1]);

	return (joined_str);
}
