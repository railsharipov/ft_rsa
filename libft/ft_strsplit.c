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

static int __get_nwords(char const *s, char c);

char	**ft_strsplit(char const *s, char c)
{
	int		idx;
	int		word_idx;
	int		start_idx;
	int		nwords;
	char	**arr;

	start_idx = 0;
	nwords = __get_nwords(s, c);

	if (NULL == s)
		return (NULL);
	
	LIBFT_ALLOC(arr, sizeof(char *) * (nwords + 1));

	idx = 0;
	word_idx = 0;
	while (word_idx < nwords)
	{
		while (s[idx] == c && s[idx] != 0)
			idx++;
		
		start_idx = idx;
		while (s[idx] != c && s[idx] != 0)
			idx++;
		
		arr[word_idx++] = ft_strsub(s, start_idx, (size_t)(idx - start_idx));
	}
	arr[word_idx] = 0;
	
	return (arr);
}

static int __get_nwords(char const *s, char c)
{
	int idx;
	int nwords;

	nwords = 0;

	idx = 0;
	if (s)
	{
		while (s[idx] == c && s[idx] != 0)
			idx++;

		while (s[idx] != 0)
		{
			nwords++;

			while (s[idx] != c && s[idx] != 0)
				idx++;

			while (s[idx] == c && s[idx] != 0)
				idx++;
		}
	}
	return (nwords);
}
