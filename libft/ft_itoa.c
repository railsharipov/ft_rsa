/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 12:52:50 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:20:08 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static char	*ft_getstr(char *str, int nb, int mult)
{
	unsigned int	i;

	i = 0;
	if (nb < 0)
		str[i++] = '-';
	while (mult)
	{
		str[i++] = (nb < 0) ? (-(nb / mult) + 48) : (nb / mult + 48);
		nb = nb % mult;
		mult = mult / 10;
	}
	str[i] = 0;
	return (str);
}

char		*ft_itoa(int n)
{
	char			*str;
	int				nb;
	int				digits;
	int				mult;

	mult = 1;
	digits = (n < 0) ? 2 : 1;
	nb = n;
	while (n / 10)
	{
		mult = mult * 10;
		n = n / 10;
		digits++;
	}
	LIBFT_ALLOC(str, digits + 1);
	return (ft_getstr(str, nb, mult));
}
