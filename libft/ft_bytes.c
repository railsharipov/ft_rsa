/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ssl_bytes.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 10:47:08 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/05 08:45:31 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <libft/bytes.h>
#include <libft/string.h>
#include <libft/std.h>

static const int	COL = 16;

void	ft_bytes_xor(void *res, void *bytes1, void *bytes2, size_t size)
{
	uint8_t	*rptr;
	uint8_t	*b1ptr;
	uint8_t	*b2ptr;
	size_t	i;

	rptr = (uint8_t *)res;
	b1ptr = (uint8_t *)bytes1;
	b2ptr = (uint8_t *)bytes2;

	i = 0;
	while (i < size)
	{
		rptr[i] = b1ptr[i] ^ b2ptr[i];
		i++;
	}
}

uint64_t	ft_bytes_to_uint(void *bytes, size_t size)
{
	uint8_t		*ptr;
	uint64_t	num;
	size_t		idx;

	num = 0;
	size = MIN(size, sizeof(uint64_t));
	ptr = (uint8_t *)bytes;

	idx = 0;
	while (idx < size)
	{
		num <<= 8;
		num |= (uint64_t)ptr[idx++];
	}
	return (num);
}

void	ft_bytes_lshift(void *bytes, size_t size, int shift)
{
	unsigned char	*ptr;
	unsigned int	tmp;
	unsigned int	rem;
	size_t			idx;

	if ((NULL == bytes) || (shift <= 0)) {
		return ;
	}
	ptr = (unsigned char *)bytes + size-1;
	rem = 0;
	idx = 0;

	while (idx++ < size)
	{
		tmp = *ptr;
		*ptr = (*ptr << shift) | rem;
		rem = tmp >> (CHAR_BIT-shift);
		ptr--;
	}
}

void	ft_bytes_rshift(void *bytes, size_t size, int shift)
{
	unsigned char	*ptr;
	unsigned int	tmp;
	unsigned int	rem;
	size_t			idx;

	if ((NULL == bytes) || (shift <= 0)) {
		return ;
	}
	ptr = (unsigned char *)bytes;
	rem = 0;
	idx = 0;

	while (idx < size)
	{
		tmp = ptr[idx];
		ptr[idx] = (ptr[idx] >> shift) | rem;
		rem = tmp << (CHAR_BIT-shift);
		idx++;
	}
}

void	ft_bytes_write_hex(int fd, void *ptr, size_t size)
{
	unsigned char		*octets;
	uint32_t	bytes;
	int			ix;
	int			iy;

	if ((size <= 0) || (NULL == ptr) || (fd < 0))
		return ;

	octets = (unsigned char *)(ptr);
	bytes = 0;

	ix = 0;
	while (ix < size)
	{
		ft_fprintf(fd, "%.4x -", bytes);

		iy = 0;
		while ((iy < COL) && (ix < size))
		{
			ft_fprintf(fd, " %.2x", octets[ix++]);
			iy++;
		}
		ix -= iy;
		while (iy++ <= COL)
		{
			ft_fprintf(fd, "   ");
		}
		iy = 0;
		while ((iy < COL) && (ix < size))
		{
			if (ft_isprint(octets[ix]))
			{
				ft_fprintf(fd, "%c", octets[ix]);
			}
			else
			{
				ft_fprintf(fd, ".");
			}
			iy++;
			ix++;
		}
		ft_fprintf(fd, "\n");
		bytes += iy;
	}
}

void	ft_bytes_dump_hex(void *ptr, size_t size, int cols, int del)
{
	unsigned char	*octets;
	size_t		ix;

	if (NULL != ptr)
	{
		octets = (unsigned char *)(ptr);

		ix = 0;
		while (ix < size)
		{
			if (del != 0)
				ft_printf("%.2x%c", octets[ix++], del);
			else
				ft_printf("%.2x", octets[ix++]);

			if (cols != 0)
				if ((ix < size) && (ix % cols == 0))
					ft_printf("\n");
		}
	}
	ft_printf("\n");
}

void	ft_bytes_print_bits(void *ptr, size_t size)
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