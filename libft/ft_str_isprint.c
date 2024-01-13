/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str_isprint.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 10:39:05 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/10 10:39:17 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

int	ft_str_isprint(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isprint(*s++))
			return (0);

	return (1);
}
