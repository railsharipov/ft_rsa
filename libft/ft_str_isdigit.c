/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str_isdigit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 10:36:45 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/10 10:38:41 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/string.h>

int	ft_str_isdigit(const char *s)
{
	if (s == NULL)
		return (0);

	while (*s != 0)
		if (!ft_isdigit(*s++))
			return (0);

	return (1);
}
