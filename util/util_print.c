/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_func.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 11:51:25 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/28 17:07:55 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>

void	util_print_intbits(uint64_t num, int numbits, int bit_split)
{
	int	j;

	j = 0;
	while (j < numbits)
	{
		if ((num << j) & ((uint64_t)1 << (numbits - 1)))
			write(1, "1", 1);
		else
			write(1, "0", 1);
		j++;
		if (j % bit_split == 0)
			write(1, " ", 1);
	}
	write(1, "\n", 1);
}

void	util_print_strbits(void *ptr, size_t size)
{
	size_t			i;
	size_t			j;

	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < 8)
		{
			if ((((unsigned char *)ptr)[i] << j) & (1 << 7))
				write(1, "1", 1);
			else
				write(1, "0", 1);
			j++;
		}
		write(1, " ", 1);
		if ((i + 1) % 8 == 0)
			write(1, "\n", 1);
		i++;
	}
	if (i % 8 != 0)
		write(1, "\n", 1);
}
