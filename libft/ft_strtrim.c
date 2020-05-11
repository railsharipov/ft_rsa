/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 10:45:27 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 21:54:17 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static int	is_space(char c)
{
	if (c == ' ' || c == '\n' || c == '\t')
		return (1);
	return (0);
}

char		*ft_strtrim(char const *s)
{
	int	start;
	int	i;

	i = 0;
	if (!(s))
		return (NULL);
	if (!(*s))
		return ((char *)s);
	while (s[i] != 0 && is_space(s[i]))
		i++;
	start = (s[i] == 0) ? 0 : i;
	while (s[i] != 0)
		i++;
	while (i >= 0 && (s[i] == 0 || is_space(s[i])))
		i--;
	return (ft_strsub(s, (unsigned int)start, (size_t)(i - start + 1)));
}
