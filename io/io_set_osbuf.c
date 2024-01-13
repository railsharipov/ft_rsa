#include <stdint.h>
#include <util/io.h>
#include <libft/string.h>
#include <libft/std.h>

int	io_set_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf)
{
	if (NULL == iodes || NULL == osbuf)
		return (-1);

	ft_bzero(iodes, sizeof(t_iodes));

	if (!FLAG(IO_OSBUF, flags))
		return (-1);

	if (!(FLAG(IO_READ, flags) || FLAG(IO_WRITE, flags)))
		return (-1);

	if (FLAG(IO_READ, flags))
		if (NULL == osbuf->content)
			return (-1);

	iodes->osbuf = osbuf;
	iodes->mode = IO_MODE_OSBUF;

	return (0);
}
