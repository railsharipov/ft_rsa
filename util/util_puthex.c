#include <ft_ssl.h>

void	util_puthex(void *ptr, int size, int cols, int del)
{
	uint8_t	*octets;
	int		ix;

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
