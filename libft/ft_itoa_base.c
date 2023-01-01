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

void	__digit_arr(uint8_t *arr, intmax_t val, int base, int len)
{
	if (val < 0)
		val = -val;

	len--;
	while (val)
	{
		arr[len] = (uint8_t)(val % base);
		val = val / base;
		len--;
	}
}

void	__base_str(char *s, uint8_t *arr, int len, int is_neg)
{
	uint8_t	*p;
	uint8_t	digit;

	p = (uint8_t *)s;

	if (is_neg)
		*p++ = '-';

	while (len--)
	{
		digit = *arr++;

		if (digit >= 0 && digit <= 9)
			*p = digit + 48;
		else if (digit >= 10 && digit <= 15)
			*p = digit + 87;
		
		p++;
	}
	
	*p = 0;
}

char	*ft_itoa_base(intmax_t value, int base)
{
	int		is_neg;
	int		len;
	uint8_t	arr[64];
	char	*s;

	len = 0;
	is_neg = 0;

	if (base > 16)
		return (ft_strdup("n/a"));

	if (value == 0)
		return (ft_strdup("0"));
	
	if (base != 10 && value < 0)
		value = -value;
	
	if (base == 10 && value < 0)
		is_neg = 1;
	
	len = __space_needed(value, base);
	__digit_arr(arr, value, base, len);

	LIBFT_ALLOC(s, len+1);
	__base_str(s, arr, len, is_neg);

	return (s);
}
