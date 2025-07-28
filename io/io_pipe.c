#include <stdint.h>
#include <io.h>
#include <libft/string.h>
#include <libft/std.h>

int io_pipe(t_iodes *iodes, uint32_t flags, t_iodes *iodes_1, t_iodes *iodes_2)
{
	IO_LOG(TRACE, "io pipe with flags=0x%x, iodes=%p, iodes_1=%p, iodes_2=%p", flags, iodes, iodes_1, iodes_2);
	
	if (NULL == iodes || NULL == iodes_1 || NULL == iodes_2) {
		IO_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}
	if (!io_is_init(iodes_1) || !io_is_init(iodes_2)) {
		IO_LOG(ERROR, "all iodes must be initialized");
		return (-1);
	}
	iodes->pipe.iodes_1 = iodes_1;
	iodes->pipe.iodes_2 = iodes_2;
	iodes->mode = IO_MODE_PIPE;

	return (0);
}
