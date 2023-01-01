/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_aatoi.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/13 09:33:08 by rsharipo          #+#    #+#             */
/*   Updated: 2018/08/22 11:25:55 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static int	__is_blank(char c)
{
	if ((c >= 9 && c <= 13) || c == 32)
		return (1);
	return (0);
}

long long	ft_atoi(const char *str)
{
	int			i;
	long long	nb;
	int			neg;
	long long	max;

	max = LLONG_MAX;
	nb = 0;

	i = 0;
	while (__is_blank(str[i]))
		i++;
	
	neg = (str[i] == '-') ? -1 : 1;

	if (str[i] == '-' || str[i] == '+')
		i++;
	
	while (str[i] >= '0' && str[i] <= '9' && str[i] != 0)
	{
		nb = nb * 10;
		nb = nb + str[i++] - 48;
	}

	return (neg * nb);
}
