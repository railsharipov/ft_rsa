#include <ft_ssl.h>

static const int	COL = 16;

void	util_hexdump(int fd, void *ptr, size_t size)
{
	uint8_t		*octets;
	uint32_t	bytes;
	int			ix;
	int			iy;

	if ((size <= 0) || (NULL == ptr) || (fd < 0))
		return ;

	octets = (uint8_t *)(ptr);
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
