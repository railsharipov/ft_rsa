/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 10:55:30 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/13 09:34:34 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <stdlib.h>

static int	__space_needed(intmax_t value, int base)
{
	int	len;

	len = 0;
	while (value)
	{
		len++;
		value = value / base;
	}
	return (len);
}

static int	*__create_arr(intmax_t val, int base, int len)
{
	static int	arr[64];

	len--;
	while (val)
	{
		arr[len] = (val < 0) ? (-1 * (int)(val % base)) : (int)(val % base);
		val = val / base;
		len--;
	}
	return (arr);
}

static char	*__create_str(int *arr, int len, int neg)
{
	static	char	s[64];
	int				i;

	i = 0;
	if (neg)
		s[0] = '-';
	while (i < len)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			s[i + neg] = arr[i] + 48;
		else if (arr[i] >= 10 && arr[i] <= 15)
			s[i + neg] = arr[i] + 87;
		i++;
	}
	s[i + neg] = 0;
	ft_bzero(arr, 64 * sizeof(int));
	return (s);
}

char	*ft_itoa_base(intmax_t value, int base)
{
	int		neg;
	int		len;
	int		*arr;

	arr = NULL;
	len = 0;
	neg = 0;
	if (value == 0)
		return ("0");
	if (base != 10 && value < 0)
		value = -value;
	else if (value < 0)
		neg = 1;
	len = __space_needed(value, base);
	arr = __create_arr(value, base, len);
	return (__create_str(arr, len, neg));
}
