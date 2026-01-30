#include <stdint.h>
#include <io.h>
#include <libft/string.h>
#include <libft/std.h>

int io_pipe(t_iodes *iodes, uint32_t flags, t_iodes *iodes_in, t_iodes *iodes_out)
{
	SSL_LOG(TRACE, "io pipe with flags=0x%x, iodes=%p, iodes_in=%p, iodes_out=%p", flags, iodes, iodes_in, iodes_out);

	if (NULL == iodes || NULL == iodes_in || NULL == iodes_out) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (!io_is_init(iodes_in) || !io_is_init(iodes_out)) {
		SSL_LOG(ERROR, "all iodes must be initialized");
		return (-1);
	}
	iodes->pipe.iodes_in = iodes_in;
	iodes->pipe.iodes_out = iodes_out;
	iodes->mode = IO_MODE_PIPE;

	return (0);
}
