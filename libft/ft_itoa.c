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

#include <libft/std.h>
#include <libft/alloc.h>

void	__tostr(char *str, intmax_t nb, intmax_t mult)
{
	int	i;

	i = 0;

	if (nb < 0)
		str[i++] = '-';

	if (nb < 0)
		nb = -nb;

	while (mult)
	{
		str[i++] = nb / mult + 48;
		nb = nb % mult;
		mult = mult / 10;
	}

	str[i] = 0;
}

char	*ft_itoa(intmax_t n)
{
	intmax_t	nb;
	intmax_t	mult;
	char		*str;
	int			len;

	mult = 1;
	len = (n < 0) ? 2 : 1;
	nb = n;

	while (n / 10)
	{
		mult = mult * 10;
		n = n / 10;
		len++;
	}

	LIBFT_ALLOC(str, len + 1);
	__tostr(str, nb, mult);

	return (str);
}
