/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/12 21:14:31 by rsharipo          #+#    #+#             */
/*   Updated: 2018/07/12 21:28:06 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <unistd.h>

void	ft_putnbr(int n)
{
	char	c;
	int		nb;
	int		mult;

	mult = 1;
	nb = n;
	while (n / 10)
	{
		mult = mult * 10;
		n = n / 10;
	}
	if (nb < 0)
		write(1, "-", 1);
	while (mult)
	{
		c = (nb < 0) ? (-(nb / mult) + 48) : (nb / mult + 48);
		write(1, &c, 1);
		nb = nb % mult;
		mult = mult / 10;
	}
}
