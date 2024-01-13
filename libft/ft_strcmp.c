/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/22 13:46:44 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/11 14:11:00 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

int		ft_strcmp(const char *s1, const char *s2)
{
	size_t i;

	// no null check, same as libc strcmp

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}

	return (s1[i] - s2[i]);
}
