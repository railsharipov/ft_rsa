/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsplit.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 11:16:09 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:23:23 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static unsigned int	__calc(char const *s, char c)
{
	unsigned int	i;
	unsigned int	num;

	i = 0;
	num = 0;
	if (s)
	{
		while (s[i] == c && s[i] != 0)
		{
			i++;
		}
		while (s[i] != 0)
		{
			num++;
			while (s[i] != c && s[i] != 0)
			{
				i++;
			}
			while (s[i] == c && s[i] != 0)
			{
				i++;
			}
		}
	}
	return (num);
}

char	**__clean(char **arr, unsigned int words)
{
	unsigned int	i;

	i = -1;
	while (arr && ++i <= words)
	{
		LIBFT_FREE(arr[i]);
	}
	LIBFT_FREE(arr);
	return (NULL);
}

char	**ft_strsplit(char const *s, char c)
{
	unsigned int	i;
	unsigned int	j;
	unsigned int	start;
	unsigned int	words;
	char			**arr;

	i = 0;
	j = 0;
	start = 0;
	words = __calc(s, c);
	if (NULL == s)
	{
		return (NULL);
	}
	LIBFT_ALLOC(arr, sizeof(char *) * (words + 1));
	while (j < words)
	{
		while (s[i] == c && s[i] != 0)
		{
			i++;
		}
		start = i;
		while (s[i] != c && s[i] != 0)
		{
			i++;
		}
		if (!(arr[j++] = ft_strsub(s, start, (size_t)(i - start))))
		{
			arr = __clean(arr, words);
		}
	}
	(arr) ? (arr[j] = 0) : 0;
	return (arr);
}
