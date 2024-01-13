/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str_isascii.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 10:39:39 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/10 10:39:55 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

int	ft_str_isascii(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isascii(*s++))
			return (0);

	return (1);
}
