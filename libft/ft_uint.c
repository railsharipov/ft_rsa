#include <unistd.h>
#include <libft/bytes.h>

int		ft_uint_lmbit(uint64_t num, int bits)
{
	bits = MIN(bits, 8 * sizeof(num));

	for (int i = bits; i > 0; i--)
	{
		if ((((uint64_t)1)<<(i-1)) & num)
			return (i);
	}
	return (0);
}

int		ft_uint_rmbit(uint64_t num, int bits)
{
	for (int i = 1; i <= bits; i++)
	{
		if ((1<<(i-1)) & num)
			return (i);
	}
	return (0);
}

void	ft_uint_print_bits(uint64_t num, int numbits, int bit_split)
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

void	ft_uint_to_bytes(void *bytes, uint64_t uint, int uint_size)
{
	uint8_t	*buf;
	int		idx;
	int		shift;

	if (!bytes) {
		return ;
	}
	buf = (unsigned char *)bytes;
	idx = 0;
	while (idx < uint_size)
	{
		shift = (uint_size - idx - 1) * 8;
		buf[idx] = (unsigned char)((uint >> shift) & 0xff);
		idx++;
	}
}