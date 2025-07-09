#include <stdint.h>
#include <io.h>
#include <libft/string.h>
#include <libft/std.h>

int	io_osbuf(t_iodes *iodes, uint32_t flags, t_ostring *osbuf)
{
	IO_LOG(TRACE, "io_osbuf: entering function with flags=0x%x, osbuf=%p", flags, osbuf);
	
	if (NULL == iodes || NULL == osbuf) {
		IO_LOG(TRACE, "io_osbuf: NULL parameter detected - iodes=%p, osbuf=%p", iodes, osbuf);
		return (-1);
	}

	IO_LOG(TRACE, "io_osbuf: initializing iodes structure");
	ft_bzero(iodes, sizeof(t_iodes));

	if (!(FLAG(IO_READ, flags) || FLAG(IO_WRITE, flags))) {
		IO_LOG(TRACE, "io_osbuf: invalid flags 0x%x - neither IO_READ nor IO_WRITE set", flags);
		return (-1);
	}

	if (FLAG(IO_READ, flags)) {
		if (NULL == osbuf->content) {
			IO_LOG(TRACE, "io_osbuf: IO_READ flag set but osbuf content is NULL");
			return (-1);
		}
		IO_LOG(TRACE, "io_osbuf: IO_READ flag set, osbuf content=%p, size=%zu", osbuf->content, osbuf->size);
	}

	IO_LOG(TRACE, "io_osbuf: setting osbuf and mode");
	iodes->osbuf = osbuf;
	iodes->mode = IO_MODE_OSBUF;

	IO_LOG(TRACE, "io_osbuf: setup successful, returning 0");
	return (0);
}
